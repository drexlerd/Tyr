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
        ws_rule_iter(ctx.ctx.ws.rules_iter[uint_t(rule)]),
        cws_rule(ctx.ctx.cws.rules[uint_t(rule)]),
        ws_rule_solve(ctx.ctx.ws.rules_solve[uint_t(rule)]),
        ws_worker(ctx.ctx.ws.worker.local()),
        and_ap(ctx.ctx.aps.and_aps[uint_t(rule)]),
        and_annot(ctx.ctx.aps.and_annots[uint_t(rule)]),
        delta_head_to_witness(ctx.ctx.aps.delta_head_to_witness[uint_t(rule)])
    {
        ws_worker.clear();
        ws_rule_iter.clear();
        ws_rule_iter.initialize(cws_rule.static_consistency_graph, AssignmentSets { ctx.ctx.cws.facts.assignment_sets, ctx.ctx.ws.facts.assignment_sets });
    }

    /// Inputs
    Index<formalism::datalog::Rule> rule;
    StratumExecutionContext<OrAP, AndAP, TP>& ctx;

    /// Workspaces
    RuleWorkspace& ws_rule;
    RuleIterationWorkspace& ws_rule_iter;
    const ConstRuleWorkspace& cws_rule;
    RuleSolveWorkspace& ws_rule_solve;
    WorkerWorkspace& ws_worker;

    /// Annotations
    AndAP& and_ap;
    AndAnnotationsMap& and_annot;
    HeadToWitness& delta_head_to_witness;

    auto get_fact_sets() const noexcept { return FactSets(ctx.ctx.cws.facts.fact_sets, ctx.ctx.ws.facts.fact_sets); }
    auto get_ground_context_solve() const noexcept
    {
        return formalism::datalog::GrounderContext { ws_worker.builder, *ws_rule_solve.repository, ws_worker.binding };
    }
    auto get_ground_context_iter() const noexcept
    {
        return formalism::datalog::GrounderContext { ws_worker.builder, ws_rule_iter.overlay_repository, ws_worker.binding };
    }
    auto get_ground_context_program() const noexcept
    {
        return formalism::datalog::ConstGrounderContext { ws_worker.builder, ctx.ctx.ws.repository, ws_worker.binding };
    }
};
}

#endif
