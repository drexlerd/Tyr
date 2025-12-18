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

#include "tyr/formalism/declarations.hpp"

#include <concepts>
#include <variant>
#include <vector>

namespace tyr::planning::match_tree
{
/**
 * Forward declarations
 */

template<typename Tag>
struct PlaceholderNode
{
};

/**
 * InverseNode
 */

template<typename Tag>
struct InverseAtomSelectorNode
{
};

template<typename Tag>
struct InverseFactSelectorNode
{
};

template<typename Tag>
struct InverseNumericConstraintSelectorNode
{
};

template<typename Tag>
struct InverseElementGeneratorNode
{
};

template<typename Tag>
struct InverseNode
{
};

/**
 * Node
 */

template<typename Tag>
struct AtomSelectorNode
{
};

template<typename Tag>
struct FactSelectorNode
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
 * NodeScoreFunction
 */

template<typename Tag>
class INodeScoreFunction;

/**
 * NodeSplitter
 */

template<typename Tag>
class INodeSplitter;

struct Options;
struct Statistics;

template<typename Tag>
class MatchTree;
template<typename Tag>
using MatchTreePtr = std::unique_ptr<MatchTree<Tag>>;

/**
 * Aliases
 */

template<typename Tag>
using PlaceholderNodeList = std::vector<PlaceholderNode<Tag>>;

template<formalism::Context C, typename Tag>
class Repository;
template<formalism::Context C, typename Tag>
using RepositoryPtr = std::unique_ptr<Repository<C, Tag>>;

template<typename Repo, typename Tag>
concept RepositoryAccess = requires(const Repo& r, Index<Tag> idx) {
    { r[idx] } -> std::same_as<const Data<Tag>&>;
};

template<typename T>
concept HasFormalismRepository = requires(const T& r) {
    { r.get_formalism_repository() } -> formalism::Context;
};

template<typename T>
concept RepositoryConcept =
    HasFormalismRepository<T> && RepositoryAccess<T, PlaceholderNode<formalism::GroundAction>>
    && RepositoryAccess<T, InverseAtomSelectorNode<formalism::GroundAction>> && RepositoryAccess<T, InverseFactSelectorNode<formalism::GroundAction>>
    && RepositoryAccess<T, InverseNumericConstraintSelectorNode<formalism::GroundAction>>
    && RepositoryAccess<T, InverseElementGeneratorNode<formalism::GroundAction>> && RepositoryAccess<T, InverseNode<formalism::GroundAction>>
    && RepositoryAccess<T, AtomSelectorNode<formalism::GroundAction>> && RepositoryAccess<T, FactSelectorNode<formalism::GroundAction>>
    && RepositoryAccess<T, NumericConstraintSelectorNode<formalism::GroundAction>> && RepositoryAccess<T, ElementGeneratorNode<formalism::GroundAction>>
    && RepositoryAccess<T, Node<formalism::GroundAction>> && RepositoryAccess<T, PlaceholderNode<formalism::GroundAxiom>>
    && RepositoryAccess<T, InverseAtomSelectorNode<formalism::GroundAxiom>> && RepositoryAccess<T, InverseFactSelectorNode<formalism::GroundAxiom>>
    && RepositoryAccess<T, InverseNumericConstraintSelectorNode<formalism::GroundAxiom>>
    && RepositoryAccess<T, InverseElementGeneratorNode<formalism::GroundAxiom>> && RepositoryAccess<T, InverseNode<formalism::GroundAxiom>>
    && RepositoryAccess<T, AtomSelectorNode<formalism::GroundAxiom>> && RepositoryAccess<T, FactSelectorNode<formalism::GroundAxiom>>
    && RepositoryAccess<T, NumericConstraintSelectorNode<formalism::GroundAxiom>> && RepositoryAccess<T, ElementGeneratorNode<formalism::GroundAxiom>>
    && RepositoryAccess<T, Node<formalism::GroundAxiom>>;

/// @brief Make Repository a trivial context.
/// @param context
/// @return
template<typename Tag, formalism::Context C>
inline const Repository<Tag, C>& get_repository(const Repository<Tag, C>& context) noexcept
{
    return context;
}

template<typename T>
concept Context = requires(const T& a) {
    { get_repository(a) } -> RepositoryConcept;
};

}

#endif
