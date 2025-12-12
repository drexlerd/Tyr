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

#ifndef TYR_GROUNDER_GENERATOR_HPP_
#define TYR_GROUNDER_GENERATOR_HPP_

#include "tyr/grounder/declarations.hpp"

namespace tyr::grounder
{

extern void ground_nullary_case(const FactsExecutionContext& fact_execution_context,
                                RuleExecutionContext& rule_execution_context,
                                RuleStageExecutionContext& rule_stage_execution_context,
                                ThreadExecutionContext& thread_execution_context);

extern void ground_unary_case(const FactsExecutionContext& fact_execution_context,
                              RuleExecutionContext& rule_execution_context,
                              RuleStageExecutionContext& rule_stage_execution_context,
                              ThreadExecutionContext& thread_execution_context);

extern void ground_general_case(const FactsExecutionContext& fact_execution_context,
                                RuleExecutionContext& rule_execution_context,
                                RuleStageExecutionContext& rule_stage_execution_context,
                                ThreadExecutionContext& thread_execution_context);

extern void ground(const FactsExecutionContext& fact_execution_context,
                   RuleExecutionContext& rule_execution_context,
                   RuleStageExecutionContext& rule_stage_execution_context,
                   ThreadExecutionContext& thread_execution_context);

}

#endif
