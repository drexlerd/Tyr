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

#ifndef TYR_SRC_PLANNING_METRIC_HPP_
#define TYR_SRC_PLANNING_METRIC_HPP_

#include "tyr/common/declarations.hpp"
#include "tyr/common/types.hpp"
#include "tyr/formalism/declarations.hpp"
#include "tyr/planning/declarations.hpp"

namespace tyr::planning
{

template<typename T>
float_t evaluate_metric(View<::cista::optional<Index<formalism::Metric>>, formalism::OverlayRepository<formalism::Repository>> metric,
                        View<::cista::optional<Index<formalism::GroundFunctionTermValue<formalism::AuxiliaryTag>>>,
                             formalism::OverlayRepository<formalism::Repository>> auxiliary_fterm_value,
                        const StateContext<T>& state_context);
}

#endif