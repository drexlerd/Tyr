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

#ifndef TYR_FORMALISM_ARITHMETIC_OPERATOR_UTILS_HPP_
#define TYR_FORMALISM_ARITHMETIC_OPERATOR_UTILS_HPP_

#include "tyr/common/closed_interval.hpp"
#include "tyr/common/declarations.hpp"
#include "tyr/formalism/declarations.hpp"

namespace tyr::formalism
{
/**
 * Unary
 */

template<IsFloatingPoint A>
inline ClosedInterval<A> apply(OpSub, const ClosedInterval<A>& el)
{
    return -el;
}

/**
 * Binary
 */

template<IsFloatingPoint A>
inline ClosedInterval<A> apply(OpAdd, const ClosedInterval<A>& lhs, const ClosedInterval<A>& rhs)
{
    return lhs + rhs;
}

template<IsFloatingPoint A>
inline ClosedInterval<A> apply(OpSub, const ClosedInterval<A>& lhs, const ClosedInterval<A>& rhs)
{
    return lhs - rhs;
}

template<IsFloatingPoint A>
inline ClosedInterval<A> apply(OpMul, const ClosedInterval<A>& lhs, const ClosedInterval<A>& rhs)
{
    return lhs * rhs;
}

template<IsFloatingPoint A>
inline ClosedInterval<A> apply(OpDiv, const ClosedInterval<A>& lhs, const ClosedInterval<A>& rhs)
{
    return lhs / rhs;
}

}

#endif
