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

#ifndef TYR_PLANNING_GROUND_TASK_HPP_
#define TYR_PLANNING_GROUND_TASK_HPP_

#include "tyr/common/config.hpp"                    // for float_t, uint_t
#include "tyr/common/dynamic_bitset.hpp"            // for test
#include "tyr/common/vector.hpp"                    // for get
#include "tyr/formalism/planning/declarations.hpp"  // for OverlayRepos...
#include "tyr/formalism/planning/fdr_context.hpp"
#include "tyr/formalism/planning/views.hpp"  // for View
#include "tyr/planning/declarations.hpp"
#include "tyr/planning/ground_task/match_tree/match_tree.hpp"  // for Matc...

#include <boost/dynamic_bitset.hpp>  // for dynamic_bitset
#include <limits>                    // for numeric_limits
#include <stddef.h>                  // for size_t
#include <vector>                    // for vector

namespace tyr::planning
{

class GroundTask
{
public:
    GroundTask(DomainPtr domain,
               formalism::planning::RepositoryPtr overlay_repository,
               View<Index<formalism::planning::FDRTask>, formalism::planning::Repository> fdr_task,
               formalism::planning::GeneralFDRContext fdr_context);

    template<formalism::FactKind T>
    size_t get_num_atoms() const noexcept;
    size_t get_num_actions() const noexcept;
    size_t get_num_axioms() const noexcept;

    const auto& get_static_atoms_bitset() const noexcept { return m_static_atoms_bitset; }
    const auto& get_static_numeric_variables() const noexcept { return m_static_numeric_variables; }
    bool test(Index<formalism::planning::GroundAtom<formalism::StaticTag>> index) const { return tyr::test(uint_t(index), m_static_atoms_bitset); }
    float_t get(Index<formalism::planning::GroundFunctionTerm<formalism::StaticTag>> index) const noexcept
    {
        return tyr::get(uint_t(index), m_static_numeric_variables, std::numeric_limits<float_t>::quiet_NaN());
    }

    const auto& get_domain() const noexcept { return m_domain; }

    auto get_task() const noexcept { return m_fdr_task; }

    auto& get_repository() noexcept { return m_overlay_repository; }
    const auto& get_repository() const noexcept { return m_overlay_repository; }

    const auto& get_action_match_tree() const noexcept { return m_action_match_tree; }
    const auto& get_axiom_match_tree_strata() const noexcept { return m_axiom_match_tree_strata; }

private:
    DomainPtr m_domain;

    formalism::planning::RepositoryPtr m_overlay_repository;
    View<Index<formalism::planning::FDRTask>, formalism::planning::Repository> m_fdr_task;
    boost::dynamic_bitset<> m_static_atoms_bitset;
    std::vector<float_t> m_static_numeric_variables;

    match_tree::MatchTreePtr<formalism::planning::GroundAction> m_action_match_tree;

    std::vector<match_tree::MatchTreePtr<formalism::planning::GroundAxiom>> m_axiom_match_tree_strata;
};

}

#endif
