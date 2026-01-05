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

#ifndef TYR_SOLVER_POLICIES_TERMINATION_HPP_
#define TYR_SOLVER_POLICIES_TERMINATION_HPP_

#include "tyr/common/config.hpp"
#include "tyr/common/dynamic_bitset.hpp"
#include "tyr/datalog/fact_sets.hpp"
#include "tyr/datalog/policies/annotation.hpp"
#include "tyr/formalism/datalog/declarations.hpp"
#include "tyr/formalism/datalog/ground_atom_index.hpp"

#include <boost/dynamic_bitset.hpp>
#include <concepts>

namespace tyr::datalog
{

template<typename T>
concept TerminationPolicyConcept = requires(T& p,
                                            const T& cp,
                                            Index<formalism::datalog::GroundAtom<formalism::FluentTag>> atom,
                                            const PredicateFactSets<formalism::FluentTag>& goals,
                                            const OrAnnotationsList& or_annot) {
    { p.set_goals(goals) } -> std::same_as<void>;
    { p.achieve(atom) } -> std::same_as<void>;
    { cp.check() } -> std::same_as<bool>;
    { cp.get_total_cost(or_annot) } -> std::same_as<Cost>;
    { p.reset() } -> std::same_as<void>;
    { p.clear() } -> std::same_as<void>;
};

class NoTerminationPolicy
{
public:
    NoTerminationPolicy() = default;

    void set_goals(const PredicateFactSets<formalism::FluentTag>& goals) {}
    void achieve(Index<formalism::datalog::GroundAtom<formalism::FluentTag>> atom) noexcept {}
    bool check() const noexcept { return false; }
    Cost get_total_cost(const OrAnnotationsList& or_annot) const noexcept { return Cost(0); }
    void reset() noexcept {}
    void clear() noexcept {}
};

class TerminationPolicy
{
public:
    explicit TerminationPolicy(size_t num_fluent_predicates) : unsat_goals(num_fluent_predicates), num_unsat_goals(0), atoms() {}

    void set_goals(const PredicateFactSets<formalism::FluentTag>& goals)
    {
        clear();

        num_unsat_goals = 0;
        for (const auto& set : goals.get_sets())
        {
            for (const auto& atom : set.get_facts().get_data())
            {
                tyr::set(atom.value, true, unsat_goals[uint_t(atom.group)]);
                ++num_unsat_goals;
                atoms.push_back(atom);
            }
        }
    }

    void achieve(Index<formalism::datalog::GroundAtom<formalism::FluentTag>> atom) noexcept
    {
        if (tyr::test(atom.value, unsat_goals[uint_t(atom.group)]))
        {
            --num_unsat_goals;
            tyr::set(atom.value, false, unsat_goals[uint_t(atom.group)]);
        }
    }

    bool check() const noexcept { return num_unsat_goals == 0; }

    Cost get_total_cost(const OrAnnotationsList& or_annot) const noexcept
    {
        auto cost = Cost(0);

        for (const auto atom : atoms)
        {
            assert(uint_t(atom.group) < or_annot.size());
            assert(atom.value < or_annot[uint_t(atom.group)].size());
            cost += or_annot[uint_t(atom.group)][atom.value];
        }

        return cost;
    }

    void reset() noexcept
    {
        num_unsat_goals = 0;
        for (auto& bitset : unsat_goals)
            bitset.reset();

        for (const auto& atom : atoms)
            tyr::set(atom.value, true, unsat_goals[uint_t(atom.group)]);
    }

    void clear() noexcept
    {
        num_unsat_goals = 0;
        for (auto& bitset : unsat_goals)
            bitset.reset();

        atoms.clear();
    }

private:
    std::vector<boost::dynamic_bitset<>> unsat_goals;
    size_t num_unsat_goals;

    IndexList<formalism::datalog::GroundAtom<formalism::FluentTag>> atoms;
};
}

#endif
