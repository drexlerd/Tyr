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
template<typename OrAP, typename AndAP, typename TP>
ProgramWorkspace<OrAP, AndAP, TP>::ProgramWorkspace(ProgramContext& context, const ConstProgramWorkspace& cws, OrAP or_ap, AndAP and_ap, TP tp) :
    program_repository(context.get_program_repository()),
    workspace_repository(context.get_workspace_repository()),
    facts(context.get_program().get_predicates<formalism::FluentTag>(),
          context.get_program().get_functions<formalism::FluentTag>(),
          context.get_domains().fluent_predicate_domains,
          context.get_domains().fluent_function_domains,
          context.get_program().get_objects().size(),
          context.get_program().get_atoms<formalism::FluentTag>(),
          context.get_program().get_fterm_values<formalism::FluentTag>(),
          context.get_workspace_repository()),
    or_ap(or_ap),
    or_annot(context.get_program().get_predicates<formalism::FluentTag>().size()),
    and_annot(),
    tp(tp),
    rules(),
    d2p(),
    planning_builder(),
    datalog_builder(),
    schedulers(create_schedulers(context.get_strata(), context.get_listeners(), program_repository)),
    cost_buckets(),
    statistics()
{
    for (uint_t i = 0; i < context.get_program().get_rules().size(); ++i)
        rules.emplace_back(
            std::make_unique<RuleWorkspace<AndAP>>(context.get_repository_factory(), program_repository, workspace_repository, cws.rules[i], and_ap));
}

template struct ProgramWorkspace<NoOrAnnotationPolicy, NoAndAnnotationPolicy, NoTerminationPolicy>;
template struct ProgramWorkspace<OrAnnotationPolicy, AndAnnotationPolicy<SumAggregation>, NoTerminationPolicy>;
template struct ProgramWorkspace<OrAnnotationPolicy, AndAnnotationPolicy<SumAggregation>, TerminationPolicy<SumAggregation>>;
template struct ProgramWorkspace<OrAnnotationPolicy, AndAnnotationPolicy<MaxAggregation>, NoTerminationPolicy>;
template struct ProgramWorkspace<OrAnnotationPolicy, AndAnnotationPolicy<MaxAggregation>, TerminationPolicy<MaxAggregation>>;

ConstProgramWorkspace::ConstProgramWorkspace(ProgramContext& context) :
    facts(context.get_program().get_predicates<formalism::StaticTag>(),
          context.get_program().get_functions<formalism::StaticTag>(),
          context.get_domains().static_predicate_domains,
          context.get_domains().static_function_domains,
          context.get_program().get_objects().size(),
          context.get_program().get_atoms<formalism::StaticTag>(),
          context.get_program().get_fterm_values<formalism::StaticTag>(),
          context.get_program_repository()),
    rules()
{
    rules.reserve(context.get_program().get_rules().size());  // Ensure enough space to avoid move on reallocation
    for (uint_t i = 0; i < context.get_program().get_rules().size(); ++i)
        rules.emplace_back(context.get_program().get_rules()[i],
                           context.get_workspace_repository(),
                           context.get_domains().rule_domains[i],
                           context.get_program().get_objects().size(),
                           context.get_program().get_predicates<formalism::FluentTag>().size(),
                           facts.assignment_sets);
}
}
