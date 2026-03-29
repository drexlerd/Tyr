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

#include "datas.hpp"

namespace tyr::formalism::planning
{
namespace
{

/**
 * Data
 */

void bind_object_builder(nb::module_& m, const std::string& name)
{
    using V = Data<Object>;

    nb::class_<V>(m, name.c_str())  //
        .def(nb::init<const std::string&>(), "name"_a);
}

void bind_variable_builder(nb::module_& m, const std::string& name)
{
    using V = Data<Variable>;

    auto cls = nb::class_<V>(m, name.c_str())  //
                   .def(nb::init<const std::string&>(), "name"_a);
    add_print(cls);
    add_hash(cls);
}

void bind_term_builder(nb::module_& m, const std::string& name)
{
    using V = Data<Term>;

    auto cls = nb::class_<V>(m, name.c_str())  //
                   .def(nb::init<typename V::template ViewVariant<Repository>>(), "value"_a);
    add_print(cls);
    add_hash(cls);
}

template<typename Tag>
void bind_relation_binding_builder(nb::module_& m, const std::string& name)
{
    using V = Data<RelationBinding<Tag>>;

    auto cls = nb::class_<V>(m, name.c_str())  //
                   .def(nb::init<View<Index<Tag>, Repository>, const ObjectViewList&>(), "relation"_a, "objects"_a);
    add_print(cls);
    add_hash(cls);
}

template<FactKind T>
void bind_predicate_builder(nb::module_& m, const std::string& name)
{
    using V = Data<Predicate<T>>;

    auto cls = nb::class_<V>(m, name.c_str())  //
                   .def(nb::init<const std::string&, uint_t>(), "name"_a, "arity"_a);
    add_print(cls);
    add_hash(cls);
}

template<FactKind T>
void bind_atom_builder(nb::module_& m, const std::string& name)
{
    using V = Data<Atom<T>>;

    auto cls = nb::class_<V>(m, name.c_str())  //
                   .def(nb::init<PredicateView<T>, const TermViewList&>(), "predicate"_a, "terms"_a);
    add_print(cls);
    add_hash(cls);
}

template<FactKind T>
void bind_ground_atom_builder(nb::module_& m, const std::string& name)
{
    using V = Data<GroundAtom<T>>;

    auto cls = nb::class_<V>(m, name.c_str())  //
                   .def(nb::init<PredicateBindingView<T>>(), "binding"_a);
    add_print(cls);
    add_hash(cls);
}

template<FactKind T>
void bind_literal_builder(nb::module_& m, const std::string& name)
{
    using V = Data<Literal<T>>;

    auto cls = nb::class_<V>(m, name.c_str())  //
                   .def(nb::init<AtomView<T>, bool>(), "atom"_a, "polarity"_a);
    add_print(cls);
    add_hash(cls);
}

template<FactKind T>
void bind_ground_literal_builder(nb::module_& m, const std::string& name)
{
    using V = Data<GroundLiteral<T>>;

    auto cls = nb::class_<V>(m, name.c_str())  //
                   .def(nb::init<GroundAtomView<T>, bool>(), "atom"_a, "polarity"_a);
    add_print(cls);
    add_hash(cls);
}

template<FactKind T>
void bind_fdr_variable_builder(nb::module_& m, const std::string& name)
{
    using V = Data<FDRVariable<T>>;

    auto cls = nb::class_<V>(m, name.c_str())  //
                   .def(nb::init<const GroundAtomViewList<T>>(), "atoms"_a);
    add_print(cls);
    add_hash(cls);
}

template<FactKind T>
void bind_fdr_fact_builder(nb::module_& m, const std::string& name)
{
    using V = Data<FDRFact<T>>;

    auto cls = nb::class_<V>(m, name.c_str())  //
                   .def(nb::init<FDRVariableView<T>, FDRValue>(), "variable"_a, "value"_a);
    add_print(cls);
    add_hash(cls);
}

template<FactKind T>
void bind_function_builder(nb::module_& m, const std::string& name)
{
    using V = Data<Function<T>>;

    auto cls = nb::class_<V>(m, name.c_str())  //
                   .def(nb::init<const std::string&, uint_t>(), "name"_a, "arity"_a);
    add_print(cls);
    add_hash(cls);
}

template<FactKind T>
void bind_function_term_builder(nb::module_& m, const std::string& name)
{
    using V = Data<FunctionTerm<T>>;

    auto cls = nb::class_<V>(m, name.c_str())  //
                   .def(nb::init<FunctionView<T>, const TermViewList&>(), "function"_a, "terms"_a);
    add_print(cls);
    add_hash(cls);
}

template<FactKind T>
void bind_ground_function_term_builder(nb::module_& m, const std::string& name)
{
    using V = Data<GroundFunctionTerm<T>>;

    auto cls = nb::class_<V>(m, name.c_str())  //
                   .def(nb::init<FunctionBindingView<T>>(), "binding"_a);
    add_print(cls);
    add_hash(cls);
}

template<FactKind T>
void bind_ground_function_term_value_builder(nb::module_& m, const std::string& name)
{
    using V = Data<GroundFunctionTermValue<T>>;

    auto cls = nb::class_<V>(m, name.c_str())  //
                   .def(nb::init<GroundFunctionTermView<T>, float_t>(), "fterm"_a, "value"_a);
    add_print(cls);
    add_hash(cls);
}

template<NumericEffectOpKind Op, FactKind T>
void bind_numeric_effect_builder(nb::module_& m, const std::string& name)
{
    using V = Data<NumericEffect<Op, T>>;

    auto cls = nb::class_<V>(m, name.c_str())  //
                   .def(nb::init<FunctionTermView<T>, FunctionExpressionView>(), "fterm"_a, "fexpr"_a);
    add_print(cls);
    add_hash(cls);
}

template<NumericEffectOpKind Op, FactKind T>
void bind_ground_numeric_effect_builder(nb::module_& m, const std::string& name)
{
    using V = Data<GroundNumericEffect<Op, T>>;

    auto cls = nb::class_<V>(m, name.c_str())  //
                   .def(nb::init<GroundFunctionTermView<T>, GroundFunctionExpressionView>(), "fterm"_a, "fexpr"_a);
    add_print(cls);
    add_hash(cls);
}

template<FactKind T>
void bind_numeric_effect_operator_builder(nb::module_& m, const std::string& name)
{
    using V = Data<NumericEffectOperator<T>>;

    auto cls = nb::class_<V>(m, name.c_str())  //
                   .def(nb::init<typename V::template ViewVariant<Repository>>(), "value"_a);
    add_print(cls);
    add_hash(cls);
}

template<FactKind T>
void bind_ground_numeric_effect_operator_builder(nb::module_& m, const std::string& name)
{
    using V = Data<GroundNumericEffectOperator<T>>;

    auto cls = nb::class_<V>(m, name.c_str())  //
                   .def(nb::init<typename V::template ViewVariant<Repository>>(), "value"_a);
    add_print(cls);
    add_hash(cls);
}

void bind_function_expression_builder(nb::module_& m, const std::string& name)
{
    using V = Data<FunctionExpression>;

    auto cls = nb::class_<V>(m, name.c_str())  //
                   .def(nb::init<typename V::template ViewVariant<Repository>>(), "value"_a);
    add_print(cls);
    add_hash(cls);
}

void bind_conjunctive_condition_builder(nb::module_& m, const std::string& name)
{
    using V = Data<ConjunctiveCondition>;

    auto cls = nb::class_<V>(m, name.c_str())  //
                   .def(nb::init<const VariableViewList&,
                                 const LiteralViewList<StaticTag>&,
                                 const LiteralViewList<FluentTag>&,
                                 const LiteralViewList<DerivedTag>&,
                                 const LiftedBooleanOperatorViewList&>(),
                        "variables"_a,
                        "static_literals"_a,
                        "fluent_literals"_a,
                        "derived_literals"_a,
                        "numeric_constraints"_a);
    add_print(cls);
    add_hash(cls);
}

void bind_conjunctive_effect_builder(nb::module_& m, const std::string& name)
{
    using V = Data<ConjunctiveEffect>;

    auto cls = nb::class_<V>(m, name.c_str())  //
                   .def(nb::init<const LiteralViewList<FluentTag>&,
                                 const NumericEffectOperatorViewList<FluentTag>&,
                                 const std::optional<NumericEffectOperatorView<AuxiliaryTag>>&>(),
                        "fluent_literals"_a,
                        "fluent_numeric_effects"_a,
                        "auxiliary_numeric_effect"_a);
    add_print(cls);
    add_hash(cls);
}

void bind_conditional_effect_builder(nb::module_& m, const std::string& name)
{
    using V = Data<ConditionalEffect>;

    auto cls = nb::class_<V>(m, name.c_str())  //
                   .def(nb::init<const VariableViewList&, ConjunctiveConditionView, ConjunctiveEffectView>(), "variables"_a, "condition"_a, "effect"_a);
    add_print(cls);
    add_hash(cls);
}

void bind_action_builder(nb::module_& m, const std::string& name)
{
    using V = Data<Action>;

    auto cls = nb::class_<V>(m, name.c_str())  //
                   .def(nb::init<const std::string&, uint_t, const VariableViewList&, ConjunctiveConditionView, const ConditionalEffectViewList&>(),
                        "name"_a,
                        "original_arity"_a,
                        "variables"_a,
                        "condition"_a,
                        "effects"_a);
    add_print(cls);
    add_hash(cls);
}

void bind_axiom_builder(nb::module_& m, const std::string& name)
{
    using V = Data<Axiom>;

    auto cls = nb::class_<V>(m, name.c_str())  //
                   .def(nb::init<const VariableViewList&, ConjunctiveConditionView, AtomView<DerivedTag>>(), "variables"_a, "body"_a, "head"_a);
    add_print(cls);
    add_hash(cls);
}

void bind_ground_function_expression_builder(nb::module_& m, const std::string& name)
{
    using V = Data<GroundFunctionExpression>;

    auto cls = nb::class_<V>(m, name.c_str())  //
                   .def(nb::init<typename V::template ViewVariant<Repository>>(), "value"_a);
    add_print(cls);
    add_hash(cls);
}

void bind_ground_conjunctive_condition_builder(nb::module_& m, const std::string& name)
{
    using V = Data<GroundConjunctiveCondition>;

    auto cls = nb::class_<V>(m, name.c_str())  //
                   .def(nb::init<const GroundLiteralViewList<StaticTag>&,
                                 const FDRFactViewList<FluentTag>&,
                                 const GroundLiteralViewList<DerivedTag>&,
                                 const GroundBooleanOperatorViewList&>(),
                        "static_literals"_a,
                        "fluent_facts"_a,
                        "derived_literals"_a,
                        "numeric_constraints"_a);
    add_print(cls);
    add_hash(cls);
}

void bind_ground_conjunctive_effect_builder(nb::module_& m, const std::string& name)
{
    using V = Data<GroundConjunctiveEffect>;

    auto cls = nb::class_<V>(m, name.c_str())  //
                   .def(nb::init<const FDRFactViewList<FluentTag>&,
                                 const GroundNumericEffectOperatorViewList<FluentTag>&,
                                 const std::optional<GroundNumericEffectOperatorView<AuxiliaryTag>>&>(),
                        "fluent_facts"_a,
                        "fluent_numeric_effects"_a,
                        "auxiliary_numeric_effect"_a);
    add_print(cls);
    add_hash(cls);
}

void bind_ground_conditional_effect_builder(nb::module_& m, const std::string& name)
{
    using V = Data<GroundConditionalEffect>;

    auto cls = nb::class_<V>(m, name.c_str())  //
                   .def(nb::init<GroundConjunctiveConditionView, GroundConjunctiveEffectView>(), "condition"_a, "effect"_a);
    add_print(cls);
    add_hash(cls);
}

void bind_ground_action_builder(nb::module_& m, const std::string& name)
{
    using V = Data<GroundAction>;

    auto cls = nb::class_<V>(m, name.c_str())  //
                   .def(nb::init<ActionBindingView, GroundConjunctiveConditionView, const GroundConditionalEffectViewList&>(),
                        "binding"_a,
                        "condition"_a,
                        "effects"_a);
    add_print(cls);
    add_hash(cls);
}

void bind_ground_axiom_builder(nb::module_& m, const std::string& name)
{
    using V = Data<GroundAxiom>;

    auto cls = nb::class_<V>(m, name.c_str())  //
                   .def(nb::init<AxiomBindingView, GroundConjunctiveConditionView, GroundAtomView<DerivedTag>>(), "binding"_a, "body"_a, "head"_a);
    add_print(cls);
    add_hash(cls);
}

void bind_metric_builder(nb::module_& m, const std::string& name)
{
    using V = Data<Metric>;

    auto cls = nb::class_<V>(m, name.c_str())  //
                   .def(nb::init<typename V::ObjectiveVariant, GroundFunctionExpressionView>(), "objective_kind"_a, "fexpr"_a);
    add_print(cls);
    add_hash(cls);
}

void bind_domain_builder(nb::module_& m, const std::string& name)
{
    using V = Data<Domain>;

    auto cls = nb::class_<V>(m, name.c_str())  //
                   .def(nb::init<const std::string&,
                                 const PredicateViewList<StaticTag>&,
                                 const PredicateViewList<FluentTag>&,
                                 const PredicateViewList<DerivedTag>&,
                                 const FunctionViewList<StaticTag>&,
                                 const FunctionViewList<FluentTag>&,
                                 const std::optional<FunctionView<AuxiliaryTag>>&,
                                 const ObjectViewList&,
                                 const ActionViewList&,
                                 const AxiomViewList&>(),
                        "name"_a,
                        "static_predicates"_a,
                        "fluent_predicates"_a,
                        "derived_predicates"_a,
                        "static_functions"_a,
                        "fluent_functions"_a,
                        "auxiliary_function"_a,
                        "constants"_a,
                        "actions"_a,
                        "axioms"_a);
    add_print(cls);
    add_hash(cls);
}

void bind_lifted_task_builder(nb::module_& m, const std::string& name)
{
    using V = Data<Task>;

    auto cls = nb::class_<V>(m, name.c_str())  //
                   .def(nb::init<const std::string&,
                                 DomainView,
                                 const PredicateViewList<DerivedTag>&,
                                 const ObjectViewList&,
                                 const GroundAtomViewList<StaticTag>&,
                                 const GroundAtomViewList<FluentTag>&,
                                 const GroundFunctionTermValueViewList<StaticTag>&,
                                 const GroundFunctionTermValueViewList<FluentTag>&,
                                 const std::optional<GroundFunctionTermValueView<AuxiliaryTag>>&,
                                 GroundConjunctiveConditionView,
                                 const std::optional<MetricView>&,
                                 const AxiomViewList&>(),
                        "name"_a,
                        "domain"_a,
                        "derived_predicates"_a,
                        "objects"_a,
                        "static_atoms"_a,
                        "fluent_atoms"_a,
                        "static_fterm_values"_a,
                        "fluent_fterm_values"_a,
                        "auxiliary_fterm_value"_a,
                        "goal"_a,
                        "metric"_a,
                        "axioms"_a);
    add_print(cls);
    add_hash(cls);
}

void bind_ground_task_builder(nb::module_& m, const std::string& name)
{
    using V = Data<FDRTask>;

    auto cls = nb::class_<V>(m, name.c_str())  //

                   .def(nb::init<const std::string&,
                                 DomainView,
                                 const PredicateViewList<DerivedTag>&,
                                 const ObjectViewList&,
                                 const GroundAtomViewList<StaticTag>&,
                                 const GroundAtomViewList<FluentTag>&,
                                 const GroundAtomViewList<DerivedTag>&,
                                 const GroundFunctionTermViewList<StaticTag>&,
                                 const GroundFunctionTermViewList<FluentTag>&,
                                 const std::optional<GroundFunctionTermView<AuxiliaryTag>>&,
                                 const GroundFunctionTermValueViewList<StaticTag>&,
                                 const GroundFunctionTermValueViewList<FluentTag>&,
                                 const std::optional<GroundFunctionTermValueView<AuxiliaryTag>>&,
                                 const std::optional<MetricView>&,
                                 const AxiomViewList&,
                                 const FDRVariableViewList<FluentTag>&,
                                 const FDRFactViewList<FluentTag>&,
                                 GroundConjunctiveConditionView,
                                 const GroundActionViewList&,
                                 const GroundAxiomViewList&>(),
                        "name"_a,
                        "domain"_a,
                        "derived_predicates"_a,
                        "objects"_a,
                        "static_atoms"_a,
                        "fluent_atoms"_a,
                        "derived_atoms"_a,
                        "static_fterm"_a,
                        "fluent_fterm"_a,
                        "auxiliary_fterm"_a,
                        "static_fterm_values"_a,
                        "fluent_fterm_values"_a,
                        "auxiliary_fterm_value"_a,
                        "metric"_a,
                        "axioms"_a,
                        "fluent_variables"_a,
                        "fluent_facts"_a,
                        "goal"_a,
                        "ground_actions"_a,
                        "ground_axioms"_a);
    add_print(cls);
    add_hash(cls);
}

template<OpKind Op, typename T>
void bind_unary_operator_builder(nb::module_& m, const std::string& name)
{
    using V = Data<UnaryOperator<Op, T>>;

    auto cls = nb::class_<V>(m, name.c_str())  //
                   .def(nb::init<View<T, Repository>>(), "arg"_a);
    add_print(cls);
    add_hash(cls);
}

template<OpKind Op, typename T>
void bind_binary_operator_builder(nb::module_& m, const std::string& name)
{
    using V = Data<BinaryOperator<Op, T>>;

    auto cls = nb::class_<V>(m, name.c_str())  //
                   .def(nb::init<View<T, Repository>, View<T, Repository>>(), "lhs"_a, "rhs"_a);
    add_print(cls);
    add_hash(cls);
}

template<OpKind Op, typename T>
void bind_multi_operator_builder(nb::module_& m, const std::string& name)
{
    using V = Data<MultiOperator<Op, T>>;

    auto cls = nb::class_<V>(m, name.c_str())  //
                   .def(nb::init<const std::vector<View<T, Repository>>&>(), "args"_a);
    add_print(cls);
    add_hash(cls);
}

template<typename T>
void bind_arithmethic_operator_builder(nb::module_& m, const std::string& name)
{
    using V = Data<ArithmeticOperator<T>>;

    auto cls = nb::class_<V>(m, name.c_str())  //
                   .def(nb::init<typename V::template ViewVariant<Repository>>(), "value"_a);
    add_print(cls);
    add_hash(cls);
}

template<typename T>
void bind_boolean_operator_builder(nb::module_& m, const std::string& name)
{
    using V = Data<BooleanOperator<T>>;

    auto cls = nb::class_<V>(m, name.c_str())  //
                   .def(nb::init<typename V::template ViewVariant<Repository>>(), "value"_a);
    add_print(cls);
    add_hash(cls);
}
}

/**
 * bind_datas
 */

void bind_datas(nb::module_& m)
{
    bind_object_builder(m, "ObjectBuilder");
    bind_variable_builder(m, "VariableBuilder");
    bind_term_builder(m, "TermBuilder");
    bind_relation_binding_builder<Predicate<StaticTag>>(m, "StaticPredicateBindingBuilder");
    bind_relation_binding_builder<Predicate<FluentTag>>(m, "FluentPredicateBindingBuilder");
    bind_relation_binding_builder<Predicate<DerivedTag>>(m, "DerivedPredicateBindingBuilder");
    bind_relation_binding_builder<Function<StaticTag>>(m, "StaticFunctionBindingBuilder");
    bind_relation_binding_builder<Function<FluentTag>>(m, "FluentFunctionBindingBuilder");
    bind_relation_binding_builder<Function<AuxiliaryTag>>(m, "AuxiliaryFunctionBindingBuilder");
    bind_relation_binding_builder<Action>(m, "ActionBindingBuilder");
    bind_relation_binding_builder<Axiom>(m, "AxiomBindingBuilder");

    bind_predicate_builder<StaticTag>(m, "StaticPredicateBuilder");
    bind_predicate_builder<FluentTag>(m, "FluentPredicateBuilder");
    bind_predicate_builder<DerivedTag>(m, "DerivedPredicateBuilder");

    bind_atom_builder<StaticTag>(m, "StaticAtomBuilder");
    bind_atom_builder<FluentTag>(m, "FluentAtomBuilder");
    bind_atom_builder<DerivedTag>(m, "DerivedAtomBuilder");

    bind_ground_atom_builder<StaticTag>(m, "StaticGroundAtomBuilder");
    bind_ground_atom_builder<FluentTag>(m, "FluentGroundAtomBuilder");
    bind_ground_atom_builder<DerivedTag>(m, "DerivedGroundAtomBuilder");

    bind_literal_builder<StaticTag>(m, "StaticLiteralBuilder");
    bind_literal_builder<FluentTag>(m, "FluentLiteralBuilder");
    bind_literal_builder<DerivedTag>(m, "DerivedLiteralBuilder");

    bind_ground_literal_builder<StaticTag>(m, "StaticGroundLiteralBuilder");
    bind_ground_literal_builder<FluentTag>(m, "FluentGroundLiteralBuilder");
    bind_ground_literal_builder<DerivedTag>(m, "DerivedGroundLiteralBuilder");

    bind_fdr_variable_builder<FluentTag>(m, "FluentFDRVariableBuilder");
    bind_fdr_fact_builder<FluentTag>(m, "FluentFDRFactBuilder");

    bind_function_builder<StaticTag>(m, "StaticFunctionBuilder");
    bind_function_builder<FluentTag>(m, "FluentFunctionBuilder");
    bind_function_builder<AuxiliaryTag>(m, "AuxiliaryFunctionBuilder");

    bind_function_term_builder<StaticTag>(m, "StaticFunctionTermBuilder");
    bind_function_term_builder<FluentTag>(m, "FluentFunctionTermBuilder");
    bind_function_term_builder<AuxiliaryTag>(m, "AuxiliaryFunctionTermBuilder");

    bind_ground_function_term_builder<StaticTag>(m, "StaticGroundFunctionTermBuilder");
    bind_ground_function_term_builder<FluentTag>(m, "FluentGroundFunctionTermBuilder");
    bind_ground_function_term_builder<AuxiliaryTag>(m, "AuxiliaryGroundFunctionTermBuilder");

    bind_ground_function_term_value_builder<StaticTag>(m, "StaticGroundFunctionTermValueBuilder");
    bind_ground_function_term_value_builder<FluentTag>(m, "FluentGroundFunctionTermValueBuilder");
    bind_ground_function_term_value_builder<AuxiliaryTag>(m, "AuxiliaryGroundFunctionTermValueBuilder");

    bind_unary_operator_builder<OpSub, Data<FunctionExpression>>(m, "UnaryOperatorSubBuilder");
    bind_binary_operator_builder<OpAdd, Data<FunctionExpression>>(m, "BinaryOperatorAddBuilder");
    bind_binary_operator_builder<OpSub, Data<FunctionExpression>>(m, "BinaryOperatorSubBuilder");
    bind_binary_operator_builder<OpMul, Data<FunctionExpression>>(m, "BinaryOperatorMulBuilder");
    bind_binary_operator_builder<OpDiv, Data<FunctionExpression>>(m, "BinaryOperatorDivBuilder");
    bind_binary_operator_builder<OpEq, Data<FunctionExpression>>(m, "BinaryOperatorEqBuilder");
    bind_binary_operator_builder<OpNe, Data<FunctionExpression>>(m, "BinaryOperatorNeBuilder");
    bind_binary_operator_builder<OpLe, Data<FunctionExpression>>(m, "BinaryOperatorLeBuilder");
    bind_binary_operator_builder<OpLt, Data<FunctionExpression>>(m, "BinaryOperatorLtBuilder");
    bind_binary_operator_builder<OpGe, Data<FunctionExpression>>(m, "BinaryOperatorGeBuilder");
    bind_binary_operator_builder<OpGt, Data<FunctionExpression>>(m, "BinaryOperatorGtBuilder");

    bind_multi_operator_builder<OpAdd, Data<FunctionExpression>>(m, "MultiOperatorAddBuilder");
    bind_multi_operator_builder<OpMul, Data<FunctionExpression>>(m, "MultiOperatorMulBuilder");

    bind_arithmethic_operator_builder<Data<FunctionExpression>>(m, "ArithmeticOperatorBuilder");
    bind_boolean_operator_builder<Data<FunctionExpression>>(m, "BooleanOperatorBuilder");

    bind_function_expression_builder(m, "FunctionExpressionBuilder");
    bind_conjunctive_condition_builder(m, "ConjunctiveConditionBuilder");

    bind_numeric_effect_builder<OpAssign, FluentTag>(m, "FluentNumericEffectAssignBuilder");
    bind_numeric_effect_builder<OpIncrease, FluentTag>(m, "FluentNumericEffectIncreaseBuilder");
    bind_numeric_effect_builder<OpDecrease, FluentTag>(m, "FluentNumericEffectDecreaseBuilder");
    bind_numeric_effect_builder<OpScaleUp, FluentTag>(m, "FluentNumericEffectScaleUpBuilder");
    bind_numeric_effect_builder<OpScaleDown, FluentTag>(m, "FluentNumericEffectScaleDownBuilder");
    bind_numeric_effect_builder<OpIncrease, AuxiliaryTag>(m, "AuxiliaryNumericEffectIncreaseBuilder");

    bind_numeric_effect_operator_builder<FluentTag>(m, "FluentNumericEffectOperatorBuilder");
    bind_numeric_effect_operator_builder<AuxiliaryTag>(m, "AuxiliaryNumericEffectOperatorBuilder");

    bind_conjunctive_effect_builder(m, "ConjunctiveEffectBuilder");
    bind_conditional_effect_builder(m, "ConditionalEffectBuilder");
    bind_action_builder(m, "ActionBuilder");
    bind_axiom_builder(m, "AxiomBuilder");

    bind_unary_operator_builder<OpSub, Data<GroundFunctionExpression>>(m, "GroundUnaryOperatorSubBuilder");
    bind_binary_operator_builder<OpAdd, Data<GroundFunctionExpression>>(m, "GroundBinaryOperatorAddBuilder");
    bind_binary_operator_builder<OpSub, Data<GroundFunctionExpression>>(m, "GroundBinaryOperatorSubBuilder");
    bind_binary_operator_builder<OpMul, Data<GroundFunctionExpression>>(m, "GroundBinaryOperatorMulBuilder");
    bind_binary_operator_builder<OpDiv, Data<GroundFunctionExpression>>(m, "GroundBinaryOperatorDivBuilder");
    bind_binary_operator_builder<OpEq, Data<GroundFunctionExpression>>(m, "GroundBinaryOperatorEqBuilder");
    bind_binary_operator_builder<OpNe, Data<GroundFunctionExpression>>(m, "GroundBinaryOperatorNeBuilder");
    bind_binary_operator_builder<OpLe, Data<GroundFunctionExpression>>(m, "GroundBinaryOperatorLeBuilder");
    bind_binary_operator_builder<OpLt, Data<GroundFunctionExpression>>(m, "GroundBinaryOperatorLtBuilder");
    bind_binary_operator_builder<OpGe, Data<GroundFunctionExpression>>(m, "GroundBinaryOperatorGeBuilder");
    bind_binary_operator_builder<OpGt, Data<GroundFunctionExpression>>(m, "GroundBinaryOperatorGtBuilder");

    bind_multi_operator_builder<OpAdd, Data<GroundFunctionExpression>>(m, "GroundMultiOperatorAddBuilder");
    bind_multi_operator_builder<OpMul, Data<GroundFunctionExpression>>(m, "GroundMultiOperatorMulBuilder");

    bind_arithmethic_operator_builder<Data<GroundFunctionExpression>>(m, "GroundArithmeticOperatorBuilder");
    bind_boolean_operator_builder<Data<GroundFunctionExpression>>(m, "GroundBooleanOperatorBuilder");

    bind_ground_function_expression_builder(m, "GroundFunctionExpressionBuilder");

    bind_ground_conjunctive_condition_builder(m, "GroundConjunctiveConditionBuilder");

    bind_ground_numeric_effect_builder<OpAssign, FluentTag>(m, "FluentGroundNumericEffectAssignBuilder");
    bind_ground_numeric_effect_builder<OpIncrease, FluentTag>(m, "FluentGroundNumericEffectIncreaseBuilder");
    bind_ground_numeric_effect_builder<OpDecrease, FluentTag>(m, "FluentGroundNumericEffectDecreaseBuilder");
    bind_ground_numeric_effect_builder<OpScaleUp, FluentTag>(m, "FluentGroundNumericEffectScaleUpBuilder");
    bind_ground_numeric_effect_builder<OpScaleDown, FluentTag>(m, "FluentGroundNumericEffectScaleDownBuilder");
    bind_ground_numeric_effect_builder<OpIncrease, AuxiliaryTag>(m, "AuxiliaryGroundNumericEffectIncreaseBuilder");

    bind_ground_numeric_effect_operator_builder<FluentTag>(m, "FluentGroundNumericEffectOperatorBuilder");
    bind_ground_numeric_effect_operator_builder<AuxiliaryTag>(m, "AuxiliaryGroundNumericEffectOperatorBuilder");

    bind_ground_conjunctive_effect_builder(m, "GroundConjunctiveEffectBuilder");
    bind_ground_conditional_effect_builder(m, "GroundConditionalEffectBuilder");
    bind_ground_action_builder(m, "GroundActionBuilder");
    bind_ground_axiom_builder(m, "GroundAxiomBuilder");

    bind_metric_builder(m, "MetricBuilder");
    bind_domain_builder(m, "DomainBuilder");
    bind_lifted_task_builder(m, "LiftedTaskBuilder");
    bind_ground_task_builder(m, "GroundTaskBuilder");
}

}
