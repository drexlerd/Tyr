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

#ifndef TYR_PLANNING_ACTION_EXECUTOR_HPP_
#define TYR_PLANNING_ACTION_EXECUTOR_HPP_

#include "tyr/common/declarations.hpp"
#include "tyr/common/types.hpp"
#include "tyr/formalism/declarations.hpp"
#include "tyr/formalism/planning/fdr_fact_data.hpp"
#include "tyr/planning/declarations.hpp"

namespace tyr::planning
{

class ActionExecutor
{
public:
    ActionExecutor() = default;

    template<typename Task>
    bool is_applicable(View<Index<formalism::GroundAction>, formalism::OverlayRepository<formalism::Repository>> action, const StateContext<Task>& state);

    template<typename Task>
    Node<Task> apply_action(const StateContext<Task>& state_context,
                            View<Index<formalism::GroundAction>, formalism::OverlayRepository<formalism::Repository>> action,
                            StateRepository<Task>& state_repository);

private:
    DataList<formalism::FDRFact<formalism::FluentTag>> m_del_effects;
    DataList<formalism::FDRFact<formalism::FluentTag>> m_add_effects;
    formalism::EffectFamilyList m_effect_families;
};
}

#endif
