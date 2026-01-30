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
#include "tyr/formalism/datalog/rule_index.hpp"

namespace tyr::datalog
{
template<OrAnnotationPolicyConcept OrAP, AndAnnotationPolicyConcept AndAP, TerminationPolicyConcept TP>
struct StratumExecutionContext;

template<OrAnnotationPolicyConcept OrAP, AndAnnotationPolicyConcept AndAP, TerminationPolicyConcept TP>
struct RuleExecutionContext;

template<OrAnnotationPolicyConcept OrAP = NoOrAnnotationPolicy,
         AndAnnotationPolicyConcept AndAP = NoAndAnnotationPolicy,
         TerminationPolicyConcept TP = NoTerminationPolicy>
class RuleWorkerExecutionContext
{
public:
    class In
    {
    public:
        explicit In(const RuleExecutionContext<OrAP, AndAP, TP>& rctx, const RuleWorkspace<AndAP>::Worker& ws_worker) :
            m_rctx(rctx),
            m_and_ap(ws_worker.solve.and_ap),
            m_ws_rule(rctx.ws_rule),
            m_cws_rule(rctx.cws_rule)
        {
        }

        /**
         * Contexts
         */

        const auto& pctx() noexcept { return m_rctx.ctx.ctx; }
        const auto& pctx() const noexcept { return m_rctx.ctx.ctx; }
        const auto& sctx() noexcept { return m_rctx.ctx; }
        const auto& sctx() const noexcept { return m_rctx.ctx; }
        const auto& rctx() noexcept { return m_rctx; }
        const auto& rctx() const noexcept { return m_rctx; }

        /**
         * Workspaces
         */

        const auto& ws_rule() noexcept { return m_ws_rule; }
        const auto& ws_rule() const noexcept { return m_ws_rule; }
        const auto& cws_rule() noexcept { return m_cws_rule; }
        const auto& cws_rule() const noexcept { return m_cws_rule; }

        const auto& and_ap() noexcept { return m_and_ap; }
        const auto& and_ap() const noexcept { return m_and_ap; }

    private:
        const RuleExecutionContext<OrAP, AndAP, TP>& m_rctx;

        const AndAP& m_and_ap;
        const RuleWorkspace<AndAP>& m_ws_rule;
        const ConstRuleWorkspace& m_cws_rule;
    };

    class Out
    {
    public:
        Out(RuleExecutionContext<OrAP, AndAP, TP>& rctx, RuleWorkspace<AndAP>::Worker& ws_worker) :
            m_ws_worker(ws_worker),
            m_const_ground_context_program(ws_worker.builder, rctx.ws_rule.common.program_repository, ws_worker.binding),
            m_ground_context_solve(ws_worker.builder, ws_worker.solve.stage_repository, ws_worker.binding),
            m_ground_context_iteration(ws_worker.builder, ws_worker.iteration.program_overlay_repository, ws_worker.binding)
        {
        }

        auto& ws_worker() noexcept { return m_ws_worker; }
        auto& const_ground_context_program() noexcept { return m_const_ground_context_program; }
        auto& ground_context_solve() noexcept { return m_ground_context_solve; }
        auto& ground_context_iteration() noexcept { return m_ground_context_iteration; }

    private:
        /**
         * Workspaces
         */

        RuleWorkspace<AndAP>::Worker& m_ws_worker;

        /**
         * Data
         */

        formalism::datalog::ConstGrounderContext<formalism::datalog::Repository> m_const_ground_context_program;
        formalism::datalog::GrounderContext<formalism::datalog::Repository> m_ground_context_solve;
        formalism::datalog::GrounderContext<formalism::OverlayRepository<formalism::datalog::Repository>> m_ground_context_iteration;
    };

    RuleWorkerExecutionContext(RuleExecutionContext<OrAP, AndAP, TP>& rctx, RuleWorkspace<AndAP>::Worker& ws_worker) :
        m_in(rctx, ws_worker),
        m_out(rctx, ws_worker)
    {
    }

    /**
     * Getters
     */

    auto& in() noexcept { return m_in; }
    const auto& in() const noexcept { return m_in; }

    auto& out() noexcept { return m_out; }
    const auto& out() const noexcept { return m_out; }

private:
    In m_in;
    Out m_out;
};

template<OrAnnotationPolicyConcept OrAP = NoOrAnnotationPolicy,
         AndAnnotationPolicyConcept AndAP = NoAndAnnotationPolicy,
         TerminationPolicyConcept TP = NoTerminationPolicy>
struct RuleExecutionContext
{
    RuleExecutionContext(Index<formalism::datalog::Rule> rule, StratumExecutionContext<OrAP, AndAP, TP>& ctx) :
        rule(rule),
        ctx(ctx),
        ws_rule(*ctx.ctx.ws.rules[uint_t(rule)]),
        cws_rule(ctx.ctx.cws.rules[uint_t(rule)])
    {
        for (auto& worker : ws_rule.worker)
            worker.iteration.clear();

        ws_rule.common.initialize_iteration(cws_rule.static_consistency_graph,
                                            AssignmentSets { ctx.ctx.cws.facts.assignment_sets, ctx.ctx.ws.facts.assignment_sets });
    }

    auto get_rule_worker_execution_context() { return RuleWorkerExecutionContext<OrAP, AndAP, TP>(*this, ws_rule.worker.local()); }

    auto get_fact_sets() const noexcept { return FactSets(ctx.ctx.cws.facts.fact_sets, ctx.ctx.ws.facts.fact_sets); }

    /// Inputs
    Index<formalism::datalog::Rule> rule;
    const StratumExecutionContext<OrAP, AndAP, TP>& ctx;

    /// Workspaces
    RuleWorkspace<AndAP>& ws_rule;
    const ConstRuleWorkspace& cws_rule;
};
}

#endif
