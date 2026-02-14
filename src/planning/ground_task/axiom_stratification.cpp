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

#include "tyr/planning/ground_task/axiom_stratification.hpp"

#include "tyr/common/equal_to.hpp"
#include "tyr/common/hash.hpp"
#include "tyr/formalism/planning/formatter.hpp"
#include "tyr/formalism/planning/repository.hpp"
#include "tyr/formalism/planning/views.hpp"

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/strong_components.hpp>
#include <boost/graph/topological_sort.hpp>

namespace f = tyr::formalism;
namespace fp = tyr::formalism::planning;

namespace tyr::planning
{

namespace details
{
enum class EdgeKind : uint8_t
{
    NonStrict = 0,
    Strict = 1
};

struct EdgeProps
{
    EdgeKind kind = EdgeKind::NonStrict;
};

// Graph of derived atom dependencies
using DepGraph = boost::adjacency_list<boost::vecS, boost::vecS, boost::directedS, boost::no_property, EdgeProps>;

using DepV = boost::graph_traits<DepGraph>::vertex_descriptor;
using DepE = boost::graph_traits<DepGraph>::edge_descriptor;

// Directed acyclic graph of strongly connected components of the dependency graph
using Dag = boost::adjacency_list<boost::vecS, boost::vecS, boost::directedS, boost::no_property, EdgeProps>;

using DagV = boost::graph_traits<Dag>::vertex_descriptor;
using DagE = boost::graph_traits<Dag>::edge_descriptor;

// Build dependency graph: nodes = derived ground atoms
static DepGraph build_dependency_graph(View<Index<fp::FDRTask>, fp::Repository> task, size_t num_atoms)
{
    DepGraph graph(num_atoms);

    for (const auto axiom : task.get_ground_axioms())
    {
        const auto h_atom = axiom.get_head().get_index();

        for (const auto literal : axiom.get_body().get_facts<f::DerivedTag>())
        {
            const auto b_atom = literal.get_atom().get_index();

            EdgeProps ep;
            ep.kind = literal.get_polarity() ? EdgeKind::NonStrict : EdgeKind::Strict;
            boost::add_edge(b_atom.value, h_atom.value, ep, graph);
        }
    }

    return graph;
}

// Run SCCs, check stratifiability, return component id per vertex and #components
static std::pair<std::vector<uint_t>, uint_t> compute_scc_and_check(const DepGraph& g)
{
    auto comp = std::vector<uint_t>(boost::num_vertices(g), std::numeric_limits<uint_t>::max());
    const auto num = boost::strong_components(g, boost::make_iterator_property_map(comp.begin(), boost::get(boost::vertex_index, g)));

    // Stratifiable iff no STRICT edge stays within the same SCC
    for (auto e_it = boost::edges(g); e_it.first != e_it.second; ++e_it.first)
    {
        DepE e = *e_it.first;
        if (g[e].kind != EdgeKind::Strict)
            continue;

        const auto u = boost::source(e, g);
        const auto v = boost::target(e, g);
        if (comp[u] == comp[v])
            throw std::runtime_error("Set of ground axioms is not stratifiable (negative cycle within SCC).");
    }

    return { comp, num };
}

// Build condensed DAG of SCCs, with edge weight inc=1 if any strict edge exists between SCCs
static Dag build_condensation_dag(const DepGraph& g, const std::vector<uint_t>& comp, uint_t num_comps)
{
    Dag dag(num_comps);

    auto best = UnorderedMap<std::pair<uint_t, uint_t>, EdgeKind> {};
    best.reserve(boost::num_edges(g));

    for (auto e_it = boost::edges(g); e_it.first != e_it.second; ++e_it.first)
    {
        const auto e = *e_it.first;
        const auto u = boost::source(e, g);
        const auto v = boost::target(e, g);

        const auto cu = comp[u];
        const auto cv = comp[v];
        if (cu == cv)
            continue;

        const auto k = std::make_pair(cu, cv);
        auto& slot = best[k];
        if (slot != EdgeKind::Strict && g[e].kind == EdgeKind::Strict)
            slot = EdgeKind::Strict;
    }

    for (const auto& [k, inc] : best)
    {
        const auto cu = k.first;
        const auto cv = k.second;
        EdgeProps ep;
        ep.kind = inc;
        boost::add_edge(cu, cv, ep, dag);
    }

    return dag;
}

// Compute minimal strata on SCC DAG: s[dst] = max(s[dst], s[src] + inc)
static std::vector<uint_t> compute_component_strata(const Dag& dag)
{
    auto topo = std::vector<DagV> {};
    topo.reserve(boost::num_vertices(dag));
    boost::topological_sort(dag, std::back_inserter(topo));  // reverse topological order

    std::reverse(topo.begin(), topo.end());

    auto s = std::vector<uint_t>(boost::num_vertices(dag), 0);

    for (const auto u : topo)
    {
        for (auto oe = boost::out_edges(u, dag); oe.first != oe.second; ++oe.first)
        {
            const auto e = *oe.first;
            const auto v = boost::target(e, dag);
            s[v] = std::max(s[v], s[u] + (dag[e].kind == EdgeKind::Strict ? 1u : 0u));
        }
    }
    return s;
}
}

GroundAxiomStrata compute_ground_axiom_stratification(View<Index<fp::FDRTask>, fp::Repository> task)
{
    const auto num_atoms = task.get_atoms<f::DerivedTag>().size();

    // 1) dependency graph
    const auto dep = details::build_dependency_graph(task, num_atoms);

    // 2) SCC + check
    const auto [comp, num_comps] = details::compute_scc_and_check(dep);

    // 3) Condensed DAG
    const auto dag = details::build_condensation_dag(dep, comp, num_comps);

    // 4) Component strata
    const auto comp_stratum = details::compute_component_strata(dag);

    // 5) Assign each derived atom a stratum
    auto atom_stratum = std::vector<uint_t>(num_atoms, 0);
    for (uint_t i = 0; i < num_atoms; ++i)
        atom_stratum[i] = comp_stratum[comp[i]];

    // 6) Bucket axioms by head stratum
    auto max_s = uint_t(0);
    for (auto s : comp_stratum)
        max_s = std::max(max_s, s);

    auto buckets = std::vector<IndexList<fp::GroundAxiom>>(max_s + 1);
    for (const auto axiom : task.get_ground_axioms())
        buckets[atom_stratum[uint_t(axiom.get_head().get_index())]].push_back(axiom.get_index());

    auto out = GroundAxiomStrata {};
    out.data.reserve(buckets.size());
    for (auto& b : buckets)
        out.data.emplace_back(GroundAxiomStratum(std::move(b)));

    return out;
}
}
