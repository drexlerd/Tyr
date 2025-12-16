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

#include "tyr/planning/formatter.hpp"

#include "tyr/common/formatter.hpp"
#include "tyr/common/iostream.hpp"
#include "tyr/formalism/formatter.hpp"
#include "tyr/formalism/overlay_repository.hpp"
#include "tyr/formalism/repository.hpp"
#include "tyr/formalism/views.hpp"
#include "tyr/planning/declarations.hpp"
#include "tyr/planning/domain.hpp"
#include "tyr/planning/ground_task/node.hpp"
#include "tyr/planning/ground_task/state.hpp"
#include "tyr/planning/lifted_task.hpp"
#include "tyr/planning/lifted_task/node.hpp"
#include "tyr/planning/lifted_task/state.hpp"
#include "tyr/planning/lifted_task/unpacked_state.hpp"

#include <boost/dynamic_bitset.hpp>
#include <fmt/core.h>
#include <fmt/ostream.h>
#include <fmt/ranges.h>
#include <ostream>

namespace tyr
{
std::ostream& print(std::ostream& os, const planning::Domain& el)
{
    fmt::print(os, "{}", to_string(el.get_domain()));
    return os;
}

std::ostream& print(std::ostream& os, const planning::LiftedTask& el)
{
    fmt::print(os, "{}", to_string(el.get_task()));
    return os;
}

std::ostream& print(std::ostream& os, const planning::GroundTask& el)
{
    // fmt::print(os, "{}", to_string(el.get_task()));
    return os;
}

std::ostream& print(std::ostream& os, const planning::Node<planning::LiftedTask>& el)
{
    os << "Node(\n";
    {
        IndentScope scope(os);

        os << print_indent << "metric value = " << el.get_state_metric() << "\n";

        os << print_indent << "state = " << el.get_state() << "\n";
    }
    os << print_indent << ")";

    return os;
}

std::ostream& print(std::ostream& os, const planning::PackedState<planning::LiftedTask>& el) { return os; }

std::ostream& print(std::ostream& os, const planning::UnpackedState<planning::LiftedTask>& el) { return os; }

std::ostream& print(std::ostream& os, const planning::State<planning::LiftedTask>& el)
{
    const auto& context = *el.get_task().get_repository();

    const auto& static_atoms_bitset = el.template get_atoms<formalism::StaticTag>();
    const auto& fluent_atoms_bitset = el.template get_atoms<formalism::FluentTag>();
    const auto& derived_atoms_bitset = el.template get_atoms<formalism::DerivedTag>();
    const auto& static_numeric_variables = el.template get_numeric_variables<formalism::StaticTag>();
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

    auto static_fterm_values = std::vector<
        std::pair<View<Index<formalism::GroundFunctionTerm<formalism::StaticTag>>, formalism::OverlayRepository<formalism::Repository>>, float_t>> {};
    for (uint_t i = 0; i < static_numeric_variables.size(); ++i)
        if (!std::isnan(static_numeric_variables[i]))
            static_fterm_values.emplace_back(make_view(Index<formalism::GroundFunctionTerm<formalism::StaticTag>>(i), context), static_numeric_variables[i]);

    auto fluent_fterm_values = std::vector<
        std::pair<View<Index<formalism::GroundFunctionTerm<formalism::FluentTag>>, formalism::OverlayRepository<formalism::Repository>>, float_t>> {};
    for (uint_t i = 0; i < fluent_numeric_variables.size(); ++i)
        if (!std::isnan(fluent_numeric_variables[i]))
            fluent_fterm_values.emplace_back(make_view(Index<formalism::GroundFunctionTerm<formalism::FluentTag>>(i), context), fluent_numeric_variables[i]);

    os << "State(\n";
    {
        IndentScope scope(os);

        os << print_indent << "static atoms = " << make_view(static_atoms, context) << "\n";

        os << print_indent << "fluent atoms = " << make_view(fluent_atoms, context) << "\n";

        os << print_indent << "derived atoms = " << make_view(derived_atoms, context) << "\n";

        os << print_indent << "static numeric variables = " << static_fterm_values << "\n";

        os << print_indent << "fluent numeric variables = " << fluent_fterm_values << "\n";
    }

    os << print_indent << ")";

    return os;
}

namespace planning
{
std::ostream& operator<<(std::ostream& os, const Domain& el) { return tyr::print(os, el); }

std::ostream& operator<<(std::ostream& os, const LiftedTask& el) { return tyr::print(os, el); }

std::ostream& operator<<(std::ostream& os, const GroundTask& el) { return tyr::print(os, el); }

std::ostream& operator<<(std::ostream& os, const Node<LiftedTask>& el) { return tyr::print(os, el); }

std::ostream& operator<<(std::ostream& os, const PackedState<LiftedTask>& el) { return tyr::print(os, el); }

std::ostream& operator<<(std::ostream& os, const UnpackedState<LiftedTask>& el) { return tyr::print(os, el); }

std::ostream& operator<<(std::ostream& os, const State<LiftedTask>& el) { return tyr::print(os, el); }
}
}
