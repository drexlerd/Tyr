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

#include "tyr/formalism/formalism.hpp"

#include <gtest/gtest.h>

namespace b = tyr::buffer;
namespace f = tyr::formalism;
namespace fp = tyr::formalism::planning;

namespace tyr::tests
{

inline IndexList<f::Object> O(std::initializer_list<size_t> xs)
{
    auto out = IndexList<f::Object> {};
    out.reserve(xs.size());
    for (auto x : xs)
        out.emplace_back(Index<f::Object>(x));
    return out;
}

TEST(TyrTests, TyrFormalismIndex)
{
    auto set = UnorderedSet<IndexList<f::Object>> {};
    auto l1 = O({ 0, 2, 3 });
    auto l2 = O({ 0, 2, 3 });

    auto hash = Hash<IndexList<f::Object>> {};
    auto equal_to = EqualTo<IndexList<f::Object>> {};
    EXPECT_EQ(hash(l1), hash(l2));
    EXPECT_TRUE(equal_to(l1, l2));

    const auto i1 = set.insert(l1).second;
    const auto i2 = set.insert(l2).second;

    EXPECT_TRUE(i1);
    EXPECT_FALSE(i2);
}

}