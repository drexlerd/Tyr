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

#include "tyr/tyr.hpp"

#include <gtest/gtest.h>

using namespace tyr::cista;
using namespace tyr::formalism;

namespace tyr::tests
{

TEST(TyrTests, TyrFormalismProxy)
{
    auto repository = Repository();
    auto buffer = Buffer();
    auto predicate_builder = Predicate<FluentTag>();
    auto object_builder = Object();
    auto variable_builder = Variable();
    auto atom_builder = Atom<FluentTag>();

    // Create a unique predicate
    predicate_builder.name = "predicate";
    predicate_builder.arity = 2;
    auto [predicate, predicate_success] = repository.get_or_create(predicate_builder, buffer);

    // Create object and variable
    object_builder.name = "a";
    auto [object, object_success] = repository.get_or_create(object_builder, buffer);

    // Create atom
    atom_builder.terms.clear();
    atom_builder.index.predicate_index = predicate->index;
    atom_builder.terms.push_back(Term(object->index));
    atom_builder.terms.push_back(Term(ParameterIndex(0)));
    auto [atom, atom_success] = repository.get_or_create(atom_builder, buffer);

    // Recurse through proxy
    auto atom_proxy = AtomProxy<FluentTag>(atom->index, repository);
    auto atom_relation_proxy = atom_proxy.get_predicate();
    auto atom_terms_proxy = atom_proxy.get_terms();

    EXPECT_EQ(atom_relation_proxy.get_name(), "predicate");
    EXPECT_EQ(atom_relation_proxy.get_arity(), 2);
    visit(
        [&](auto&& arg)
        {
            using ProxyType = std::decay_t<decltype(arg)>;

            if constexpr (std::is_same_v<ProxyType, ObjectProxy<>>)
            {
                EXPECT_EQ(arg.get_index(), object->index);
            }
            else
            {
                FAIL() << "Expected ObjectProxy for first term, got a different proxy type";
            }
        },
        atom_terms_proxy[0]);
    visit(
        [&](auto&& arg)
        {
            using ProxyType = std::decay_t<decltype(arg)>;

            if constexpr (std::is_same_v<ProxyType, ParameterIndex>)
            {
                EXPECT_EQ(arg, ParameterIndex(0));
            }
            else
            {
                FAIL() << "Expected VariableProxy for first term, got a different proxy type";
            }
        },
        atom_terms_proxy[1]);
}

}