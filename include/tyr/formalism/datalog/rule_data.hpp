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

#ifndef TYR_FORMALISM_DATALOG_RULE_DATA_HPP_
#define TYR_FORMALISM_DATALOG_RULE_DATA_HPP_

#include "tyr/common/types.hpp"
#include "tyr/common/types_utils.hpp"
#include "tyr/formalism/datalog/atom_index.hpp"
#include "tyr/formalism/datalog/conjunctive_condition_index.hpp"
#include "tyr/formalism/datalog/declarations.hpp"
#include "tyr/formalism/variable_index.hpp"

namespace tyr
{
template<>
struct Data<formalism::datalog::Rule>
{
    Index<formalism::datalog::Rule> index;
    IndexList<formalism::Variable> variables;
    uint_t num_fixed_prefix_variables;  ///< Number of variables in the prefix that must be preserved during transformations
    Index<formalism::datalog::ConjunctiveCondition> body;
    Index<formalism::datalog::Atom<formalism::FluentTag>> head;
    uint_t cost;

    Data() = default;
    Data(Index<formalism::datalog::Rule> index,
         IndexList<formalism::Variable> variables,
         uint_t num_fixed_prefix_variables,
         Index<formalism::datalog::ConjunctiveCondition> body,
         Index<formalism::datalog::Atom<formalism::FluentTag>> head,
         uint_t cost) :
        index(index),
        variables(std::move(variables)),
        num_fixed_prefix_variables(num_fixed_prefix_variables),
        body(body),
        head(head),
        cost(cost)
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
        tyr::clear(num_fixed_prefix_variables);
        tyr::clear(body);
        tyr::clear(head);
        tyr::clear(cost);
    }

    auto cista_members() const noexcept { return std::tie(index, variables, num_fixed_prefix_variables, body, head, cost); }
    auto identifying_members() const noexcept { return std::tie(variables, num_fixed_prefix_variables, body, head, cost); }
};
}

#endif
