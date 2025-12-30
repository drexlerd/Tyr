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

#include "tyr/planning/lifted_task/axiom_evaluator.hpp"

#include "../task_utils.hpp"                     // for insert_fact_s...
#include "tyr/common/comparators.hpp"            // for operator!=
#include "tyr/common/equal_to.hpp"               // for EqualTo
#include "tyr/common/formatter.hpp"              // for operator<<
#include "tyr/common/hash.hpp"                   // for Hash
#include "tyr/common/vector.hpp"                 // for View
#include "tyr/formalism/overlay_repository.hpp"  // for OverlayReposi...
#include "tyr/formalism/planning/declarations.hpp"
#include "tyr/formalism/planning/merge_datalog.hpp"   // for MergeContext
#include "tyr/formalism/planning/merge_planning.hpp"  // for MergeContext
#include "tyr/formalism/planning/repository.hpp"      // for Repository
#include "tyr/formalism/planning/views.hpp"
#include "tyr/grounder/execution_contexts.hpp"
#include "tyr/grounder/fact_sets.hpp"    // for FactSets, Pre...
#include "tyr/planning/lifted_task.hpp"  // for LiftedTask
#include "tyr/planning/lifted_task.hpp"
#include "tyr/planning/lifted_task/unpacked_state.hpp"  // for UnpackedState
#include "tyr/solver/bottom_up.hpp"                     // for solve_bottom_up

#include <cista/containers/hash_storage.h>  // for operator!=
#include <gtl/phmap.hpp>                    // for operator!=
#include <utility>                          // for pair

using namespace tyr::formalism;
using namespace tyr::grounder;
using namespace tyr::solver;

namespace tyr::planning
{

static void insert_unextended_state(const UnpackedState<LiftedTask>& unpacked_state,
                                    const OverlayRepository<formalism::planning::Repository>& atoms_context,
                                    ProgramExecutionContext& axiom_context)
{
    axiom_context.facts_execution_context.reset<FluentTag>();
    axiom_context.task_to_program_execution_context.clear();

    insert_fluent_atoms_to_fact_set(unpacked_state.get_atoms<FluentTag>(), atoms_context, axiom_context);

    insert_fact_sets_into_assignment_sets(axiom_context);
}

static void read_derived_atoms_from_program_context(const AxiomEvaluatorProgram& axiom_program,
                                                    UnpackedState<LiftedTask>& unpacked_state,
                                                    formalism::OverlayRepository<formalism::planning::Repository>& task_repository,
                                                    ProgramExecutionContext& axiom_context)
{
    axiom_context.program_to_task_execution_context.clear();

    /// --- Initialize derived atoms in unpacked state

    auto merge_context = formalism::planning::MergePlanningContext { axiom_context.planning_builder,
                                                                     task_repository,
                                                                     axiom_context.program_to_task_execution_context.merge_cache };

    /// TODO: store facts by predicate such that we can swap the iteration, i.e., first over get_predicate_to_predicate_mapping, then facts of the predicate
    for (const auto fact : axiom_context.facts_execution_context.fact_sets.fluent_sets.predicate.get_facts())
    {
        if (axiom_program.get_predicate_to_predicate_mapping().contains(fact.get_predicate().get_index()))
        {
            // TODO: pass the predicate mapping here so that we can skip merging the predicate :)
            const auto ground_atom = formalism::planning::merge_d2p<formalism::FluentTag,
                                                                    formalism::datalog::Repository,
                                                                    formalism::OverlayRepository<formalism::planning::Repository>,
                                                                    formalism::DerivedTag>(fact, merge_context)
                                         .first;

            unpacked_state.set(ground_atom);
        }
    }
}

AxiomEvaluator<LiftedTask>::AxiomEvaluator(std::shared_ptr<LiftedTask> task) :
    m_task(task),
    m_axiom_context(task->get_axiom_program().get_program(),
                    task->get_axiom_program().get_repository(),
                    task->get_axiom_program().get_domains(),
                    task->get_axiom_program().get_strata(),
                    task->get_axiom_program().get_listeners())
{
}

void AxiomEvaluator<LiftedTask>::compute_extended_state(UnpackedState<LiftedTask>& unpacked_state)
{
    insert_unextended_state(unpacked_state, *m_task->get_repository(), m_axiom_context);

    solve_bottom_up(m_axiom_context);

    read_derived_atoms_from_program_context(m_task->get_axiom_program(), unpacked_state, *m_task->get_repository(), m_axiom_context);
}

}
