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

#include "tyr/planning/algorithms/gbfs_lazy.hpp"

#include "tyr/common/segmented_vector.hpp"
#include "tyr/planning/algorithms/openlists/alternating.hpp"
#include "tyr/planning/algorithms/utils.hpp"
#include "tyr/planning/search_node.hpp"
#include "tyr/planning/state_index.hpp"

namespace tyr::planning::gbfs_lazy
{

/**
 * GBFS search node
 */

struct SearchNode
{
    float_t g_value;
    StateIndex parent_state;
    SearchNodeStatus status;
    bool preferred;
    bool compatible;
};

static_assert(sizeof(SearchNode) == 16);

using SearchNodeVector = SegmentedVector<SearchNode>;

static SearchNode& get_or_create_search_node(size_t state_index, SearchNodeVector& search_nodes)
{
    static auto default_node = SearchNode { std::numeric_limits<float_t>::infinity(), StateIndex::max(), SearchNodeStatus::NEW, false, false };

    while (state_index >= search_nodes.size())
    {
        search_nodes.push_back(default_node);
    }
    return search_nodes[state_index];
}

/**
 * GBFS queue
 */

struct GreedyQueueEntry
{
    using KeyType = std::tuple<uint_t, SearchNodeStatus>;
    using ItemType = StateIndex;

    StateIndex packed_state;
    uint_t step;
    SearchNodeStatus status;

    KeyType get_key() const { return std::make_tuple(step, status); }
    ItemType get_item() const { return packed_state; }
};

static_assert(sizeof(GreedyQueueEntry) == 12);

struct ExhaustiveQueueEntry
{
    using KeyType = std::tuple<float_t, float_t, uint_t, SearchNodeStatus>;
    using ItemType = StateIndex;

    float_t g_value;
    float_t h_value;
    StateIndex packed_state;
    uint_t step;
    SearchNodeStatus status;

    KeyType get_key() const { return std::make_tuple(h_value, g_value, step, status); }
    ItemType get_item() const { return packed_state; }
};

static_assert(sizeof(ExhaustiveQueueEntry) == 32);

using GreedyQueue = PriorityQueue<GreedyQueueEntry>;
using ExhaustiveQueue = PriorityQueue<ExhaustiveQueueEntry>;

template<typename Task>
SearchResult<Task> find_solution(const std::shared_ptr<Task>& task, const std::shared_ptr<Heuristic<Task>>& heuristic, const Options<Task>& options)
{
}

}
