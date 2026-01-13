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

#include "tyr/common/comparators.hpp"  // for operator!=
#include "tyr/common/equal_to.hpp"     // for EqualTo
#include "tyr/common/formatter.hpp"    // for operator<<
#include "tyr/common/hash.hpp"         // for Hash
#include "tyr/common/vector.hpp"       // for View
#include "tyr/datalog/bottom_up.hpp"   // for solve_bottom_up
#include "tyr/datalog/contexts/program.hpp"
#include "tyr/datalog/fact_sets.hpp"             // for FactSets, Pre...
#include "tyr/formalism/overlay_repository.hpp"  // for OverlayReposi...
#include "tyr/formalism/planning/declarations.hpp"
#include "tyr/formalism/planning/merge_datalog.hpp"   // for MergeContext
#include "tyr/formalism/planning/merge_planning.hpp"  // for MergeContext
#include "tyr/formalism/planning/repository.hpp"      // for Repository
#include "tyr/formalism/planning/views.hpp"
#include "tyr/planning/lifted_task.hpp"  // for LiftedTask
#include "tyr/planning/lifted_task.hpp"
#include "tyr/planning/lifted_task/unpacked_state.hpp"  // for UnpackedState
#include "tyr/planning/task_utils.hpp"                  // for insert_fact_s...

#include <cista/containers/hash_storage.h>  // for operator!=
#include <gtl/phmap.hpp>                    // for operator!=
#include <utility>                          // for pair

namespace d = tyr::datalog;
namespace f = tyr::formalism;
namespace fd = tyr::formalism::datalog;
namespace fp = tyr::formalism::planning;

namespace tyr::planning
{

static void insert_unextended_state(const UnpackedState<LiftedTask>& unpacked_state,
                                    const f::OverlayRepository<fp::Repository>& atoms_context,
                                    d::ProgramWorkspace& ws,
                                    const d::ConstProgramWorkspace& cws)
{
    ws.facts.reset();

    insert_fluent_atoms_to_fact_set(unpacked_state.get_atoms<f::FluentTag>(), atoms_context, ws);

    insert_fact_sets_into_assignment_sets(ws, cws);
}

static void read_derived_atoms_from_program_context(const AxiomEvaluatorProgram& axiom_program,
                                                    UnpackedState<LiftedTask>& unpacked_state,
                                                    f::OverlayRepository<fp::Repository>& task_repository,
                                                    d::ProgramWorkspace& ws)
{
    ws.d2p.clear();

    /// --- Initialize derived atoms in unpacked state

    auto merge_context = fp::MergePlanningContext { ws.planning_builder, task_repository };

    /// TODO: store facts by predicate such that we can swap the iteration, i.e., first over get_predicate_to_predicate_mapping, then facts of the predicate
    for (const auto& set : ws.facts.fact_sets.predicate.get_sets())
    {
        for (const auto& fact : set.get_facts())
        {
            if (axiom_program.get_predicate_to_predicate_mapping().contains(fact.get_predicate().get_index()))
            {
                // TODO: pass the predicate mapping here so that we can skip merging the predicate :)
                const auto ground_atom =
                    fp::merge_d2p<f::FluentTag, fd::Repository, f::OverlayRepository<fp::Repository>, f::DerivedTag>(fact, merge_context).first;

                unpacked_state.set(ground_atom);
            }
        }
    }
}

AxiomEvaluator<LiftedTask>::AxiomEvaluator(std::shared_ptr<LiftedTask> task) :
    m_task(std::move(task)),
    m_workspace(m_task->get_axiom_program().get_program_context(), m_task->get_axiom_program().get_const_program_workspace()),
    m_aps(d::NoOrAnnotationPolicy(),
          std::vector<d::NoAndAnnotationPolicy>(m_workspace.rule_deltas.size()),
          d::OrAnnotationsList(),
          std::vector<d::AndAnnotationsMap>(m_workspace.rule_deltas.size()),
          std::vector<d::HeadToWitness>(m_workspace.rule_deltas.size())),
    m_tp(d::NoTerminationPolicy())
{
}

void AxiomEvaluator<LiftedTask>::compute_extended_state(UnpackedState<LiftedTask>& unpacked_state)
{
    insert_unextended_state(unpacked_state, *m_task->get_repository(), m_workspace, m_task->get_axiom_program().get_const_program_workspace());

    auto ctx = d::ProgramExecutionContext(m_workspace, m_task->get_axiom_program().get_const_program_workspace(), m_aps, m_tp);

    d::solve_bottom_up(ctx);

    read_derived_atoms_from_program_context(m_task->get_axiom_program(), unpacked_state, *m_task->get_repository(), m_workspace);
}

}
