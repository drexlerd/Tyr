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

#include "tyr/common/comparators.hpp"                // for operator!=
#include "tyr/common/config.hpp"                     // for uint_t, flo...
#include "tyr/common/formatter.hpp"                  // for operator<<
#include "tyr/common/iostream.hpp"                   // for print_indent
#include "tyr/common/types.hpp"                      // for make_view
#include "tyr/common/variant.hpp"                    // for visit
#include "tyr/formalism/overlay_repository.hpp"      // for OverlayRepo...
#include "tyr/formalism/planning/fdr_fact_data.hpp"  // for Data
#include "tyr/formalism/planning/fdr_value.hpp"      // for FDRValue
#include "tyr/formalism/planning/formatter.hpp"      // for operator<<
#include "tyr/formalism/planning/repository.hpp"     // for Repository
#include "tyr/formalism/planning/views.hpp"
#include "tyr/planning/algorithms/gbfs_lazy/statistics.hpp"  // for Statistics
#include "tyr/planning/declarations.hpp"
#include "tyr/planning/domain.hpp"             // for Domain
#include "tyr/planning/ground_task.hpp"        // for GroundTask
#include "tyr/planning/ground_task/node.hpp"   // for Node
#include "tyr/planning/ground_task/state.hpp"  // for State
#include "tyr/planning/ground_task/unpacked_state.hpp"
#include "tyr/planning/lifted_task.hpp"        // for LiftedTask
#include "tyr/planning/lifted_task/node.hpp"   // for Node
#include "tyr/planning/lifted_task/state.hpp"  // for State
#include "tyr/planning/lifted_task/unpacked_state.hpp"

#include <boost/dynamic_bitset.hpp>  // for dynamic_bitset
#include <cmath>                     // for isnan
#include <fmt/base.h>                // for vformat_to
#include <fmt/ostream.h>             // for print
#include <memory>                    // for __shared_pt...
#include <ostream>                   // for char_traits
#include <string>                    // for basic_string
#include <tuple>                     // for operator==
#include <utility>                   // for pair
#include <vector>                    // for vector

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
    fmt::print(os, "{}", to_string(el.get_task()));
    return os;
}

std::ostream& print(std::ostream& os, const planning::Node<planning::LiftedTask>& el)
{
    os << "Node(\n";
    {
        IndentScope scope(os);

        os << print_indent << "metric value = " << el.get_metric() << "\n";

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

    auto static_atoms = IndexList<formalism::planning::GroundAtom<formalism::StaticTag>> {};
    for (auto i = static_atoms_bitset.find_first(); i != boost::dynamic_bitset<>::npos; i = static_atoms_bitset.find_next(i))
        static_atoms.push_back(Index<formalism::planning::GroundAtom<formalism::StaticTag>>(i));

    auto fluent_atoms = IndexList<formalism::planning::GroundAtom<formalism::FluentTag>> {};
    for (auto i = fluent_atoms_bitset.find_first(); i != boost::dynamic_bitset<>::npos; i = fluent_atoms_bitset.find_next(i))
        fluent_atoms.push_back(Index<formalism::planning::GroundAtom<formalism::FluentTag>>(i));

    auto derived_atoms = IndexList<formalism::planning::GroundAtom<formalism::DerivedTag>> {};
    for (auto i = derived_atoms_bitset.find_first(); i != boost::dynamic_bitset<>::npos; i = derived_atoms_bitset.find_next(i))
        derived_atoms.push_back(Index<formalism::planning::GroundAtom<formalism::DerivedTag>>(i));

    auto static_fterm_values = std::vector<
        std::pair<View<Index<formalism::planning::GroundFunctionTerm<formalism::StaticTag>>, formalism::OverlayRepository<formalism::planning::Repository>>,
                  float_t>> {};
    for (uint_t i = 0; i < static_numeric_variables.size(); ++i)
        if (!std::isnan(static_numeric_variables[i]))
            static_fterm_values.emplace_back(make_view(Index<formalism::planning::GroundFunctionTerm<formalism::StaticTag>>(i), context),
                                             static_numeric_variables[i]);

    auto fluent_fterm_values = std::vector<
        std::pair<View<Index<formalism::planning::GroundFunctionTerm<formalism::FluentTag>>, formalism::OverlayRepository<formalism::planning::Repository>>,
                  float_t>> {};
    for (uint_t i = 0; i < fluent_numeric_variables.size(); ++i)
        if (!std::isnan(fluent_numeric_variables[i]))
            fluent_fterm_values.emplace_back(make_view(Index<formalism::planning::GroundFunctionTerm<formalism::FluentTag>>(i), context),
                                             fluent_numeric_variables[i]);

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

std::ostream& print(std::ostream& os, const planning::Node<planning::GroundTask>& el)
{
    os << "Node(\n";
    {
        IndentScope scope(os);

        os << print_indent << "metric value = " << el.get_metric() << "\n";

        os << print_indent << "state = " << el.get_state() << "\n";
    }
    os << print_indent << ")";

    return os;
}

std::ostream& print(std::ostream& os, const planning::PackedState<planning::GroundTask>& el) { return os; }

std::ostream& print(std::ostream& os, const planning::UnpackedState<planning::GroundTask>& el) { return os; }

std::ostream& print(std::ostream& os, const planning::State<planning::GroundTask>& el)
{
    const auto& context = *el.get_task().get_repository();

    const auto& static_atoms_bitset = el.template get_atoms<formalism::StaticTag>();
    const auto& fluent_values = el.get_fluent_values();
    const auto& derived_atoms_bitset = el.template get_atoms<formalism::DerivedTag>();
    const auto& static_numeric_variables = el.template get_numeric_variables<formalism::StaticTag>();
    const auto& fluent_numeric_variables = el.template get_numeric_variables<formalism::FluentTag>();

    auto static_atoms = IndexList<formalism::planning::GroundAtom<formalism::StaticTag>> {};
    for (auto i = static_atoms_bitset.find_first(); i != boost::dynamic_bitset<>::npos; i = static_atoms_bitset.find_next(i))
        static_atoms.push_back(Index<formalism::planning::GroundAtom<formalism::StaticTag>>(i));

    auto fluent_facts = DataList<formalism::planning::FDRFact<formalism::FluentTag>> {};
    for (uint_t i = 0; i < fluent_values.size(); ++i)
        if (fluent_values[i] != formalism::planning::FDRValue::none())
            fluent_facts.push_back(
                Data<formalism::planning::FDRFact<formalism::FluentTag>>(Index<formalism::planning::FDRVariable<formalism::FluentTag>>(i), fluent_values[i]));
    auto derived_atoms = IndexList<formalism::planning::GroundAtom<formalism::DerivedTag>> {};
    for (auto i = derived_atoms_bitset.find_first(); i != boost::dynamic_bitset<>::npos; i = derived_atoms_bitset.find_next(i))
        derived_atoms.push_back(Index<formalism::planning::GroundAtom<formalism::DerivedTag>>(i));

    auto static_fterm_values = std::vector<
        std::pair<View<Index<formalism::planning::GroundFunctionTerm<formalism::StaticTag>>, formalism::OverlayRepository<formalism::planning::Repository>>,
                  float_t>> {};
    for (uint_t i = 0; i < static_numeric_variables.size(); ++i)
        if (!std::isnan(static_numeric_variables[i]))
            static_fterm_values.emplace_back(make_view(Index<formalism::planning::GroundFunctionTerm<formalism::StaticTag>>(i), context),
                                             static_numeric_variables[i]);

    auto fluent_fterm_values = std::vector<
        std::pair<View<Index<formalism::planning::GroundFunctionTerm<formalism::FluentTag>>, formalism::OverlayRepository<formalism::planning::Repository>>,
                  float_t>> {};
    for (uint_t i = 0; i < fluent_numeric_variables.size(); ++i)
        if (!std::isnan(fluent_numeric_variables[i]))
            fluent_fterm_values.emplace_back(make_view(Index<formalism::planning::GroundFunctionTerm<formalism::FluentTag>>(i), context),
                                             fluent_numeric_variables[i]);

    os << "State(\n";
    {
        IndentScope scope(os);

        os << print_indent << "static atoms = " << make_view(static_atoms, context) << "\n";

        os << print_indent << "fluent facts = " << make_view(fluent_facts, context) << "\n";

        os << print_indent << "derived atoms = " << make_view(derived_atoms, context) << "\n";

        os << print_indent << "static numeric variables = " << static_fterm_values << "\n";

        os << print_indent << "fluent numeric variables = " << fluent_fterm_values << "\n";
    }

    os << print_indent << ")";

    return os;
}

std::ostream& print(std::ostream& os, const planning::gbfs_lazy::Statistics& el)
{
    fmt::print(os,
               "[GBFS] Search time: {}ms\n"
               "[GBFS] Number of generated states: {}\n"
               "[GBFS] Number of expanded states: {}\n"
               "[GBFS] Number of pruned states: {}",
               el.get_search_time_ms().count(),
               el.get_num_generated(),
               el.get_num_expanded(),
               el.get_num_pruned());

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

std::ostream& operator<<(std::ostream& os, const Node<GroundTask>& el) { return tyr::print(os, el); }

std::ostream& operator<<(std::ostream& os, const PackedState<GroundTask>& el) { return tyr::print(os, el); }

std::ostream& operator<<(std::ostream& os, const UnpackedState<GroundTask>& el) { return tyr::print(os, el); }

std::ostream& operator<<(std::ostream& os, const State<GroundTask>& el) { return tyr::print(os, el); }

namespace gbfs_lazy
{
std::ostream& operator<<(std::ostream& os, const Statistics& el) { return tyr::print(os, el); }
}  // end gbfs_lazy
}
}
