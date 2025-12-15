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

#include "metric.hpp"

#include "tyr/common/optional.hpp"
#include "tyr/formalism/overlay_repository.hpp"
#include "tyr/formalism/repository.hpp"
#include "tyr/formalism/views.hpp"
#include "tyr/planning/applicability.hpp"
#include "tyr/planning/ground_task.hpp"
#include "tyr/planning/lifted_task.hpp"

namespace tyr::planning
{

template<typename T>
float_t evaluate_metric(View<::cista::optional<Index<formalism::Metric>>, formalism::OverlayRepository<formalism::Repository>> metric,
                        View<::cista::optional<Index<formalism::GroundFunctionTermValue<formalism::AuxiliaryTag>>>,
                             formalism::OverlayRepository<formalism::Repository>> auxiliary_fterm_value,
                        const StateContext<T>& state_context)
{
    if (auxiliary_fterm_value)
        return auxiliary_fterm_value.value().get_value();

    return metric ? evaluate(metric.value().get_fexpr(), state_context) : 0.;
}

template float_t evaluate_metric(View<::cista::optional<Index<formalism::Metric>>, formalism::OverlayRepository<formalism::Repository>> metric,
                                 View<::cista::optional<Index<formalism::GroundFunctionTermValue<formalism::AuxiliaryTag>>>,
                                      formalism::OverlayRepository<formalism::Repository>> auxiliary_fterm_value,
                                 const StateContext<LiftedTask>& state_context);
}
