/*
 * Copyright (C) 2023-2025 Dominik Drexler and Simon Stahlberg
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

#ifndef TYR_PYTHON_INIT_DECLARATIONS_HPP
#define TYR_PYTHON_INIT_DECLARATIONS_HPP

#include <iostream>
#include <iterator>
#include <nanobind/intrusive/ref.h>
#include <nanobind/nanobind.h>
#include <nanobind/stl/array.h>
#include <nanobind/stl/bind_map.h>     ///< TODO: implement our own with PyImmutable
#include <nanobind/stl/bind_vector.h>  ///< TODO: implement our own with PyImmutable
#include <nanobind/stl/chrono.h>
#include <nanobind/stl/filesystem.h>
#include <nanobind/stl/map.h>
#include <nanobind/stl/optional.h>
#include <nanobind/stl/pair.h>
#include <nanobind/stl/set.h>
#include <nanobind/stl/shared_ptr.h>
#include <nanobind/stl/string.h>
#include <nanobind/stl/tuple.h>
#include <nanobind/stl/unordered_map.h>
#include <nanobind/stl/unordered_set.h>
#include <nanobind/stl/variant.h>
#include <nanobind/stl/vector.h>
// Ensure mimir is included last to ensure that specializations are available
#include <tyr/tyr.hpp>

namespace nb = nanobind;
using namespace nb::literals;

/**
 * Opague types are exceptions to auto stl bindings
 */

// Formalism
// NB_MAKE_OPAQUE(mm::formalism::ActionList);

namespace tyr
{
template<typename T>
struct PyImmutable
{
    explicit PyImmutable(const T& obj) : obj_(obj) {}

    const T& obj_;  // Read-only reference
};

/**
 * init - declarations:
 */

namespace formalism
{
extern void bind_module_definitions(nb::module_& m);
}
namespace formalism::planning
{
extern void bind_module_definitions(nb::module_& m);
}
namespace planning
{
extern void bind_module_definitions(nb::module_& m);
}
}

#endif
