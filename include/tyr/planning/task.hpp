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

#ifndef TYR_PLANNING_TASK_HPP_
#define TYR_PLANNING_TASK_HPP_

#include <concepts>
#include <memory>

namespace tyr::planning
{
struct GroundTag
{
};
struct LiftedTag
{
};

template<typename Tag>
concept TaskKind = std::same_as<Tag, GroundTag> || std::same_as<Tag, LiftedTag>;

template<TaskKind Kind>
class Task;

using LiftedTask = Task<LiftedTag>;
using LiftedTaskPtr = std::shared_ptr<LiftedTask>;
using GroundTask = Task<GroundTag>;
using GroundTaskPtr = std::shared_ptr<GroundTask>;

}

#endif
