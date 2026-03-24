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

namespace tyr
{
namespace
{
void bind_execution_context(nb::module_& m)
{
    nb::class_<ExecutionContext>(m, "ExecutionContext")
        .def(nb::new_([](size_t num_threads) { return ExecutionContext::create(num_threads); }), "num_threads")
        .def_prop_ro("num_threads", &ExecutionContext::get_num_threads);
}
}

void bind_module_definitions(nb::module_& m) { bind_execution_context(m); }
}
