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

#ifndef TYR_PYTHON_COMMON_BINDINGS_HPP
#define TYR_PYTHON_COMMON_BINDINGS_HPP

#include "../init_declarations.hpp"

namespace tyr
{

template<typename T>
void bind_index(nb::module_& m, const std::string& name)
{
    nb::class_<T>(m, name.c_str())  //
        .def("__int__", [](const T& i) { return (uint_t) i; })
        .def("__index__", [](const T& i) { return (uint_t) i; })
        .def("__hash__", [](const T& i) { return (uint_t) i; })

        .def("__eq__", [](const T& a, const T& b) { return a == b; })
        .def("__lt__", [](const T& a, const T& b) { return a < b; })
        .def("__le__", [](const T& a, const T& b) { return a <= b; })
        .def("__gt__", [](const T& a, const T& b) { return a > b; })
        .def("__ge__", [](const T& a, const T& b) { return a >= b; })

        .def("__repr__", [name](const T& i) { return name + "(" + std::to_string((uint_t) i) + ")"; });
}

template<typename T>
void bind_fixed_uint(nb::module_& m, const std::string& name)
{
    using value_type = typename T::value_type;

    nb::class_<T>(m, name.c_str())
        // construction
        .def(nb::init<>())  // default -> MAX sentinel in your mixin
        .def(nb::init<value_type>(), nb::arg("value"))

        // conversions / numeric protocol
        .def("__int__", [](const T& x) { return (value_type) x; })
        .def("__index__", [](const T& x) { return (value_type) x; })
        .def("__hash__", [](const T& x) { return (value_type) x; })

        // useful accessors
        .def("value", &T::value)
        .def_static("max", &T::max)
        .def_static("MAX", []() { return T::MAX; })  // Python-side constant value

        // comparisons
        .def("__eq__", [](const T& a, const T& b) { return a == b; })
        .def("__lt__", [](const T& a, const T& b) { return a < b; })
        .def("__le__", [](const T& a, const T& b) { return a <= b; })
        .def("__gt__", [](const T& a, const T& b) { return a > b; })
        .def("__ge__", [](const T& a, const T& b) { return a >= b; })

        // arithmetic (only what you implemented)
        .def(
            "__add__",
            [](const T& a, value_type b) { return a + b; },
            nb::is_operator())
        .def(
            "__sub__",
            [](const T& a, value_type b) { return a - b; },
            nb::is_operator())
        .def(
            "__iadd__",
            [](T& a, value_type b)
            {
                a = a + b;
                return a;
            },
            nb::is_operator())
        .def(
            "__isub__",
            [](T& a, value_type b)
            {
                a = a - b;
                return a;
            },
            nb::is_operator())

        // ++ convenience (Python has no ++, but you can expose an increment method)
        .def("inc",
             [](T& a)
             {
                 ++a;
                 return a;
             })

        // repr
        .def("__repr__", [name](const T& x) { return name + "(" + std::to_string((value_type) x) + ")"; });
}

}

#endif