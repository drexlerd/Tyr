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

#ifndef TYR_PLANNING_FORMATTER_HPP_
#define TYR_PLANNING_FORMATTER_HPP_

#include "tyr/common/formatter.hpp"
#include "tyr/planning/declarations.hpp"
#include "tyr/planning/ground_task.hpp"
#include "tyr/planning/ground_task/node.hpp"
#include "tyr/planning/ground_task/state_data.hpp"
#include "tyr/planning/ground_task/state_view.hpp"
#include "tyr/planning/ground_task/unpacked_state.hpp"
#include "tyr/planning/lifted_task/node.hpp"
#include "tyr/planning/lifted_task/state_data.hpp"
#include "tyr/planning/lifted_task/state_view.hpp"
#include "tyr/planning/lifted_task/unpacked_state.hpp"
#include "tyr/planning/plan.hpp"

#include <ostream>

namespace tyr
{

extern std::ostream& print(std::ostream& os, const planning::LiftedTask& el);

extern std::ostream& print(std::ostream& os, const planning::GroundTask& el);

extern std::ostream& print(std::ostream& os, const Data<planning::State<planning::LiftedTag>>& el);

extern std::ostream& print(std::ostream& os, const planning::UnpackedState<planning::LiftedTag>& el);

extern std::ostream& print(std::ostream& os, const Data<planning::State<planning::GroundTag>>& el);

extern std::ostream& print(std::ostream& os, const planning::UnpackedState<planning::GroundTag>& el);

extern std::ostream& print(std::ostream& os, const planning::Statistics& el);

template<planning::TaskKind Kind>
std::ostream& print(std::ostream& os, const planning::StateView<Kind>& el);

template<planning::TaskKind Kind>
std::ostream& print(std::ostream& os, const planning::Node<Kind>& el);

template<planning::TaskKind Kind>
std::ostream& print(std::ostream& os, const planning::Plan<Kind>& el);

namespace planning
{

extern std::ostream& operator<<(std::ostream& os, const LiftedTask& el);

extern std::ostream& operator<<(std::ostream& os, const GroundTask& el);

extern std::ostream& operator<<(std::ostream& os, const Data<State<LiftedTag>>& el);

extern std::ostream& operator<<(std::ostream& os, const UnpackedState<LiftedTag>& el);

extern std::ostream& operator<<(std::ostream& os, const Data<State<GroundTag>>& el);

extern std::ostream& operator<<(std::ostream& os, const UnpackedState<GroundTag>& el);

extern std::ostream& operator<<(std::ostream& os, const Statistics& el);

template<TaskKind Kind>
std::ostream& operator<<(std::ostream& os, const StateView<Kind>& el);

template<TaskKind Kind>
std::ostream& operator<<(std::ostream& os, const Node<Kind>& el);

template<TaskKind Kind>
std::ostream& operator<<(std::ostream& os, const Plan<Kind>& el);

}
}

#endif
