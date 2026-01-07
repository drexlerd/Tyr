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
#include "tyr/planning/ground_task/packed_state.hpp"
#include "tyr/planning/ground_task/state.hpp"
#include "tyr/planning/ground_task/unpacked_state.hpp"
#include "tyr/planning/lifted_task/node.hpp"
#include "tyr/planning/lifted_task/packed_state.hpp"
#include "tyr/planning/lifted_task/state.hpp"
#include "tyr/planning/lifted_task/unpacked_state.hpp"

#include <ostream>

namespace tyr
{

extern std::ostream& print(std::ostream& os, const planning::Domain& el);

extern std::ostream& print(std::ostream& os, const planning::LiftedTask& el);

extern std::ostream& print(std::ostream& os, const planning::GroundTask& el);

extern std::ostream& print(std::ostream& os, const planning::Node<planning::LiftedTask>& el);

extern std::ostream& print(std::ostream& os, const planning::PackedState<planning::LiftedTask>& el);

extern std::ostream& print(std::ostream& os, const planning::UnpackedState<planning::LiftedTask>& el);

extern std::ostream& print(std::ostream& os, const planning::State<planning::LiftedTask>& el);

extern std::ostream& print(std::ostream& os, const planning::Node<planning::GroundTask>& el);

extern std::ostream& print(std::ostream& os, const planning::PackedState<planning::GroundTask>& el);

extern std::ostream& print(std::ostream& os, const planning::UnpackedState<planning::GroundTask>& el);

extern std::ostream& print(std::ostream& os, const planning::State<planning::GroundTask>& el);

extern std::ostream& print(std::ostream& os, const planning::gbfs_lazy::Statistics& el);

namespace planning
{
extern std::ostream& operator<<(std::ostream& os, const Domain& el);

extern std::ostream& operator<<(std::ostream& os, const LiftedTask& el);

extern std::ostream& operator<<(std::ostream& os, const GroundTask& el);

extern std::ostream& operator<<(std::ostream& os, const Node<LiftedTask>& el);

extern std::ostream& operator<<(std::ostream& os, const PackedState<LiftedTask>& el);

extern std::ostream& operator<<(std::ostream& os, const UnpackedState<LiftedTask>& el);

extern std::ostream& operator<<(std::ostream& os, const State<LiftedTask>& el);

extern std::ostream& operator<<(std::ostream& os, const Node<GroundTask>& el);

extern std::ostream& operator<<(std::ostream& os, const PackedState<GroundTask>& el);

extern std::ostream& operator<<(std::ostream& os, const UnpackedState<GroundTask>& el);

extern std::ostream& operator<<(std::ostream& os, const State<GroundTask>& el);

namespace gbfs_lazy
{
extern std::ostream& operator<<(std::ostream& os, const Statistics& el);
}  // end gbfs_lazy
}
}

#endif
