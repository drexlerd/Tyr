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

#include "../utils.hpp"

#include <gtest/gtest.h>
#include <oneapi/tbb/enumerable_thread_specific.h>
#include <oneapi/tbb/global_control.h>
#include <oneapi/tbb/parallel_for.h>
#include <tyr/analysis/analysis.hpp>
#include <tyr/formalism/formalism.hpp>
#include <tyr/grounder/grounder.hpp>

using namespace tyr::buffer;
using namespace tyr::formalism;

namespace tyr::tests
{

TEST(TyrTests, TyrGrounderGenerator)
{
    auto [program, repository] = create_example_problem();

    std::cout << program << std::endl;

    /**
     * Analysis 1
     */

    auto domains = analysis::compute_variable_domains(program);
    auto strata = analysis::compute_rule_stratification(program);
    auto listeners = analysis::compute_listeners(strata, program.get_context());

    /**
     * Allocation 1: Execution contexts
     */

    auto program_execution_context = grounder::ProgramExecutionContext(program, repository, domains, strata, listeners);

    /**
     * Parallelization 1: Lock-free rule grounding
     */

    const uint_t num_rules = program.get_rules().size();

    tbb::parallel_for(uint_t { 0 },
                      num_rules,
                      [&](uint_t i)
                      {
                          auto& facts_execution_context = program_execution_context.facts_execution_context;
                          auto& rule_execution_context = program_execution_context.rule_execution_contexts[i];
                          auto& rule_stage_execution_context = program_execution_context.rule_stage_execution_contexts[i];
                          auto& thread_execution_context = program_execution_context.thread_execution_contexts.local();  // thread-local
                          thread_execution_context.clear();

                          grounder::ground(facts_execution_context, rule_execution_context, rule_stage_execution_context, thread_execution_context);
                      });
}
}