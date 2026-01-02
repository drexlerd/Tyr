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

#ifndef TYR_SOLVER_BOTTOM_UP_HPP_
#define TYR_SOLVER_BOTTOM_UP_HPP_

#include "tyr/common/config.hpp"
#include "tyr/common/types.hpp"
#include "tyr/datalog/declarations.hpp"
#include "tyr/formalism/datalog/declarations.hpp"
#include "tyr/formalism/datalog/ground_atom_index.hpp"

#include <concepts>
#include <vector>

namespace tyr::datalog
{
/*
template<typename... Ts>
using Annotation = std::tuple<Ts...>;

template<typename... Ts>
using AnnotationList = std::vector<Annotation<Ts...>>;

template<typename T>
concept AnnotationPolicy = requires(T& p, Index<formalism::datalog::GroundAtom<formalism::FluentTag>> atom, uint_t cost) {
    { p.annotate(atom, cost) } -> std::same_as<void>;
};

template<typename T>
concept TerminationPolicy = requires(T& p, Index<formalism::datalog::GroundAtom<formalism::FluentTag>> atom) {
    { p.achieve(atom) } -> std::same_as<bool>;
};

struct NoAnnotationPolicy
{
    void annotate(Index<formalism::datalog::GroundAtom<formalism::FluentTag>> atom, uint_t cost) const noexcept {}
};

struct NoTerminationPolicy
{
    bool achieve(Index<formalism::datalog::GroundAtom<formalism::FluentTag>> atom) const noexcept { return false; }
};
*/

// template<AnnotationPolicy AP = NoAnnotationPolicy, TerminationPolicy TP = NoTerminationPolicy>
extern void solve_bottom_up(datalog::ProgramWorkspace& ws, const datalog::ConstProgramWorkspace& cws);

}

#endif
