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

#include "tyr/common/comparators.hpp"     // for operat...
#include "tyr/common/dynamic_bitset.hpp"  // for set
#include "tyr/common/vector.hpp"          // for View, set
#include "tyr/formalism/planning/formatter.hpp"
#include "tyr/formalism/planning/views.hpp"  // for View
#include "tyr/planning/lifted_task/task_grounder.hpp"
#include "tyr/planning/task_utils.hpp"

#include <tuple>
#include <utility>

namespace f = tyr::formalism;
namespace fp = tyr::formalism::planning;

namespace tyr::planning
{
namespace
{
std::vector<analysis::DomainListListList> compute_parameter_domains_per_cond_effect_per_action(fp::TaskView task)
{
    auto result = std::vector<analysis::DomainListListList> {};

    const auto variable_domains = analysis::compute_variable_domains(task);

    for (uint_t action_index = 0; action_index < task.get_domain().get_actions().size(); ++action_index)
    {
        const auto action = task.get_domain().get_actions()[action_index];

        auto parameter_domains_per_cond_effect = analysis::DomainListListList {};

        for (uint_t cond_effect_index = 0; cond_effect_index < action.get_effects().size(); ++cond_effect_index)
        {
            const auto cond_effect = action.get_effects()[cond_effect_index];

            assert(variable_domains.action_domains[action_index].second[cond_effect_index].size() == action.get_arity() + cond_effect.get_arity());

            auto parameter_domains = analysis::DomainListList {};

            for (uint_t i = action.get_arity(); i < action.get_arity() + cond_effect.get_arity(); ++i)
                parameter_domains.push_back(variable_domains.action_domains[action_index].second[cond_effect_index][i]);

            parameter_domains_per_cond_effect.push_back(std::move(parameter_domains));
        }

        result.push_back(std::move(parameter_domains_per_cond_effect));
    }

    return result;
}
}

Task<LiftedTag>::Task(formalism::planning::PlanningTask task) :
    m_task(std::move(task)),
    m_static_atoms_bitset(),
    m_static_numeric_variables(),
    m_axiom_program(get_task()),
    m_action_program(get_task()),
    m_parameter_domains_per_cond_effect_per_action(compute_parameter_domains_per_cond_effect_per_action(get_task())),
    m_rpg_program(get_task())
{
    for (const auto atom : get_task().template get_atoms<f::StaticTag>())
        set(uint_t(atom.get_index()), true, m_static_atoms_bitset);

    for (const auto fterm_value : get_task().template get_fterm_values<f::StaticTag>())
        set(uint_t(fterm_value.get_fterm().get_index()), fterm_value.get_value(), m_static_numeric_variables, std::numeric_limits<float_t>::quiet_NaN());
}

std::shared_ptr<LiftedTask> LiftedTask::create(formalism::planning::PlanningTask task) { return std::make_shared<LiftedTask>(std::move(task)); }

GroundTaskPtr LiftedTask::instantiate_ground_task(ExecutionContext& execution_context) { return ground_task(*this, execution_context); }

}
