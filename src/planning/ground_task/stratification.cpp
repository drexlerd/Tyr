/*
 * Copyright (C) 2025 Dominik Drexler
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#include "tyr/planning/ground_task/stratification.hpp"

#include "tyr/common/equal_to.hpp"
#include "tyr/common/hash.hpp"
#include "tyr/formalism/formatter.hpp"
#include "tyr/formalism/overlay_repository.hpp"
#include "tyr/formalism/repository.hpp"
#include "tyr/formalism/views.hpp"

using namespace tyr::formalism;

namespace tyr::planning
{

namespace details
{
enum class StratumStatus
{
    UNCONSTRAINED = 0,
    LOWER = 1,
    STRICTLY_LOWER = 2,
};

struct GroundAtomStrata
{
    std::vector<UnorderedSet<Index<GroundAtom<DerivedTag>>>> strata;
};

static GroundAtomStrata compute_atom_stratification(View<Index<FDRTask>, OverlayRepository<Repository>> task)
{
    auto R = UnorderedMap<Index<GroundAtom<DerivedTag>>, UnorderedMap<Index<GroundAtom<DerivedTag>>, StratumStatus>> {};

    const auto& atoms = task.get_atoms<DerivedTag>().get_data();

    // lines 2-4
    for (const auto atom_1 : atoms)
    {
        for (const auto atom_2 : atoms)
        {
            R[atom_1][atom_2] = StratumStatus::UNCONSTRAINED;
        }
    }

    // lines 5-10
    for (const auto axiom : task.get_ground_axioms())
    {
        const auto head_atom = axiom.get_head().get_index();

        for (const auto literal : axiom.get_body().get_facts<DerivedTag>())
        {
            const auto body_atom = literal.get_atom().get_index();

            if (!literal.get_polarity())
            {
                R[body_atom][head_atom] = StratumStatus::STRICTLY_LOWER;
            }
            else
            {
                R[body_atom][head_atom] = StratumStatus::LOWER;
            }
        }
    }

    // lines 11-15
    for (const auto& atom_1 : atoms)
    {
        for (const auto& atom_2 : atoms)
        {
            for (const auto& atom_3 : atoms)
            {
                if (std::min(static_cast<int>(R.at(atom_2).at(atom_1)), static_cast<int>(R.at(atom_1).at(atom_3))) > 0)
                {
                    R.at(atom_2).at(atom_3) = static_cast<StratumStatus>(std::max(
                        { static_cast<int>(R.at(atom_2).at(atom_1)), static_cast<int>(R.at(atom_1).at(atom_3)), static_cast<int>(R.at(atom_2).at(atom_3)) }));
                }
            }
        }
    }

    // lines 16-27
    if (std::any_of(atoms.begin(), atoms.end(), [&R](const auto& atom) { return R.at(atom).at(atom) == StratumStatus::STRICTLY_LOWER; }))
    {
        throw std::runtime_error("Set of rules is not stratifiable.");
    }

    auto atoms_strata = GroundAtomStrata {};
    auto remaining = UnorderedSet<Index<GroundAtom<DerivedTag>>>(atoms.begin(), atoms.end());
    while (!remaining.empty())
    {
        auto stratum = UnorderedSet<Index<GroundAtom<DerivedTag>>> {};
        for (const auto& atom_1 : remaining)
        {
            if (std::all_of(remaining.begin(),
                            remaining.end(),
                            [&R, &atom_1](const auto& atom_2) { return R.at(atom_2).at(atom_1) != StratumStatus::STRICTLY_LOWER; }))
            {
                stratum.insert(atom_1);
            }
        }

        for (const auto& atom : stratum)
        {
            remaining.erase(atom);
        }

        atoms_strata.strata.push_back(std::move(stratum));
    }

    return atoms_strata;
}
}

GroundAxiomStrata compute_ground_axiom_stratification(View<Index<FDRTask>, OverlayRepository<Repository>> task)
{
    const auto atom_stratification = details::compute_atom_stratification(task);

    auto axiom_strata = GroundAxiomStrata {};

    auto remaining_axioms = UnorderedSet<Index<GroundAxiom>>(task.get_ground_axioms().get_data().begin(), task.get_ground_axioms().get_data().end());

    for (const auto& atom_stratum : atom_stratification.strata)
    {
        auto stratum = UnorderedSet<Index<GroundAxiom>> {};

        for (const auto axiom : remaining_axioms)
        {
            if (atom_stratum.count(make_view(axiom, task.get_context()).get_head().get_index()))
            {
                stratum.insert(axiom);
            }
        }

        for (const auto axiom : stratum)
        {
            remaining_axioms.erase(axiom);
        }

        axiom_strata.data.push_back(GroundAxiomStratum(stratum.begin(), stratum.end()));
    }

    // std::cout << rule_strata.data << std::endl;

    return axiom_strata;
}
}
