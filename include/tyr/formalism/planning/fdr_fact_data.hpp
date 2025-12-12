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

#ifndef TYR_FORMALISM_FDR_FACT_DATA_HPP_
#define TYR_FORMALISM_FDR_FACT_DATA_HPP_

#include "tyr/common/types.hpp"
#include "tyr/common/types_utils.hpp"
#include "tyr/common/variant.hpp"
#include "tyr/formalism/declarations.hpp"
#include "tyr/formalism/planning/fdr_value.hpp"
#include "tyr/formalism/planning/fdr_variable_index.hpp"

namespace tyr
{
template<formalism::FactKind T>
struct Data<formalism::FDRFact<T>>
{
    using Tag = formalism::FDRFact<T>;

    Index<formalism::FDRVariable<T>> variable;
    formalism::FDRValue value;

    Data() = default;
    Data(Index<formalism::FDRVariable<T>> variable, formalism::FDRValue value) : variable(variable), value(value) {}

    void clear() noexcept
    {
        tyr::clear(variable);
        tyr::clear(value);
    }

    auto cista_members() const noexcept { return std::tie(variable, value); }
    auto identifying_members() const noexcept { return std::tie(variable, value); }
};

}

#endif
