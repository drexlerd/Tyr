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

#ifndef TYR_FORMALISM_GROUND_FUNCTION_TERM_VALUE_DATA_HPP_
#define TYR_FORMALISM_GROUND_FUNCTION_TERM_VALUE_DATA_HPP_

#include "tyr/formalism/declarations.hpp"
#include "tyr/formalism/ground_function_term_index.hpp"
#include "tyr/formalism/ground_function_term_value_index.hpp"
#include "tyr/formalism/term_data.hpp"

namespace tyr
{

template<formalism::IsStaticOrFluentTag T>
struct Data<formalism::GroundFunctionTermValue<T>>
{
    using Tag = formalism::GroundFunctionTermValue<T>;

    Index<formalism::GroundFunctionTermValue<T>> index;
    Index<formalism::GroundFunctionTerm<T>> term;
    float_t value;

    Data() = default;
    Data(Index<formalism::GroundFunctionTermValue<T>> index, Index<formalism::GroundFunctionTerm<T>> term, float_t value) :
        index(index),
        term(term),
        value(value)
    {
    }
    Data(const Data& other) = delete;
    Data& operator=(const Data& other) = delete;
    Data(Data&& other) = default;
    Data& operator=(Data&& other) = default;

    auto cista_members() const noexcept { return std::tie(index, term, value); }
    auto identifying_members() const noexcept { return std::tie(index.group, term, value); }
};
}

#endif
