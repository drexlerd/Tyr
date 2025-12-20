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

#ifndef TYR_PLANNING_GROUND_TASK_MATCH_TREE_NODES_FACT_DATA_HPP_
#define TYR_PLANNING_GROUND_TASK_MATCH_TREE_NODES_FACT_DATA_HPP_

#include "tyr/common/types.hpp"
#include "tyr/common/types_utils.hpp"
#include "tyr/formalism/planning/fdr_fact_data.hpp"
#include "tyr/planning/ground_task/match_tree/declarations.hpp"
#include "tyr/planning/ground_task/match_tree/nodes/fact_index.hpp"
#include "tyr/planning/ground_task/match_tree/nodes/node_data.hpp"

namespace tyr
{
template<typename Tag>
struct Data<planning::match_tree::FactSelectorNode<Tag>>
{
    Index<planning::match_tree::FactSelectorNode<Tag>> index;
    Data<formalism::FDRFact<formalism::FluentTag>> fact;
    DataList<planning::match_tree::Node<Tag>> children;
    ::cista::optional<Data<planning::match_tree::Node<Tag>>> dontcare_child;

    Data() = default;
    Data(Index<planning::match_tree::FactSelectorNode<Tag>> index,
         Data<formalism::FDRFact<formalism::FluentTag>> fact,
         DataList<planning::match_tree::Node<Tag>> children,
         ::cista::optional<Data<planning::match_tree::Node<Tag>>> dontcare_child) :
        index(index),
        fact(fact),
        children(std::move(children)),
        dontcare_child(dontcare_child)
    {
    }
    Data(const Data& other) = delete;
    Data& operator=(const Data& other) = delete;
    Data(Data&& other) = default;
    Data& operator=(Data&& other) = default;

    void clear() noexcept
    {
        tyr::clear(index);
        tyr::clear(fact);
        tyr::clear(children);
        tyr::clear(dontcare_child);
    }

    auto cista_members() const noexcept { return std::tie(index, fact, children, dontcare_child); }
    auto identifying_members() const noexcept { return std::tie(fact, children, dontcare_child); }
};
}

#endif
