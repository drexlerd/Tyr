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

using namespace tyr::cista;
using namespace tyr::formalism;

namespace tyr::tests
{

TEST(TyrTests, TyrFormalismView)
{
    auto repository = Repository();
    auto buffer = Buffer();
    auto predicate_builder = Data<Predicate<FluentTag>>();
    auto object_builder = Data<Object>();
    auto variable_builder = Data<Variable>();
    auto atom_builder = Data<Atom<FluentTag>>();

    // Create a unique predicate
    predicate_builder.name = "predicate";
    predicate_builder.arity = 2;
    canonicalize(predicate_builder);
    auto [predicate, predicate_success] = repository.get_or_create(predicate_builder, buffer);

    // Create object and variable
    object_builder.name = "a";
    canonicalize(object_builder);
    auto [object, object_success] = repository.get_or_create(object_builder, buffer);

    // Create atom
    atom_builder.terms.clear();
    atom_builder.index.group = predicate.get_index();
    atom_builder.terms.push_back(Data<Term>(object.get_index()));
    atom_builder.terms.push_back(Data<Term>(ParameterIndex(0)));
    canonicalize(atom_builder);
    auto [atom, atom_success] = repository.get_or_create(atom_builder, buffer);

    // Recurse through proxy
    auto atom_proxy = View<Index<Atom<FluentTag>>, Repository>(atom.get_index(), repository);
    auto atom_relation_proxy = atom_proxy.get_predicate();
    auto atom_terms_proxy = atom_proxy.get_terms();

    EXPECT_EQ(atom_relation_proxy.get_name(), "predicate");
    EXPECT_EQ(atom_relation_proxy.get_arity(), 2);
    visit(
        [&](auto&& arg)
        {
            using Alternative = std::decay_t<decltype(arg)>;

            if constexpr (std::is_same_v<Alternative, View<Index<Object>, Repository>>)
            {
                EXPECT_EQ(arg.get_index(), object.get_index());
            }
            else
            {
                FAIL() << "Expected ObjectView for first term, got a different proxy type";
            }
        },
        atom_terms_proxy[0].get());
    visit(
        [&](auto&& arg)
        {
            using Alternative = std::decay_t<decltype(arg)>;

            if constexpr (std::is_same_v<Alternative, ParameterIndex>)
            {
                EXPECT_EQ(arg, ParameterIndex(0));
            }
            else
            {
                FAIL() << "Expected VariableView for first term, got a different proxy type";
            }
        },
        atom_terms_proxy[1].get());
}

}