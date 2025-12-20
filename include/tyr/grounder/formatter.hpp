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

#ifndef TYR_GROUNDER_FORMATTER_HPP_
#define TYR_GROUNDER_FORMATTER_HPP_

#include "tyr/common/formatter.hpp"
#include "tyr/grounder/declarations.hpp"

namespace tyr
{
extern std::ostream& print(std::ostream& os, const grounder::VertexAssignment& el);

extern std::ostream& print(std::ostream& os, const grounder::EdgeAssignment& el);

template<formalism::Context C>
std::ostream& print(std::ostream& os, const grounder::details::Vertex<C>& el);

template<formalism::Context C>
std::ostream& print(std::ostream& os, const grounder::details::Edge<C>& el);

template<formalism::Context C>
std::ostream& print(std::ostream& os, const grounder::StaticConsistencyGraph<C>& el);

namespace grounder
{
namespace details
{
template<formalism::Context C>
std::ostream& operator<<(std::ostream& os, const Vertex<C>& el);

template<formalism::Context C>
std::ostream& operator<<(std::ostream& os, const Edge<C>& el);
}  // end namespace details

extern std::ostream& operator<<(std::ostream& os, const VertexAssignment& el);

extern std::ostream& operator<<(std::ostream& os, const EdgeAssignment& el);

template<formalism::Context C>
std::ostream& operator<<(std::ostream& os, const StaticConsistencyGraph<C>& el);
}  // end namespace grounder
}

#endif