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

#include "tyr/planning/programs/rpg.hpp"

#include "common.hpp"
#include "tyr/common/unordered_set.hpp"
#include "tyr/formalism/datalog/formatter.hpp"
#include "tyr/formalism/datalog/repository.hpp"
#include "tyr/formalism/datalog/views.hpp"
#include "tyr/formalism/overlay_repository.hpp"
#include "tyr/formalism/planning/formatter.hpp"
#include "tyr/formalism/planning/merge_datalog.hpp"
#include "tyr/formalism/planning/repository.hpp"
#include "tyr/formalism/planning/views.hpp"

namespace f = tyr::formalism;
namespace fp = tyr::formalism::planning;
namespace fd = tyr::formalism::datalog;

namespace tyr::planning
{
namespace rpg
{
template<f::FactKind T>
void collect_parameters(View<Index<fp::Atom<T>>, f::OverlayRepository<fp::Repository>> atom, UnorderedSet<f::ParameterIndex>& result)
{
    for (const auto term : atom.get_terms())
    {
        visit(
            [&](auto&& arg)
            {
                using Alternative = std::decay_t<decltype(arg)>;

                if constexpr (std::is_same_v<Alternative, f::ParameterIndex>)
                    result.insert(arg);
                else if constexpr (std::is_same_v<Alternative, View<Index<f::Object>, f::OverlayRepository<fp::Repository>>>) {}
                else
                    static_assert(dependent_false<Alternative>::value, "Missing case");
            },
            term.get_variant());
    }
}

template<f::FactKind T>
void collect_parameters(View<Index<fp::Literal<T>>, f::OverlayRepository<fp::Repository>> literal, UnorderedSet<f::ParameterIndex>& result)
{
    collect_parameters(literal.get_atom(), result);
}

static void collect_parameters(View<Index<fp::ConjunctiveCondition>, f::OverlayRepository<fp::Repository>> condition, UnorderedSet<f::ParameterIndex>& result)
{
    for (const auto literal : condition.get_literals<f::StaticTag>())
        collect_parameters(literal, result);

    for (const auto literal : condition.get_literals<f::FluentTag>())
        if (literal.get_polarity())
            collect_parameters(literal, result);
}

static void collect_parameters(View<Index<fp::ConjunctiveEffect>, f::OverlayRepository<fp::Repository>> effect, UnorderedSet<f::ParameterIndex>& result)
{
    for (const auto literal : effect.get_literals())
        if (literal.get_polarity())
            collect_parameters(literal, result);
}

static void collect_parameters(View<Index<fp::ConditionalEffect>, f::OverlayRepository<fp::Repository>> effect, UnorderedSet<f::ParameterIndex>& result)
{
    collect_parameters(effect.get_condition(), result);
    collect_parameters(effect.get_effect(), result);
}

static void collect_parameters(View<Index<fp::Action>, f::OverlayRepository<fp::Repository>> action, UnorderedSet<f::ParameterIndex>& result)
{
    for (const auto effect : action.get_effects())
        collect_parameters(effect, result);
}

static auto sorted_params(const UnorderedSet<f::ParameterIndex>& s)
{
    std::vector<f::ParameterIndex> v(s.begin(), s.end());
    std::sort(v.begin(), v.end(), [](auto a, auto b) { return uint_t(a) < uint_t(b); });
    return v;
}

static bool should_keep(View<Data<f::Term>, f::OverlayRepository<fp::Repository>> term, const UnorderedMap<f::ParameterIndex, f::ParameterIndex>& mapping)
{
    return visit(
        [&](auto&& arg)
        {
            using Alternative = std::decay_t<decltype(arg)>;

            if constexpr (std::is_same_v<Alternative, f::ParameterIndex>)
                return mapping.contains(arg);
            else if constexpr (std::is_same_v<Alternative, View<Index<f::Object>, f::OverlayRepository<fp::Repository>>>)
                return true;
            else
                static_assert(dependent_false<Alternative>::value, "Missing case");
        },
        term.get_variant());
}

template<f::FactKind T>
bool should_keep(View<Index<fp::Atom<T>>, f::OverlayRepository<fp::Repository>> atom, const UnorderedMap<f::ParameterIndex, f::ParameterIndex>& mapping)
{
    return std::all_of(atom.get_terms().begin(), atom.get_terms().end(), [&](auto&& term) { return should_keep(term, mapping); });
}

static bool should_keep(View<Index<fp::Literal<f::StaticTag>>, f::OverlayRepository<fp::Repository>> literal,
                        const UnorderedMap<f::ParameterIndex, f::ParameterIndex>& mapping)
{
    return should_keep(literal.get_atom(), mapping);
}

static bool should_keep(View<Index<fp::Literal<f::FluentTag>>, f::OverlayRepository<fp::Repository>> literal,
                        const UnorderedMap<f::ParameterIndex, f::ParameterIndex>& mapping)
{
    return literal.get_polarity() && should_keep(literal.get_atom(), mapping);
}

static bool should_keep(View<Index<fp::Literal<f::StaticTag>>, f::OverlayRepository<fp::Repository>> literal) { return true; }

static bool should_keep(View<Index<fp::Literal<f::FluentTag>>, f::OverlayRepository<fp::Repository>> literal) { return literal.get_polarity(); }

static auto merge(View<Data<f::Term>, f::OverlayRepository<fp::Repository>> element, const UnorderedMap<f::ParameterIndex, f::ParameterIndex>& mapping)
{
    return visit(
        [&](auto&& arg)
        {
            using Alternative = std::decay_t<decltype(arg)>;

            if constexpr (std::is_same_v<Alternative, f::ParameterIndex>)
                return Data<f::Term>(mapping.at(arg));
            else if constexpr (std::is_same_v<Alternative, View<Index<f::Object>, f::OverlayRepository<fp::Repository>>>)
                return Data<f::Term>(arg.get_index());
            else
                static_assert(dependent_false<Alternative>::value, "Missing case");
        },
        element.get_variant());
}

template<f::FactKind T>
auto merge(View<Index<fp::Atom<T>>, f::OverlayRepository<fp::Repository>> element,
           const UnorderedMap<f::ParameterIndex, f::ParameterIndex>& mapping,
           formalism::planning::MergeDatalogContext<formalism::datalog::Repository>& context)
{
    auto atom_ptr = context.builder.get_builder<fd::Atom<T>>();
    auto& atom = *atom_ptr;
    atom.clear();

    atom.predicate = element.get_predicate().get_index();
    for (const auto term : element.get_terms())
        atom.terms.push_back(merge(term, mapping));

    canonicalize(atom);
    return context.destination.get_or_create(atom, context.builder.get_buffer());
}

template<f::FactKind T>
auto merge(View<Index<fp::Literal<T>>, f::OverlayRepository<fp::Repository>> element,
           const UnorderedMap<f::ParameterIndex, f::ParameterIndex>& mapping,
           formalism::planning::MergeDatalogContext<formalism::datalog::Repository>& context)
{
    auto literal_ptr = context.builder.get_builder<fd::Literal<T>>();
    auto& literal = *literal_ptr;
    literal.clear();

    literal.polarity = element.get_polarity();
    literal.atom = merge(element.get_atom(), mapping, context).first;

    canonicalize(literal);
    return context.destination.get_or_create(literal, context.builder.get_buffer());
}

static auto create_applicability_predicate(View<Index<formalism::planning::Action>, formalism::OverlayRepository<formalism::planning::Repository>> action,
                                           const UnorderedSet<f::ParameterIndex>& parameters,
                                           formalism::planning::MergeDatalogContext<formalism::datalog::Repository>& context)
{
    auto predicate_ptr = context.builder.get_builder<formalism::Predicate<formalism::FluentTag>>();
    auto& predicate = *predicate_ptr;
    predicate.clear();

    predicate.name = create_applicability_name(action);
    predicate.arity = parameters.size();

    canonicalize(predicate);
    return context.destination.get_or_create(predicate, context.builder.get_buffer());
}

static auto create_applicability_atom(View<Index<formalism::planning::Action>, formalism::OverlayRepository<formalism::planning::Repository>> action,
                                      const UnorderedSet<f::ParameterIndex>& parameters,
                                      formalism::planning::MergeDatalogContext<formalism::datalog::Repository>& context)
{
    auto atom_ptr = context.builder.get_builder<formalism::datalog::Atom<formalism::FluentTag>>();
    auto& atom = *atom_ptr;
    atom.clear();

    const auto applicability_predicate = make_view(create_applicability_predicate(action, parameters, context).first, context.destination);

    atom.predicate = applicability_predicate.get_index();
    for (const auto param : parameters)
        atom.terms.push_back(Data<formalism::Term>(param));

    canonicalize(atom);
    return context.destination.get_or_create(atom, context.builder.get_buffer());
}

static auto create_applicability_literal(View<Index<formalism::planning::Action>, formalism::OverlayRepository<formalism::planning::Repository>> action,
                                         const UnorderedSet<f::ParameterIndex>& parameters,
                                         formalism::planning::MergeDatalogContext<formalism::datalog::Repository>& context)
{
    auto literal_ptr = context.builder.get_builder<formalism::datalog::Literal<formalism::FluentTag>>();
    auto& literal = *literal_ptr;
    literal.clear();

    literal.polarity = true;
    literal.atom = create_applicability_atom(action, parameters, context).first;

    canonicalize(literal);
    return context.destination.get_or_create(literal, context.builder.get_buffer());
}

static auto create_applicability_rule(View<Index<formalism::planning::Action>, formalism::OverlayRepository<formalism::planning::Repository>> action,
                                      const UnorderedSet<f::ParameterIndex>& parameters,
                                      formalism::planning::MergeDatalogContext<formalism::datalog::Repository>& context)
{
    auto rule_ptr = context.builder.get_builder<formalism::datalog::Rule>();
    auto& rule = *rule_ptr;
    rule.clear();

    auto conj_cond_ptr = context.builder.get_builder<formalism::datalog::ConjunctiveCondition>();
    auto& conj_cond = *conj_cond_ptr;
    conj_cond.clear();

    for (const auto variable : action.get_variables())
        conj_cond.variables.push_back(merge_p2d(variable, context).first);

    // Keep negated static atoms because they are monotonic
    for (const auto literal : action.get_condition().template get_literals<formalism::StaticTag>())
        if (should_keep(literal))
            conj_cond.static_literals.push_back(merge_p2d(literal, context).first);

    for (const auto literal : action.get_condition().template get_literals<formalism::FluentTag>())
        if (should_keep(literal))
            conj_cond.fluent_literals.push_back(merge_p2d(literal, context).first);

    canonicalize(conj_cond);
    const auto new_conj_cond = make_view(context.destination.get_or_create(conj_cond, context.builder.get_buffer()).first, context.destination);

    rule.variables = new_conj_cond.get_variables().get_data();
    rule.body = new_conj_cond.get_index();
    rule.head = create_applicability_atom(action, parameters, context).first;
    rule.cost = uint_t(1);

    canonicalize(rule);
    return context.destination.get_or_create(rule, context.builder.get_buffer());
}

static auto create_applicability_atom(View<Index<formalism::planning::Action>, formalism::OverlayRepository<formalism::planning::Repository>> action,
                                      const UnorderedSet<f::ParameterIndex>& head_parameters,
                                      const UnorderedMap<f::ParameterIndex, f::ParameterIndex>& mapping,
                                      formalism::planning::MergeDatalogContext<formalism::datalog::Repository>& context)
{
    auto atom_ptr = context.builder.get_builder<formalism::datalog::Atom<formalism::FluentTag>>();
    auto& atom = *atom_ptr;
    atom.clear();

    const auto applicability_predicate = make_view(create_applicability_predicate(action, head_parameters, context).first, context.destination);

    atom.predicate = applicability_predicate.get_index();
    for (const auto param : head_parameters)
        atom.terms.push_back(Data<formalism::Term>(mapping.at(param)));

    canonicalize(atom);
    return context.destination.get_or_create(atom, context.builder.get_buffer());
}

static auto create_applicability_literal(View<Index<formalism::planning::Action>, formalism::OverlayRepository<formalism::planning::Repository>> action,
                                         const UnorderedSet<f::ParameterIndex>& head_parameters,
                                         const UnorderedMap<f::ParameterIndex, f::ParameterIndex>& mapping,
                                         formalism::planning::MergeDatalogContext<formalism::datalog::Repository>& context)
{
    auto literal_ptr = context.builder.get_builder<formalism::datalog::Literal<formalism::FluentTag>>();
    auto& literal = *literal_ptr;
    literal.clear();

    literal.polarity = true;
    literal.atom = create_applicability_atom(action, head_parameters, mapping, context).first;

    canonicalize(literal);
    return context.destination.get_or_create(literal, context.builder.get_buffer());
}

static auto
create_cond_effect_rule(View<Index<formalism::planning::Action>, formalism::OverlayRepository<formalism::planning::Repository>> action,
                        View<Index<formalism::planning::ConditionalEffect>, formalism::OverlayRepository<formalism::planning::Repository>> cond_eff,
                        View<Index<formalism::planning::Atom<formalism::FluentTag>>, formalism::OverlayRepository<formalism::planning::Repository>> effect,
                        const UnorderedSet<f::ParameterIndex>& head_parameters,
                        const UnorderedMap<f::ParameterIndex, f::ParameterIndex>& mapping,
                        formalism::planning::MergeDatalogContext<formalism::datalog::Repository>& context)
{
    auto rule_ptr = context.builder.get_builder<formalism::datalog::Rule>();
    auto& rule = *rule_ptr;
    rule.clear();

    auto conj_cond_ptr = context.builder.get_builder<formalism::datalog::ConjunctiveCondition>();
    auto& conj_cond = *conj_cond_ptr;
    conj_cond.clear();

    // Action parameter may get deleted.
    for (uint_t param = 0; param < action.get_arity(); ++param)
        if (mapping.contains(f::ParameterIndex(param)))
            conj_cond.variables.push_back(merge_p2d(action.get_variables()[uint_t(param)], context).first);
    for (const auto literal : action.get_condition().get_literals<formalism::StaticTag>())
        if (should_keep(literal, mapping))
            conj_cond.static_literals.push_back(merge(literal, mapping, context).first);
    conj_cond.fluent_literals.push_back(create_applicability_literal(action, head_parameters, mapping, context).first);

    for (const auto variable : cond_eff.get_variables())
        conj_cond.variables.push_back(merge_p2d(variable, context).first);

    for (const auto literal : cond_eff.get_condition().template get_literals<formalism::StaticTag>())
        if (should_keep(literal, mapping))
            conj_cond.static_literals.push_back(merge(literal, mapping, context).first);

    for (const auto literal : cond_eff.get_condition().template get_literals<formalism::FluentTag>())
        if (should_keep(literal, mapping))
            conj_cond.fluent_literals.push_back(merge(literal, mapping, context).first);

    canonicalize(conj_cond);
    const auto new_conj_cond = make_view(context.destination.get_or_create(conj_cond, context.builder.get_buffer()).first, context.destination);

    rule.variables = new_conj_cond.get_variables().get_data();
    rule.body = new_conj_cond.get_index();
    rule.head = merge(effect, mapping, context).first;
    rule.cost = uint_t(0);

    canonicalize(rule);
    return context.destination.get_or_create(rule, context.builder.get_buffer());
}

auto compute_applicability_head_parameters(View<Index<fp::Action>, f::OverlayRepository<fp::Repository>> action)
{
    // Collect all parameter indices mentioned in positive fluent literals or negative static literals
    auto parameters = UnorderedSet<f::ParameterIndex> {};
    collect_parameters(action, parameters);

    // Remove parameters that are bound in any conditional effect effect
    for (auto it = parameters.begin(); it != parameters.end();)
    {
        if (uint_t(*it) >= action.get_arity())
            it = parameters.erase(it);
        else
            ++it;
    }

    return parameters;
}

auto compute_applicability_parameter_mapping(View<Index<fp::Action>, f::OverlayRepository<fp::Repository>> action,
                                             View<Index<fp::ConditionalEffect>, f::OverlayRepository<fp::Repository>> effect,
                                             const UnorderedSet<f::ParameterIndex>& head_parameters)
{
    auto mapping = UnorderedMap<f::ParameterIndex, f::ParameterIndex> {};

    auto new_param = uint_t(0);
    // Sort by original parameter index so the remapping is order-preserving:
    // kept parameters are condensed to 0..k-1 while preserving their relative order.
    for (const auto& param : sorted_params(head_parameters))
        mapping.emplace(param, f::ParameterIndex { new_param++ });

    // Collect parameters that are bound in the conditional effect.
    auto effect_parameters = UnorderedSet<f::ParameterIndex> {};
    collect_parameters(effect, effect_parameters);

    // Remove parameter that are bound in the action.
    for (auto it = effect_parameters.begin(); it != effect_parameters.end();)
    {
        if (uint_t(*it) < action.get_arity())
            it = effect_parameters.erase(it);
        else
            ++it;
    }

    // Sort for same reason as above.
    for (const auto& param : sorted_params(effect_parameters))
        mapping.emplace(param, f::ParameterIndex { new_param++ });

    return mapping;
}

static void translate_action_to_delete_free_rules(View<Index<fp::Action>, f::OverlayRepository<fp::Repository>> action,
                                                  Data<fd::Program>& program,
                                                  fp::MergeDatalogContext<fd::Repository>& context,
                                                  RPGProgram::AppPredicateToActionsMapping& predicate_to_actions)
{
    // Collect all parameter indices mentioned in positive fluent literals or negative static literals
    auto head_parameters = compute_applicability_head_parameters(action);

    const auto applicability_predicate = create_applicability_predicate(action, head_parameters, context).first;

    program.fluent_predicates.push_back(applicability_predicate);

    [[maybe_unused]] const auto [it, inserted] = predicate_to_actions.emplace(applicability_predicate, action.get_index());
    assert(inserted);

    // TODO: pass effect variables to construction of applicability atom
    const auto applicability_rule = create_applicability_rule(action, head_parameters, context).first;

    program.rules.push_back(applicability_rule);

    for (const auto cond_eff : action.get_effects())
    {
        auto parameter_mapping = compute_applicability_parameter_mapping(action, cond_eff, head_parameters);

        for (const auto literal : cond_eff.get_effect().get_literals())
        {
            if (!literal.get_polarity())
                continue;  /// ignore delete effects

            program.rules.push_back(create_cond_effect_rule(action, cond_eff, literal.get_atom(), head_parameters, parameter_mapping, context).first);
        }
    }
}

static Index<fd::Program> create_program(View<Index<fp::Task>, f::OverlayRepository<fp::Repository>> task,
                                         fd::Repository& destination,
                                         RPGProgram::AppPredicateToActionsMapping& predicate_to_actions)
{
    auto builder = fd::Builder();
    auto context = fp::MergeDatalogContext<fd::Repository>(builder, destination);
    auto program_ptr = builder.get_builder<fd::Program>();
    auto& program = *program_ptr;
    program.clear();

    for (const auto predicate : task.get_domain().get_predicates<f::StaticTag>())
        program.static_predicates.push_back(fp::merge_p2d(predicate, context).first);

    for (const auto predicate : task.get_domain().get_predicates<f::FluentTag>())
        program.fluent_predicates.push_back(fp::merge_p2d(predicate, context).first);

    // We can ignore auxiliary function total-cost because it never occurs in a condition

    for (const auto object : task.get_domain().get_constants())
        program.objects.push_back(fp::merge_p2d(object, context).first);
    for (const auto object : task.get_objects())
        program.objects.push_back(fp::merge_p2d(object, context).first);

    for (const auto atom : task.get_atoms<f::StaticTag>())
        program.static_atoms.push_back(fp::merge_p2d(atom, context).first);

    for (const auto atom : task.get_atoms<f::FluentTag>())
        program.fluent_atoms.push_back(fp::merge_p2d(atom, context).first);

    for (const auto action : task.get_domain().get_actions())
        translate_action_to_delete_free_rules(action, program, context, predicate_to_actions);

    canonicalize(program);
    return destination.get_or_create(program, builder.get_buffer()).first;
}

static auto create_program_context(View<Index<fp::Task>, f::OverlayRepository<fp::Repository>> task,
                                   RPGProgram::AppPredicateToActionsMapping& predicate_to_actions)
{
    auto repository = std::make_shared<fd::Repository>();
    auto program = create_program(task, *repository, predicate_to_actions);
    auto domains = analysis::compute_variable_domains(make_view(program, *repository));
    auto strata = analysis::compute_rule_stratification(make_view(program, *repository));
    auto listeners = analysis::compute_listeners(strata, *repository);

    return datalog::ProgramContext(program, std::move(repository), std::move(domains), std::move(strata), std::move(listeners));
}

}

RPGProgram::RPGProgram(View<Index<fp::Task>, f::OverlayRepository<fp::Repository>> task) :
    m_predicate_to_actions(),
    m_program_context(rpg::create_program_context(task, m_predicate_to_actions)),
    m_program_workspace(m_program_context)
{
    // std::cout << m_program_context.get_program() << std::endl;
}

const RPGProgram::AppPredicateToActionsMapping& RPGProgram::get_predicate_to_actions_mapping() const noexcept { return m_predicate_to_actions; }

datalog::ProgramContext& RPGProgram::get_program_context() noexcept { return m_program_context; }

const datalog::ProgramContext& RPGProgram::get_program_context() const noexcept { return m_program_context; }

const datalog::ConstProgramWorkspace& RPGProgram::get_const_program_workspace() const noexcept { return m_program_workspace; }

}
