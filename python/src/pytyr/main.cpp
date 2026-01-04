/*
 * Copyright (C) 2023 Dominik Drexler and Simon Stahlberg
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

#include "init_declarations.hpp"

namespace nb = nanobind;
using namespace nb::literals;

namespace tyr
{

NB_MODULE(pytyr, m)
{
    // Create submodules before binding to avoid missing bindings

    auto formalism = m.def_submodule("formalism");
    m.attr("formalism") = formalism;

    auto formalism_planning = formalism.def_submodule("planning");
    formalism.attr("planning") = formalism_planning;

    auto planning = m.def_submodule("planning");
    m.attr("planning") = planning;

    formalism::bind_module_definitions(formalism);

    formalism::planning::bind_module_definitions(formalism_planning);

    planning::bind_module_definitions(planning);
}

}
