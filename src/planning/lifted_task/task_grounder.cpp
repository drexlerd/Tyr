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

#include "tyr/planning/lifted_task/task_grounder.hpp"

#include "tyr/analysis/domains.hpp"
#include "tyr/common/dynamic_bitset.hpp"
#include "tyr/common/vector.hpp"
#include "tyr/datalog/bottom_up.hpp"
#include "tyr/datalog/contexts/program.hpp"
#include "tyr/datalog/policies/annotation.hpp"
#include "tyr/datalog/policies/termination.hpp"
#include "tyr/datalog/workspaces/program.hpp"
#include "tyr/formalism/canonicalization.hpp"
#include "tyr/formalism/planning/builder.hpp"
#include "tyr/formalism/planning/formatter.hpp"
#include "tyr/formalism/planning/grounder.hpp"
#include "tyr/formalism/planning/merge.hpp"
#include "tyr/formalism/planning/repository.hpp"
#include "tyr/formalism/planning/views.hpp"
#include "tyr/planning/applicability.hpp"
#include "tyr/planning/declarations.hpp"
#include "tyr/planning/domain.hpp"
#include "tyr/planning/ground_task.hpp"
#include "tyr/planning/lifted_task.hpp"
#include "tyr/planning/programs/ground.hpp"
#include "tyr/planning/task_utils.hpp"

namespace d = tyr::datalog;
namespace f = tyr::formalism;
namespace fp = tyr::formalism::planning;

namespace tyr::planning
{
static auto remap_fdr_fact(View<Data<fp::FDRFact<f::FluentTag>>, fp::Repository> fact,
                           fp::GeneralFDRContext<fp::Repository>& fdr_context,
                           fp::MergeContext<fp::Repository>& context)
{
    // Ensure that remapping is unambiguous
    assert(fact.get_variable().get_domain_size() == 2);

    auto new_atom = merge_p2p(fact.get_variable().get_atoms().front(), context).first;
    auto new_fact = fdr_context.get_fact(new_atom);

    // value 1 -> keep positive fact
    if (fact.get_value() != fp::FDRValue::none())
        return new_fact;

    // value 0 -> same variable, value 0
    new_fact.value = fp::FDRValue::none();

    return new_fact;
}

static auto create_ground_fdr_conjunctive_condition(View<Index<fp::GroundConjunctiveCondition>, fp::Repository> element,
                                                    fp::GeneralFDRContext<fp::Repository>& fdr_context,
                                                    fp::MergeContext<fp::Repository>& context)
{
    auto fdr_conj_cond_ptr = context.builder.get_builder<fp::GroundConjunctiveCondition>();
    auto& fdr_conj_cond = *fdr_conj_cond_ptr;
    fdr_conj_cond.clear();

    for (const auto literal : element.get_facts<f::StaticTag>())
        fdr_conj_cond.static_literals.push_back(merge_p2p(literal, context).first);

    for (const auto fact : element.get_facts<f::FluentTag>())
        fdr_conj_cond.fluent_facts.push_back(remap_fdr_fact(fact, fdr_context, context));

    for (const auto literal : element.get_facts<f::DerivedTag>())
        fdr_conj_cond.derived_literals.push_back(merge_p2p(literal, context).first);

    for (const auto numeric_constraint : element.get_numeric_constraints())
        fdr_conj_cond.numeric_constraints.push_back(merge_p2p(numeric_constraint, context));

    canonicalize(fdr_conj_cond);
    return context.destination.get_or_create(fdr_conj_cond, context.builder.get_buffer());
}

static auto create_ground_conjunctive_effect(View<Index<fp::GroundConjunctiveEffect>, fp::Repository> element,
                                             fp::GeneralFDRContext<fp::Repository>& fdr_context,
                                             fp::MergeContext<fp::Repository>& context)
{
    auto fdr_conj_eff_ptr = context.builder.get_builder<fp::GroundConjunctiveEffect>();
    auto& fdr_conj_eff = *fdr_conj_eff_ptr;
    fdr_conj_eff.clear();

    auto assign = UnorderedMap<Index<fp::FDRVariable<f::FluentTag>>, fp::FDRValue> {};

    for (const auto fact : element.get_facts())
        fdr_conj_eff.facts.push_back(remap_fdr_fact(fact, fdr_context, context));

    for (const auto numeric_effect : element.get_numeric_effects())
        fdr_conj_eff.numeric_effects.push_back(merge_p2p(numeric_effect, context));

    if (element.get_auxiliary_numeric_effect().has_value())
        fdr_conj_eff.auxiliary_numeric_effect = merge_p2p(element.get_auxiliary_numeric_effect().value(), context);

    canonicalize(fdr_conj_eff);
    return context.destination.get_or_create(fdr_conj_eff, context.builder.get_buffer());
}

static auto create_ground_conditional_effect(View<Index<fp::GroundConditionalEffect>, fp::Repository> element,
                                             fp::GeneralFDRContext<fp::Repository>& fdr_context,
                                             fp::MergeContext<fp::Repository>& context)
{
    auto fdr_cond_eff_ptr = context.builder.get_builder<fp::GroundConditionalEffect>();
    auto& fdr_cond_eff = *fdr_cond_eff_ptr;
    fdr_cond_eff.clear();

    fdr_cond_eff.condition = create_ground_fdr_conjunctive_condition(element.get_condition(), fdr_context, context).first;
    fdr_cond_eff.effect = create_ground_conjunctive_effect(element.get_effect(), fdr_context, context).first;

    canonicalize(fdr_cond_eff);
    return context.destination.get_or_create(fdr_cond_eff, context.builder.get_buffer());
}

static auto create_ground_action(View<Index<fp::GroundAction>, fp::Repository> element,
                                 fp::GeneralFDRContext<fp::Repository>& fdr_context,
                                 fp::MergeContext<fp::Repository>& context)
{
    auto fdr_action_ptr = context.builder.get_builder<fp::GroundAction>();
    auto& fdr_action = *fdr_action_ptr;
    fdr_action.clear();

    fdr_action.binding = merge_p2p(element.get_binding(), context).first;
    fdr_action.action = element.get_action().get_index();
    fdr_action.condition = create_ground_fdr_conjunctive_condition(element.get_condition(), fdr_context, context).first;
    for (const auto cond_eff : element.get_effects())
        fdr_action.effects.push_back(create_ground_conditional_effect(cond_eff, fdr_context, context).first);

    canonicalize(fdr_action);
    return context.destination.get_or_create(fdr_action, context.builder.get_buffer());
}

static auto create_ground_axiom(View<Index<fp::GroundAxiom>, fp::Repository> element,
                                fp::GeneralFDRContext<fp::Repository>& fdr_context,
                                fp::MergeContext<fp::Repository>& context)
{
    auto fdr_axiom_ptr = context.builder.get_builder<fp::GroundAxiom>();
    auto& fdr_axiom = *fdr_axiom_ptr;
    fdr_axiom.clear();

    fdr_axiom.binding = merge_p2p(element.get_binding(), context).first;
    fdr_axiom.axiom = element.get_axiom().get_index();
    fdr_axiom.body = create_ground_fdr_conjunctive_condition(element.get_body(), fdr_context, context).first;
    fdr_axiom.head = merge_p2p(element.get_head(), context).first;

    canonicalize(fdr_axiom);
    return context.destination.get_or_create(fdr_axiom, context.builder.get_buffer());
}

// TODO: create stronger mutex groups
static auto create_mutex_groups(View<IndexList<fp::GroundAtom<f::FluentTag>>, fp::Repository> atoms, fp::MergeContext<fp::Repository>& context)
{
    auto mutex_groups = std::vector<std::vector<View<Index<fp::GroundAtom<f::FluentTag>>, fp::Repository>>> {};

    for (const auto atom : atoms)
    {
        auto group = std::vector<View<Index<fp::GroundAtom<f::FluentTag>>, fp::Repository>> {};
        group.push_back(make_view(merge_p2p(atom, context).first, context.destination));
        mutex_groups.push_back(group);
    }

    return mutex_groups;
}

static auto create_task(View<Index<fp::Task>, fp::Repository> task,
                        View<IndexList<fp::GroundAtom<f::FluentTag>>, fp::Repository> fluent_atoms,
                        View<IndexList<fp::GroundAtom<f::DerivedTag>>, fp::Repository> derived_atoms,
                        View<IndexList<fp::GroundFunctionTerm<f::FluentTag>>, fp::Repository> fluent_fterms,
                        View<IndexList<fp::GroundAction>, fp::Repository> actions,
                        View<IndexList<fp::GroundAxiom>, fp::Repository> axioms,
                        fp::Repository& repository)
{
    auto builder = fp::Builder();
    auto fdr_task_ptr = builder.get_builder<fp::FDRTask>();
    auto& fdr_task = *fdr_task_ptr;
    fdr_task.clear();

    auto merge_context = fp::MergeContext { builder, repository };

    fdr_task.name = task.get_name();
    fdr_task.domain = task.get_domain().get_index();
    for (const auto predicate : task.get_derived_predicates())
        fdr_task.derived_predicates.push_back(merge_p2p(predicate, merge_context).first);
    for (const auto object : task.get_objects())
        fdr_task.objects.push_back(merge_p2p(object, merge_context).first);

    for (const auto atom : task.get_atoms<f::StaticTag>())
        fdr_task.static_atoms.push_back(merge_p2p(atom, merge_context).first);
    for (const auto atom : fluent_atoms)
        fdr_task.fluent_atoms.push_back(merge_p2p(atom, merge_context).first);
    for (const auto atom : derived_atoms)
        fdr_task.derived_atoms.push_back(merge_p2p(atom, merge_context).first);
    for (const auto fterm : fluent_fterms)
        fdr_task.fluent_fterms.push_back(merge_p2p(fterm, merge_context).first);

    for (const auto fterm_value : task.get_fterm_values<f::StaticTag>())
        fdr_task.static_fterm_values.push_back(merge_p2p(fterm_value, merge_context).first);
    for (const auto fterm_value : task.get_fterm_values<f::FluentTag>())
        fdr_task.fluent_fterm_values.push_back(merge_p2p(fterm_value, merge_context).first);
    if (task.get_auxiliary_fterm_value().has_value())
        fdr_task.auxiliary_fterm_value = merge_p2p(task.get_auxiliary_fterm_value().value(), merge_context).first;
    if (task.get_metric())
        fdr_task.metric = merge_p2p(task.get_metric().value(), merge_context).first;
    for (const auto axiom : task.get_axioms())
        fdr_task.axioms.push_back(merge_p2p(axiom, merge_context).first);

    /// --- Create FDR context
    auto mutex_groups = create_mutex_groups(fluent_atoms, merge_context);
    auto fdr_context = fp::GeneralFDRContext<fp::Repository>(mutex_groups, repository);

    /// --- Create FDR variables
    for (const auto variable : fdr_context.get_variables())
        fdr_task.fluent_variables.push_back(variable.get_index());

    /// --- Create FDR fluent facts
    for (const auto atom : task.get_atoms<f::FluentTag>())
        fdr_task.fluent_facts.push_back(fdr_context.get_fact(merge_p2p(atom, merge_context).first));

    /// --- Create FDR goal
    fdr_task.goal = create_ground_fdr_conjunctive_condition(task.get_goal(), fdr_context, merge_context).first;

    /// --- Create FDR actions and axioms
    for (const auto action : actions)
        fdr_task.ground_actions.push_back(create_ground_action(action, fdr_context, merge_context).first);
    for (const auto axiom : axioms)
        fdr_task.ground_axioms.push_back(create_ground_axiom(axiom, fdr_context, merge_context).first);

    canonicalize(fdr_task);

    return std::make_pair(make_view(repository.get_or_create(fdr_task, builder.get_buffer()).first, repository), std::move(fdr_context));
}

static auto create_fdr_task(DomainPtr domain,
                            View<Index<fp::Task>, fp::Repository> task,
                            IndexList<fp::GroundAtom<f::FluentTag>> fluent_atoms,
                            IndexList<fp::GroundAtom<f::DerivedTag>> derived_atoms,
                            IndexList<fp::GroundFunctionTerm<f::FluentTag>> fluent_fterms,
                            IndexList<fp::GroundAction> actions,
                            IndexList<fp::GroundAxiom> axioms)
{
    auto task_repository = std::make_shared<fp::Repository>(domain->get_repository().get());

    const auto [fdr_task, fdr_context] = create_task(task,
                                                     make_view(fluent_atoms, task.get_context()),
                                                     make_view(derived_atoms, task.get_context()),
                                                     make_view(fluent_fterms, task.get_context()),
                                                     make_view(actions, task.get_context()),
                                                     make_view(axioms, task.get_context()),
                                                     *task_repository);

    return std::make_shared<GroundTask>(domain, task_repository, fdr_task, fdr_context);
}

GroundTaskPtr ground_task(LiftedTask& lifted_task)
{
    auto ground_program = GroundTaskProgram(lifted_task.get_task());

    // std::cout << lifted_task.get_task().get_domain() << std::endl;
    // std::cout << lifted_task.get_task() << std::endl;

    // std::cout << ground_program.get_program_context().get_program() << std::endl;

    const auto const_workspace = d::ConstProgramWorkspace(ground_program.get_program_context());

    auto workspace = d::ProgramWorkspace<d::NoOrAnnotationPolicy, d::NoAndAnnotationPolicy, d::NoTerminationPolicy>(ground_program.get_program_context(),
                                                                                                                    const_workspace,
                                                                                                                    d::NoOrAnnotationPolicy(),
                                                                                                                    d::NoAndAnnotationPolicy(),
                                                                                                                    d::NoTerminationPolicy());

    auto ctx = d::ProgramExecutionContext(workspace, const_workspace);
    ctx.clear();

    d::solve_bottom_up(ctx);

    workspace.d2p.clear();

    auto fluent_assign = UnorderedMap<Index<fp::FDRVariable<f::FluentTag>>, fp::FDRValue> {};
    auto derived_assign = UnorderedMap<Index<fp::GroundAtom<f::DerivedTag>>, bool> {};
    auto iter_workspace = itertools::cartesian_set::Workspace<Index<f::Object>> {};

    /// --- Ground Atoms

    auto fluent_atoms_set = UnorderedSet<Index<fp::GroundAtom<f::FluentTag>>>();
    auto derived_atoms_set = UnorderedSet<Index<fp::GroundAtom<f::DerivedTag>>>();
    auto fluent_fterms_set = UnorderedSet<Index<fp::GroundFunctionTerm<f::FluentTag>>>();
    // TODO: collect fluent function terms

    for (const auto atom : lifted_task.get_task().get_atoms<f::FluentTag>())
        fluent_atoms_set.insert(atom.get_index());

    // Collect the goal facts
    for (const auto fact : lifted_task.get_task().get_goal().get_facts<f::FluentTag>())
        for (const auto atom : fact.get_variable().get_atoms())
            fluent_atoms_set.insert(atom.get_index());

    for (const auto literal : lifted_task.get_task().get_goal().get_facts<f::DerivedTag>())
        derived_atoms_set.insert(literal.get_atom().get_index());

    /// --- Ground Actions

    auto ground_actions_set = UnorderedSet<Index<fp::GroundAction>> {};

    auto static_atoms_bitset = boost::dynamic_bitset<>();
    for (const auto atom : lifted_task.get_task().get_atoms<f::StaticTag>())
        set(uint_t(atom.get_index()), true, static_atoms_bitset);

    /// TODO: store facts by predicate such that we can swap the iteration, i.e., first over get_predicate_to_actions_mapping, then facts of the predicate
    for (const auto& set : workspace.facts.fact_sets.predicate.get_sets())
    {
        for (const auto& fact : set.get_facts())
        {
            const auto& mapping = ground_program.get_predicate_to_actions_mapping();

            if (const auto it = mapping.find(fact.get_predicate().get_index()); it != mapping.end())
            {
                const auto action_index = it->second;

                workspace.d2p.binding = fact.get_objects().get_data();
                auto grounder_context = fp::GrounderContext { workspace.planning_builder, *lifted_task.get_repository(), workspace.d2p.binding };

                const auto action = make_view(action_index, grounder_context.destination);

                const auto ground_action_index = fp::ground(action,
                                                            grounder_context,
                                                            lifted_task.get_parameter_domains_per_cond_effect_per_action()[action_index.get_value()],
                                                            fluent_assign,
                                                            iter_workspace,
                                                            *lifted_task.get_fdr_context())
                                                     .first;

                const auto ground_action = make_view(ground_action_index, grounder_context.destination);

                assert(is_statically_applicable(ground_action, static_atoms_bitset));

                if (is_consistent(ground_action, fluent_assign, derived_assign))
                {
                    ground_actions_set.insert(ground_action.get_index());

                    for (const auto fact : ground_action.get_condition().get_facts<f::FluentTag>())
                        for (const auto atom : fact.get_variable().get_atoms())
                            fluent_atoms_set.insert(atom.get_index());

                    for (const auto literal : ground_action.get_condition().get_facts<f::DerivedTag>())
                        derived_atoms_set.insert(literal.get_atom().get_index());

                    for (const auto cond_effect : ground_action.get_effects())
                    {
                        for (const auto fact : cond_effect.get_condition().get_facts<f::FluentTag>())
                            for (const auto atom : fact.get_variable().get_atoms())
                                fluent_atoms_set.insert(atom.get_index());

                        for (const auto literal : cond_effect.get_condition().get_facts<f::DerivedTag>())
                            derived_atoms_set.insert(literal.get_atom().get_index());

                        for (const auto fact : cond_effect.get_effect().get_facts())
                            for (const auto atom : fact.get_variable().get_atoms())
                                fluent_atoms_set.insert(atom.get_index());
                    }
                }
            }
        }
    }

    /// --- Ground Axioms

    auto ground_axioms_set = UnorderedSet<Index<fp::GroundAxiom>> {};

    /// TODO: store facts by predicate such that we can swap the iteration, i.e., first over get_predicate_to_axioms_mapping, then facts of the predicate
    for (const auto& set : workspace.facts.fact_sets.predicate.get_sets())
    {
        for (const auto& fact : set.get_facts())
        {
            const auto& mapping = ground_program.get_predicate_to_axioms_mapping();

            if (const auto it = mapping.find(fact.get_predicate().get_index()); it != mapping.end())
            {
                const auto axiom_index = it->second;

                workspace.d2p.binding = fact.get_objects().get_data();
                auto grounder_context = fp::GrounderContext { workspace.planning_builder, *lifted_task.get_repository(), workspace.d2p.binding };

                const auto axiom = make_view(axiom_index, grounder_context.destination);

                const auto ground_axiom_index = fp::ground(axiom, grounder_context, *lifted_task.get_fdr_context()).first;

                const auto ground_axiom = make_view(ground_axiom_index, grounder_context.destination);

                assert(is_statically_applicable(ground_axiom, static_atoms_bitset));

                if (is_consistent(ground_axiom, fluent_assign, derived_assign))
                {
                    ground_axioms_set.insert(ground_axiom.get_index());

                    for (const auto fact : ground_axiom.get_body().get_facts<f::FluentTag>())
                        for (const auto atom : fact.get_variable().get_atoms())
                            fluent_atoms_set.insert(atom.get_index());

                    for (const auto literal : ground_axiom.get_body().get_facts<f::DerivedTag>())
                        derived_atoms_set.insert(literal.get_atom().get_index());

                    derived_atoms_set.insert(ground_axiom.get_head().get_index());
                }
            }
        }
    }

    auto fluent_atoms = IndexList<fp::GroundAtom<f::FluentTag>>(fluent_atoms_set.begin(), fluent_atoms_set.end());
    auto derived_atoms = IndexList<fp::GroundAtom<f::DerivedTag>>(derived_atoms_set.begin(), derived_atoms_set.end());
    auto fluent_fterms = IndexList<fp::GroundFunctionTerm<f::FluentTag>>(fluent_fterms_set.begin(), fluent_fterms_set.end());
    auto ground_actions = IndexList<fp::GroundAction>(ground_actions_set.begin(), ground_actions_set.end());
    auto ground_axioms = IndexList<fp::GroundAxiom>(ground_axioms_set.begin(), ground_axioms_set.end());

    canonicalize(fluent_atoms);
    canonicalize(derived_atoms);
    canonicalize(fluent_fterms);
    canonicalize(ground_actions);
    canonicalize(ground_axioms);

    std::cout << "Num fluent atoms: " << fluent_atoms.size() << std::endl;
    std::cout << "Num derived atoms: " << derived_atoms.size() << std::endl;
    std::cout << "Num fluent fterms: " << fluent_fterms.size() << std::endl;
    std::cout << "Num ground actions: " << ground_actions.size() << std::endl;
    std::cout << "Num ground axioms: " << ground_axioms.size() << std::endl;

    return create_fdr_task(lifted_task.get_domain(), lifted_task.get_task(), fluent_atoms, derived_atoms, fluent_fterms, ground_actions, ground_axioms);
}

}
