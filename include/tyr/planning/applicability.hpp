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

#ifndef TYR_PLANNING_APPLICABILITY_HPP_
#define TYR_PLANNING_APPLICABILITY_HPP_

#include "tyr/common/dynamic_bitset.hpp"
#include "tyr/common/equal_to.hpp"
#include "tyr/common/hash.hpp"
#include "tyr/common/vector.hpp"
#include "tyr/formalism/arithmetic_operator_utils.hpp"
#include "tyr/formalism/boolean_operator_utils.hpp"
#include "tyr/formalism/planning/declarations.hpp"
#include "tyr/formalism/planning/ground_numeric_effect_operator_utils.hpp"
#include "tyr/formalism/planning/repository.hpp"
#include "tyr/formalism/planning/views.hpp"
#include "tyr/planning/node.hpp"

#include <algorithm>
#include <boost/dynamic_bitset.hpp>
#include <concepts>
#include <iterator>
#include <limits>
#include <numeric>
#include <stdexcept>
#include <vector>

namespace tyr::planning
{

template<typename Task>
struct StateContext
{
    const Task& task;
    const UnpackedState<Task>& unpacked_state;
    float_t auxiliary_value;
};

/**
 * evaluate
 */

// Forward declarations

template<typename Task>
inline float_t evaluate(float_t element, const StateContext<Task>& context);

template<typename Task, formalism::ArithmeticOpKind O>
inline float_t
evaluate(View<Index<formalism::planning::UnaryOperator<O, Data<formalism::planning::GroundFunctionExpression>>>, formalism::planning::Repository> element,
         const StateContext<Task>& context);

template<typename Task, formalism::OpKind O>
inline float_t
evaluate(View<Index<formalism::planning::BinaryOperator<O, Data<formalism::planning::GroundFunctionExpression>>>, formalism::planning::Repository> element,
         const StateContext<Task>& context);

template<typename Task, formalism::ArithmeticOpKind O>
inline float_t
evaluate(View<Index<formalism::planning::MultiOperator<O, Data<formalism::planning::GroundFunctionExpression>>>, formalism::planning::Repository> element,
         const StateContext<Task>& context);

template<typename Task, formalism::FactKind T>
inline float_t evaluate(View<Index<formalism::planning::GroundFunctionTerm<T>>, formalism::planning::Repository> element, const StateContext<Task>& context);

template<typename Task>
inline float_t evaluate(View<Data<formalism::planning::GroundFunctionExpression>, formalism::planning::Repository> element, const StateContext<Task>& context);

template<typename Task>
inline float_t
evaluate(View<Data<formalism::planning::ArithmeticOperator<Data<formalism::planning::GroundFunctionExpression>>>, formalism::planning::Repository> element,
         const StateContext<Task>& context);

template<typename Task>
inline bool
evaluate(View<Data<formalism::planning::BooleanOperator<Data<formalism::planning::GroundFunctionExpression>>>, formalism::planning::Repository> element,
         const StateContext<Task>& context);

template<typename Task, formalism::planning::NumericEffectOpKind Op, formalism::FactKind T>
inline float_t evaluate(View<Index<formalism::planning::GroundNumericEffect<Op, T>>, formalism::planning::Repository> element,
                        const StateContext<Task>& context);

template<typename Task, formalism::FactKind T>
inline float_t evaluate(View<Data<formalism::planning::GroundNumericEffectOperator<T>>, formalism::planning::Repository> element,
                        const StateContext<Task>& context);

// Implementations

template<typename Task>
inline float_t evaluate(float_t element, const StateContext<Task>& context)
{
    return element;
}

template<typename Task, formalism::ArithmeticOpKind O>
inline float_t
evaluate(View<Index<formalism::planning::UnaryOperator<O, Data<formalism::planning::GroundFunctionExpression>>>, formalism::planning::Repository> element,
         const StateContext<Task>& context)
{
    return formalism::apply(O {}, evaluate(element.get_arg(), context));
}

template<typename Task, formalism::OpKind O>
inline float_t
evaluate(View<Index<formalism::planning::BinaryOperator<O, Data<formalism::planning::GroundFunctionExpression>>>, formalism::planning::Repository> element,
         const StateContext<Task>& context)
{
    return formalism::apply(O {}, evaluate(element.get_lhs(), context), evaluate(element.get_rhs(), context));
}

template<typename Task, formalism::ArithmeticOpKind O>
inline float_t
evaluate(View<Index<formalism::planning::MultiOperator<O, Data<formalism::planning::GroundFunctionExpression>>>, formalism::planning::Repository> element,
         const StateContext<Task>& context)
{
    const auto child_fexprs = element.get_args();

    return std::accumulate(std::next(child_fexprs.begin()),  // Start from the second expression
                           child_fexprs.end(),
                           evaluate(child_fexprs.front(), context),
                           [&](const auto& value, const auto& child_expr)
                           { return formalism::apply(formalism::OpMul {}, value, evaluate(child_expr, context)); });
}

template<typename Task>
inline float_t evaluate(View<Index<formalism::planning::GroundFunctionTerm<formalism::StaticTag>>, formalism::planning::Repository> element,
                        const StateContext<Task>& context)
{
    return context.task.get(element.get_index());
}

template<typename Task>
inline float_t evaluate(View<Index<formalism::planning::GroundFunctionTerm<formalism::FluentTag>>, formalism::planning::Repository> element,
                        const StateContext<Task>& context)
{
    return context.unpacked_state.get(element.get_index());
}

template<typename Task>
inline float_t evaluate(View<Index<formalism::planning::GroundFunctionTerm<formalism::AuxiliaryTag>>, formalism::planning::Repository> element,
                        const StateContext<Task>& context)
{
    return context.auxiliary_value;
}

template<typename Task>
inline float_t evaluate(View<Data<formalism::planning::GroundFunctionExpression>, formalism::planning::Repository> element, const StateContext<Task>& context)
{
    return visit([&](auto&& arg) { return evaluate(arg, context); }, element.get_variant());
}

template<typename Task>
inline float_t
evaluate(View<Data<formalism::planning::ArithmeticOperator<Data<formalism::planning::GroundFunctionExpression>>>, formalism::planning::Repository> element,
         const StateContext<Task>& context)
{
    return visit([&](auto&& arg) { return evaluate(arg, context); }, element.get_variant());
}

template<typename Task>
inline bool
evaluate(View<Data<formalism::planning::BooleanOperator<Data<formalism::planning::GroundFunctionExpression>>>, formalism::planning::Repository> element,
         const StateContext<Task>& context)
{
    return visit([&](auto&& arg) { return evaluate(arg, context); }, element.get_variant());
}

template<typename Task, formalism::planning::NumericEffectOpKind Op, formalism::FactKind T>
inline float_t evaluate(View<Index<formalism::planning::GroundNumericEffect<Op, T>>, formalism::planning::Repository> element,
                        const StateContext<Task>& context)
{
    return formalism::planning::apply(Op {}, evaluate(element.get_fterm(), context), evaluate(element.get_fexpr(), context));
}

template<typename Task, formalism::FactKind T>
inline float_t evaluate(View<Data<formalism::planning::GroundNumericEffectOperator<T>>, formalism::planning::Repository> element,
                        const StateContext<Task>& context)
{
    return visit([&](auto&& arg) { return evaluate(arg, context); }, element.get_variant());
}

/**
 * is_applicable_if_fires
 */

template<typename Task>
inline bool is_applicable_if_fires(View<Index<formalism::planning::GroundConditionalEffect>, formalism::planning::Repository> element,
                                   const StateContext<Task>& context,
                                   formalism::planning::EffectFamilyList& ref_fluent_effect_families)
{
    if (!is_applicable(element.get_condition(), context))
        return true;

    // Important: only modify effect families if condition is satisfied
    return is_applicable(element.get_effect(), context, ref_fluent_effect_families);
}

/**
 * is_applicable
 */

template<typename Task>
inline bool is_applicable(View<Index<formalism::planning::GroundLiteral<formalism::StaticTag>>, formalism::planning::Repository> element,
                          const StateContext<Task>& context)
{
    return context.task.test(element.get_atom().get_index()) == element.get_polarity();
}

template<typename Task>
inline bool is_applicable(View<Index<formalism::planning::GroundLiteral<formalism::DerivedTag>>, formalism::planning::Repository> element,
                          const StateContext<Task>& context)
{
    return context.unpacked_state.test(element.get_atom().get_index()) == element.get_polarity();
}

template<typename Task, formalism::FactKind T>
inline bool is_applicable(View<IndexList<formalism::planning::GroundLiteral<T>>, formalism::planning::Repository> elements, const StateContext<Task>& context)
{
    return std::all_of(elements.begin(), elements.end(), [&](auto&& arg) { return is_applicable(arg, context); });
}

template<typename Task>
inline bool is_applicable(View<Data<formalism::planning::FDRFact<formalism::FluentTag>>, formalism::planning::Repository> element,
                          const StateContext<Task>& context)
{
    return context.unpacked_state.get(element.get_variable().get_index()) == element.get_value();
}

template<typename Task>
inline bool is_applicable(View<DataList<formalism::planning::FDRFact<formalism::FluentTag>>, formalism::planning::Repository> elements,
                          const StateContext<Task>& context)
{
    return std::all_of(elements.begin(), elements.end(), [&](auto&& arg) { return is_applicable(arg, context); });
}

template<typename Task>
inline bool is_applicable(
    View<DataList<formalism::planning::BooleanOperator<Data<formalism::planning::GroundFunctionExpression>>>, formalism::planning::Repository> elements,
    const StateContext<Task>& context)
{
    return std::all_of(elements.begin(), elements.end(), [&](auto&& arg) { return evaluate(arg, context); });
}

template<typename Task, formalism::planning::NumericEffectOpKind Op>
inline bool is_applicable(View<Index<formalism::planning::GroundNumericEffect<Op, formalism::FluentTag>>, formalism::planning::Repository> element,
                          const StateContext<Task>& context,
                          formalism::planning::EffectFamilyList& ref_fluent_effect_families)
{
    const auto fterm_index = element.get_fterm().get_index();
    ref_fluent_effect_families.resize(fterm_index.get_value() + 1, formalism::planning::EffectFamily::NONE);

    // Check non-conflicting effects
    if (!is_compatible_effect_family(Op::family, ref_fluent_effect_families[fterm_index.get_value()]))
        return false;  /// incompatible effects

    ref_fluent_effect_families[fterm_index.get_value()] = Op::family;

    // Check fterm is well-defined in context
    if constexpr (!std::is_same_v<Op, formalism::planning::OpAssign>)
    {
        if (std::isnan(context.unpacked_state.get(fterm_index)))
            return false;  /// target function is undefined and operator is not assign
    }

    // Check fexpr is well-defined in context
    return !std::isnan(evaluate(element.get_fexpr(), context));
}

template<typename Task>
inline bool is_applicable(View<Data<formalism::planning::GroundNumericEffectOperator<formalism::FluentTag>>, formalism::planning::Repository> element,
                          const StateContext<Task>& context,
                          formalism::planning::EffectFamilyList& ref_fluent_effect_families)
{
    return visit([&](auto&& arg) { return is_applicable(arg, context, ref_fluent_effect_families); }, element.get_variant());
}

template<typename Task>
inline bool is_applicable(View<DataList<formalism::planning::GroundNumericEffectOperator<formalism::FluentTag>>, formalism::planning::Repository> elements,
                          const StateContext<Task>& context,
                          formalism::planning::EffectFamilyList& ref_fluent_effect_families)
{
    return std::all_of(elements.begin(), elements.end(), [&](auto&& arg) { return is_applicable(arg, context, ref_fluent_effect_families); });
}

template<typename Task>
inline bool is_applicable(
    View<Index<formalism::planning::GroundNumericEffect<formalism::planning::OpIncrease, formalism::AuxiliaryTag>>, formalism::planning::Repository> element,
    const StateContext<Task>& context)
{
    // Check fexpr is well-defined in context
    return !std::isnan(evaluate(element.get_fexpr(), context));
}

template<typename Task>
inline bool is_applicable(View<Data<formalism::planning::GroundNumericEffectOperator<formalism::AuxiliaryTag>>, formalism::planning::Repository> element,
                          const StateContext<Task>& context)
{
    return visit([&](auto&& arg) { return is_applicable(arg, context); }, element.get_variant());
}

// GroundConjunctiveCondition

template<typename Task>
inline bool is_applicable(View<Index<formalism::planning::GroundConjunctiveCondition>, formalism::planning::Repository> element,
                          const StateContext<Task>& context)
{
    return is_applicable(element.template get_facts<formalism::StaticTag>(), context)      //
           && is_applicable(element.template get_facts<formalism::FluentTag>(), context)   //
           && is_applicable(element.template get_facts<formalism::DerivedTag>(), context)  //
           && is_applicable(element.get_numeric_constraints(), context);
}

// GroundConjunctiveEffect

template<typename Task>
inline bool is_applicable(View<Index<formalism::planning::GroundConjunctiveEffect>, formalism::planning::Repository> element,
                          const StateContext<Task>& context,
                          formalism::planning::EffectFamilyList& ref_fluent_effect_families)
{
    return is_applicable(element.get_numeric_effects(), context, ref_fluent_effect_families)
           && (!element.get_auxiliary_numeric_effect().has_value() || is_applicable(element.get_auxiliary_numeric_effect().value(), context));
}

// GroundConditionalEffectList

template<typename Task>
inline bool are_applicable_if_fires(View<IndexList<formalism::planning::GroundConditionalEffect>, formalism::planning::Repository> elements,
                                    const StateContext<Task>& context,
                                    formalism::planning::EffectFamilyList& out_fluent_effect_families)
{
    out_fluent_effect_families.clear();

    return std::all_of(elements.begin(),
                       elements.end(),
                       [&](auto&& cond_effect) { return is_applicable_if_fires(cond_effect, context, out_fluent_effect_families); });
}

// GroundAction

template<typename Task>
inline bool is_applicable(View<Index<formalism::planning::GroundAction>, formalism::planning::Repository> element,
                          const StateContext<Task>& context,
                          formalism::planning::EffectFamilyList& out_fluent_effect_families)
{
    return is_applicable(element.get_condition(), context) && are_applicable_if_fires(element.get_effects(), context, out_fluent_effect_families);
}

// GroundAxiom

template<typename Task>
inline bool is_applicable(View<Index<formalism::planning::GroundAxiom>, formalism::planning::Repository> element, const StateContext<Task>& context)
{
    return is_applicable(element.get_body(), context);
}

/**
 * is_statically_applicable
 */

inline bool is_statically_applicable(View<Index<formalism::planning::GroundLiteral<formalism::StaticTag>>, formalism::planning::Repository> element,
                                     const boost::dynamic_bitset<>& static_atoms)
{
    return tyr::test(uint_t(element.get_atom().get_index()), static_atoms) == element.get_polarity();
}

inline bool is_statically_applicable(View<IndexList<formalism::planning::GroundLiteral<formalism::StaticTag>>, formalism::planning::Repository> elements,
                                     const boost::dynamic_bitset<>& static_atoms)
{
    return std::all_of(elements.begin(), elements.end(), [&](auto&& arg) { return is_statically_applicable(arg, static_atoms); });
}

// GroundConjunctiveCondition

inline bool is_statically_applicable(View<Index<formalism::planning::GroundConjunctiveCondition>, formalism::planning::Repository> element,
                                     const boost::dynamic_bitset<>& static_atoms)
{
    return is_statically_applicable(element.template get_facts<formalism::StaticTag>(), static_atoms);
}

// GroundAction

inline bool is_statically_applicable(View<Index<formalism::planning::GroundAction>, formalism::planning::Repository> element,
                                     const boost::dynamic_bitset<>& static_atoms)
{
    return is_statically_applicable(element.get_condition(), static_atoms);
}

// GroundAxiom

inline bool is_statically_applicable(View<Index<formalism::planning::GroundAxiom>, formalism::planning::Repository> element,
                                     const boost::dynamic_bitset<>& static_atoms)
{
    return is_statically_applicable(element.get_body(), static_atoms);
}

/**
 * is_dynamically_applicable
 */

// GroundConjunctiveCondition

template<typename Task>
inline bool is_dynamically_applicable(View<Index<formalism::planning::GroundConjunctiveCondition>, formalism::planning::Repository> element,
                                      const StateContext<Task>& context)
{
    return is_applicable(element.template get_facts<formalism::FluentTag>(), context)      //
           && is_applicable(element.template get_facts<formalism::DerivedTag>(), context)  //
           && is_applicable(element.get_numeric_constraints(), context);
}

/**
 * is_consistent
 */

// GroundConjunctiveCondition

inline bool is_consistent(View<Index<formalism::planning::GroundConjunctiveCondition>, formalism::planning::Repository> element,
                          UnorderedMap<Index<formalism::planning::FDRVariable<formalism::FluentTag>>, formalism::planning::FDRValue>& fluent_assign,
                          UnorderedMap<Index<formalism::planning::GroundAtom<formalism::DerivedTag>>, bool>& derived_assign)
{
    for (const auto fact : element.template get_facts<formalism::FluentTag>())
    {
        if (const auto it = fluent_assign.find(fact.get_variable().get_index()); it != fluent_assign.end() && it->second != fact.get_value())
            return false;
        else
            fluent_assign.emplace(fact.get_variable().get_index(), fact.get_value());
    }

    for (const auto literal : element.template get_facts<formalism::DerivedTag>())
    {
        if (const auto it = derived_assign.find(literal.get_atom().get_index()); it != derived_assign.end() && it->second != literal.get_polarity())
            return false;
        else
            derived_assign.emplace(literal.get_atom().get_index(), literal.get_polarity());
    }

    return true;
}

// GroundAction

inline bool is_consistent(View<Index<formalism::planning::GroundAction>, formalism::planning::Repository> element,
                          UnorderedMap<Index<formalism::planning::FDRVariable<formalism::FluentTag>>, formalism::planning::FDRValue>& out_fluent_assign,
                          UnorderedMap<Index<formalism::planning::GroundAtom<formalism::DerivedTag>>, bool>& out_derived_assign)
{
    out_fluent_assign.clear();
    out_derived_assign.clear();
    return is_consistent(element.get_condition(), out_fluent_assign, out_derived_assign);
}

// GroundAxiom

inline bool is_consistent(View<Index<formalism::planning::GroundAxiom>, formalism::planning::Repository> element,
                          UnorderedMap<Index<formalism::planning::FDRVariable<formalism::FluentTag>>, formalism::planning::FDRValue>& out_fluent_assign,
                          UnorderedMap<Index<formalism::planning::GroundAtom<formalism::DerivedTag>>, bool>& out_derived_assign)
{
    out_fluent_assign.clear();
    out_derived_assign.clear();
    return is_consistent(element.get_body(), out_fluent_assign, out_derived_assign);
}

}

#endif
