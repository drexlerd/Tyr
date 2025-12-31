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

#include "tyr/datalog/formatter.hpp"

#include "tyr/common/formatter.hpp"    // for to_string
#include "tyr/datalog/assignment.hpp"  // for EdgeAssignment, VertexAssignment
#include "tyr/datalog/consistency_graph.hpp"
#include "tyr/datalog/formatter.hpp"
#include "tyr/formalism/datalog/views.hpp"

#include <fmt/base.h>     // for vformat_to
#include <fmt/format.h>   // for format
#include <fmt/ostream.h>  // for print
#include <fmt/ranges.h>   // for join
#include <ostream>        // for ostream
#include <ranges>         // for transform, views
#include <string>         // for basic_string

namespace tyr
{
std::ostream& print(std::ostream& os, const datalog::VertexAssignment& el)
{
    fmt::print(os, "[{}/{}]", to_string(el.index), to_string(el.object));
    return os;
}

std::ostream& print(std::ostream& os, const datalog::EdgeAssignment& el)
{
    fmt::print(os, "[{}/{}, {}/{}]", to_string(el.first_index), to_string(el.first_object), to_string(el.second_index), to_string(el.second_object));
    return os;
}

std::ostream& print(std::ostream& os, const datalog::details::Vertex& el)
{
    fmt::print(os, "[{}/{}]", to_string(el.get_parameter_index()), to_string(el.get_object_index()));
    return os;
}

std::ostream& print(std::ostream& os, const datalog::details::Edge& el)
{
    fmt::print(os, "[{}, {}]", to_string(el.get_src()), to_string(el.get_dst()));
    return os;
}

std::ostream& print(std::ostream& os, const datalog::StaticConsistencyGraph& el)
{
    fmt::print(
        os,
        "graph Tree {{\n\n{}\n\n{}\n}}",
        fmt::join(el.get_vertices() | std::views::transform([&](auto&& arg) { return fmt::format("n{} [label=\"{}\"];", arg.get_index(), to_string(arg)); }),
                  "\n"),
        fmt::join(el.get_edges()
                      | std::views::transform([&](auto&& arg) { return fmt::format("n{} -- n{};", arg.get_src().get_index(), arg.get_dst().get_index()); }),
                  "\n"));

    return os;
}

namespace datalog
{
namespace details
{
std::ostream& operator<<(std::ostream& os, const Vertex& el) { return print(os, el); }

std::ostream& operator<<(std::ostream& os, const Edge& el) { return print(os, el); }

}  // end namespace details

std::ostream& operator<<(std::ostream& os, const VertexAssignment& el) { return print(os, el); }

std::ostream& operator<<(std::ostream& os, const EdgeAssignment& el) { return print(os, el); }

std::ostream& operator<<(std::ostream& os, const StaticConsistencyGraph& el) { return print(os, el); }

}  // end namespace datalog
}
