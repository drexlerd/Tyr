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

#ifndef TYR_PLANNING_DECLARATIONS_HPP_
#define TYR_PLANNING_DECLARATIONS_HPP_

#include "tyr/planning/task.hpp"

#include <concepts>
#include <memory>

namespace tyr::planning
{

template<TaskKind Kind>
class Node;
template<TaskKind Kind>
struct LabeledNode;

template<TaskKind Kind>
class UnpackedState;
template<TaskKind Kind>
class State;

template<TaskKind Kind>
struct StateContext;

template<TaskKind Kind>
class SuccessorGenerator;
template<TaskKind Kind>
class StateRepository;
template<TaskKind Kind>
class AxiomEvaluator;

template<TaskKind Kind>
class Heuristic;
template<TaskKind Kind>
using HeuristicPtr = std::shared_ptr<Heuristic<Kind>>;

template<TaskKind Kind>
class PruningStrategy;
template<TaskKind Kind>
using PruningStrategyPtr = std::shared_ptr<PruningStrategy<Kind>>;

template<TaskKind Kind>
class GoalStrategy;
template<TaskKind Kind>
using GoalStrategyPtr = std::shared_ptr<GoalStrategy<Kind>>;

template<TaskKind Kind>
class Plan;

class Statistics;

namespace astar_eager
{
template<TaskKind Kind>
class EventHandler;
template<TaskKind Kind>
using EventHandlerPtr = std::shared_ptr<EventHandler<Kind>>;
template<TaskKind Kind>
class DefaultEventHandler;
template<TaskKind Kind>
using DefaultEventHandlerPtr = std::shared_ptr<DefaultEventHandler<Kind>>;
}

namespace gbfs_lazy
{
template<TaskKind Kind>
class EventHandler;
template<TaskKind Kind>
using EventHandlerPtr = std::shared_ptr<EventHandler<Kind>>;
template<TaskKind Kind>
class DefaultEventHandler;
template<TaskKind Kind>
using DefaultEventHandlerPtr = std::shared_ptr<DefaultEventHandler<Kind>>;
}
}

#endif
