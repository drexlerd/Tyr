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

#ifndef TYR_PYTHON_FORMALISM_PLANNING_BINDINGS_HPP
#define TYR_PYTHON_FORMALISM_PLANNING_BINDINGS_HPP

#include "../../init_declarations.hpp"

namespace tyr::formalism::planning
{
template<FactKind T>
void bind_predicate(nb::module_& m, const std::string& name)
{
    nb::class_<View<Index<Predicate<T>>, Repository>>(m, name.c_str())
        .def("__str__", [](View<Index<Predicate<T>>, Repository> self) { return to_string(self); })
        .def("get_name", &View<Index<Predicate<T>>, Repository>::get_name);
}

template<FactKind T>
void bind_function(nb::module_& m, const std::string& name)
{
    nb::class_<View<Index<Function<T>>, Repository>>(m, name.c_str())
        .def("__str__", [](View<Index<Function<T>>, Repository> self) { return to_string(self); })
        .def("get_name", &View<Index<Function<T>>, Repository>::get_name);
}

template<FactKind T>
void bind_atom(nb::module_& m, const std::string& name)
{
    nb::class_<View<Index<Atom<T>>, Repository>>(m, name.c_str())  //
        .def("__str__", [](View<Index<Atom<T>>, Repository> self) { return to_string(self); });
}

template<FactKind T>
void bind_ground_atom(nb::module_& m, const std::string& name)
{
    nb::class_<View<Index<GroundAtom<T>>, Repository>>(m, name.c_str())  //
        .def("__str__", [](View<Index<GroundAtom<T>>, Repository> self) { return to_string(self); });
}

template<FactKind T>
void bind_literal(nb::module_& m, const std::string& name)
{
    nb::class_<View<Index<Literal<T>>, Repository>>(m, name.c_str())  //
        .def("__str__", [](View<Index<Literal<T>>, Repository> self) { return to_string(self); });
}

template<FactKind T>
void bind_ground_literal(nb::module_& m, const std::string& name)
{
    nb::class_<View<Index<GroundLiteral<T>>, Repository>>(m, name.c_str())  //
        .def("__str__", [](View<Index<GroundLiteral<T>>, Repository> self) { return to_string(self); });
}
}

#endif
