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

#ifndef TYR_GROUNDER_KPKC_UTILS_HPP_
#define TYR_GROUNDER_KPKC_UTILS_HPP_

#include "tyr/grounder/declarations.hpp"
#include "tyr/grounder/kpkc_data.hpp"

namespace tyr::grounder::kpkc
{

/// @brief Helper to allocate a DenseKPartiteGraph from a given StaticConsistencyGraph.
template<formalism::Context C>
DenseKPartiteGraph allocate_dense_graph(const StaticConsistencyGraph<C>& sparse_graph);

/// @brief Helper to allocate a Workspace from a given StaticConsistencyGraph.
template<formalism::Context C>
Workspace allocate_workspace(const StaticConsistencyGraph<C>& sparse_graph);

template<formalism::Context C>
void initialize_dense_graph_and_workspace(const StaticConsistencyGraph<C>& sparse_graph,
                                          const AssignmentSets<C>& assignment_sets,
                                          DenseKPartiteGraph& ref_graph,
                                          Workspace& ref_workspace);

}

#endif