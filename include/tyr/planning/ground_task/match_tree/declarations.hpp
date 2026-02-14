/*
 * Copyright (C) 2023 Dominik Drexler and Simon Stahlberg
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

#ifndef TYR_PLANNING_GROUND_TASK_MATCH_TREE_DECLARATIONS_HPP_
#define TYR_PLANNING_GROUND_TASK_MATCH_TREE_DECLARATIONS_HPP_

#include "tyr/formalism/planning/declarations.hpp"

#include <concepts>
#include <variant>
#include <vector>

namespace tyr::planning::match_tree
{
/**
 * Nodes
 */

template<typename Tag>
struct AtomSelectorNode
{
};

template<typename Tag>
struct VariableSelectorNode
{
};

template<typename Tag>
struct NumericConstraintSelectorNode
{
};

template<typename Tag>
struct ElementGeneratorNode
{
};

template<typename Tag>
struct Node
{
};

/**
 * MatchTree
 */

template<typename Tag>
class MatchTree;
template<typename Tag>
using MatchTreePtr = std::unique_ptr<MatchTree<Tag>>;

/**
 * Aliases
 */

template<typename Tag>
class Repository;
template<typename Tag>
using RepositoryPtr = std::unique_ptr<Repository<Tag>>;

template<typename Repo, typename Tag>
concept RepositoryAccess = requires(const Repo& r, Index<Tag> idx) {
    { r[idx] } -> std::same_as<const Data<Tag>&>;
};

template<typename T>
concept HasFormalismRepository = requires(const T& r) {
    { r.get_formalism_repository() } -> formalism::planning::Context;
};

template<typename T>
concept RepositoryConcept =
    HasFormalismRepository<T> && RepositoryAccess<T, AtomSelectorNode<formalism::planning::GroundAction>>
    && RepositoryAccess<T, VariableSelectorNode<formalism::planning::GroundAction>>
    && RepositoryAccess<T, NumericConstraintSelectorNode<formalism::planning::GroundAction>>
    && RepositoryAccess<T, ElementGeneratorNode<formalism::planning::GroundAction>> && RepositoryAccess<T, AtomSelectorNode<formalism::planning::GroundAxiom>>
    && RepositoryAccess<T, VariableSelectorNode<formalism::planning::GroundAxiom>>
    && RepositoryAccess<T, NumericConstraintSelectorNode<formalism::planning::GroundAxiom>>
    && RepositoryAccess<T, ElementGeneratorNode<formalism::planning::GroundAxiom>>;

/// @brief Make Repository a trivial context.
/// @param context
/// @return
template<typename Tag>
inline const Repository<Tag>& get_repository(const Repository<Tag>& context) noexcept
{
    return context;
}

template<typename T>
concept Context = requires(const T& a) {
    { get_repository(a) } -> RepositoryConcept;
};

}

#endif
