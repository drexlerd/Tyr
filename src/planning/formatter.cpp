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

#include "tyr/common/chrono.hpp"
#include "tyr/common/comparators.hpp"                // for operator!=
#include "tyr/common/config.hpp"                     // for uint_t, flo...
#include "tyr/common/formatter.hpp"                  // for operator<<
#include "tyr/common/iostream.hpp"                   // for print_indent
#include "tyr/common/types.hpp"                      // for make_view
#include "tyr/common/variant.hpp"                    // for visit
#include "tyr/formalism/planning/fdr_fact_data.hpp"  // for Data
#include "tyr/formalism/planning/fdr_value.hpp"      // for FDRValue
#include "tyr/formalism/planning/formatter.hpp"      // for operator<<
#include "tyr/formalism/planning/repository.hpp"     // for Repository
#include "tyr/formalism/planning/views.hpp"
#include "tyr/planning/algorithms/statistics.hpp"  // for Statistics
#include "tyr/planning/declarations.hpp"
#include "tyr/planning/ground_task.hpp"       // for GroundTask
#include "tyr/planning/ground_task/node.hpp"  // for Node
#include "tyr/planning/ground_task/state_repository.hpp"
#include "tyr/planning/ground_task/state_view.hpp"  // for State
#include "tyr/planning/ground_task/unpacked_state.hpp"
#include "tyr/planning/lifted_task.hpp"
#include "tyr/planning/lifted_task/node.hpp"  // for Node
#include "tyr/planning/lifted_task/state_repository.hpp"
#include "tyr/planning/lifted_task/state_view.hpp"  // for State
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

std::ostream& print(std::ostream& os, const Data<planning::State<planning::LiftedTag>>& el) { return os; }

std::ostream& print(std::ostream& os, const planning::UnpackedState<planning::LiftedTag>& el) { return os; }

std::ostream& print(std::ostream& os, const Data<planning::State<planning::GroundTag>>& el) { return os; }

std::ostream& print(std::ostream& os, const planning::UnpackedState<planning::GroundTag>& el) { return os; }

std::ostream& print(std::ostream& os, const planning::Statistics& el)
{
    fmt::print(os,
               "[Search] Search time: {} ms ({} ns)\n"
               "[Search] Number of expanded states: {}\n"
               "[Search] Number of generated states: {}\n"
               "[Search] Number of pruned states: {}",
               to_ms(el.get_search_time()),
               to_ns(el.get_search_time()),
               el.get_num_expanded(),
               el.get_num_generated(),
               el.get_num_pruned());

    return os;
}

template<planning::TaskKind Kind>
std::ostream& print(std::ostream& os, const planning::StateView<Kind>& el)
{
    auto static_atoms = std::vector<formalism::planning::GroundAtomView<formalism::StaticTag>> {};
    for (auto&& atom : el.get_static_atoms_view())
        static_atoms.push_back(atom);

    auto fluent_facts = std::vector<formalism::planning::FDRFactView<formalism::FluentTag>> {};
    for (auto&& fact : el.get_fluent_facts_view())
        if (fact.has_value())
            fluent_facts.push_back(fact);

    auto derived_atoms = std::vector<formalism::planning::GroundAtomView<formalism::DerivedTag>> {};
    for (auto&& atom : el.get_derived_atoms_view())
        derived_atoms.push_back(atom);

    auto static_fterm_values = std::vector<formalism::planning::GroundFunctionTermViewValuePair<formalism::StaticTag>> {};
    for (auto&& fterm_value : el.get_static_fterm_values_view())
        static_fterm_values.push_back(fterm_value);

    auto fluent_fterm_values = std::vector<formalism::planning::GroundFunctionTermViewValuePair<formalism::FluentTag>> {};
    for (auto&& fterm_value : el.get_fluent_fterm_values_view())
        fluent_fterm_values.push_back(fterm_value);

    os << "State(\n";
    {
        IndentScope scope(os);

        os << print_indent << "index = " << el.get_index() << "\n";

        os << print_indent << "static atoms = " << static_atoms << "\n";

        os << print_indent << "fluent atoms = " << fluent_facts << "\n";

        os << print_indent << "derived atoms = " << derived_atoms << "\n";

        os << print_indent << "static numeric variables = " << static_fterm_values << "\n";

        os << print_indent << "fluent numeric variables = " << fluent_fterm_values << "\n";
    }

    os << print_indent << ")";

    return os;
}

template std::ostream& print(std::ostream& os, const planning::StateView<planning::LiftedTag>& el);
template std::ostream& print(std::ostream& os, const planning::StateView<planning::GroundTag>& el);

template<planning::TaskKind Kind>
std::ostream& print(std::ostream& os, const planning::Node<Kind>& el)
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

template std::ostream& print(std::ostream& os, const planning::Node<planning::LiftedTag>& el);
template std::ostream& print(std::ostream& os, const planning::Node<planning::GroundTag>& el);

template<planning::TaskKind Kind>
std::ostream& print(std::ostream& os, const planning::Plan<Kind>& el)
{
    for (const auto& labeled_node : el.get_labeled_succ_nodes())
        os << std::make_pair(labeled_node.label, formalism::planning::PlanFormatting()) << "\n";

    return os;
}

template std::ostream& print(std::ostream& os, const planning::Plan<planning::LiftedTag>& el);
template std::ostream& print(std::ostream& os, const planning::Plan<planning::GroundTag>& el);

namespace planning
{

std::ostream& operator<<(std::ostream& os, const LiftedTask& el) { return tyr::print(os, el); }

std::ostream& operator<<(std::ostream& os, const GroundTask& el) { return tyr::print(os, el); }

std::ostream& operator<<(std::ostream& os, const Data<State<LiftedTag>>& el) { return tyr::print(os, el); }

std::ostream& operator<<(std::ostream& os, const UnpackedState<LiftedTag>& el) { return tyr::print(os, el); }

std::ostream& operator<<(std::ostream& os, const Data<State<GroundTag>>& el) { return tyr::print(os, el); }

std::ostream& operator<<(std::ostream& os, const UnpackedState<GroundTag>& el) { return tyr::print(os, el); }

std::ostream& operator<<(std::ostream& os, const Statistics& el) { return tyr::print(os, el); }

template<TaskKind Kind>
std::ostream& operator<<(std::ostream& os, const StateView<Kind>& el)
{
    return tyr::print(os, el);
}

template std::ostream& operator<<(std::ostream& os, const StateView<LiftedTag>& el);
template std::ostream& operator<<(std::ostream& os, const StateView<GroundTag>& el);

template<TaskKind Kind>
std::ostream& operator<<(std::ostream& os, const Node<Kind>& el)
{
    return tyr::print(os, el);
}

template std::ostream& operator<<(std::ostream& os, const Node<LiftedTag>& el);
template std::ostream& operator<<(std::ostream& os, const Node<GroundTag>& el);

template<TaskKind Kind>
std::ostream& operator<<(std::ostream& os, const Plan<Kind>& el)
{
    return tyr::print(os, el);
}

template std::ostream& operator<<(std::ostream& os, const Plan<LiftedTag>& el);
template std::ostream& operator<<(std::ostream& os, const Plan<GroundTag>& el);

}
}
