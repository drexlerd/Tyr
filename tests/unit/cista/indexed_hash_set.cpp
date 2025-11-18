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

#include "tyr/cista/indexed_hash_set.hpp"

#include "tyr/formalism/relation.hpp"

#include <gtest/gtest.h>

using namespace tyr::cista;
using namespace tyr::formalism;

namespace tyr::tests
{

TEST(TyrTests, TyrCistaIndexedHashSet)
{
    auto repository = IndexedHashSet<RelationImpl<FluentTag>>();
    auto buffer = Buffer();
    auto builder = RelationImpl<FluentTag>();

    // Create a unique relation
    builder.name = "relation_0";
    builder.arity = 2;

    auto [relation_0, success_0] = repository.insert(builder, buffer);

    EXPECT_EQ(relation_0->index.value, 0);
    EXPECT_EQ(relation_0->name, builder.name);
    EXPECT_EQ(relation_0->arity, builder.arity);

    // Create a unique relation
    builder.name = "relation_1";
    builder.arity = 3;

    auto [relation_1, success_1] = repository.insert(builder, buffer);

    EXPECT_EQ(relation_1->index.value, 1);
    EXPECT_EQ(relation_1->name, builder.name);
    EXPECT_EQ(relation_1->arity, builder.arity);

    // Create an existing relation
    builder.name = "relation_1";
    builder.arity = 3;

    auto [relation_2, success_2] = repository.insert(builder, buffer);

    EXPECT_EQ(relation_2->index.value, 1);
    EXPECT_EQ(relation_2->name, builder.name);
    EXPECT_EQ(relation_2->arity, builder.arity);
}

}