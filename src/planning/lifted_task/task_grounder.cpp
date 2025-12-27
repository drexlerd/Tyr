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

#include "../task_utils.hpp"
#include "tyr/analysis/domains.hpp"
#include "tyr/common/dynamic_bitset.hpp"
#include "tyr/common/vector.hpp"
#include "tyr/formalism/builder.hpp"
#include "tyr/formalism/canonicalization.hpp"
#include "tyr/formalism/formatter.hpp"
#include "tyr/formalism/grounder_planning.hpp"
#include "tyr/formalism/merge_datalog.hpp"
#include "tyr/formalism/merge_planning.hpp"
#include "tyr/grounder/consistency_graph.hpp"
#include "tyr/grounder/execution_contexts.hpp"
#include "tyr/grounder/generator.hpp"
#include "tyr/planning/applicability.hpp"
#include "tyr/planning/declarations.hpp"
#include "tyr/planning/domain.hpp"
#include "tyr/planning/ground_task.hpp"
#include "tyr/planning/ground_task/axiom_evaluator.hpp"
#include "tyr/planning/ground_task/unpacked_state.hpp"
#include "tyr/planning/lifted_task/node.hpp"
#include "tyr/planning/lifted_task/packed_state.hpp"
#include "tyr/planning/lifted_task/state.hpp"
#include "tyr/planning/lifted_task/state_repository.hpp"
#include "tyr/planning/lifted_task/task_grounder.hpp"
#include "tyr/planning/lifted_task/unpacked_state.hpp"
#include "tyr/planning/programs/ground.hpp"
#include "tyr/solver/bottom_up.hpp"

using namespace tyr::formalism;
using namespace tyr::grounder;
using namespace tyr::solver;

namespace tyr::planning
{
static auto remap_fdr_fact(View<Data<FDRFact<FluentTag>>, OverlayRepository<Repository>> fact,
                           GeneralFDRContext<OverlayRepository<Repository>>& fdr_context,
                           MergeContext<OverlayRepository<Repository>>& context)
{
    // Ensure that remapping is unambiguous
    assert(fact.get_variable().get_domain_size() == 2);

    auto new_atom = merge(fact.get_variable().get_atoms().front(), context).first;
    auto new_fact = fdr_context.get_fact(new_atom);

    // value 1 -> keep positive fact
    if (fact.get_value() != FDRValue::none())
        return new_fact;

    // value 0 -> same variable, value 0
    new_fact.value = FDRValue::none();

    return new_fact;
}

static auto create_ground_fdr_conjunctive_condition(View<Index<GroundFDRConjunctiveCondition>, OverlayRepository<Repository>> element,
                                                    GeneralFDRContext<OverlayRepository<Repository>>& fdr_context,
                                                    MergeContext<OverlayRepository<Repository>>& context)
{
    auto fdr_conj_cond_ptr = context.builder.get_builder<GroundFDRConjunctiveCondition>();
    auto& fdr_conj_cond = *fdr_conj_cond_ptr;
    fdr_conj_cond.clear();

    for (const auto literal : element.get_facts<StaticTag>())
        fdr_conj_cond.static_literals.push_back(merge(literal, context).first);

    for (const auto fact : element.get_facts<FluentTag>())
        fdr_conj_cond.fluent_facts.push_back(remap_fdr_fact(fact, fdr_context, context));

    for (const auto literal : element.get_facts<DerivedTag>())
        fdr_conj_cond.derived_literals.push_back(merge(literal, context).first);

    for (const auto numeric_constraint : element.get_numeric_constraints())
        fdr_conj_cond.numeric_constraints.push_back(merge(numeric_constraint, context));

    canonicalize(fdr_conj_cond);
    return context.destination.get_or_create(fdr_conj_cond, context.builder.get_buffer());
}

static auto create_ground_conjunctive_effect(View<Index<GroundConjunctiveEffect>, OverlayRepository<Repository>> element,
                                             GeneralFDRContext<OverlayRepository<Repository>>& fdr_context,
                                             MergeContext<OverlayRepository<Repository>>& context)
{
    auto fdr_conj_eff_ptr = context.builder.get_builder<GroundConjunctiveEffect>();
    auto& fdr_conj_eff = *fdr_conj_eff_ptr;
    fdr_conj_eff.clear();

    auto assign = UnorderedMap<Index<FDRVariable<FluentTag>>, FDRValue> {};

    for (const auto fact : element.get_facts())
        fdr_conj_eff.facts.push_back(remap_fdr_fact(fact, fdr_context, context));

    for (const auto numeric_effect : element.get_numeric_effects())
        fdr_conj_eff.numeric_effects.push_back(merge(numeric_effect, context));

    if (element.get_auxiliary_numeric_effect().has_value())
        fdr_conj_eff.auxiliary_numeric_effect = merge(element.get_auxiliary_numeric_effect().value(), context);

    canonicalize(fdr_conj_eff);
    return context.destination.get_or_create(fdr_conj_eff, context.builder.get_buffer());
}

static auto create_ground_conditional_effect(View<Index<GroundConditionalEffect>, OverlayRepository<Repository>> element,
                                             GeneralFDRContext<OverlayRepository<Repository>>& fdr_context,
                                             MergeContext<OverlayRepository<Repository>>& context)
{
    auto fdr_cond_eff_ptr = context.builder.get_builder<GroundConditionalEffect>();
    auto& fdr_cond_eff = *fdr_cond_eff_ptr;
    fdr_cond_eff.clear();

    fdr_cond_eff.condition = create_ground_fdr_conjunctive_condition(element.get_condition(), fdr_context, context).first;
    fdr_cond_eff.effect = create_ground_conjunctive_effect(element.get_effect(), fdr_context, context).first;

    canonicalize(fdr_cond_eff);
    return context.destination.get_or_create(fdr_cond_eff, context.builder.get_buffer());
}

static auto create_ground_action(View<Index<GroundAction>, OverlayRepository<Repository>> element,
                                 GeneralFDRContext<OverlayRepository<Repository>>& fdr_context,
                                 MergeContext<OverlayRepository<Repository>>& context)
{
    auto fdr_action_ptr = context.builder.get_builder<GroundAction>();
    auto& fdr_action = *fdr_action_ptr;
    fdr_action.clear();

    fdr_action.binding = merge(element.get_binding(), context).first;
    fdr_action.action = element.get_action().get_index();
    fdr_action.condition = create_ground_fdr_conjunctive_condition(element.get_condition(), fdr_context, context).first;
    for (const auto cond_eff : element.get_effects())
        fdr_action.effects.push_back(create_ground_conditional_effect(cond_eff, fdr_context, context).first);

    canonicalize(fdr_action);
    return context.destination.get_or_create(fdr_action, context.builder.get_buffer());
}

static auto create_ground_axiom(View<Index<GroundAxiom>, OverlayRepository<Repository>> element,
                                GeneralFDRContext<OverlayRepository<Repository>>& fdr_context,
                                MergeContext<OverlayRepository<Repository>>& context)
{
    auto fdr_axiom_ptr = context.builder.get_builder<GroundAxiom>();
    auto& fdr_axiom = *fdr_axiom_ptr;
    fdr_axiom.clear();

    fdr_axiom.binding = merge(element.get_binding(), context).first;
    fdr_axiom.axiom = element.get_axiom().get_index();
    fdr_axiom.body = create_ground_fdr_conjunctive_condition(element.get_body(), fdr_context, context).first;
    fdr_axiom.head = merge(element.get_head(), context).first;

    canonicalize(fdr_axiom);
    return context.destination.get_or_create(fdr_axiom, context.builder.get_buffer());
}

// TODO: create stronger mutex groups
static auto create_mutex_groups(View<IndexList<GroundAtom<FluentTag>>, OverlayRepository<Repository>> atoms,
                                MergeContext<OverlayRepository<Repository>>& context)
{
    auto mutex_groups = std::vector<std::vector<View<Index<GroundAtom<FluentTag>>, OverlayRepository<Repository>>>> {};

    for (const auto atom : atoms)
    {
        auto group = std::vector<View<Index<GroundAtom<FluentTag>>, OverlayRepository<Repository>>> {};
        group.push_back(make_view(merge(atom, context).first, context.destination));
        mutex_groups.push_back(group);
    }

    return mutex_groups;
}

static auto create_task(View<Index<Task>, OverlayRepository<Repository>> task,
                        View<IndexList<GroundAtom<FluentTag>>, OverlayRepository<Repository>> fluent_atoms,
                        View<IndexList<GroundAtom<DerivedTag>>, OverlayRepository<Repository>> derived_atoms,
                        View<IndexList<GroundFunctionTerm<FluentTag>>, OverlayRepository<Repository>> fluent_fterms,
                        View<IndexList<GroundAction>, OverlayRepository<Repository>> actions,
                        View<IndexList<GroundAxiom>, OverlayRepository<Repository>> axioms,
                        OverlayRepository<Repository>& repository)
{
    auto builder = Builder();
    auto fdr_task_ptr = builder.get_builder<FDRTask>();
    auto& fdr_task = *fdr_task_ptr;
    fdr_task.clear();

    auto merge_cache = MergeCache {};
    auto merge_context = MergeContext { builder, repository, merge_cache };

    fdr_task.name = task.get_name();
    fdr_task.domain = task.get_domain().get_index();
    for (const auto predicate : task.get_derived_predicates())
        fdr_task.derived_predicates.push_back(merge(predicate, merge_context).first);
    for (const auto object : task.get_objects())
        fdr_task.objects.push_back(merge(object, merge_context).first);

    for (const auto atom : task.get_atoms<StaticTag>())
        fdr_task.static_atoms.push_back(merge(atom, merge_context).first);
    for (const auto atom : fluent_atoms)
        fdr_task.fluent_atoms.push_back(merge(atom, merge_context).first);
    for (const auto atom : derived_atoms)
        fdr_task.derived_atoms.push_back(merge(atom, merge_context).first);
    for (const auto fterm : fluent_fterms)
        fdr_task.fluent_fterms.push_back(merge(fterm, merge_context).first);

    for (const auto fterm_value : task.get_fterm_values<StaticTag>())
        fdr_task.static_fterm_values.push_back(merge(fterm_value, merge_context).first);
    for (const auto fterm_value : task.get_fterm_values<FluentTag>())
        fdr_task.fluent_fterm_values.push_back(merge(fterm_value, merge_context).first);
    if (task.get_auxiliary_fterm_value().has_value())
        fdr_task.auxiliary_fterm_value = merge(task.get_auxiliary_fterm_value().value(), merge_context).first;
    if (task.get_metric())
        fdr_task.metric = merge(task.get_metric().value(), merge_context).first;
    for (const auto axiom : task.get_axioms())
        fdr_task.axioms.push_back(merge(axiom, merge_context).first);

    /// --- Create FDR context
    auto mutex_groups = create_mutex_groups(fluent_atoms, merge_context);
    auto fdr_context = GeneralFDRContext<OverlayRepository<Repository>>(mutex_groups, repository);

    /// --- Create FDR variables
    for (const auto variable : fdr_context.get_variables())
        fdr_task.fluent_variables.push_back(variable.get_index());

    /// --- Create FDR fluent facts
    for (const auto atom : task.get_atoms<FluentTag>())
        fdr_task.fluent_facts.push_back(fdr_context.get_fact(merge(atom, merge_context).first));

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
                            View<Index<Task>, OverlayRepository<Repository>> task,
                            IndexList<GroundAtom<FluentTag>> fluent_atoms,
                            IndexList<GroundAtom<DerivedTag>> derived_atoms,
                            IndexList<GroundFunctionTerm<FluentTag>> fluent_fterms,
                            IndexList<GroundAction> actions,
                            IndexList<GroundAxiom> axioms)
{
    auto repository = std::make_shared<Repository>();
    auto overlay_repository = std::make_shared<OverlayRepository<Repository>>(*domain->get_repository(), *repository);

    const auto [fdr_task, fdr_context] = create_task(task,
                                                     make_view(fluent_atoms, task.get_context()),
                                                     make_view(derived_atoms, task.get_context()),
                                                     make_view(fluent_fterms, task.get_context()),
                                                     make_view(actions, task.get_context()),
                                                     make_view(axioms, task.get_context()),
                                                     *overlay_repository);

    return std::make_shared<GroundTask>(domain, repository, overlay_repository, fdr_task, fdr_context);
}

GroundTaskPtr ground_task(DomainPtr domain,
                          View<Index<Task>, OverlayRepository<Repository>> task,
                          OverlayRepository<Repository>& overlay_repository,
                          BinaryFDRContext<OverlayRepository<Repository>>& fdr_context)
{
    auto ground_program = GroundTaskProgram(task);

    auto ground_context = ProgramExecutionContext(ground_program.get_program(),
                                                  ground_program.get_repository(),
                                                  ground_program.get_domains(),
                                                  ground_program.get_strata(),
                                                  ground_program.get_listeners());

    solve_bottom_up(ground_context);

    auto aggregated_statistics = RuleExecutionContext::compute_aggregate_statistics(ground_context.rule_execution_contexts);

    auto to_ms = [](auto d) { return std::chrono::duration_cast<std::chrono::duration<double, std::milli>>(d).count(); };

    std::cout << "num_rules: " << ground_context.rule_execution_contexts.size() << std::endl;
    std::cout << "init_total_time_min: " << to_ms(aggregated_statistics.init_total_time_min) << " ms" << std::endl;
    std::cout << "init_total_time_max: " << to_ms(aggregated_statistics.init_total_time_max) << " ms" << std::endl;
    std::cout << "init_total_time_median: " << to_ms(aggregated_statistics.init_total_time_median) << " ms" << std::endl;
    std::cout << "ground_total_time_min: " << to_ms(aggregated_statistics.ground_total_time_min) << " ms" << std::endl;
    std::cout << "ground_total_time_max: " << to_ms(aggregated_statistics.ground_total_time_max) << " ms" << std::endl;
    std::cout << "ground_total_time_median: " << to_ms(aggregated_statistics.ground_total_time_median) << " ms" << std::endl;
    std::cout << "ground_seq_total_time: " << to_ms(ground_context.statistics.ground_seq_total_time) << " ms" << std::endl;
    std::cout << "merge_seq_total_time: " << to_ms(ground_context.statistics.merge_seq_total_time) << " ms" << std::endl;
    const auto total_time = (ground_context.statistics.ground_seq_total_time + ground_context.statistics.merge_seq_total_time).count();
    const auto parallel_time = ground_context.statistics.ground_seq_total_time.count();
    std::cout << "parallel_fraction: " << ((total_time > 0) ? static_cast<double>(parallel_time) / total_time : 1.0) << std::endl;

    ground_context.program_to_task_execution_context.clear();

    auto& fluent_assign = ground_context.planning_execution_context.fluent_assign;
    auto& derived_assign = ground_context.planning_execution_context.derived_assign;
    auto& iter_workspace = ground_context.planning_execution_context.iter_workspace;

    /// --- Ground Atoms

    auto fluent_atoms_set = UnorderedSet<Index<GroundAtom<FluentTag>>>();
    auto derived_atoms_set = UnorderedSet<Index<GroundAtom<DerivedTag>>>();
    auto fluent_fterms_set = UnorderedSet<Index<GroundFunctionTerm<FluentTag>>>();
    // TODO: collect fluent function terms

    for (const auto atom : task.get_atoms<FluentTag>())
        fluent_atoms_set.insert(atom.get_index());

    // Collect the goal facts
    for (const auto fact : task.get_goal().get_facts<FluentTag>())
        for (const auto atom : fact.get_variable().get_atoms())
            fluent_atoms_set.insert(atom.get_index());

    for (const auto literal : task.get_goal().get_facts<DerivedTag>())
        derived_atoms_set.insert(literal.get_atom().get_index());

    /// --- Ground Actions

    auto ground_actions_set = UnorderedSet<Index<GroundAction>> {};

    auto static_atoms_bitset = boost::dynamic_bitset<>();
    for (const auto atom : task.get_atoms<StaticTag>())
        set(uint_t(atom.get_index()), true, static_atoms_bitset);

    auto parameter_domains_per_cond_effect_per_action = compute_parameter_domains_per_cond_effect_per_action(task);

    /// TODO: store facts by predicate such that we can swap the iteration, i.e., first over get_predicate_to_actions_mapping, then facts of the predicate
    for (const auto fact : ground_context.facts_execution_context.fact_sets.fluent_sets.predicate.get_facts())
    {
        if (ground_program.get_predicate_to_actions_mapping().contains(fact.get_predicate().get_index()))
        {
            ground_context.program_to_task_execution_context.binding = fact.get_binding().get_objects().get_data();
            auto grounder_context = GrounderContext { ground_context.builder, overlay_repository, ground_context.program_to_task_execution_context.binding };

            for (const auto action_index : ground_program.get_predicate_to_actions_mapping().at(fact.get_predicate().get_index()))
            {
                const auto action = make_view(action_index, grounder_context.destination);

                const auto ground_action_index = ground_planning(action,
                                                                 grounder_context,
                                                                 parameter_domains_per_cond_effect_per_action[action_index.get_value()],
                                                                 fluent_assign,
                                                                 iter_workspace,
                                                                 fdr_context)
                                                     .first;

                const auto ground_action = make_view(ground_action_index, grounder_context.destination);

                if (is_statically_applicable(ground_action, static_atoms_bitset) && is_consistent(ground_action, fluent_assign, derived_assign))
                {
                    ground_actions_set.insert(ground_action.get_index());

                    for (const auto fact : ground_action.get_condition().get_facts<FluentTag>())
                        for (const auto atom : fact.get_variable().get_atoms())
                            fluent_atoms_set.insert(atom.get_index());

                    for (const auto literal : ground_action.get_condition().get_facts<DerivedTag>())
                        derived_atoms_set.insert(literal.get_atom().get_index());

                    for (const auto cond_effect : ground_action.get_effects())
                    {
                        for (const auto fact : cond_effect.get_condition().get_facts<FluentTag>())
                            for (const auto atom : fact.get_variable().get_atoms())
                                fluent_atoms_set.insert(atom.get_index());

                        for (const auto literal : cond_effect.get_condition().get_facts<DerivedTag>())
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

    auto ground_axioms_set = UnorderedSet<Index<GroundAxiom>> {};

    /// TODO: store facts by predicate such that we can swap the iteration, i.e., first over get_predicate_to_axioms_mapping, then facts of the predicate
    for (const auto fact : ground_context.facts_execution_context.fact_sets.fluent_sets.predicate.get_facts())
    {
        if (ground_program.get_predicate_to_axioms_mapping().contains(fact.get_predicate().get_index()))
        {
            ground_context.program_to_task_execution_context.binding = fact.get_binding().get_objects().get_data();
            auto grounder_context = GrounderContext { ground_context.builder, overlay_repository, ground_context.program_to_task_execution_context.binding };

            for (const auto axiom_index : ground_program.get_predicate_to_axioms_mapping().at(fact.get_predicate().get_index()))
            {
                const auto axiom = make_view(axiom_index, grounder_context.destination);

                const auto ground_axiom_index = ground_planning(axiom, grounder_context, fdr_context).first;

                const auto ground_axiom = make_view(ground_axiom_index, grounder_context.destination);

                if (is_statically_applicable(ground_axiom, static_atoms_bitset) && is_consistent(ground_axiom, fluent_assign, derived_assign))
                {
                    ground_axioms_set.insert(ground_axiom.get_index());

                    for (const auto fact : ground_axiom.get_body().get_facts<FluentTag>())
                        for (const auto atom : fact.get_variable().get_atoms())
                            fluent_atoms_set.insert(atom.get_index());

                    for (const auto literal : ground_axiom.get_body().get_facts<DerivedTag>())
                        derived_atoms_set.insert(literal.get_atom().get_index());

                    derived_atoms_set.insert(ground_axiom.get_head().get_index());
                }
            }
        }
    }

    auto fluent_atoms = IndexList<GroundAtom<FluentTag>>(fluent_atoms_set.begin(), fluent_atoms_set.end());
    auto derived_atoms = IndexList<GroundAtom<DerivedTag>>(derived_atoms_set.begin(), derived_atoms_set.end());
    auto fluent_fterms = IndexList<GroundFunctionTerm<FluentTag>>(fluent_fterms_set.begin(), fluent_fterms_set.end());
    auto ground_actions = IndexList<GroundAction>(ground_actions_set.begin(), ground_actions_set.end());
    auto ground_axioms = IndexList<GroundAxiom>(ground_axioms_set.begin(), ground_axioms_set.end());

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

    return create_fdr_task(domain, task, fluent_atoms, derived_atoms, fluent_fterms, ground_actions, ground_axioms);
}

}
