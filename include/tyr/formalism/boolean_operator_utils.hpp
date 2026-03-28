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

#ifndef TYR_FORMALISM_BOOLEAN_OPERATOR_UTILS_HPP_
#define TYR_FORMALISM_BOOLEAN_OPERATOR_UTILS_HPP_

#include "tyr/common/closed_interval.hpp"
#include "tyr/common/declarations.hpp"
#include "tyr/common/types.hpp"
#include "tyr/formalism/declarations.hpp"

#include <cmath>
#include <iomanip>
#include <iostream>
#include <type_traits>

namespace tyr::formalism
{

template<std::floating_point T>
struct FloatCmp
{
    static constexpr T abs_eps = static_cast<T>(1e-12);
    static constexpr T rel_eps = static_cast<T>(1e-12);

    static T tol(T a, T b) noexcept { return std::max(abs_eps, rel_eps * std::max(std::abs(a), std::abs(b))); }

    static bool eq(T a, T b) noexcept { return std::abs(a - b) <= tol(a, b); }

    static bool ne(T a, T b) noexcept { return !eq(a, b); }

    static bool le(T a, T b) noexcept { return a <= b + tol(a, b); }

    static bool lt(T a, T b) noexcept { return a < b - tol(a, b); }

    static bool ge(T a, T b) noexcept { return a + tol(a, b) >= b; }

    static bool gt(T a, T b) noexcept { return a > b + tol(a, b); }
};

namespace detail
{
template<typename T>
bool eq_scalar(T lhs, T rhs) noexcept
{
    if constexpr (std::is_floating_point_v<T>)
        return FloatCmp<T>::eq(lhs, rhs);
    else
        return lhs == rhs;
}

template<typename T>
bool ne_scalar(T lhs, T rhs) noexcept
{
    if constexpr (std::is_floating_point_v<T>)
        return FloatCmp<T>::ne(lhs, rhs);
    else
        return lhs != rhs;
}

template<typename T>
bool ge_scalar(T lhs, T rhs) noexcept
{
    if constexpr (std::is_floating_point_v<T>)
        return FloatCmp<T>::ge(lhs, rhs);
    else
        return lhs >= rhs;
}

template<typename T>
bool gt_scalar(T lhs, T rhs) noexcept
{
    if constexpr (std::is_floating_point_v<T>)
        return FloatCmp<T>::gt(lhs, rhs);
    else
        return lhs > rhs;
}

template<typename T>
bool le_scalar(T lhs, T rhs) noexcept
{
    if constexpr (std::is_floating_point_v<T>)
        return FloatCmp<T>::le(lhs, rhs);
    else
        return lhs <= rhs;
}

template<typename T>
bool lt_scalar(T lhs, T rhs) noexcept
{
    if constexpr (std::is_floating_point_v<T>)
        return FloatCmp<T>::lt(lhs, rhs);
    else
        return lhs < rhs;
}
}

/**
 * Scalar comparisons
 */

template<typename T>
bool apply(OpEq, T lhs, T rhs) noexcept
{
    return detail::eq_scalar(lhs, rhs);
}

template<typename T>
bool apply(OpNe, T lhs, T rhs) noexcept
{
    return detail::ne_scalar(lhs, rhs);
}

template<typename T>
bool apply(OpGe, T lhs, T rhs) noexcept
{
    return detail::ge_scalar(lhs, rhs);
}

template<typename T>
bool apply(OpGt, T lhs, T rhs) noexcept
{
    return detail::gt_scalar(lhs, rhs);
}

template<typename T>
bool apply(OpLe, T lhs, T rhs) noexcept
{
    return detail::le_scalar(lhs, rhs);
}

template<typename T>
bool apply(OpLt, T lhs, T rhs) noexcept
{
    return detail::lt_scalar(lhs, rhs);
}

/**
 * Existential interval comparisons
 */

template<std::floating_point A>
bool apply_existential(OpEq, const ClosedInterval<A>& lhs, const ClosedInterval<A>& rhs)
{
    if (empty(lhs) || empty(rhs))
        return false;

    return detail::le_scalar(lower(lhs), upper(rhs)) && detail::ge_scalar(upper(lhs), lower(rhs));
}

template<std::floating_point A>
bool apply_existential(OpNe, const ClosedInterval<A>& lhs, const ClosedInterval<A>& rhs)
{
    if (empty(lhs) || empty(rhs))
        return false;

    const bool lhs_is_point = detail::eq_scalar(lower(lhs), upper(lhs));
    const bool rhs_is_point = detail::eq_scalar(lower(rhs), upper(rhs));

    if (lhs_is_point && rhs_is_point)
        return detail::ne_scalar(lower(lhs), lower(rhs));

    return true;
}

template<std::floating_point A>
bool apply_existential(OpGe, const ClosedInterval<A>& lhs, const ClosedInterval<A>& rhs)
{
    if (empty(lhs) || empty(rhs))
        return false;

    return detail::ge_scalar(upper(lhs), lower(rhs));
}

template<std::floating_point A>
bool apply_existential(OpGt, const ClosedInterval<A>& lhs, const ClosedInterval<A>& rhs)
{
    if (empty(lhs) || empty(rhs))
        return false;

    return detail::gt_scalar(upper(lhs), lower(rhs));
}

template<std::floating_point A>
bool apply_existential(OpLe, const ClosedInterval<A>& lhs, const ClosedInterval<A>& rhs)
{
    if (empty(lhs) || empty(rhs))
        return false;

    return detail::le_scalar(lower(lhs), upper(rhs));
}

template<std::floating_point A>
bool apply_existential(OpLt, const ClosedInterval<A>& lhs, const ClosedInterval<A>& rhs)
{
    if (empty(lhs) || empty(rhs))
        return false;

    return detail::lt_scalar(lower(lhs), upper(rhs));
}

}

#endif