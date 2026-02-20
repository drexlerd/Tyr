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

#ifndef TYR_DATALOG_CONTEXTS_STRATUM_HPP_
#define TYR_DATALOG_CONTEXTS_STRATUM_HPP_

#include "tyr/datalog/contexts/rule.hpp"
#include "tyr/datalog/declarations.hpp"
#include "tyr/datalog/policies/annotation.hpp"
#include "tyr/datalog/policies/termination.hpp"

namespace tyr::datalog
{
template<OrAnnotationPolicyConcept OrAP, AndAnnotationPolicyConcept AndAP, TerminationPolicyConcept TP>
struct ProgramExecutionContext;

template<OrAnnotationPolicyConcept OrAP = NoOrAnnotationPolicy,
         AndAnnotationPolicyConcept AndAP = NoAndAnnotationPolicy,
         TerminationPolicyConcept TP = NoTerminationPolicy>
struct StratumExecutionContext
{
    StratumExecutionContext(RuleSchedulerStratum& scheduler, const ProgramExecutionContext<OrAP, AndAP, TP>& ctx) : scheduler(scheduler), ctx(ctx)
    {
        // Initialize delta facts
        ctx.ws.facts.delta_fact_sets.insert(ctx.ws.facts.fact_sets);
    }

    /**
     * Initialization
     */

    /**
     * Subcontext
     */

    auto get_rule_execution_context(Index<formalism::datalog::Rule> rule) { return RuleExecutionContext<OrAP, AndAP, TP> { rule, *this }; }

    RuleSchedulerStratum& scheduler;
    const ProgramExecutionContext<OrAP, AndAP, TP>& ctx;
};
}

#endif
