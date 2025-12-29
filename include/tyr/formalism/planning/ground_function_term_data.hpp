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

#ifndef TYR_FORMALISM_PLANNING_GROUND_FUNCTION_TERM_DATA_HPP_
#define TYR_FORMALISM_PLANNING_GROUND_FUNCTION_TERM_DATA_HPP_

#include "tyr/common/types.hpp"
#include "tyr/common/types_utils.hpp"
#include "tyr/formalism/binding_index.hpp"
#include "tyr/formalism/function_index.hpp"
#include "tyr/formalism/planning/declarations.hpp"
#include "tyr/formalism/planning/ground_function_term_index.hpp"

namespace tyr
{

template<formalism::FactKind T>
struct Data<formalism::planning::GroundFunctionTerm<T>>
{
    Index<formalism::planning::GroundFunctionTerm<T>> index;
    Index<formalism::Function<T>> function;
    Index<formalism::Binding> binding;

    Data() = default;
    Data(Index<formalism::planning::GroundFunctionTerm<T>> index, Index<formalism::Function<T>> function, Index<formalism::Binding> binding) :
        index(index),
        function(function),
        binding(binding)
    {
    }
    Data(const Data& other) = delete;
    Data& operator=(const Data& other) = delete;
    Data(Data&& other) = default;
    Data& operator=(Data&& other) = default;

    void clear() noexcept
    {
        tyr::clear(index);
        tyr::clear(function);
        tyr::clear(binding);
    }

    auto cista_members() const noexcept { return std::tie(index, function, binding); }
    auto identifying_members() const noexcept { return std::tie(function, binding); }
};
}

#endif
