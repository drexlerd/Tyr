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

#ifndef TYR_FORMALISM_PREDICATE_HPP_
#define TYR_FORMALISM_PREDICATE_HPP_

#include "tyr/formalism/declarations.hpp"
#include "tyr/formalism/predicate_index.hpp"

namespace tyr::formalism
{

template<IsStaticOrFluentTag T>
struct Predicate
{
    PredicateIndex<T> index;
    ::cista::offset::string name;
    uint_t arity;

    Predicate() = default;
    Predicate(PredicateIndex<T> index, ::cista::offset::string name, uint_t arity) : index(index), name(std::move(name)), arity(arity) {}
    Predicate(const Predicate& other) = delete;
    Predicate& operator=(const Predicate& other) = delete;
    Predicate(Predicate&& other) = default;
    Predicate& operator=(Predicate&& other) = default;

    auto cista_members() const noexcept { return std::tie(index, name, arity); }
    auto identifying_members() const noexcept { return std::tie(name, arity); }
};
}

#endif
