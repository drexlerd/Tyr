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

#include "tyr/datalog/workspaces/program.hpp"

namespace a = tyr::analysis;
namespace f = tyr::formalism;
namespace fd = tyr::formalism::datalog;

namespace tyr::datalog
{
ConstProgramWorkspace::ConstProgramWorkspace(ProgramContext& context) :
    facts(context.get_program().get_predicates<formalism::StaticTag>(),
          context.get_program().get_functions<formalism::StaticTag>(),
          context.get_domains().static_predicate_domains,
          context.get_domains().static_function_domains,
          context.get_program().get_objects().size(),
          context.get_program().get_atoms<formalism::StaticTag>(),
          context.get_program().get_fterm_values<formalism::StaticTag>()),
    rules()
{
    rules.reserve(context.get_program().get_rules().size());  // Ensure enough space to avoid move on reallocation
    for (uint_t i = 0; i < context.get_program().get_rules().size(); ++i)
        rules.emplace_back(context.get_program().get_rules()[i].get_index(),
                           context.get_repository(),
                           context.get_domains().rule_domains[i],
                           context.get_program().get_objects().size(),
                           context.get_program().get_predicates<formalism::FluentTag>().size(),
                           facts.assignment_sets);
}
}
