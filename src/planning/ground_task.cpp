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

#include "tyr/planning/ground_task.hpp"

#include "tyr/formalism/builder.hpp"
#include "tyr/formalism/canonicalization.hpp"
#include "tyr/formalism/formatter.hpp"
#include "tyr/formalism/merge_planning.hpp"

using namespace tyr::formalism;

namespace tyr::planning
{
static auto remap_fdr_fact(View<Data<FDRFact<FluentTag>>, OverlayRepository<Repository>> fact,
                           GeneralFDRContext<OverlayRepository<Repository>>& fdr_context,
                           MergeContext<OverlayRepository<Repository>, OverlayRepository<Repository>>& context)
{
    // Ensure that remapping is unambiguous
    assert(fact.get_variable().get_domain_size() == 2);

    auto new_atom = merge(fact.get_variable().get_atoms().front(), context);
    auto new_fact = fdr_context.get_fact(new_atom);

    // value 1 -> keep positive fact
    if (fact.get_value() != FDRValue::none())
        return new_fact;

    // value 0 -> same variable, value 0
    auto new_false_fact = new_fact.get_data();
    new_false_fact.value = FDRValue::none();

    return make_view(new_false_fact, context.destination);
}

static auto create_ground_fdr_conjunctive_condition(View<Index<GroundFDRConjunctiveCondition>, OverlayRepository<Repository>> element,
                                                    GeneralFDRContext<OverlayRepository<Repository>>& fdr_context,
                                                    MergeContext<OverlayRepository<Repository>, OverlayRepository<Repository>>& context)
{
    auto fdr_conj_cond_ptr = context.builder.get_builder<GroundFDRConjunctiveCondition>();
    auto& fdr_conj_cond = *fdr_conj_cond_ptr;
    fdr_conj_cond.clear();

    for (const auto literal : element.get_facts<StaticTag>())
        fdr_conj_cond.static_literals.push_back(merge(literal, context).get_index());

    for (const auto fact : element.get_facts<FluentTag>())
        fdr_conj_cond.fluent_facts.push_back(remap_fdr_fact(fact, fdr_context, context).get_data());

    for (const auto literal : element.get_facts<DerivedTag>())
        fdr_conj_cond.derived_literals.push_back(merge(literal, context).get_index());

    for (const auto numeric_constraint : element.get_numeric_constraints())
        fdr_conj_cond.numeric_constraints.push_back(merge(numeric_constraint, context).get_data());

    canonicalize(fdr_conj_cond);
    return context.destination.get_or_create(fdr_conj_cond, context.builder.get_buffer()).first;
}

static auto create_ground_conjunctive_effect(View<Index<GroundConjunctiveEffect>, OverlayRepository<Repository>> element,
                                             GeneralFDRContext<OverlayRepository<Repository>>& fdr_context,
                                             MergeContext<OverlayRepository<Repository>, OverlayRepository<Repository>>& context)
{
    auto fdr_conj_eff_ptr = context.builder.get_builder<GroundConjunctiveEffect>();
    auto& fdr_conj_eff = *fdr_conj_eff_ptr;
    fdr_conj_eff.clear();

    auto assign = UnorderedMap<Index<FDRVariable<FluentTag>>, FDRValue> {};

    for (const auto fact : element.get_facts())
        fdr_conj_eff.facts.push_back(remap_fdr_fact(fact, fdr_context, context).get_data());

    for (const auto numeric_effect : element.get_numeric_effects())
        fdr_conj_eff.numeric_effects.push_back(merge(numeric_effect, context).get_data());

    if (element.get_auxiliary_numeric_effect().has_value())
        fdr_conj_eff.auxiliary_numeric_effect = merge(element.get_auxiliary_numeric_effect().value(), context).get_data();

    canonicalize(fdr_conj_eff);
    return context.destination.get_or_create(fdr_conj_eff, context.builder.get_buffer()).first;
}

static auto create_ground_conditional_effect(View<Index<GroundConditionalEffect>, OverlayRepository<Repository>> element,
                                             GeneralFDRContext<OverlayRepository<Repository>>& fdr_context,
                                             MergeContext<OverlayRepository<Repository>, OverlayRepository<Repository>>& context)
{
    auto fdr_cond_eff_ptr = context.builder.get_builder<GroundConditionalEffect>();
    auto& fdr_cond_eff = *fdr_cond_eff_ptr;
    fdr_cond_eff.clear();

    fdr_cond_eff.condition = create_ground_fdr_conjunctive_condition(element.get_condition(), fdr_context, context).get_index();
    fdr_cond_eff.effect = create_ground_conjunctive_effect(element.get_effect(), fdr_context, context).get_index();

    canonicalize(fdr_cond_eff);
    return context.destination.get_or_create(fdr_cond_eff, context.builder.get_buffer()).first;
}

static auto create_ground_action(View<Index<GroundAction>, OverlayRepository<Repository>> element,
                                 GeneralFDRContext<OverlayRepository<Repository>>& fdr_context,
                                 MergeContext<OverlayRepository<Repository>, OverlayRepository<Repository>>& context)
{
    auto fdr_action_ptr = context.builder.get_builder<GroundAction>();
    auto& fdr_action = *fdr_action_ptr;
    fdr_action.clear();

    fdr_action.binding = merge(element.get_binding(), context).get_index();
    fdr_action.action = element.get_action().get_index();
    fdr_action.condition = create_ground_fdr_conjunctive_condition(element.get_condition(), fdr_context, context).get_index();
    for (const auto cond_eff : element.get_effects())
        fdr_action.effects.push_back(create_ground_conditional_effect(cond_eff, fdr_context, context).get_index());

    canonicalize(fdr_action);
    return context.destination.get_or_create(fdr_action, context.builder.get_buffer()).first;
}

static auto create_ground_axiom(View<Index<GroundAxiom>, OverlayRepository<Repository>> element,
                                GeneralFDRContext<OverlayRepository<Repository>>& fdr_context,
                                MergeContext<OverlayRepository<Repository>, OverlayRepository<Repository>>& context)
{
    auto fdr_axiom_ptr = context.builder.get_builder<GroundAxiom>();
    auto& fdr_axiom = *fdr_axiom_ptr;
    fdr_axiom.clear();

    fdr_axiom.binding = merge(element.get_binding(), context).get_index();
    fdr_axiom.axiom = element.get_axiom().get_index();
    fdr_axiom.body = create_ground_fdr_conjunctive_condition(element.get_body(), fdr_context, context).get_index();
    fdr_axiom.head = merge(element.get_head(), context).get_index();

    canonicalize(fdr_axiom);
    return context.destination.get_or_create(fdr_axiom, context.builder.get_buffer()).first;
}

// TODO: create stronger mutex groups
auto create_mutex_groups(View<IndexList<GroundAtom<FluentTag>>, OverlayRepository<Repository>> atoms,
                         MergeContext<OverlayRepository<Repository>, OverlayRepository<Repository>>& context)
{
    auto mutex_groups = std::vector<std::vector<View<Index<GroundAtom<FluentTag>>, OverlayRepository<Repository>>>> {};

    for (const auto atom : atoms)
    {
        auto group = std::vector<View<Index<GroundAtom<FluentTag>>, OverlayRepository<Repository>>> {};
        group.push_back(merge(atom, context));
        mutex_groups.push_back(group);
    }

    return mutex_groups;
}

auto create_task(View<Index<Task>, OverlayRepository<Repository>> task,
                 View<IndexList<GroundAtom<FluentTag>>, OverlayRepository<Repository>> fluent_atoms,
                 View<IndexList<GroundAtom<DerivedTag>>, OverlayRepository<Repository>> derived_atoms,
                 View<IndexList<GroundAction>, OverlayRepository<Repository>> actions,
                 View<IndexList<GroundAxiom>, OverlayRepository<Repository>> axioms,
                 OverlayRepository<Repository>& repository)
{
    auto builder = Builder();
    auto fdr_task_ptr = builder.get_builder<FDRTask>();
    auto& fdr_task = *fdr_task_ptr;
    fdr_task.clear();

    auto merge_cache = MergeCache<OverlayRepository<Repository>, OverlayRepository<Repository>> {};
    auto merge_context = MergeContext<OverlayRepository<Repository>, OverlayRepository<Repository>> { builder, repository, merge_cache };

    fdr_task.name = task.get_name();
    fdr_task.domain = task.get_domain().get_index();
    for (const auto predicate : task.get_derived_predicates())
        fdr_task.derived_predicates.push_back(merge(predicate, merge_context).get_index());
    for (const auto object : task.get_objects())
        fdr_task.objects.push_back(merge(object, merge_context).get_index());

    for (const auto atom : task.get_atoms<StaticTag>())
        fdr_task.static_atoms.push_back(merge(atom, merge_context).get_index());
    for (const auto atom : fluent_atoms)
        fdr_task.fluent_atoms.push_back(merge(atom, merge_context).get_index());
    for (const auto atom : derived_atoms)
        fdr_task.derived_atoms.push_back(merge(atom, merge_context).get_index());
    for (const auto fterm_value : task.get_fterm_values<StaticTag>())
        fdr_task.static_fterm_values.push_back(merge(fterm_value, merge_context).get_index());
    for (const auto fterm_value : task.get_fterm_values<FluentTag>())
        fdr_task.fluent_fterm_values.push_back(merge(fterm_value, merge_context).get_index());
    if (task.get_auxiliary_fterm_value().has_value())
        fdr_task.auxiliary_fterm_value = merge(task.get_auxiliary_fterm_value().value(), merge_context).get_index();
    if (task.get_metric())
        fdr_task.metric = merge(task.get_metric().value(), merge_context).get_index();
    for (const auto axiom : task.get_axioms())
        fdr_task.axioms.push_back(merge(axiom, merge_context).get_index());

    /// --- Create FDR context
    auto mutex_groups = create_mutex_groups(fluent_atoms, merge_context);
    auto fdr_context = GeneralFDRContext<OverlayRepository<Repository>>(mutex_groups, repository);

    /// --- Create FDR variables
    for (const auto variable : fdr_context.get_variables())
        fdr_task.fluent_variables.push_back(variable.get_index());

    /// --- Create FDR fluent facts
    for (const auto atom : task.get_atoms<FluentTag>())
        fdr_task.fluent_facts.push_back(fdr_context.get_fact(merge(atom, merge_context)).get_data());

    /// --- Create FDR goal
    fdr_task.goal = create_ground_fdr_conjunctive_condition(task.get_goal(), fdr_context, merge_context).get_index();

    /// --- Create FDR actions and axioms
    for (const auto action : actions)
        fdr_task.ground_actions.push_back(create_ground_action(action, fdr_context, merge_context).get_index());
    for (const auto axiom : axioms)
        fdr_task.ground_axioms.push_back(create_ground_axiom(axiom, fdr_context, merge_context).get_index());

    canonicalize(fdr_task);

    return std::make_pair(repository.get_or_create(fdr_task, builder.get_buffer()).first, std::move(fdr_context));
}

std::shared_ptr<GroundTask> GroundTask::create(DomainPtr domain,
                                               RepositoryPtr task_repository,
                                               OverlayRepositoryPtr<Repository> task_overlay_repository,
                                               View<Index<Task>, OverlayRepository<Repository>> task,
                                               IndexList<GroundAtom<FluentTag>> fluent_atoms,
                                               IndexList<GroundAtom<DerivedTag>> derived_atoms,
                                               IndexList<GroundAction> actions,
                                               IndexList<GroundAxiom> axioms)
{
    auto repository = std::make_shared<Repository>();
    auto overlay_repository = std::make_shared<OverlayRepository<Repository>>(*domain->get_repository(), *repository);

    const auto [fdr_task, fdr_context] = create_task(task,
                                                     make_view(fluent_atoms, *task_overlay_repository),
                                                     make_view(derived_atoms, *task_overlay_repository),
                                                     make_view(actions, *task_overlay_repository),
                                                     make_view(axioms, *task_overlay_repository),
                                                     *overlay_repository);

    return std::make_shared<GroundTask>(domain, repository, overlay_repository, fdr_task, fdr_context);
}

GroundTask::GroundTask(DomainPtr domain,
                       RepositoryPtr m_repository,
                       OverlayRepositoryPtr<Repository> overlay_repository,
                       View<Index<FDRTask>, OverlayRepository<Repository>> fdr_task,
                       GeneralFDRContext<OverlayRepository<Repository>> fdr_context) :
    m_domain(std::move(domain)),
    m_repository(std::move(m_repository)),
    m_overlay_repository(std::move(overlay_repository)),
    m_fdr_task(fdr_task),
    m_fdr_context(fdr_context),
    m_fluent_layout(create_layouts<FluentTag, OverlayRepository<Repository>, uint_t>(m_fdr_task.get_fluent_variables()))
{
    // std::cout << m_fdr_task << std::endl;
}

Node<GroundTask> get_initial_node() {}

void GroundTask::compute_extended_state(UnpackedState<GroundTask>& unpacked_state) {}

std::vector<std::pair<View<Index<GroundAction>, OverlayRepository<Repository>>, Node<GroundTask>>>
GroundTask::get_labeled_successor_nodes(const Node<GroundTask>& node)
{
    auto result = std::vector<std::pair<View<Index<GroundAction>, OverlayRepository<Repository>>, Node<GroundTask>>> {};
    return result;
}

void GroundTask::get_labeled_successor_nodes(const Node<GroundTask>& node,
                                             std::vector<std::pair<View<Index<GroundAction>, OverlayRepository<Repository>>, Node<GroundTask>>>& out_nodes)
{
}

template<FactKind T>
size_t GroundTask::get_num_atoms() const noexcept
{
    return m_fdr_task.template get_atoms<T>().size();
}

template size_t GroundTask::get_num_atoms<FluentTag>() const noexcept;
template size_t GroundTask::get_num_atoms<DerivedTag>() const noexcept;

size_t GroundTask::get_num_actions() const noexcept { return m_fdr_task.get_ground_actions().size(); }

size_t GroundTask::get_num_axioms() const noexcept { return m_fdr_task.get_ground_axioms().size(); }
}
