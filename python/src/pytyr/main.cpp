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

NB_MODULE(pytyr, m)
{
    // Create submodules before binding to avoid missing bindings

    auto formalism = m.def_submodule("formalism");
    auto formalism_planning = formalism.def_submodule("planning");

    tyr::formalism::bind_module_definitions(formalism);
    tyr::formalism::planning::bind_module_definitions(formalism_planning);

    auto planning = m.def_submodule("planning");
    auto planning_ground = planning.def_submodule("ground");
    auto planning_ground_astar_eager = planning_ground.def_submodule("astar_eager");
    auto planning_ground_gbfs_lazy = planning_ground.def_submodule("gbfs_lazy");
    auto planning_lifted = planning.def_submodule("lifted");
    auto planning_lifted_astar_eager = planning_lifted.def_submodule("astar_eager");
    auto planning_lifted_gbfs_lazy = planning_lifted.def_submodule("gbfs_lazy");

    tyr::planning::bind_module_definitions(planning);
    tyr::planning::bind_ground_module_definitions(planning_ground);
    tyr::planning::bind_lifted_module_definitions(planning_lifted);
    tyr::planning::astar_eager::bind_ground_module_definitions(planning_ground_astar_eager);
    tyr::planning::astar_eager::bind_lifted_module_definitions(planning_lifted_astar_eager);
    tyr::planning::gbfs_lazy::bind_ground_module_definitions(planning_ground_gbfs_lazy);
    tyr::planning::gbfs_lazy::bind_lifted_module_definitions(planning_lifted_gbfs_lazy);
}
