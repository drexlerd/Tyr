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

#include "bindings.hpp"

namespace tyr::formalism::planning
{

void bind_module_definitions(nb::module_& m)
{
    nb::class_<View<Index<Object>, Repository>>(m, "Object")  //
        .def("__str__", [](View<Index<Object>, Repository> self) { return to_string(self); })
        .def("get_name", &View<Index<Object>, Repository>::get_name);

    bind_predicate<StaticTag>(m, "StaticPredicate");
    bind_predicate<FluentTag>(m, "FluentPredicate");
    bind_predicate<DerivedTag>(m, "DerivedPredicate");

    bind_function<StaticTag>(m, "StaticFunction");
    bind_function<FluentTag>(m, "FluentFunction");
    bind_function<AuxiliaryTag>(m, "AuxiliaryFunction");

    nb::class_<View<Index<Action>, Repository>>(m, "Action")  //
        .def("__str__", [](View<Index<Action>, Repository> self) { return to_string(self); })
        .def("get_name", &View<Index<Action>, Repository>::get_name)
        .def("get_original_arity", &View<Index<Action>, Repository>::get_original_arity)
        .def("get_arity", &View<Index<Action>, Repository>::get_arity)
        .def("get_variables", &View<Index<Action>, Repository>::get_variables)
        .def("get_condition", &View<Index<Action>, Repository>::get_condition)
        .def("get_effects", &View<Index<Action>, Repository>::get_effects);
}

}