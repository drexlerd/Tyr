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
#include "tyr/datalog/fact_sets.hpp"  // for FactSets, Pre...
#include "tyr/formalism/planning/declarations.hpp"
#include "tyr/formalism/planning/merge_datalog.hpp"   // for MergeContext
#include "tyr/formalism/planning/merge_planning.hpp"  // for MergeContext
#include "tyr/formalism/planning/repository.hpp"      // for Repository
#include "tyr/formalism/planning/views.hpp"
#include "tyr/planning/lifted_task.hpp"  // for LiftedTag
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

static void insert_unextended_state(const UnpackedState<LiftedTag>& unpacked_state,
                                    const fp::Repository& atoms_context,
                                    fp::MergeDatalogContext& merge_context,
                                    d::TaggedFactSets<f::FluentTag>& fact_sets,
                                    d::TaggedAssignmentSets<f::FluentTag>& assignment_sets)
{
    fact_sets.reset();
    assignment_sets.reset();

    insert_fluent_atoms_to_fact_set(unpacked_state, atoms_context, merge_context, fact_sets);

    assignment_sets.insert(fact_sets);
}

static void read_derived_atoms_from_program_context(const AxiomEvaluatorProgram& axiom_program,
                                                    UnpackedState<LiftedTag>& unpacked_state,
                                                    fp::MergePlanningContext& merge_context,
                                                    d::TaggedFactSets<f::FluentTag>& fact_sets)
{
    for (const auto& set : fact_sets.predicate.get_sets())
    {
        for (const auto& binding : set.get_bindings())
        {
            if (axiom_program.get_predicate_to_predicate_mapping().contains(binding.get_relation()))
            {
                const auto ground_atom = fp::merge_atom_d2p<f::FluentTag, f::DerivedTag>(binding, merge_context).first.get_index();

                unpacked_state.set(ground_atom);
            }
        }
    }
}

AxiomEvaluator<LiftedTag>::AxiomEvaluator(std::shared_ptr<Task<LiftedTag>> task, ExecutionContextPtr execution_context) :
    m_task(std::move(task)),
    m_execution_context(std::move(execution_context)),
    m_workspace(m_task->get_axiom_program().get_program_context(),
                m_task->get_axiom_program().get_const_program_workspace(),
                d::NoOrAnnotationPolicy(),
                d::NoAndAnnotationPolicy(),
                d::NoTerminationPolicy())
{
}

std::shared_ptr<AxiomEvaluator<LiftedTag>> AxiomEvaluator<LiftedTag>::create(std::shared_ptr<Task<LiftedTag>> task, ExecutionContextPtr execution_context)
{
    return std::make_shared<AxiomEvaluator<LiftedTag>>(std::move(task), std::move(execution_context));
}

void AxiomEvaluator<LiftedTag>::compute_extended_state(UnpackedState<LiftedTag>& unpacked_state)
{
    auto merge_datalog_context = fp::MergeDatalogContext { m_workspace.datalog_builder, m_workspace.workspace_repository };

    insert_unextended_state(unpacked_state, *m_task->get_repository(), merge_datalog_context, m_workspace.facts.fact_sets, m_workspace.facts.assignment_sets);

    auto ctx = d::ProgramExecutionContext(m_workspace, m_task->get_axiom_program().get_const_program_workspace());
    ctx.clear();

    m_execution_context->arena().execute([&] { d::solve_bottom_up(ctx); });

    auto merge_planning_context = fp::MergePlanningContext { m_workspace.planning_builder, *m_task->get_repository() };

    read_derived_atoms_from_program_context(m_task->get_axiom_program(), unpacked_state, merge_planning_context, m_workspace.facts.fact_sets);
}

static_assert(AxiomEvaluatorConcept<AxiomEvaluator<LiftedTag>, LiftedTag>);

}
