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

#include "tyr/grounder/formatter.hpp"

#include "tyr/common/formatter.hpp"  // for to_string
#include "tyr/formalism/views.hpp"
#include "tyr/grounder/assignment.hpp"  // for EdgeAssignment, VertexAssignment
#include "tyr/grounder/consistency_graph.hpp"
#include "tyr/grounder/formatter.hpp"

#include <fmt/base.h>     // for vformat_to
#include <fmt/format.h>   // for format
#include <fmt/ostream.h>  // for print
#include <fmt/ranges.h>   // for join
#include <ostream>        // for ostream
#include <ranges>         // for transform, views
#include <string>         // for basic_string

namespace tyr
{
std::ostream& print(std::ostream& os, const grounder::VertexAssignment& el)
{
    fmt::print(os, "[{}/{}]", to_string(el.index), to_string(el.object));
    return os;
}

std::ostream& print(std::ostream& os, const grounder::EdgeAssignment& el)
{
    fmt::print(os, "[{}/{}, {}/{}]", to_string(el.first_index), to_string(el.first_object), to_string(el.second_index), to_string(el.second_object));
    return os;
}

template<formalism::Context C>
std::ostream& print(std::ostream& os, const grounder::details::Vertex<C>& el)
{
    fmt::print(os, "[{}/{}]", to_string(el.get_parameter_index()), to_string(el.get_object_index()));
    return os;
}

template std::ostream& print(std::ostream& os, const grounder::details::Vertex<formalism::Repository>& el);

template<formalism::Context C>
std::ostream& print(std::ostream& os, const grounder::details::Edge<C>& el)
{
    fmt::print(os, "[{}, {}]", to_string(el.get_src()), to_string(el.get_dst()));
    return os;
}

template std::ostream& print(std::ostream& os, const grounder::details::Edge<formalism::Repository>& el);

template<formalism::Context C>
std::ostream& print(std::ostream& os, const grounder::StaticConsistencyGraph<C>& el)
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

template std::ostream& print(std::ostream& os, const grounder::StaticConsistencyGraph<formalism::Repository>& el);

namespace grounder
{
namespace details
{
template<formalism::Context C>
std::ostream& operator<<(std::ostream& os, const Vertex<C>& el)
{
    return print(os, el);
}

template std::ostream& operator<<(std::ostream& os, const Vertex<formalism::Repository>& el);

template<formalism::Context C>
std::ostream& operator<<(std::ostream& os, const Edge<C>& el)
{
    return print(os, el);
}

template std::ostream& operator<<(std::ostream& os, const Edge<formalism::Repository>& el);

}  // end namespace details

std::ostream& operator<<(std::ostream& os, const VertexAssignment& el) { return print(os, el); }

std::ostream& operator<<(std::ostream& os, const EdgeAssignment& el) { return print(os, el); }

template<formalism::Context C>
std::ostream& operator<<(std::ostream& os, const StaticConsistencyGraph<C>& el)
{
    return print(os, el);
}

template std::ostream& operator<<(std::ostream& os, const StaticConsistencyGraph<formalism::Repository>& el);

}  // end namespace grounder
}
