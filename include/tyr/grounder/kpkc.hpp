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

#ifndef TYR_GROUNDER_KPKC_HPP_
#define TYR_GROUNDER_KPKC_HPP_

#include <boost/dynamic_bitset.hpp>
#include <cstddef>
#include <cstdint>
#include <span>
#include <tyr/common/config.hpp>
#include <vector>

namespace tyr::grounder::kpkc
{
/**
 * V = num vertices
 * K = num partitions
 */

struct DynamicBitsetView
{
    boost::dynamic_bitset<>* data;
    size_t begin;
    size_t end;

    DynamicBitsetView(boost::dynamic_bitset<>& data) : data(&data), begin(0), end(data.size()) {}

    DynamicBitsetView(boost::dynamic_bitset<>& data, size_t begin, size_t end) : data(&data), begin(begin), end(end) { assert(begin <= end); }

    DynamicBitsetView(DynamicBitsetView parent, size_t relative_begin, size_t relative_end) : data(parent.data)
    {
        // Check bounds against parent's size
        assert(relative_begin <= relative_end);
        assert(parent.begin + relative_end <= parent.end);
        assert(data == parent.data);

        begin = parent.begin + relative_begin;
        end = parent.begin + relative_end;
    }

    DynamicBitsetView slice(size_t i, size_t d)
    {
        assert(d > 0);
        return DynamicBitsetView(*data, begin + d * i, begin + d * (i + 1));
    }

    decltype(auto) test(size_t bit) const
    {
        assert(bit < size());
        return data->test(begin + bit);
    }

    void set(size_t bit)
    {
        assert(bit < size());
        data->set(begin + bit);
    }

    size_t size() const noexcept { return end - begin; }
};

struct DynamicBitsetConstView
{
    const boost::dynamic_bitset<>* data;
    size_t begin;
    size_t end;

    DynamicBitsetConstView(const boost::dynamic_bitset<>& data) : data(&data), begin(0), end(data.size()) {}

    DynamicBitsetConstView(const boost::dynamic_bitset<>& data, size_t begin, size_t end) : data(&data), begin(begin), end(end) { assert(begin <= end); }

    DynamicBitsetConstView slice(size_t i, size_t d)
    {
        assert(d > 0);
        return DynamicBitsetConstView(*data, begin + d * i, begin + d * (i + 1));
    }

    decltype(auto) test(size_t bit) const
    {
        assert(bit < size());
        return data->test(begin + bit);
    }

    size_t size() const noexcept { return end - begin; }
};

/// @brief `DenseConsistencyGraph` is a dense representation of the consistency graph for a rule and a set of facts.
struct DenseConsistencyGraph
{
    boost::dynamic_bitset<> adjacency_matrix;  ///< Dimensions V x V
    std::vector<uint_t> vertex_partitioning;   ///< Dimensions V
    std::vector<uint_t> partition_offsets;     ///< Dimensions K + 1
    size_t num_vertices;
    size_t k;

    explicit DenseConsistencyGraph(size_t num_vertices, size_t k) :
        adjacency_matrix(num_vertices * num_vertices, false),
        vertex_partitioning(k),
        partition_offsets(k + 1),
        num_vertices(num_vertices),
        k(k)
    {
    }

    DynamicBitsetConstView get_adjacency_matrix() const { return DynamicBitsetConstView(adjacency_matrix); }
    std::span<const uint_t> get_vertex_partition(size_t vertex) const
    {
        return std::span<const uint_t>(vertex_partitioning.begin() + partition_offsets[vertex], vertex_partitioning.begin() + partition_offsets[vertex + 1]);
    }
    size_t get_num_vertices() const { return num_vertices; }
    size_t get_k() const { return k; }
};

/// @brief `Workspace` is preallocated memory for a rule.
struct Workspace
{
    boost::dynamic_bitset<> compatible_vertices;  ///< Dimensions K x K x K
    boost::dynamic_bitset<> partition_bits;       ///< Dimensions K
    std::vector<uint_t> partial_solution;         ///< Dimensions K
    size_t k;

    DynamicBitsetView get_compatible_vertices() { return DynamicBitsetView(compatible_vertices); }
    DynamicBitsetView get_partition_bits() { return DynamicBitsetView(partition_bits); }
    std::vector<uint_t>& get_partial_solution() { return partial_solution; }
    size_t get_k() const { return k; }
};

template<typename Callback>
void create_k_clique_in_k_partite_graph_generator(const DenseConsistencyGraph& graph, Workspace& workspace, Callback&& callback);
}

#endif