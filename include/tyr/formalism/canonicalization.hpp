

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

#ifndef TYR_FORMALISM_CANONICALIZATION_HPP_
#define TYR_FORMALISM_CANONICALIZATION_HPP_

#include "tyr/common/canonicalization.hpp"
#include "tyr/common/comparators.hpp"
#include "tyr/formalism/datas.hpp"

#include <algorithm>

namespace tyr::formalism
{

/**
 * Datalog
 */

inline bool is_canonical(const Data<Variable>& data) { return true; }

inline bool is_canonical(const Data<Object>& data) { return true; }

inline bool is_canonical(const Data<Binding>& data) { return true; }

inline bool is_canonical(const Data<Term>& data) { return true; }

template<FactKind T>
bool is_canonical(const Data<Predicate<T>>& data)
{
    return true;
}

template<FactKind T>
bool is_canonical(const Data<Function<T>>& data)
{
    return true;
}

inline void canonicalize(Data<Variable>& data)
{  // Trivially canonical
}

inline void canonicalize(Data<Object>& data)
{
    // Trivially canonical
}

inline void canonicalize(Data<Binding>& data)
{
    // Trivially canonical
}

inline void canonicalize(Data<Term>& data)
{
    // Trivially canonical
}

template<FactKind T>
void canonicalize(Data<Predicate<T>>& data)
{
    // Trivially canonical
}

template<FactKind T>
void canonicalize(Data<Function<T>>& data)
{
    // Trivially canonical
}

}

#endif
