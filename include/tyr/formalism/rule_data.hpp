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

#ifndef TYR_FORMALISM_RULE_DATA_HPP_
#define TYR_FORMALISM_RULE_DATA_HPP_

#include "tyr/common/types.hpp"
#include "tyr/common/types_utils.hpp"
#include "tyr/formalism/atom_index.hpp"
#include "tyr/formalism/conjunctive_condition_index.hpp"
#include "tyr/formalism/declarations.hpp"
#include "tyr/formalism/variable_index.hpp"

namespace tyr
{
template<>
struct Data<formalism::Rule>
{
    using Tag = formalism::Rule;

    Index<formalism::Rule> index;
    IndexList<formalism::Variable> variables;
    Index<formalism::ConjunctiveCondition> body;
    Index<formalism::Atom<formalism::FluentTag>> head;

    Data() = default;
    Data(Index<formalism::Rule> index,
         IndexList<formalism::Variable> variables,
         Index<formalism::ConjunctiveCondition> body,
         Index<formalism::Atom<formalism::FluentTag>> head) :
        index(index),
        variables(std::move(variables)),
        body(body),
        head(head)
    {
    }
    Data(const Data& other) = delete;
    Data& operator=(const Data& other) = delete;
    Data(Data&& other) = default;
    Data& operator=(Data&& other) = default;

    void clear() noexcept
    {
        tyr::clear(index);
        tyr::clear(variables);
        tyr::clear(body);
        tyr::clear(head);
    }

    auto cista_members() const noexcept { return std::tie(index, variables, body, head); }
    auto identifying_members() const noexcept { return std::tie(variables, body, head); }
};
}

#endif
