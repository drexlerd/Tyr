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

#ifndef TYR_FORMALISM_PREDICATE_PROXY_HPP_
#define TYR_FORMALISM_PREDICATE_PROXY_HPP_

#include "tyr/formalism/declarations.hpp"
#include "tyr/formalism/predicate_index.hpp"
#include "tyr/formalism/repository.hpp"

namespace tyr::formalism
{
template<IsStaticOrFluentTag T>
class PredicateProxy
{
private:
    const Repository* repository;
    PredicateIndex<T> index;

public:
    PredicateProxy(const Repository& repository, PredicateIndex<T> index) : repository(&repository), index(index) {}

    const auto& get() const { return repository->operator[]<Predicate<T>>(index); }

    auto get_index() const { return index; }
    const auto& get_name() const { return get().name; }
    auto get_arity() const { return get().arity; }
};
}

#endif
