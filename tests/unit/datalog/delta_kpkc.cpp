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

#include "tyr/datalog/delta_kpkc.hpp"

#include "tyr/common/formatter.hpp"

#include <gtest/gtest.h>

namespace d = tyr::datalog;

namespace tyr::tests
{

static auto enumerate_cliques(d::DeltaKPKC& kpkc)
{
    auto result = std::vector<std::vector<uint_t>> {};
    kpkc.for_each_new_k_clique([&](auto&& clique) { result.push_back(clique); });
    return result;
}

TEST(TyrTests, TyrDatalogDeltaKPKCStandard3)
{
    const auto nv = uint_t(6);
    const auto k = uint_t(3);

    auto const_graph = d::delta_kpkc::ConstGraph {};
    const_graph.num_vertices = nv;
    const_graph.k = k;
    const_graph.partitions = std::vector<std::vector<uint_t>> { { 0, 1 }, { 2, 3 }, { 4, 5 } };
    const_graph.vertex_to_partition = std::vector<uint_t> { 0, 0, 1, 1, 2, 2 };

    auto delta_graph = d::delta_kpkc::Graph {};
    delta_graph.adjacency_matrix.resize(nv);
    for (auto& bitset : delta_graph.adjacency_matrix)
        bitset.resize(nv, false);
    delta_graph.vertices.resize(nv, true);  // all active
    delta_graph.adjacency_matrix[0].set(2);
    delta_graph.adjacency_matrix[2].set(0);
    delta_graph.adjacency_matrix[0].set(3);
    delta_graph.adjacency_matrix[3].set(0);
    delta_graph.adjacency_matrix[0].set(4);
    delta_graph.adjacency_matrix[4].set(0);
    delta_graph.adjacency_matrix[1].set(5);
    delta_graph.adjacency_matrix[5].set(1);
    delta_graph.adjacency_matrix[2].set(4);
    delta_graph.adjacency_matrix[4].set(2);
    delta_graph.adjacency_matrix[3].set(4);
    delta_graph.adjacency_matrix[4].set(3);

    auto full_graph = d::delta_kpkc::Graph {};
    full_graph.adjacency_matrix.resize(nv);
    for (auto& bitset : full_graph.adjacency_matrix)
        bitset.resize(nv, false);
    full_graph.vertices.resize(nv, true);  // all active
    full_graph.adjacency_matrix[0].set(2);
    full_graph.adjacency_matrix[2].set(0);
    full_graph.adjacency_matrix[0].set(3);
    full_graph.adjacency_matrix[3].set(0);
    full_graph.adjacency_matrix[0].set(4);
    full_graph.adjacency_matrix[4].set(0);
    full_graph.adjacency_matrix[1].set(5);
    full_graph.adjacency_matrix[5].set(1);
    full_graph.adjacency_matrix[2].set(4);
    full_graph.adjacency_matrix[4].set(2);
    full_graph.adjacency_matrix[3].set(4);
    full_graph.adjacency_matrix[4].set(3);

    auto workspace = d::delta_kpkc::Workspace {};
    workspace.compatible_vertices.resize(k);
    for (uint_t i = 0; i < k; ++i)
    {
        workspace.compatible_vertices[i].resize(k);
        for (uint_t j = 0; j < k; ++j)
            workspace.compatible_vertices[i][j].resize(const_graph.partitions[j].size());
    }
    workspace.partition_bits.resize(k);
    workspace.partial_solution.reserve(k);

    auto dkpkc = d::DeltaKPKC(std::move(const_graph), std::move(delta_graph), std::move(full_graph), std::move(workspace));

    auto cliques = enumerate_cliques(dkpkc);

    EXPECT_EQ(cliques.size(), 2);
    EXPECT_EQ(cliques, (std::vector<std::vector<uint_t>> { { 0, 2, 4 }, { 0, 3, 4 } }));
}

TEST(TyrTests, TyrDatalogDeltaKPKCDelta3)
{
    const auto nv = uint_t(6);
    const auto k = uint_t(3);

    auto const_graph = d::delta_kpkc::ConstGraph {};
    const_graph.num_vertices = nv;
    const_graph.k = k;
    const_graph.partitions = std::vector<std::vector<uint_t>> { { 0, 1 }, { 2, 3 }, { 4, 5 } };
    const_graph.vertex_to_partition = std::vector<uint_t> { 0, 0, 1, 1, 2, 2 };

    auto delta_graph = d::delta_kpkc::Graph {};
    delta_graph.adjacency_matrix.resize(nv);
    for (auto& bitset : delta_graph.adjacency_matrix)
        bitset.resize(nv, false);
    delta_graph.vertices.resize(nv, false);
    delta_graph.vertices.set(0);
    delta_graph.vertices.set(2);
    delta_graph.vertices.set(3);
    delta_graph.vertices.set(5);
    delta_graph.adjacency_matrix[0].set(5);
    delta_graph.adjacency_matrix[5].set(0);
    delta_graph.adjacency_matrix[2].set(5);
    delta_graph.adjacency_matrix[5].set(2);
    delta_graph.adjacency_matrix[3].set(5);
    delta_graph.adjacency_matrix[5].set(3);

    auto full_graph = d::delta_kpkc::Graph {};
    full_graph.adjacency_matrix.resize(nv);
    for (auto& bitset : full_graph.adjacency_matrix)
        bitset.resize(nv, false);
    full_graph.vertices.resize(nv, true);  // all active
    full_graph.adjacency_matrix[0].set(2);
    full_graph.adjacency_matrix[2].set(0);
    full_graph.adjacency_matrix[0].set(3);
    full_graph.adjacency_matrix[3].set(0);
    full_graph.adjacency_matrix[0].set(4);
    full_graph.adjacency_matrix[4].set(0);
    full_graph.adjacency_matrix[1].set(5);
    full_graph.adjacency_matrix[5].set(1);
    full_graph.adjacency_matrix[2].set(4);
    full_graph.adjacency_matrix[4].set(2);
    full_graph.adjacency_matrix[3].set(4);
    full_graph.adjacency_matrix[4].set(3);
    full_graph.adjacency_matrix[0].set(5);
    full_graph.adjacency_matrix[5].set(0);
    full_graph.adjacency_matrix[2].set(5);
    full_graph.adjacency_matrix[5].set(2);
    full_graph.adjacency_matrix[3].set(5);
    full_graph.adjacency_matrix[5].set(3);

    auto workspace = d::delta_kpkc::Workspace {};
    workspace.compatible_vertices.resize(k);
    for (uint_t i = 0; i < k; ++i)
    {
        workspace.compatible_vertices[i].resize(k);
        for (uint_t j = 0; j < k; ++j)
            workspace.compatible_vertices[i][j].resize(const_graph.partitions[j].size());
    }
    workspace.partition_bits.resize(k);
    workspace.partial_solution.reserve(k);

    auto dkpkc = d::DeltaKPKC(std::move(const_graph), std::move(delta_graph), std::move(full_graph), std::move(workspace));

    auto cliques = enumerate_cliques(dkpkc);

    EXPECT_EQ(cliques.size(), 2);
    EXPECT_EQ(cliques, (std::vector<std::vector<uint_t>> { { 0, 5, 2 }, { 0, 5, 3 } }));
}

TEST(TyrTests, TyrDatalogDeltaKPKCStandard4)
{
    const auto nv = uint_t(8);
    const auto k = uint_t(4);

    auto const_graph = d::delta_kpkc::ConstGraph {};
    const_graph.num_vertices = nv;
    const_graph.k = k;
    const_graph.partitions = std::vector<std::vector<uint_t>> { { 0, 1 }, { 2, 3 }, { 4, 5 }, { 6, 7 } };
    const_graph.vertex_to_partition = std::vector<uint_t> { 0, 0, 1, 1, 2, 2, 3, 3 };

    auto delta_graph = d::delta_kpkc::Graph {};
    delta_graph.adjacency_matrix.resize(nv);
    for (auto& bitset : delta_graph.adjacency_matrix)
        bitset.resize(nv, false);
    delta_graph.vertices.resize(nv, true);  // all active
    delta_graph.adjacency_matrix[0].set(2);
    delta_graph.adjacency_matrix[2].set(0);
    delta_graph.adjacency_matrix[0].set(3);
    delta_graph.adjacency_matrix[3].set(0);
    delta_graph.adjacency_matrix[0].set(4);
    delta_graph.adjacency_matrix[4].set(0);
    delta_graph.adjacency_matrix[1].set(5);
    delta_graph.adjacency_matrix[5].set(1);
    delta_graph.adjacency_matrix[2].set(4);
    delta_graph.adjacency_matrix[4].set(2);
    delta_graph.adjacency_matrix[3].set(4);
    delta_graph.adjacency_matrix[4].set(3);

    delta_graph.adjacency_matrix[0].set(7);
    delta_graph.adjacency_matrix[7].set(0);
    delta_graph.adjacency_matrix[2].set(7);
    delta_graph.adjacency_matrix[7].set(2);
    delta_graph.adjacency_matrix[3].set(7);
    delta_graph.adjacency_matrix[7].set(3);
    delta_graph.adjacency_matrix[4].set(7);
    delta_graph.adjacency_matrix[7].set(4);

    auto full_graph = d::delta_kpkc::Graph {};
    full_graph.adjacency_matrix.resize(nv);
    for (auto& bitset : full_graph.adjacency_matrix)
        bitset.resize(nv, false);
    full_graph.vertices.resize(nv, true);  // all active
    full_graph.adjacency_matrix[0].set(2);
    full_graph.adjacency_matrix[2].set(0);
    full_graph.adjacency_matrix[0].set(3);
    full_graph.adjacency_matrix[3].set(0);
    full_graph.adjacency_matrix[0].set(4);
    full_graph.adjacency_matrix[4].set(0);
    full_graph.adjacency_matrix[1].set(5);
    full_graph.adjacency_matrix[5].set(1);
    full_graph.adjacency_matrix[2].set(4);
    full_graph.adjacency_matrix[4].set(2);
    full_graph.adjacency_matrix[3].set(4);
    full_graph.adjacency_matrix[4].set(3);

    full_graph.adjacency_matrix[0].set(7);
    full_graph.adjacency_matrix[7].set(0);
    full_graph.adjacency_matrix[2].set(7);
    full_graph.adjacency_matrix[7].set(2);
    full_graph.adjacency_matrix[3].set(7);
    full_graph.adjacency_matrix[7].set(3);
    full_graph.adjacency_matrix[4].set(7);
    full_graph.adjacency_matrix[7].set(4);

    auto workspace = d::delta_kpkc::Workspace {};
    workspace.compatible_vertices.resize(k);
    for (uint_t i = 0; i < k; ++i)
    {
        workspace.compatible_vertices[i].resize(k);
        for (uint_t j = 0; j < k; ++j)
            workspace.compatible_vertices[i][j].resize(const_graph.partitions[j].size());
    }
    workspace.partition_bits.resize(k);
    workspace.partial_solution.reserve(k);

    auto dkpkc = d::DeltaKPKC(std::move(const_graph), std::move(delta_graph), std::move(full_graph), std::move(workspace));

    auto cliques = enumerate_cliques(dkpkc);

    EXPECT_EQ(cliques.size(), 2);
    EXPECT_EQ(cliques, (std::vector<std::vector<uint_t>> { { 0, 2, 4, 7 }, { 0, 3, 4, 7 } }));
}

TEST(TyrTests, TyrDatalogDeltaKPKCDelta4)
{
    const auto nv = uint_t(8);
    const auto k = uint_t(4);

    auto const_graph = d::delta_kpkc::ConstGraph {};
    const_graph.num_vertices = nv;
    const_graph.k = k;
    const_graph.partitions = std::vector<std::vector<uint_t>> { { 0, 1 }, { 2, 3 }, { 4, 5 }, { 6, 7 } };
    const_graph.vertex_to_partition = std::vector<uint_t> { 0, 0, 1, 1, 2, 2, 3, 3 };

    auto delta_graph = d::delta_kpkc::Graph {};
    delta_graph.adjacency_matrix.resize(nv);
    for (auto& bitset : delta_graph.adjacency_matrix)
        bitset.resize(nv, false);
    delta_graph.vertices.resize(nv, false);  // all active
    delta_graph.vertices.set(0);
    delta_graph.vertices.set(2);
    delta_graph.vertices.set(4);
    delta_graph.vertices.set(6);
    delta_graph.adjacency_matrix[0].set(6);
    delta_graph.adjacency_matrix[6].set(0);
    delta_graph.adjacency_matrix[2].set(6);
    delta_graph.adjacency_matrix[6].set(2);
    delta_graph.adjacency_matrix[4].set(6);
    delta_graph.adjacency_matrix[6].set(4);

    auto full_graph = d::delta_kpkc::Graph {};
    full_graph.adjacency_matrix.resize(nv);
    for (auto& bitset : full_graph.adjacency_matrix)
        bitset.resize(nv, false);
    full_graph.vertices.resize(nv, true);  // all active
    full_graph.adjacency_matrix[0].set(2);
    full_graph.adjacency_matrix[2].set(0);
    full_graph.adjacency_matrix[0].set(3);
    full_graph.adjacency_matrix[3].set(0);
    full_graph.adjacency_matrix[0].set(4);
    full_graph.adjacency_matrix[4].set(0);
    full_graph.adjacency_matrix[1].set(5);
    full_graph.adjacency_matrix[5].set(1);
    full_graph.adjacency_matrix[2].set(4);
    full_graph.adjacency_matrix[4].set(2);
    full_graph.adjacency_matrix[3].set(4);
    full_graph.adjacency_matrix[4].set(3);

    full_graph.adjacency_matrix[0].set(7);
    full_graph.adjacency_matrix[7].set(0);
    full_graph.adjacency_matrix[2].set(7);
    full_graph.adjacency_matrix[7].set(2);
    full_graph.adjacency_matrix[3].set(7);
    full_graph.adjacency_matrix[7].set(3);
    full_graph.adjacency_matrix[4].set(7);
    full_graph.adjacency_matrix[7].set(4);

    full_graph.adjacency_matrix[0].set(6);
    full_graph.adjacency_matrix[6].set(0);
    full_graph.adjacency_matrix[2].set(6);
    full_graph.adjacency_matrix[6].set(2);
    full_graph.adjacency_matrix[4].set(6);
    full_graph.adjacency_matrix[6].set(4);

    auto workspace = d::delta_kpkc::Workspace {};
    workspace.compatible_vertices.resize(k);
    for (uint_t i = 0; i < k; ++i)
    {
        workspace.compatible_vertices[i].resize(k);
        for (uint_t j = 0; j < k; ++j)
            workspace.compatible_vertices[i][j].resize(const_graph.partitions[j].size());
    }
    workspace.partition_bits.resize(k);
    workspace.partial_solution.reserve(k);

    auto dkpkc = d::DeltaKPKC(std::move(const_graph), std::move(delta_graph), std::move(full_graph), std::move(workspace));

    auto cliques = enumerate_cliques(dkpkc);

    EXPECT_EQ(cliques.size(), 1);
    EXPECT_EQ(cliques, (std::vector<std::vector<uint_t>> { { 0, 6, 2, 4 } }));
}

}