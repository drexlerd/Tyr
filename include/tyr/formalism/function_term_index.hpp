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

#ifndef TYR_FORMALISM_FUNCTION_TERM_INDEX_HPP_
#define TYR_FORMALISM_FUNCTION_TERM_INDEX_HPP_

#include "tyr/common/equal_to.hpp"
#include "tyr/common/index_mixins.hpp"
#include "tyr/formalism/declarations.hpp"

namespace tyr
{
template<formalism::IsStaticOrFluentTag T>
struct Index<formalism::FunctionTerm<T>> : IndexMixin<Index<formalism::FunctionTerm<T>>>
{
    using Tag = formalism::FunctionTerm<T>;

    // Inherit constructors
    using Base = IndexMixin<Index<formalism::FunctionTerm<T>>>;
    using Base::Base;
};
}

#endif
