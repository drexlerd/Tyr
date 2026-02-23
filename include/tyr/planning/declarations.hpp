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

#include <concepts>
#include <memory>

namespace tyr::planning
{
class LiftedTask;
using LiftedTaskPtr = std::shared_ptr<LiftedTask>;
class GroundTask;
using GroundTaskPtr = std::shared_ptr<GroundTask>;
class Domain;
using DomainPtr = std::shared_ptr<Domain>;

template<typename Task>
class Node;
template<typename Task>
struct LabeledNode;
template<typename Task>
class PackedState;
template<typename Task>
class UnpackedState;
template<typename Task>
class State;

template<typename Task>
struct StateContext;

template<typename Task>
class SuccessorGenerator;
template<typename Task>
class StateRepository;
template<typename Task>
class AxiomEvaluator;

template<typename Task>
class Heuristic;
template<typename Task>
using HeuristicPtr = std::shared_ptr<Heuristic<Task>>;

template<typename Task>
class PruningStrategy;
template<typename Task>
using PruningStrategyPtr = std::shared_ptr<PruningStrategy<Task>>;

template<typename Task>
class GoalStrategy;
template<typename Task>
using GoalStrategyPtr = std::shared_ptr<GoalStrategy<Task>>;

template<typename Task>
class Plan;

class Statistics;

namespace astar_eager
{
template<typename Task>
class EventHandler;
template<typename Task>
using EventHandlerPtr = std::shared_ptr<EventHandler<Task>>;
template<typename Task>
class DefaultEventHandler;
template<typename Task>
using DefaultEventHandlerPtr = std::shared_ptr<DefaultEventHandler<Task>>;
}

namespace gbfs_lazy
{
template<typename Task>
class EventHandler;
template<typename Task>
using EventHandlerPtr = std::shared_ptr<EventHandler<Task>>;
template<typename Task>
class DefaultEventHandler;
template<typename Task>
using DefaultEventHandlerPtr = std::shared_ptr<DefaultEventHandler<Task>>;
}
}

#endif
