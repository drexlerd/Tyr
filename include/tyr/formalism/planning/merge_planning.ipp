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

#include "tyr/common/equal_to.hpp"
#include "tyr/common/hash.hpp"
#include "tyr/common/macros.hpp"
#include "tyr/common/tuple.hpp"
#include "tyr/formalism/datalog/builder.hpp"
#include "tyr/formalism/datalog/canonicalization.hpp"
#include "tyr/formalism/datalog/declarations.hpp"
#include "tyr/formalism/datalog/indices.hpp"
#include "tyr/formalism/datalog/repository.hpp"
#include "tyr/formalism/datalog/views.hpp"
#include "tyr/formalism/planning/builder.hpp"
#include "tyr/formalism/planning/canonicalization.hpp"
#include "tyr/formalism/planning/declarations.hpp"
#include "tyr/formalism/planning/indices.hpp"
#include "tyr/formalism/planning/merge_planning_decl.hpp"
#include "tyr/formalism/planning/repository.hpp"
#include "tyr/formalism/planning/views.hpp"
#include "tyr/formalism/views.hpp"

namespace tyr::formalism::planning
{

// Common

TYR_INLINE_IMPL std::pair<VariableView, bool> merge_d2p(formalism::datalog::VariableView element, MergePlanningContext& context)
{
    auto variable_ptr = context.builder.template get_builder<formalism::Variable>();
    auto& variable = *variable_ptr;
    variable.clear();

    variable.name = element.get_name();

    canonicalize(variable);
    return context.destination.get_or_create(variable);
}

TYR_INLINE_IMPL std::pair<ObjectView, bool> merge_d2p(formalism::datalog::ObjectView element, MergePlanningContext& context)
{
    auto object_ptr = context.builder.template get_builder<formalism::Object>();
    auto& object = *object_ptr;
    object.clear();

    object.name = element.get_name();

    canonicalize(object);
    return context.destination.get_or_create(object);
}

TYR_INLINE_IMPL std::pair<BindingView, bool> merge_d2p(formalism::datalog::BindingView element, MergePlanningContext& context)
{
    auto binding_ptr = context.builder.template get_builder<formalism::Binding>();
    auto& binding = *binding_ptr;
    binding.clear();

    binding.objects = element.get_data().objects;

    canonicalize(binding);
    return context.destination.get_or_create(binding);
}

TYR_INLINE_IMPL Data<formalism::Term> merge_d2p(formalism::datalog::TermView element, MergePlanningContext& context)
{
    return visit(
        [&](auto&& arg)
        {
            using Alternative = std::decay_t<decltype(arg)>;

            if constexpr (std::is_same_v<Alternative, formalism::ParameterIndex>)
                return Data<formalism::Term>(arg);
            else if constexpr (std::is_same_v<Alternative, formalism::datalog::ObjectView>)
                return Data<formalism::Term>(merge_d2p(arg, context).first.get_index());
            else
                static_assert(dependent_false<Alternative>::value, "Missing case");
        },
        element.get_variant());
}

// Propositional

template<FactKind T_SRC, FactKind T_DST>
std::pair<PredicateView<T_DST>, bool> merge_d2p(formalism::datalog::PredicateView<T_SRC> element, MergePlanningContext& context)
{
    auto predicate_ptr = context.builder.template get_builder<formalism::Predicate<T_DST>>();
    auto& predicate = *predicate_ptr;
    predicate.clear();

    predicate.name = element.get_name();
    predicate.arity = element.get_arity();

    canonicalize(predicate);
    return context.destination.get_or_create(predicate);
}

template<FactKind T_SRC, FactKind T_DST>
std::pair<AtomView<T_DST>, bool> merge_d2p(formalism::datalog::AtomView<T_SRC> element, MergePlanningContext& context)
{
    auto atom_ptr = context.builder.template get_builder<Atom<T_DST>>();
    auto& atom = *atom_ptr;
    atom.clear();

    atom.predicate = merge_d2p<T_SRC, T_DST>(element.get_predicate(), context).first.get_index();
    for (const auto term : element.get_terms())
        atom.terms.push_back(merge_d2p(term, context));

    canonicalize(atom);
    return context.destination.get_or_create(atom);
}

template<FactKind T_SRC, FactKind T_DST>
std::pair<PredicateBindingView<T_DST>, bool>
merge_d2p(PredicateView<T_DST> predicate, formalism::datalog::PredicateBindingView<T_SRC> element, MergePlanningContext& context)
{
    auto binding_ptr = context.builder.template get_builder<Binding>();
    auto& binding = *binding_ptr;
    binding.clear();

    for (const auto object : element.get_objects())
        binding.objects.push_back(object.get_index());

    canonicalize(binding);
    return context.destination.get_or_create(predicate, binding.objects);
}

template<FactKind T_SRC, FactKind T_DST>
std::pair<GroundAtomView<T_DST>, bool> merge_d2p(formalism::datalog::GroundAtomView<T_SRC> element, MergePlanningContext& context)
{
    auto atom_ptr = context.builder.template get_builder<GroundAtom<T_DST>>();
    auto& atom = *atom_ptr;
    atom.clear();

    const auto predicate_view = merge_d2p<T_SRC, T_DST>(element.get_predicate(), context).first;
    atom.predicate = predicate_view.get_index();
    atom.row = merge_d2p(predicate_view, element.get_row(), context).first.get_index().second;

    canonicalize(atom);
    return context.destination.get_or_create(atom);
}

template<FactKind T_SRC, FactKind T_DST>
std::pair<LiteralView<T_DST>, bool> merge_d2p(formalism::datalog::LiteralView<T_SRC> element, MergePlanningContext& context)
{
    auto literal_ptr = context.builder.template get_builder<Literal<T_DST>>();
    auto& literal = *literal_ptr;
    literal.clear();

    literal.polarity = element.get_polarity();
    literal.atom = merge_d2p<T_SRC, T_DST>(element.get_atom(), context).first.get_index();

    canonicalize(literal);
    return context.destination.get_or_create(literal);
}

template<FactKind T_SRC, FactKind T_DST>
std::pair<GroundLiteralView<T_DST>, bool> merge_d2p(formalism::datalog::GroundLiteralView<T_SRC> element, MergePlanningContext& context)
{
    auto literal_ptr = context.builder.template get_builder<GroundLiteral<T_DST>>();
    auto& literal = *literal_ptr;
    literal.clear();

    literal.polarity = element.get_polarity();
    literal.atom = merge_d2p<T_SRC, T_DST>(element.get_atom(), context).first.get_index();

    canonicalize(literal);
    return context.destination.get_or_create(literal);
}

// Numeric

template<FactKind T>
std::pair<FunctionView<T>, bool> merge_d2p(formalism::datalog::FunctionView<T> element, MergePlanningContext& context)
{
    auto function_ptr = context.builder.template get_builder<formalism::Function<T>>();
    auto& function = *function_ptr;
    function.clear();

    function.name = element.get_name();
    function.arity = element.get_arity();

    canonicalize(function);
    return context.destination.get_or_create(function);
}

template<FactKind T>
std::pair<FunctionTermView<T>, bool> merge_d2p(formalism::datalog::FunctionTermView<T> element, MergePlanningContext& context)
{
    auto fterm_ptr = context.builder.template get_builder<FunctionTerm<T>>();
    auto& fterm = *fterm_ptr;
    fterm.clear();

    fterm.function = element.get_function().get_index();
    for (const auto term : element.get_terms())
        fterm.terms.push_back(merge_d2p(term, context));

    canonicalize(fterm);
    return context.destination.get_or_create(fterm);
}

template<FactKind T>
std::pair<FunctionBindingView<T>, bool> merge_d2p(FunctionView<T> function, formalism::datalog::FunctionBindingView<T> element, MergePlanningContext& context)
{
    auto binding_ptr = context.builder.template get_builder<Binding>();
    auto& binding = *binding_ptr;
    binding.clear();

    for (const auto object : element.get_objects())
        binding.objects.push_back(object.get_index());

    canonicalize(binding);
    return context.destination.get_or_create(function, binding.objects);
}

template<FactKind T>
std::pair<GroundFunctionTermView<T>, bool> merge_d2p(formalism::datalog::GroundFunctionTermView<T> element, MergePlanningContext& context)
{
    auto fterm_ptr = context.builder.template get_builder<GroundFunctionTerm<T>>();
    auto& fterm = *fterm_ptr;
    fterm.clear();

    const auto function_view = merge_d2p(element.get_function(), context).first;
    fterm.function = function_view.get_index();
    fterm.row = merge_d2p(function_view, element.get_row(), context).first.get_index().second;

    canonicalize(fterm);
    return context.destination.get_or_create(fterm);
}

template<FactKind T>
std::pair<GroundFunctionTermValueView<T>, bool> merge_d2p(formalism::datalog::GroundFunctionTermValueView<T> element, MergePlanningContext& context)
{
    auto fterm_value_ptr = context.builder.template get_builder<GroundFunctionTermValue<T>>();
    auto& fterm_value = *fterm_value_ptr;
    fterm_value.clear();

    fterm_value.fterm = merge_d2p(element.get_fterm(), context).first.get_index();
    fterm_value.value = element.get_value();

    canonicalize(fterm_value);
    return context.destination.get_or_create(fterm_value);
}

TYR_INLINE_IMPL Data<FunctionExpression> merge_d2p(formalism::datalog::FunctionExpressionView element, MergePlanningContext& context)
{
    return visit(
        [&](auto&& arg)
        {
            using Alternative = std::decay_t<decltype(arg)>;

            if constexpr (std::is_same_v<Alternative, float_t>)
                return Data<FunctionExpression>(arg);
            else if constexpr (std::is_same_v<Alternative, formalism::datalog::LiftedArithmeticOperatorView>)
                return Data<FunctionExpression>(merge_d2p(arg, context));
            else if constexpr (std::is_same_v<Alternative, formalism::datalog::FunctionTermView<AuxiliaryTag>>)
                throw std::logic_error("AuxiliaryTag FunctionTerm must not be merged.");
            else
                return Data<FunctionExpression>(merge_d2p(arg, context).first.get_index());
        },
        element.get_variant());
}

TYR_INLINE_IMPL Data<GroundFunctionExpression> merge_d2p(formalism::datalog::GroundFunctionExpressionView element, MergePlanningContext& context)
{
    return visit(
        [&](auto&& arg)
        {
            using Alternative = std::decay_t<decltype(arg)>;

            if constexpr (std::is_same_v<Alternative, float_t>)
                return Data<GroundFunctionExpression>(arg);
            else if constexpr (std::is_same_v<Alternative, formalism::datalog::GroundArithmeticOperatorView>)
                return Data<GroundFunctionExpression>(merge_d2p(arg, context));
            else if constexpr (std::is_same_v<Alternative, formalism::datalog::GroundFunctionTermView<AuxiliaryTag>>)
                throw std::logic_error("AuxiliaryTag GroundFunctionTerm must not be merged.");
            else
                return Data<GroundFunctionExpression>(merge_d2p(arg, context).first.get_index());
        },
        element.get_variant());
}

template<OpKind O, typename T>
std::pair<UnaryOperatorView<O, to_planning_payload_t<T>>, bool> merge_d2p(formalism::datalog::UnaryOperatorView<O, T> element, MergePlanningContext& context)
{
    using T_DST = to_planning_payload_t<T>;

    auto unary_ptr = context.builder.template get_builder<UnaryOperator<O, T_DST>>();
    auto& unary = *unary_ptr;
    unary.clear();

    unary.arg = merge_d2p(element.get_arg(), context);

    canonicalize(unary);
    return context.destination.get_or_create(unary);
}

template<OpKind O, typename T>
std::pair<BinaryOperatorView<O, to_planning_payload_t<T>>, bool> merge_d2p(formalism::datalog::BinaryOperatorView<O, T> element, MergePlanningContext& context)
{
    using T_DST = to_planning_payload_t<T>;

    auto binary_ptr = context.builder.template get_builder<BinaryOperator<O, T_DST>>();
    auto& binary = *binary_ptr;
    binary.clear();

    binary.lhs = merge_d2p(element.get_lhs(), context);
    binary.rhs = merge_d2p(element.get_rhs(), context);

    canonicalize(binary);
    return context.destination.get_or_create(binary);
}

template<OpKind O, typename T>
std::pair<MultiOperatorView<O, to_planning_payload_t<T>>, bool> merge_d2p(formalism::datalog::MultiOperatorView<O, T> element, MergePlanningContext& context)
{
    using T_DST = to_planning_payload_t<T>;

    auto multi_ptr = context.builder.template get_builder<MultiOperator<O, T_DST>>();
    auto& multi = *multi_ptr;
    multi.clear();

    for (const auto arg : element.get_args())
        multi.args.push_back(merge_d2p(arg, context));

    canonicalize(multi);
    return context.destination.get_or_create(multi);
}

template<typename T>
Data<ArithmeticOperator<to_planning_payload_t<T>>> merge_d2p(formalism::datalog::ArithmeticOperatorView<T> element, MergePlanningContext& context)
{
    using T_DST = to_planning_payload_t<T>;

    return visit([&](auto&& arg) { return Data<ArithmeticOperator<T_DST>>(merge_d2p(arg, context).first.get_index()); }, element.get_variant());
}

template<typename T>
Data<BooleanOperator<to_planning_payload_t<T>>> merge_d2p(formalism::datalog::BooleanOperatorView<T> element, MergePlanningContext& context)
{
    using T_DST = to_planning_payload_t<T>;

    return visit([&](auto&& arg) { return Data<BooleanOperator<T_DST>>(merge_d2p(arg, context).first.get_index()); }, element.get_variant());
}

}
