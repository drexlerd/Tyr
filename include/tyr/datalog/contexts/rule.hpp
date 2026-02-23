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

#include <tbb/global_control.h>
#include <tbb/info.h>

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
            m_cws_rule(rctx.cws_rule),
            m_fact_sets(rctx.ctx.ctx.cws.facts.fact_sets, rctx.ctx.ctx.ws.facts.fact_sets)
        {
        }

        const auto& ws_rule() noexcept { return m_ws_rule; }
        const auto& ws_rule() const noexcept { return m_ws_rule; }
        const auto& cws_rule() noexcept { return m_cws_rule; }
        const auto& cws_rule() const noexcept { return m_cws_rule; }

        const auto& and_ap() noexcept { return m_and_ap; }
        const auto& and_ap() const noexcept { return m_and_ap; }
        const auto& or_annot() noexcept { return m_rctx.ctx.ctx.ws.or_annot; }
        const auto& or_annot() const noexcept { return m_rctx.ctx.ctx.ws.or_annot; }
        const auto& cost_buckets() noexcept { return m_rctx.ctx.ctx.ws.cost_buckets; }
        const auto& cost_buckets() const noexcept { return m_rctx.ctx.ctx.ws.cost_buckets; }
        const auto& program_repository() noexcept { return m_rctx.ws_rule.common.program_repository; }
        const auto& program_repository() const noexcept { return m_rctx.ws_rule.common.program_repository; }
        const auto& fact_sets() noexcept { return m_fact_sets; }
        const auto& fact_sets() const noexcept { return m_fact_sets; }

    private:
        const RuleExecutionContext<OrAP, AndAP, TP>& m_rctx;

        const AndAP& m_and_ap;
        const RuleWorkspace<AndAP>& m_ws_rule;
        const ConstRuleWorkspace& m_cws_rule;

        const FactSets m_fact_sets;
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

        auto& kpkc_workspace() noexcept { return m_ws_worker.iteration.kpkc_workspace; }
        auto& head_to_witness() noexcept { return m_ws_worker.iteration.head_to_witness; }
        auto& heads() noexcept { return m_ws_worker.iteration.heads; }

        auto& applicability_check_pool() noexcept { return m_ws_worker.solve.applicability_check_pool; }
        auto& seen_bindings_dbg() noexcept { return m_ws_worker.solve.seen_bindings_dbg; }
        auto& pending_rules() noexcept { return m_ws_worker.solve.pending_rules; }

        auto& const_ground_context_program() noexcept { return m_const_ground_context_program; }
        auto& ground_context_solve() noexcept { return m_ground_context_solve; }
        auto& ground_context_iteration() noexcept { return m_ground_context_iteration; }

    private:
        RuleWorkspace<AndAP>::Worker& m_ws_worker;

        formalism::datalog::ConstGrounderContext m_const_ground_context_program;
        formalism::datalog::GrounderContext m_ground_context_solve;
        formalism::datalog::GrounderContext m_ground_context_iteration;
    };

    RuleWorkerExecutionContext(RuleExecutionContext<OrAP, AndAP, TP>& rctx, RuleWorkspace<AndAP>::Worker& ws_worker) :
        m_rctx(rctx),
        m_ws_worker(ws_worker),
        m_in(rctx, ws_worker),
        m_out(rctx, ws_worker)
    {
    }

    /**
     * Initialization
     */

    void clear_iteration() noexcept { m_ws_worker.iteration.clear(); }
    void clear_solve() noexcept { m_ws_worker.solve.clear(); }
    void clear() noexcept
    {
        clear_iteration();
        clear_solve();
    }

    /**
     * Getters
     */

    auto& in() noexcept { return m_in; }
    const auto& in() const noexcept { return m_in; }

    auto& out() noexcept { return m_out; }
    const auto& out() const noexcept { return m_out; }

private:
    RuleExecutionContext<OrAP, AndAP, TP>& m_rctx;
    RuleWorkspace<AndAP>::Worker& m_ws_worker;

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
    }

    /**
     * Initialization
     */

    void initialize()
    {
        // std::cout << cws_rule.get_rule() << std::endl;

        ws_rule.common.initialize_iteration(cws_rule.static_consistency_graph,
                                            ctx.ctx.ws.facts.delta_fact_sets,
                                            AssignmentSets { ctx.ctx.cws.facts.assignment_sets, ctx.ctx.ws.facts.assignment_sets });
    }

    void clear_common() noexcept { ws_rule.common.clear(); }
    void clear_worker() noexcept
    {
        for (auto& worker : ws_rule.worker)
            worker.clear();
    }
    void clear_iteration() noexcept
    {
        for (auto& worker : ws_rule.worker)
            worker.iteration.clear();
    }
    void clear_solve() noexcept
    {
        for (auto& worker : ws_rule.worker)
            worker.solve.clear();
    }
    void clear() noexcept
    {
        clear_common();
        clear_worker();
    }

    /**
     * Subcontext
     */

    auto get_rule_worker_execution_context() { return RuleWorkerExecutionContext<OrAP, AndAP, TP>(*this, ws_rule.worker.local()); }

    /// Inputs
    Index<formalism::datalog::Rule> rule;
    const StratumExecutionContext<OrAP, AndAP, TP>& ctx;

    /// Workspaces
    RuleWorkspace<AndAP>& ws_rule;
    const ConstRuleWorkspace& cws_rule;
};
}

#endif
