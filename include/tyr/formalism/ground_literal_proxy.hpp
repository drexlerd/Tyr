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

#ifndef TYR_FORMALISM_GROUND_LITERAL_PROXY_HPP_
#define TYR_FORMALISM_GROUND_LITERAL_PROXY_HPP_

#include "tyr/formalism/declarations.hpp"
#include "tyr/formalism/ground_atom_proxy.hpp"
#include "tyr/formalism/ground_literal_index.hpp"
#include "tyr/formalism/predicate_proxy.hpp"
#include "tyr/formalism/repository.hpp"

namespace tyr::formalism
{
template<IsStaticOrFluentTag T>
class GroundLiteralProxy
{
private:
    const Repository* repository;
    GroundLiteralIndex<T> index;

public:
    GroundLiteralProxy(const Repository& repository, GroundLiteralIndex<T> index) : repository(&repository), index(index) {}

    const auto& get() const { return repository->operator[]<GroundLiteral<T>>(index); }

    auto get_index() const { return index; }
    auto get_predicate() const { return PredicateProxy(*repository, index.predicate_index); }
    auto get_atom() const { return GroundAtomProxy((*repository), get().atom_index); }
    auto get_polarity() const { return get().polarity; }
};
}

#endif
