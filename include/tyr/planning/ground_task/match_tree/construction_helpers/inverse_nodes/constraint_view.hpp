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

#ifndef TYR_PLANNING_GROUND_TASK_MATCH_TREE_CONSTRUCTION_HELPERS_INVERSE_NODES_CONSTRAINT_VIEW_HPP_
#define TYR_PLANNING_GROUND_TASK_MATCH_TREE_CONSTRUCTION_HELPERS_INVERSE_NODES_CONSTRAINT_VIEW_HPP_

#include "tyr/common/types.hpp"
#include "tyr/common/types_utils.hpp"
#include "tyr/formalism/boolean_operator_view.hpp"
#include "tyr/planning/ground_task/match_tree/construction_helpers/inverse_nodes/constraint_data.hpp"
#include "tyr/planning/ground_task/match_tree/construction_helpers/inverse_nodes/constraint_index.hpp"
#include "tyr/planning/ground_task/match_tree/construction_helpers/inverse_nodes/node_view.hpp"
#include "tyr/planning/ground_task/match_tree/declarations.hpp"

namespace tyr
{
template<typename Tag, planning::match_tree::Context C>
class View<Index<planning::match_tree::InverseNumericConstraintSelectorNode<Tag>>, C>
{
private:
    const C* m_context;
    Index<planning::match_tree::InverseNumericConstraintSelectorNode<Tag>> m_handle;

public:
    View(Index<planning::match_tree::InverseNumericConstraintSelectorNode<Tag>> handle, const C& context) noexcept : m_context(&context), m_handle(handle) {}

    const auto& get_data() const noexcept { return get_repository(*m_context)[m_handle]; }
    const auto& get_context() const noexcept { return *m_context; }
    const auto& get_handle() const noexcept { return m_handle; }

    auto get_constraint() const noexcept { return make_view(get_data().constraint, m_context->get_formalism_repository()); }
    auto get_true_child() const noexcept { return make_view(get_data().true_child, *m_context); }
    auto get_false_child() const noexcept { return make_view(get_data().false_child, *m_context); }
    auto get_dontcare_child() const noexcept { return make_view(get_data().dontcare_child, *m_context); }

    auto identifying_members() const noexcept { return std::tie(m_context, m_handle); }
};
}

#endif
