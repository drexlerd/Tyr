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

#ifndef TYR_PLANNING_LIFTED_TASK_HPP_
#define TYR_PLANNING_LIFTED_TASK_HPP_

#include "tyr/common/config.hpp"          // for float_t, uint_t
#include "tyr/common/dynamic_bitset.hpp"  // for test
#include "tyr/common/vector.hpp"          // for get
#include "tyr/formalism/overlay_repository.hpp"
#include "tyr/formalism/planning/declarations.hpp"  // for OverlayRepos...
#include "tyr/formalism/planning/fdr_context.hpp"
#include "tyr/formalism/planning/views.hpp"  // for View
#include "tyr/planning/declarations.hpp"
#include "tyr/planning/programs/action.hpp"
#include "tyr/planning/programs/axiom.hpp"

#include <boost/dynamic_bitset.hpp>  // for dynamic_bitset
#include <limits>                    // for numeric_limits
#include <memory>                    // for shared_ptr
#include <vector>                    // for vector

namespace tyr::planning
{

class LiftedTask
{
public:
    LiftedTask(DomainPtr domain,
               formalism::planning::RepositoryPtr repository,
               formalism::OverlayRepositoryPtr<formalism::planning::Repository> overlay_repository,
               View<Index<formalism::planning::Task>, formalism::OverlayRepository<formalism::planning::Repository>> task,
               std::shared_ptr<formalism::planning::BinaryFDRContext<formalism::OverlayRepository<formalism::planning::Repository>>> fdr_context);

    GroundTaskPtr get_ground_task();

    /**
     * Getters
     */

    const auto& get_domain() const noexcept { return m_domain; }

    auto get_task() const noexcept { return m_task; }

    const auto& get_fdr_context() const noexcept { return m_fdr_context; }

    const auto& get_repository() const noexcept { return m_overlay_repository; }

    const auto& get_axiom_program() const noexcept { return m_axiom_program; }
    const auto& get_action_program() const noexcept { return m_action_program; }
    const auto& get_parameter_domains_per_cond_effect_per_action() const noexcept { return m_parameter_domains_per_cond_effect_per_action; }

    const auto& get_static_atoms_bitset() const noexcept { return m_static_atoms_bitset; }
    const auto& get_static_numeric_variables() const noexcept { return m_static_numeric_variables; }
    bool test(Index<formalism::planning::GroundAtom<formalism::StaticTag>> index) const { return tyr::test(uint_t(index), m_static_atoms_bitset); }
    float_t get(Index<formalism::planning::GroundFunctionTerm<formalism::StaticTag>> index) const
    {
        return tyr::get(uint_t(index), m_static_numeric_variables, std::numeric_limits<float_t>::quiet_NaN());
    }

private:
    DomainPtr m_domain;
    formalism::planning::RepositoryPtr m_repository;
    formalism::OverlayRepositoryPtr<formalism::planning::Repository> m_overlay_repository;
    View<Index<formalism::planning::Task>, formalism::OverlayRepository<formalism::planning::Repository>> m_task;
    std::shared_ptr<formalism::planning::BinaryFDRContext<formalism::OverlayRepository<formalism::planning::Repository>>> m_fdr_context;
    boost::dynamic_bitset<> m_static_atoms_bitset;
    std::vector<float_t> m_static_numeric_variables;

    AxiomEvaluatorProgram m_axiom_program;

    ApplicableActionProgram m_action_program;
    std::vector<analysis::DomainListListList> m_parameter_domains_per_cond_effect_per_action;
};

}

#endif
