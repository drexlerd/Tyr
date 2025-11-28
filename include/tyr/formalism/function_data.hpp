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

#ifndef TYR_FORMALISM_FUNCTION_DATA_HPP_
#define TYR_FORMALISM_FUNCTION_DATA_HPP_

#include "tyr/formalism/declarations.hpp"
#include "tyr/formalism/function_index.hpp"

namespace tyr
{

template<formalism::IsFactTag T>
struct Data<formalism::Function<T>>
{
    using Tag = formalism::Function<T>;

    Index<formalism::Function<T>> index;
    ::cista::offset::string name;
    uint_t arity;

    Data() = default;
    Data(Index<formalism::Function<T>>, ::cista::offset::string name, uint_t arity) : index(index), name(std::move(name)), arity(arity) {}
    Data(const Data& other) = delete;
    Data& operator=(const Data& other) = delete;
    Data(Data&& other) = default;
    Data& operator=(Data&& other) = default;

    auto cista_members() const noexcept { return std::tie(index, name, arity); }
    auto identifying_members() const noexcept { return std::tie(name, arity); }
};
}

#endif
