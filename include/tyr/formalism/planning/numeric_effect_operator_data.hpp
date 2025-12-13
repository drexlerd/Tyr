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

#ifndef TYR_FORMALISM_PLANNING_NUMERIC_EFFECT_OPERATOR_DATA_HPP_
#define TYR_FORMALISM_PLANNING_NUMERIC_EFFECT_OPERATOR_DATA_HPP_

#include "tyr/common/types.hpp"
#include "tyr/common/types_utils.hpp"
#include "tyr/common/variant.hpp"
#include "tyr/formalism/declarations.hpp"
#include "tyr/formalism/planning/numeric_effect_index.hpp"

namespace tyr
{

template<>
struct Data<formalism::NumericEffectOperator<formalism::FluentTag>>
{
    using Tag = formalism::NumericEffectOperator<formalism::FluentTag>;

    using Variant = ::cista::offset::variant<Index<formalism::NumericEffect<formalism::OpAssign, formalism::FluentTag>>,
                                             Index<formalism::NumericEffect<formalism::OpIncrease, formalism::FluentTag>>,
                                             Index<formalism::NumericEffect<formalism::OpDecrease, formalism::FluentTag>>,
                                             Index<formalism::NumericEffect<formalism::OpScaleUp, formalism::FluentTag>>,
                                             Index<formalism::NumericEffect<formalism::OpScaleDown, formalism::FluentTag>>>;

    Variant value;

    Data() = default;
    Data(Variant value) : value(value) {}

    void clear() noexcept { tyr::clear(value); }

    auto cista_members() const noexcept { return std::tie(value); }
    auto identifying_members() const noexcept { return std::tie(value); }
};

template<>
struct Data<formalism::NumericEffectOperator<formalism::AuxiliaryTag>>
{
    using Tag = formalism::NumericEffectOperator<formalism::AuxiliaryTag>;

    using Variant = ::cista::offset::variant<Index<formalism::NumericEffect<formalism::OpIncrease, formalism::AuxiliaryTag>>>;

    Variant value;

    Data() = default;
    Data(Variant value) : value(value) {}

    void clear() noexcept
    {
        value.destruct();
        new (&value) Variant {};
    }

    friend bool operator==(const Data& lhs, const Data& rhs) { return EqualTo<Variant> {}(lhs.value, rhs.value); }
    friend bool operator!=(const Data& lhs, const Data& rhs) { return lhs.value != rhs.value; }
    friend bool operator<=(const Data& lhs, const Data& rhs) { return lhs.value <= rhs.value; }
    friend bool operator<(const Data& lhs, const Data& rhs) { return lhs.value < rhs.value; }
    friend bool operator>=(const Data& lhs, const Data& rhs) { return lhs.value >= rhs.value; }
    friend bool operator>(const Data& lhs, const Data& rhs) { return lhs.value > rhs.value; }

    auto cista_members() const noexcept { return std::tie(value); }
    auto identifying_members() const noexcept { return std::tie(value); }
};

}

#endif
