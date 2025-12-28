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

#include "tyr/planning/lifted_task.hpp"

#include "task_utils.hpp"
#include "tyr/common/comparators.hpp"            // for operat...
#include "tyr/common/dynamic_bitset.hpp"         // for set
#include "tyr/common/vector.hpp"                 // for View, set
#include "tyr/formalism/overlay_repository.hpp"  // for Overla...
#include "tyr/formalism/views.hpp"               // for View
#include "tyr/planning/lifted_task/task_grounder.hpp"

#include <tuple>
#include <utility>

using namespace tyr::formalism;

namespace tyr::planning
{

LiftedTask::LiftedTask(DomainPtr domain,
                       RepositoryPtr repository,
                       OverlayRepositoryPtr<Repository> overlay_repository,
                       View<Index<Task>, OverlayRepository<Repository>> task,
                       std::shared_ptr<BinaryFDRContext<OverlayRepository<Repository>>> fdr_context) :
    m_domain(std::move(domain)),
    m_repository(std::move(repository)),
    m_overlay_repository(std::move(overlay_repository)),
    m_task(task),
    m_fdr_context(fdr_context),
    m_static_atoms_bitset(),
    m_static_numeric_variables(),
    m_axiom_program(m_task),
    m_action_program(m_task),
    m_parameter_domains_per_cond_effect_per_action(compute_parameter_domains_per_cond_effect_per_action(m_task))
{
    for (const auto atom : m_task.template get_atoms<StaticTag>())
        set(uint_t(atom.get_index()), true, m_static_atoms_bitset);

    for (const auto fterm_value : m_task.template get_fterm_values<StaticTag>())
        set(uint_t(fterm_value.get_fterm().get_index()), fterm_value.get_value(), m_static_numeric_variables, std::numeric_limits<float_t>::quiet_NaN());
}

GroundTaskPtr LiftedTask::get_ground_task() { return ground_task(*this); }

}
