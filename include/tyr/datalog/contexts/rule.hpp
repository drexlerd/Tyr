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

#ifndef TYR_DATALOG_CONTEXTS_RULE_HPP_
#define TYR_DATALOG_CONTEXTS_RULE_HPP_

#include "tyr/datalog/assignment_sets.hpp"
#include "tyr/datalog/declarations.hpp"
#include "tyr/datalog/fact_sets.hpp"
#include "tyr/datalog/policies/annotation.hpp"
#include "tyr/datalog/policies/termination.hpp"
#include "tyr/datalog/workspaces/rule.hpp"
#include "tyr/datalog/workspaces/rule_delta.hpp"
#include "tyr/datalog/workspaces/worker.hpp"
#include "tyr/formalism/datalog/rule_index.hpp"

namespace tyr::datalog
{
template<OrAnnotationPolicyConcept OrAP, AndAnnotationPolicyConcept AndAP, TerminationPolicyConcept TP>
struct StratumExecutionContext;

template<OrAnnotationPolicyConcept OrAP = NoOrAnnotationPolicy,
         AndAnnotationPolicyConcept AndAP = NoAndAnnotationPolicy,
         TerminationPolicyConcept TP = NoTerminationPolicy>
struct RuleExecutionContext
{
    RuleExecutionContext(Index<formalism::datalog::Rule> rule, StratumExecutionContext<OrAP, AndAP, TP>& ctx) :
        rule(rule),
        ctx(ctx),
        ws_rule(ctx.ctx.ws.rules[uint_t(rule)]),
        cws_rule(ctx.ctx.cws.rules[uint_t(rule)]),
        ws_rule_delta(ctx.ctx.ws.rule_deltas[uint_t(rule)]),
        ws_rule_persistent(ctx.ctx.ws.rule_persistents[uint_t(rule)]),
        ws_worker(ctx.ctx.ws.worker.local()),
        and_ap(ctx.ctx.aps.and_aps[uint_t(rule)]),
        and_annot(ctx.ctx.aps.and_annots[uint_t(rule)]),
        delta_head_to_witness(ctx.ctx.aps.delta_head_to_witness[uint_t(rule)]),
        fact_sets(FactSets(ctx.ctx.cws.facts.fact_sets, ctx.ctx.ws.facts.fact_sets)),
        ground_context_delta(formalism::datalog::GrounderContext { ws_worker.builder, *ws_rule_delta.repository, ws_rule_delta.binding }),
        ground_context_iteration(formalism::datalog::GrounderContext { ws_worker.builder, ws_rule.overlay_repository, ws_rule_delta.binding }),
        ground_context_persistent(formalism::datalog::GrounderContext { ws_worker.builder, ws_rule_persistent.overlay_repository, ws_rule_delta.binding })
    {
        ws_worker.clear();
        ws_rule.clear();
        ws_rule.initialize(cws_rule.static_consistency_graph, AssignmentSets { ctx.ctx.cws.facts.assignment_sets, ctx.ctx.ws.facts.assignment_sets });
    }

    /// Inputs
    Index<formalism::datalog::Rule> rule;
    StratumExecutionContext<OrAP, AndAP, TP>& ctx;

    /// Workspaces
    RuleIterationWorkspace& ws_rule;
    const ConstRuleWorkspace& cws_rule;
    RuleDeltaWorkspace& ws_rule_delta;
    RulePersistentWorkspace& ws_rule_persistent;
    WorkerWorkspace& ws_worker;

    /// Annotations
    AndAP& and_ap;
    AndAnnotationsMap& and_annot;
    HeadToWitness& delta_head_to_witness;

    // Derivatives
    FactSets fact_sets;
    formalism::datalog::GrounderContext<formalism::datalog::Repository> ground_context_delta;
    formalism::datalog::GrounderContext<formalism::OverlayRepository<formalism::datalog::Repository>> ground_context_iteration;
    formalism::datalog::GrounderContext<formalism::OverlayRepository<formalism::datalog::Repository>> ground_context_persistent;
};
}

#endif
