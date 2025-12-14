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

#ifndef TYR_FORMALISM_PLANNING_GROUND_CONDITIONAL_EFFECT_VIEW_HPP_
#define TYR_FORMALISM_PLANNING_GROUND_CONDITIONAL_EFFECT_VIEW_HPP_

#include "tyr/common/types.hpp"
#include "tyr/formalism/declarations.hpp"
#include "tyr/formalism/planning/ground_conditional_effect_index.hpp"
#include "tyr/formalism/planning/ground_conjunctive_effect_view.hpp"
#include "tyr/formalism/planning/ground_fdr_conjunctive_condition_view.hpp"

namespace tyr
{

template<formalism::Context C>
class View<Index<formalism::GroundConditionalEffect>, C>
{
private:
    const C* m_context;
    Index<formalism::GroundConditionalEffect> m_handle;

public:
    using Tag = formalism::GroundConditionalEffect;

    View(Index<formalism::GroundConditionalEffect> handle, const C& context) : m_context(&context), m_handle(handle) {}

    const auto& get_data() const { return get_repository(*m_context)[m_handle]; }
    const auto& get_context() const noexcept { return *m_context; }
    const auto& get_handle() const noexcept { return m_handle; }

    auto get_index() const noexcept { return m_handle; }
    auto get_condition() const noexcept { return make_view(get_data().condition, *m_context); }
    auto get_effect() const noexcept { return make_view(get_data().effect, *m_context); }

    auto identifying_members() const noexcept { return std::tie(m_context, m_handle); }
};
}

#endif
