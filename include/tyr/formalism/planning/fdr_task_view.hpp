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

#ifndef TYR_FORMALISM_PLANNING_FDR_TASK_VIEW_HPP_
#define TYR_FORMALISM_PLANNING_FDR_TASK_VIEW_HPP_

#include "tyr/common/optional.hpp"
#include "tyr/common/types.hpp"
#include "tyr/common/vector.hpp"
#include "tyr/formalism/declarations.hpp"
#include "tyr/formalism/function_view.hpp"
#include "tyr/formalism/ground_function_term_value_view.hpp"
#include "tyr/formalism/object_view.hpp"
#include "tyr/formalism/planning/action_view.hpp"
#include "tyr/formalism/planning/axiom_view.hpp"
#include "tyr/formalism/planning/domain_index.hpp"
#include "tyr/formalism/planning/domain_view.hpp"
#include "tyr/formalism/planning/fdr_fact_view.hpp"
#include "tyr/formalism/planning/fdr_task_index.hpp"
#include "tyr/formalism/planning/fdr_variable_view.hpp"
#include "tyr/formalism/planning/ground_action_view.hpp"
#include "tyr/formalism/planning/ground_axiom_view.hpp"
#include "tyr/formalism/planning/ground_fdr_conjunctive_condition_view.hpp"
#include "tyr/formalism/planning/metric_view.hpp"
#include "tyr/formalism/predicate_view.hpp"

namespace tyr
{

template<formalism::Context C>
class View<Index<formalism::FDRTask>, C>
{
private:
    const C* m_context;
    Index<formalism::FDRTask> m_handle;

public:
    using Tag = formalism::FDRTask;

    View(Index<formalism::FDRTask> handle, const C& context) : m_context(&context), m_handle(handle) {}

    const auto& get_data() const { return get_repository(*m_context)[m_handle]; }
    const auto& get_context() const noexcept { return *m_context; }
    const auto& get_handle() const noexcept { return m_handle; }

    auto get_index() const noexcept { return m_handle; }
    const auto& get_name() const noexcept { return get_data().name; }
    auto get_domain() const noexcept { return make_view(get_data().domain, *m_context); }
    auto get_derived_predicates() const noexcept { return make_view(get_data().derived_predicates, *m_context); }
    auto get_objects() const noexcept { return make_view(get_data().objects, *m_context); }
    template<formalism::FactKind T>
    auto get_atoms() const noexcept
    {
        return make_view(get_data().template get_atoms<T>(), *m_context);
    }
    template<formalism::FactKind T>
    auto get_fterm_values() const noexcept
    {
        return make_view(get_data().template get_fterm_values<T>(), *m_context);
    }
    auto get_auxiliary_fterm_value() const noexcept { return make_view(get_data().auxiliary_fterm_value, *m_context); }
    auto get_goal() const noexcept { return make_view(get_data().goal, *m_context); }
    auto get_metric() const noexcept { return make_view(get_data().metric, *m_context); }
    auto get_axioms() const noexcept { return make_view(get_data().axioms, *m_context); }
    template<formalism::FactKind T>
    auto get_variables() const noexcept
    {
        return make_view(get_data().template get_variables<T>(), *m_context);
    }
    auto get_fluent_facts() const noexcept { return make_view(get_data().fluent_facts, *m_context); }
    auto get_ground_actions() const noexcept { return make_view(get_data().ground_actions, *m_context); }
    auto get_ground_axioms() const noexcept { return make_view(get_data().ground_axioms, *m_context); }

    auto identifying_members() const noexcept { return std::tie(m_context, m_handle); }
};
}

#endif
