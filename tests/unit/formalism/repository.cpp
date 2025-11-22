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

#include "tyr/formalism/formalism.hpp"

#include <gtest/gtest.h>

using namespace tyr::cista;
using namespace tyr::formalism;

namespace tyr::tests
{

TEST(TyrTests, TyrFormalismRepository)
{
    auto repository = Repository();
    auto buffer = Buffer();
    auto predicate_builder = Data<Predicate<FluentTag>>();
    auto object_builder = Data<Object>();
    auto atom_builder = Data<Atom<FluentTag>>();

    // Create a unique predicate
    predicate_builder.name = "predicate_0";
    predicate_builder.arity = 2;

    auto [predicate_0, predicate_success_0] = repository.get_or_create(predicate_builder, buffer);

    EXPECT_TRUE(predicate_success_0);
    EXPECT_EQ(predicate_0->index.value, 0);
    EXPECT_EQ(predicate_0->name, predicate_builder.name);
    EXPECT_EQ(predicate_0->arity, predicate_builder.arity);

    // Create a unique predicate
    predicate_builder.name = "predicate_1";
    predicate_builder.arity = 3;

    auto [predicate_1, predicate_success_1] = repository.get_or_create(predicate_builder, buffer);

    EXPECT_TRUE(predicate_success_1);
    EXPECT_EQ(predicate_1->index.value, 1);
    EXPECT_EQ(predicate_1->name, predicate_builder.name);
    EXPECT_EQ(predicate_1->arity, predicate_builder.arity);

    // Create same predicate again
    predicate_builder.name = "predicate_1";
    predicate_builder.arity = 3;

    auto [predicate_2, predicate_success_2] = repository.get_or_create(predicate_builder, buffer);

    EXPECT_FALSE(predicate_success_2);
    EXPECT_EQ(predicate_2->index.value, 1);
    EXPECT_EQ(predicate_2->name, predicate_builder.name);
    EXPECT_EQ(predicate_2->arity, predicate_builder.arity);

    // Create objects
    object_builder.name = "a";
    auto [object_0, object_success_0] = repository.get_or_create(object_builder, buffer);
    EXPECT_TRUE(object_success_0);
    EXPECT_EQ(object_0->name, object_builder.name);

    object_builder.name = "b";
    auto [object_1, object_success_1] = repository.get_or_create(object_builder, buffer);
    EXPECT_TRUE(object_success_1);
    EXPECT_EQ(object_1->name, object_builder.name);

    object_builder.name = "c";
    auto [object_2, object_success_2] = repository.get_or_create(object_builder, buffer);
    EXPECT_TRUE(object_success_2);
    EXPECT_EQ(object_2->name, object_builder.name);

    // Create atom
    atom_builder.terms.clear();
    atom_builder.index.group = predicate_0->index;
    atom_builder.terms.push_back(Data<Term>(object_0->index));
    atom_builder.terms.push_back(Data<Term>(object_1->index));
    auto [atom_0, atom_success_0] = repository.get_or_create(atom_builder, buffer);

    EXPECT_TRUE(atom_success_0);
    EXPECT_EQ(atom_0->terms, atom_builder.terms);

    // Create same atom again
    auto [atom_1, atom_success_1] = repository.get_or_create(atom_builder, buffer);
    EXPECT_FALSE(atom_success_1);
}

TEST(TyrTests, TyrFormalismProxy)
{
    auto repository = Repository();
    auto buffer = Buffer();
    auto predicate_builder = Data<Predicate<FluentTag>>();
    auto object_builder = Data<Object>();
    auto atom_builder = Data<Atom<FluentTag>>();

    // Create a unique predicate
    predicate_builder.name = "predicate_0";
    predicate_builder.arity = 2;
    auto [predicate_0, predicate_success_0] = repository.get_or_create(predicate_builder, buffer);

    // Create objects
    object_builder.name = "a";
    auto [object_0, object_success_0] = repository.get_or_create(object_builder, buffer);
    object_builder.name = "b";
    auto [object_1, object_success_1] = repository.get_or_create(object_builder, buffer);

    // Create atom
    atom_builder.terms.clear();
    atom_builder.index.group = predicate_0->index;
    atom_builder.terms.push_back(Data<Term>(object_0->index));
    atom_builder.terms.push_back(Data<Term>(object_1->index));
    auto [atom_0, atom_success_0] = repository.get_or_create(atom_builder, buffer);
}

}