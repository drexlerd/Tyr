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

#ifndef TYR_FORMALISM_PLANNING_METRIC_DATA_HPP_
#define TYR_FORMALISM_PLANNING_METRIC_DATA_HPP_

#include "tyr/common/types.hpp"
#include "tyr/common/types_utils.hpp"
#include "tyr/formalism/planning/declarations.hpp"
#include "tyr/formalism/planning/ground_function_expression_data.hpp"
#include "tyr/formalism/planning/metric_index.hpp"

namespace tyr
{

template<>
struct Data<formalism::planning::Metric>
{
    using ObjectiveVariant = ::cista::offset::variant<formalism::planning::Minimize, formalism::planning::Maximize>;

    Index<formalism::planning::Metric> index;
    ObjectiveVariant objective;
    Data<formalism::planning::GroundFunctionExpression> fexpr;

    Data() = default;
    Data(Index<formalism::planning::Metric> index, ObjectiveVariant objective, Data<formalism::planning::GroundFunctionExpression> fexpr) :
        index(index),
        objective(objective),
        fexpr(fexpr)
    {
    }
    Data(const Data& other) = delete;
    Data& operator=(const Data& other) = delete;
    Data(Data&& other) = default;
    Data& operator=(Data&& other) = default;

    void clear() noexcept
    {
        tyr::clear(index);
        tyr::clear(objective);
        tyr::clear(fexpr);
    }

    auto cista_members() const noexcept { return std::tie(index, objective, fexpr); }
    auto identifying_members() const noexcept { return std::tie(objective, fexpr); }
};
}

#endif
