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

#ifndef TYR_DATALOG_KPKC_UTILS_HPP_
#define TYR_DATALOG_KPKC_UTILS_HPP_

#include "tyr/datalog/declarations.hpp"
#include "tyr/datalog/kpkc_data.hpp"

namespace tyr::datalog::kpkc
{

/// @brief Helper to allocate a DenseKPartiteGraph from a given StaticConsistencyGraph.
extern DenseKPartiteGraph allocate_dense_graph(const StaticConsistencyGraph& sparse_graph);

/// @brief Helper to allocate a Workspace from a given StaticConsistencyGraph.
extern Workspace allocate_workspace(const StaticConsistencyGraph& sparse_graph);

extern void initialize_dense_graph_and_workspace(const StaticConsistencyGraph& sparse_graph,
                                                 const AssignmentSets& assignment_sets,
                                                 DenseKPartiteGraph& ref_graph,
                                                 Workspace& ref_workspace);

}

#endif