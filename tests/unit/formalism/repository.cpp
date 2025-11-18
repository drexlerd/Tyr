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

#include "tyr/formalism/repository.hpp"

#include <gtest/gtest.h>

using namespace tyr::cista;
using namespace tyr::formalism;

namespace tyr::tests
{

TEST(TyrTests, TyrFormalismRepository)
{
    auto repository = Repository();
    auto buffer = Buffer();
    auto relation_builder = RelationImpl<FluentTag>();
    auto symbol_builder = SymbolImpl();
    auto atom_builder = AtomImpl<FluentTag>();

    // Create a unique relation
    relation_builder.name = "relation_0";
    relation_builder.arity = 2;

    auto [relation_0, relation_success_0] = repository.get_or_create(relation_builder, buffer);

    EXPECT_TRUE(relation_success_0);
    EXPECT_EQ(relation_0->index.value, 0);
    EXPECT_EQ(relation_0->name, "relation_0");
    EXPECT_EQ(relation_0->arity, 2);

    // Create a unique relation
    relation_builder.name = "relation_1";
    relation_builder.arity = 3;

    auto [relation_1, relation_success_1] = repository.get_or_create(relation_builder, buffer);

    EXPECT_TRUE(relation_success_1);
    EXPECT_EQ(relation_1->index.value, 1);
    EXPECT_EQ(relation_1->name, "relation_1");
    EXPECT_EQ(relation_1->arity, 3);

    // Create an existing relation
    relation_builder.name = "relation_1";
    relation_builder.arity = 3;

    auto [relation_2, relation_success_2] = repository.get_or_create(relation_builder, buffer);

    EXPECT_FALSE(relation_success_2);
    EXPECT_EQ(relation_2->index.value, 1);
    EXPECT_EQ(relation_2->name, "relation_1");
    EXPECT_EQ(relation_2->arity, 3);

    // Create some symbols
    symbol_builder.name = "a";
    auto [symbol_0, symbol_success_0] = repository.get_or_create(symbol_builder, buffer);
    symbol_builder.name = "b";
    auto [symbol_1, symbol_success_1] = repository.get_or_create(symbol_builder, buffer);
    symbol_builder.name = "c";
    auto [symbol_2, symbol_success_2] = repository.get_or_create(symbol_builder, buffer);

    // Create atom
    atom_builder.terms.clear();
    atom_builder.index.relation_index = relation_0->index;
    atom_builder.terms.push_back(Term(symbol_0->index));
    atom_builder.terms.push_back(Term(symbol_1->index));
    auto [atom_0, atom_success_0] = repository.get_or_create(atom_builder, buffer);

    EXPECT_TRUE(atom_success_0);
    EXPECT_TRUE(EqualTo<TermList> {}(atom_0->terms, TermList { Term(symbol_0->index), Term(symbol_1->index) }));

    // Create same atom again
    auto [atom_1, atom_success_1] = repository.get_or_create(atom_builder, buffer);
    EXPECT_FALSE(atom_success_1);
}

}