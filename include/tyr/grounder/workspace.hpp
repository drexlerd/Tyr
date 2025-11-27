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

#ifndef TYR_GROUNDER_WORKSPACE_HPP_
#define TYR_GROUNDER_WORKSPACE_HPP_

#include "tyr/formalism/declarations.hpp"
#include "tyr/grounder/consistency_graph.hpp"

namespace tyr::grounder
{

template<formalism::IsContext C>
struct ImmutableRuleWorkspace
{
    // Fact infos
    const FactSets<C>& fact_sets;
    const AssignmentSets& assignment_sets;

    // Rule infos
    const View<Index<formalism::Rule>, C> rule;
    const StaticConsistencyGraph<C> static_consistency_graph;
    const kpkc::DenseKPartiteGraph& consistency_graph;
};

template<formalism::IsContext C>
struct MutableRuleWorkspace
{
    formalism::ScopedRepository<C>& repository;
    kpkc::Workspace& kpkc_workspace;

    IndexList<formalism::Object>& binding;
    formalism::Builder& builder;
    cista::Buffer& buffer;

    IndexList<formalism::GroundRule>& ground_rules;
};

}

#endif
