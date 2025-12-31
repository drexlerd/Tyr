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

#ifndef TYR_DATALOG_KPKC_DATA_HPP_
#define TYR_DATALOG_KPKC_DATA_HPP_

#include <boost/dynamic_bitset.hpp>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <span>
#include <tyr/common/config.hpp>
#include <vector>

namespace tyr::datalog::kpkc
{
/**
 * V = num vertices
 * K = num partitions
 */

/// @brief `DenseKPartiteGraph` is a dense representation of the consistency graph for a rule and a set of facts.
struct DenseKPartiteGraph
{
    std::vector<boost::dynamic_bitset<>> adjacency_matrix;  ///< Dimensions V x V
    std::vector<std::vector<uint_t>> partitions;            ///< Dimensions K x V
    size_t num_vertices;
    size_t k;
};

/// @brief `Workspace` is preallocated memory for a rule.
struct Workspace
{
    std::vector<std::vector<boost::dynamic_bitset<>>> compatible_vertices;  ///< Dimensions K x K x V
    boost::dynamic_bitset<> partition_bits;                                 ///< Dimensions K
    std::vector<uint_t> partial_solution;                                   ///< Dimensions K
    boost::dynamic_bitset<> consistent_vertices;
    std::vector<uint_t> consistent_vertices_vec;
    size_t num_vertices;
    size_t k;
};

}

#endif