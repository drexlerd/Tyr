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

#ifndef TYR_FORMALISM_PLANNING_FUNCTION_TERM_DATA_HPP_
#define TYR_FORMALISM_PLANNING_FUNCTION_TERM_DATA_HPP_

#include "tyr/common/types.hpp"
#include "tyr/common/types_utils.hpp"
#include "tyr/formalism/planning/declarations.hpp"
#include "tyr/formalism/planning/function_term_index.hpp"
#include "tyr/formalism/term_data.hpp"

namespace tyr
{

template<formalism::FactKind T>
struct Data<formalism::planning::FunctionTerm<T>>
{
    Index<formalism::planning::FunctionTerm<T>> index;
    Index<formalism::Function<T>> function;
    DataList<formalism::Term> terms;

    Data() = default;
    Data(Index<formalism::Function<T>> function_, DataList<formalism::Term> terms_) : index(), function(function_), terms(std::move(terms_)) {}
    // Python constructor
    template<typename C>
    Data(View<Index<formalism::Function<T>>, C> function_, const std::vector<View<Data<formalism::Term>, C>>& terms_) : index(), function(), terms()
    {
        set(function_, function);
        set(terms_, terms);
    }
    Data(const Data& other) = delete;
    Data& operator=(const Data& other) = delete;
    Data(Data&& other) = default;
    Data& operator=(Data&& other) = default;

    void clear() noexcept
    {
        tyr::clear(index);
        tyr::clear(function);
        tyr::clear(terms);
    }

    auto cista_members() const noexcept { return std::tie(index, function, terms); }
    auto identifying_members() const noexcept { return std::tie(function, terms); }
};

static_assert(!uses_trivial_storage_v<formalism::planning::FunctionTerm<formalism::StaticTag>>);
}

#endif
