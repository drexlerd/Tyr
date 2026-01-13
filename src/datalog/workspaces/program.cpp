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

ProgramWorkspace::ProgramWorkspace(ProgramContext& context, const ConstProgramWorkspace& cws) :
    repository(context.get_repository()),
    facts(context.get_program().get_predicates<formalism::FluentTag>(),
          context.get_program().get_functions<formalism::FluentTag>(),
          context.get_domains().fluent_predicate_domains,
          context.get_domains().fluent_function_domains,
          context.get_program().get_objects().size(),
          context.get_program().get_atoms<formalism::FluentTag>(),
          context.get_program().get_fterm_values<formalism::FluentTag>()),
    rules(),
    rule_deltas(context.get_program().get_rules().size()),
    rule_persistents(),
    d2p(),
    worker(),
    planning_builder(),
    datalog_builder(),
    schedulers(create_schedulers(context.get_strata(), context.get_listeners(), context.get_repository())),
    cost_buckets(),
    statistics()
{
    for (uint_t i = 0; i < context.get_program().get_rules().size(); ++i)
    {
        rules.emplace_back(context.get_repository(), cws.rules[i].static_consistency_graph);
        rule_persistents.emplace_back(context.get_repository());
    }
}

void ProgramWorkspace::clear() noexcept { cost_buckets.clear(); }

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
    for (uint_t i = 0; i < context.get_program().get_rules().size(); ++i)
        rules.emplace_back(context.get_program().get_rules()[i].get_index(),
                           context.get_repository(),
                           context.get_domains().rule_domains[i],
                           facts.assignment_sets);
}

}
