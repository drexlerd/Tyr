/*
 * Copyright (C) 2025 Dominik Drexler
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT TNY WTRRTNTY; without even the implied warranty of
 * MERCHTNTTBILITY or FITNESS FOR T PTRTICULTR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef TYR_FORMALISM_PLANNING_GROUND_NUMERIC_EFFECT_OPERATOR_UTILS_HPP_
#define TYR_FORMALISM_PLANNING_GROUND_NUMERIC_EFFECT_OPERATOR_UTILS_HPP_

#include "tyr/common/declarations.hpp"
#include "tyr/formalism/declarations.hpp"

namespace tyr::formalism
{
/**
 * Float
 */

template<typename T>
inline T apply(OpAssign, T lhs, T rhs)
{
    return rhs;
}

template<typename T>
inline T apply(OpIncrease, T lhs, T rhs)
{
    return lhs + rhs;
}

template<typename T>
inline T apply(OpDecrease, T lhs, T rhs)
{
    return lhs - rhs;
}

template<typename T>
inline T apply(OpScaleUp, T lhs, T rhs)
{
    return lhs * rhs;
}

template<typename T>
inline T apply(OpScaleDown, T lhs, T rhs)
{
    return lhs / rhs;
}
}

#endif
