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

#ifndef TYR_PLANNING_FORMATTER_HPP_
#define TYR_PLANNING_FORMATTER_HPP_

#include "tyr/common/formatter.hpp"
#include "tyr/formalism/formatter.hpp"
#include "tyr/planning/declarations.hpp"
#include "tyr/planning/node.hpp"
#include "tyr/planning/state.hpp"

#include <boost/dynamic_bitset.hpp>
#include <fmt/core.h>
#include <fmt/ostream.h>
#include <fmt/ranges.h>
#include <ostream>

namespace tyr
{
extern std::ostream& print(std::ostream& os, const planning::Domain& el);

extern std::ostream& print(std::ostream& os, const planning::LiftedTask& el);

extern std::ostream& print(std::ostream& os, const planning::GroundTask& el);

template<typename Task>
std::ostream& print(std::ostream& os, const planning::Node<Task>& el)
{
    fmt::print(os, "Node({}, metric={})", to_string(el.get_state()), el.get_state_metric());
    return os;
}

template<typename Task>
std::ostream& print(std::ostream& os, const planning::PackedState<Task>& el)
{
    return os;
}

template<typename Task>
std::ostream& print(std::ostream& os, const planning::UnpackedState<Task>& el)
{
    return os;
}

template<typename Task>
std::ostream& print(std::ostream& os, const planning::State<Task>& el)
{
    const auto& context = *el.get_task().get_repository();

    const auto& static_atoms_bitset = el.template get_atoms<formalism::StaticTag>();
    const auto& fluent_atoms_bitset = el.template get_atoms<formalism::FluentTag>();
    const auto& derived_atoms_bitset = el.template get_atoms<formalism::DerivedTag>();
    const auto& fluent_numeric_variables = el.template get_numeric_variables<formalism::FluentTag>();

    auto static_atoms = IndexList<formalism::GroundAtom<formalism::StaticTag>> {};
    for (auto i = static_atoms_bitset.find_first(); i != boost::dynamic_bitset<>::npos; i = static_atoms_bitset.find_next(i))
        static_atoms.push_back(Index<formalism::GroundAtom<formalism::StaticTag>>(i));

    auto fluent_atoms = IndexList<formalism::GroundAtom<formalism::FluentTag>> {};
    for (auto i = fluent_atoms_bitset.find_first(); i != boost::dynamic_bitset<>::npos; i = fluent_atoms_bitset.find_next(i))
        fluent_atoms.push_back(Index<formalism::GroundAtom<formalism::FluentTag>>(i));

    auto derived_atoms = IndexList<formalism::GroundAtom<formalism::DerivedTag>> {};
    for (auto i = derived_atoms_bitset.find_first(); i != boost::dynamic_bitset<>::npos; i = derived_atoms_bitset.find_next(i))
        derived_atoms.push_back(Index<formalism::GroundAtom<formalism::DerivedTag>>(i));

    fmt::print(
        os,
        "State(static atoms={}, fluent atoms={}, derived atoms={})",
        to_string(View<IndexList<formalism::GroundAtom<formalism::StaticTag>>, formalism::OverlayRepository<formalism::Repository>>(static_atoms, context)),
        to_string(View<IndexList<formalism::GroundAtom<formalism::FluentTag>>, formalism::OverlayRepository<formalism::Repository>>(fluent_atoms, context)),
        to_string(View<IndexList<formalism::GroundAtom<formalism::DerivedTag>>, formalism::OverlayRepository<formalism::Repository>>(derived_atoms, context)));

    return os;
}

namespace planning
{
extern std::ostream& operator<<(std::ostream& os, const Domain& el);

extern std::ostream& operator<<(std::ostream& os, const LiftedTask& el);

extern std::ostream& operator<<(std::ostream& os, const GroundTask& el);

template<typename Task>
std::ostream& print(std::ostream& os, const Node<Task>& el)
{
    return tyr::print(os, el);
}

template<typename Task>
std::ostream& print(std::ostream& os, const PackedState<Task>& el)
{
    return tyr::print(os, el);
}

template<typename Task>
std::ostream& print(std::ostream& os, const UnpackedState<Task>& el)
{
    return tyr::print(os, el);
}

template<typename Task>
std::ostream& print(std::ostream& os, const State<Task>& el)
{
    return tyr::print(os, el);
}
}
}

#endif
