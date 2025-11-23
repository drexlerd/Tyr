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

#include "tyr/tyr.hpp"

#include <gtest/gtest.h>

using namespace tyr::cista;
using namespace tyr::formalism;

namespace tyr::tests
{

TEST(TyrTests, TyrCistaIndexedHashSet)
{
    auto repository = IndexedHashSet<Predicate<FluentTag>>();
    auto buffer = Buffer();
    auto builder = Data<Predicate<FluentTag>>();

    // Create a unique predicate
    builder.name = "predicate_0";
    builder.arity = 2;

    auto [predicate_0, success_0] = repository.insert(builder, buffer);

    EXPECT_EQ(predicate_0->index.value, 0);
    EXPECT_EQ(predicate_0->name, builder.name);
    EXPECT_EQ(predicate_0->arity, builder.arity);

    // Create a unique predicate
    builder.name = "predicate_1";
    builder.arity = 3;

    auto [predicate_1, success_1] = repository.insert(builder, buffer);

    EXPECT_EQ(predicate_1->index.value, 1);
    EXPECT_EQ(predicate_1->name, builder.name);
    EXPECT_EQ(predicate_1->arity, builder.arity);

    // Create an existing predicate
    builder.name = "predicate_1";
    builder.arity = 3;

    auto [predicate_2, success_2] = repository.insert(builder, buffer);

    EXPECT_EQ(predicate_2->index.value, 1);
    EXPECT_EQ(predicate_2->name, builder.name);
    EXPECT_EQ(predicate_2->arity, builder.arity);
}

}