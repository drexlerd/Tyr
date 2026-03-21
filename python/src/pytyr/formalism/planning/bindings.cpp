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

#include "bindings.hpp"

#include "../../common/bindings.hpp"

namespace tyr::formalism::planning
{

void bind_module_definitions(nb::module_& m)
{
    {
        nb::class_<loki::ParserOptions>(m, "ParserOptions")
            .def(nb::init<>())
            .def_rw("strict", &loki::ParserOptions::strict, "Enable strict mode")
            .def_rw("verbose", &loki::ParserOptions::verbose, "Enable verbose output");
    }

    {
        nb::class_<Parser>(m, "Parser")
            .def(nb::init<const fs::path&, const loki::ParserOptions&>(), "domain_filepath"_a, "parser_options"_a)
            .def(nb::init<const std::string&, const fs::path&, const loki::ParserOptions&>(), "domain_description"_a, "domain_filepath"_a, "parser_options"_a)
            .def("parse_task", nb::overload_cast<const fs::path&, const loki::ParserOptions&>(&Parser::parse_task), "task_filepath"_a, "parser_options"_a)
            .def("parse_task",
                 nb::overload_cast<const std::string&, const fs::path&, const loki::ParserOptions&>(&Parser::parse_task),
                 "task_description"_a,
                 "task_filepath"_a,
                 "parser_options"_a)
            .def("get_domain", &Parser::get_domain);
    }

    nb::class_<Minimize>(m, "Minimize")
        .def("__str__", [](const Minimize& self) { return to_string(self); })
        .def("__repr__", [](const Minimize& self) { return to_string(self); });

    nb::class_<Maximize>(m, "Maximize")
        .def("__str__", [](const Maximize& self) { return to_string(self); })
        .def("__repr__", [](const Maximize& self) { return to_string(self); });

    /**
     * Common
     */

    bind_fixed_uint<FDRValue>(m, "FDRValue");
    bind_fixed_uint<ParameterIndex>(m, "ParameterIndex");

    /**
     * Index
     */

    bind_index<Index<Object>>(m, "ObjectIndex");
    bind_index<Index<Binding>>(m, "BindingIndex");
    bind_index<Index<Variable>>(m, "VariableIndex");

    bind_relation_binding_index<Predicate<StaticTag>>(m, "StaticPredicateBindingIndex");
    bind_relation_binding_index<Predicate<FluentTag>>(m, "FluentPredicateBindingIndex");
    bind_relation_binding_index<Predicate<DerivedTag>>(m, "DerivedPredicateBindingIndex");
    bind_relation_binding_index<Function<StaticTag>>(m, "StaticFunctionBindingIndex");
    bind_relation_binding_index<Function<FluentTag>>(m, "FluentFunctionBindingIndex");
    bind_relation_binding_index<Function<AuxiliaryTag>>(m, "AuxiliaryFunctionBindingIndex");
    bind_relation_binding_index<Action>(m, "ActionBindingIndex");
    bind_relation_binding_index<Axiom>(m, "AxiomBindingIndex");

    bind_index<Index<Predicate<StaticTag>>>(m, "StaticPredicateIndex");
    bind_index<Index<Predicate<FluentTag>>>(m, "FluentPredicateIndex");
    bind_index<Index<Predicate<DerivedTag>>>(m, "DerivedPredicateIndex");

    bind_index<Index<Atom<StaticTag>>>(m, "StaticAtomIndex");
    bind_index<Index<Atom<FluentTag>>>(m, "FluentAtomIndex");
    bind_index<Index<Atom<DerivedTag>>>(m, "DerivedAtomIndex");

    bind_index<Index<GroundAtom<StaticTag>>>(m, "StaticGroundAtomIndex");
    bind_index<Index<GroundAtom<FluentTag>>>(m, "FluentGroundAtomIndex");
    bind_index<Index<GroundAtom<DerivedTag>>>(m, "DerivedGroundAtomIndex");

    bind_index<Index<Literal<StaticTag>>>(m, "StaticLiteralIndex");
    bind_index<Index<Literal<FluentTag>>>(m, "FluentLiteralIndex");
    bind_index<Index<Literal<DerivedTag>>>(m, "DerivedLiteralIndex");

    bind_index<Index<GroundLiteral<StaticTag>>>(m, "StaticGroundLiteralIndex");
    bind_index<Index<GroundLiteral<FluentTag>>>(m, "FluentGroundLiteralIndex");
    bind_index<Index<GroundLiteral<DerivedTag>>>(m, "DerivedGroundLiteralIndex");

    bind_index<Index<FDRVariable<FluentTag>>>(m, "FluentFDRVariableIndex");

    bind_index<Index<Function<StaticTag>>>(m, "StaticFunctionIndex");
    bind_index<Index<Function<FluentTag>>>(m, "FluentFunctionIndex");
    bind_index<Index<Function<AuxiliaryTag>>>(m, "AuxiliaryFunctionIndex");

    bind_index<Index<FunctionTerm<StaticTag>>>(m, "StaticFunctionTermIndex");
    bind_index<Index<FunctionTerm<FluentTag>>>(m, "FluentFunctionTermIndex");
    bind_index<Index<FunctionTerm<AuxiliaryTag>>>(m, "AuxiliaryFunctionTermIndex");

    bind_index<Index<GroundFunctionTerm<StaticTag>>>(m, "StaticGroundFunctionTermIndex");
    bind_index<Index<GroundFunctionTerm<FluentTag>>>(m, "FluentGroundFunctionTermIndex");
    bind_index<Index<GroundFunctionTerm<AuxiliaryTag>>>(m, "AuxiliaryGroundFunctionTermIndex");

    bind_index<Index<GroundFunctionTermValue<StaticTag>>>(m, "StaticGroundFunctionTermValueIndex");
    bind_index<Index<GroundFunctionTermValue<FluentTag>>>(m, "FluentGroundFunctionTermValueIndex");
    bind_index<Index<GroundFunctionTermValue<AuxiliaryTag>>>(m, "AuxiliaryGroundFunctionTermValueIndex");

    bind_index<Index<UnaryOperator<OpSub, Data<FunctionExpression>>>>(m, "UnaryOperatorSubIndex");

    bind_index<Index<BinaryOperator<OpAdd, Data<FunctionExpression>>>>(m, "BinaryOperatorAddIndex");
    bind_index<Index<BinaryOperator<OpSub, Data<FunctionExpression>>>>(m, "BinaryOperatorSubIndex");
    bind_index<Index<BinaryOperator<OpMul, Data<FunctionExpression>>>>(m, "BinaryOperatorMulIndex");
    bind_index<Index<BinaryOperator<OpDiv, Data<FunctionExpression>>>>(m, "BinaryOperatorDivIndex");
    bind_index<Index<BinaryOperator<OpEq, Data<FunctionExpression>>>>(m, "BinaryOperatorEqIndex");
    bind_index<Index<BinaryOperator<OpNe, Data<FunctionExpression>>>>(m, "BinaryOperatorNeIndex");
    bind_index<Index<BinaryOperator<OpLe, Data<FunctionExpression>>>>(m, "BinaryOperatorLeIndex");
    bind_index<Index<BinaryOperator<OpLt, Data<FunctionExpression>>>>(m, "BinaryOperatorLtIndex");
    bind_index<Index<BinaryOperator<OpGe, Data<FunctionExpression>>>>(m, "BinaryOperatorGeIndex");
    bind_index<Index<BinaryOperator<OpGt, Data<FunctionExpression>>>>(m, "BinaryOperatorGtIndex");

    bind_index<Index<MultiOperator<OpAdd, Data<FunctionExpression>>>>(m, "MultiOperatorAddIndex");
    bind_index<Index<MultiOperator<OpMul, Data<FunctionExpression>>>>(m, "MultiOperatorMulIndex");

    bind_index<Index<ConjunctiveCondition>>(m, "ConjunctiveConditionIndex");

    bind_index<Index<NumericEffect<OpAssign, FluentTag>>>(m, "FluentNumericEffectAssignIndex");
    bind_index<Index<NumericEffect<OpIncrease, FluentTag>>>(m, "FluentNumericEffectIncreaseIndex");
    bind_index<Index<NumericEffect<OpDecrease, FluentTag>>>(m, "FluentNumericEffectDecreaseIndex");
    bind_index<Index<NumericEffect<OpScaleUp, FluentTag>>>(m, "FluentNumericEffectScaleUpIndex");
    bind_index<Index<NumericEffect<OpScaleDown, FluentTag>>>(m, "FluentNumericEffectScaleDownIndex");
    bind_index<Index<NumericEffect<OpIncrease, AuxiliaryTag>>>(m, "AuxiliaryNumericEffectIncreaseIndex");

    bind_index<Index<ConjunctiveEffect>>(m, "ConjunctiveEffectIndex");
    bind_index<Index<ConditionalEffect>>(m, "ConditionalEffectIndex");
    bind_index<Index<Action>>(m, "ActionIndex");
    bind_index<Index<Axiom>>(m, "AxiomIndex");

    bind_index<Index<UnaryOperator<OpSub, Data<GroundFunctionExpression>>>>(m, "GroundUnaryOperatorSubIndex");

    bind_index<Index<BinaryOperator<OpAdd, Data<GroundFunctionExpression>>>>(m, "GroundBinaryOperatorAddIndex");
    bind_index<Index<BinaryOperator<OpSub, Data<GroundFunctionExpression>>>>(m, "GroundBinaryOperatorSubIndex");
    bind_index<Index<BinaryOperator<OpMul, Data<GroundFunctionExpression>>>>(m, "GroundBinaryOperatorMulIndex");
    bind_index<Index<BinaryOperator<OpDiv, Data<GroundFunctionExpression>>>>(m, "GroundBinaryOperatorDivIndex");
    bind_index<Index<BinaryOperator<OpEq, Data<GroundFunctionExpression>>>>(m, "GroundBinaryOperatorEqIndex");
    bind_index<Index<BinaryOperator<OpNe, Data<GroundFunctionExpression>>>>(m, "GroundBinaryOperatorNeIndex");
    bind_index<Index<BinaryOperator<OpLe, Data<GroundFunctionExpression>>>>(m, "GroundBinaryOperatorLeIndex");
    bind_index<Index<BinaryOperator<OpLt, Data<GroundFunctionExpression>>>>(m, "GroundBinaryOperatorLtIndex");
    bind_index<Index<BinaryOperator<OpGe, Data<GroundFunctionExpression>>>>(m, "GroundBinaryOperatorGeIndex");
    bind_index<Index<BinaryOperator<OpGt, Data<GroundFunctionExpression>>>>(m, "GroundBinaryOperatorGtIndex");

    bind_index<Index<MultiOperator<OpAdd, Data<GroundFunctionExpression>>>>(m, "GroundMultiOperatorAddIndex");
    bind_index<Index<MultiOperator<OpMul, Data<GroundFunctionExpression>>>>(m, "GroundMultiOperatorMulIndex");

    bind_index<Index<GroundConjunctiveCondition>>(m, "GroundConjunctiveConditionIndex");

    bind_index<Index<GroundNumericEffect<OpAssign, FluentTag>>>(m, "FluentGroundNumericEffectAssignIndex");
    bind_index<Index<GroundNumericEffect<OpIncrease, FluentTag>>>(m, "FluentGroundNumericEffectIncreaseIndex");
    bind_index<Index<GroundNumericEffect<OpDecrease, FluentTag>>>(m, "FluentGroundNumericEffectDecreaseIndex");
    bind_index<Index<GroundNumericEffect<OpScaleUp, FluentTag>>>(m, "FluentGroundNumericEffectScaleUpIndex");
    bind_index<Index<GroundNumericEffect<OpScaleDown, FluentTag>>>(m, "FluentGroundNumericEffectScaleDownIndex");
    bind_index<Index<GroundNumericEffect<OpIncrease, AuxiliaryTag>>>(m, "AuxiliaryGroundNumericEffectIncreaseIndex");

    bind_index<Index<GroundConjunctiveEffect>>(m, "GroundConjunctiveEffectIndex");
    bind_index<Index<GroundConditionalEffect>>(m, "GroundConditionalEffectIndex");
    bind_index<Index<GroundAction>>(m, "GroundActionIndex");
    bind_index<Index<GroundAxiom>>(m, "GroundAxiomIndex");
    bind_index<Index<Metric>>(m, "MetricIndex");
    bind_index<Index<Domain>>(m, "DomainIndex");
    bind_index<Index<Task>>(m, "LiftedTaskIndex");
    bind_index<Index<FDRTask>>(m, "GroundTaskIndex");

    /**
     * Data
     */

    bind_object_builder(m, "ObjectBuilder");
    bind_variable_builder(m, "VariableBuilder");
    bind_term_builder(m, "TermBuilder");
    bind_binding_builder(m, "BindingBuilder");

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

    /**
     * Views
     */

    bind_object(m, "Object");
    bind_variable(m, "Variable");
    bind_term(m, "Term");

    bind_relation_binding<Predicate<StaticTag>>(m, "StaticPredicateBinding");
    bind_relation_binding<Predicate<FluentTag>>(m, "FluentPredicateBinding");
    bind_relation_binding<Predicate<DerivedTag>>(m, "DerivedPredicateBinding");
    bind_relation_binding<Function<StaticTag>>(m, "StaticFunctionBinding");
    bind_relation_binding<Function<FluentTag>>(m, "FluentFunctionBinding");
    bind_relation_binding<Function<AuxiliaryTag>>(m, "AuxiliaryFunctionBinding");
    bind_relation_binding<Action>(m, "ActionBinding");
    bind_relation_binding<Axiom>(m, "AxiomBinding");

    bind_predicate<StaticTag>(m, "StaticPredicate");
    bind_predicate<FluentTag>(m, "FluentPredicate");
    bind_predicate<DerivedTag>(m, "DerivedPredicate");

    bind_atom<StaticTag>(m, "StaticAtom");
    bind_atom<FluentTag>(m, "FluentAtom");
    bind_atom<DerivedTag>(m, "DerivedAtom");

    bind_ground_atom<StaticTag>(m, "StaticGroundAtom");
    bind_ground_atom<FluentTag>(m, "FluentGroundAtom");
    bind_ground_atom<DerivedTag>(m, "DerivedGroundAtom");

    bind_literal<StaticTag>(m, "StaticLiteral");
    bind_literal<FluentTag>(m, "FluentLiteral");
    bind_literal<DerivedTag>(m, "DerivedLiteral");

    bind_ground_literal<StaticTag>(m, "StaticGroundLiteral");
    bind_ground_literal<FluentTag>(m, "FluentGroundLiteral");
    bind_ground_literal<DerivedTag>(m, "DerivedGroundLiteral");

    bind_fdr_variable<FluentTag>(m, "FluentFDRVariable");
    bind_fdr_fact<FluentTag>(m, "FluentFDRFact");

    bind_function<StaticTag>(m, "StaticFunction");
    bind_function<FluentTag>(m, "FluentFunction");
    bind_function<AuxiliaryTag>(m, "AuxiliaryFunction");

    bind_function_term<StaticTag>(m, "StaticFunctionTerm");
    bind_function_term<FluentTag>(m, "FluentFunctionTerm");
    bind_function_term<AuxiliaryTag>(m, "AuxiliaryFunctionTerm");

    bind_ground_function_term<StaticTag>(m, "StaticGroundFunctionTerm");
    bind_ground_function_term<FluentTag>(m, "FluentGroundFunctionTerm");
    bind_ground_function_term<AuxiliaryTag>(m, "AuxiliaryGroundFunctionTerm");

    bind_ground_function_term_value<StaticTag>(m, "StaticGroundFunctionTermValue");
    bind_ground_function_term_value<FluentTag>(m, "FluentGroundFunctionTermValue");
    bind_ground_function_term_value<AuxiliaryTag>(m, "AuxiliaryGroundFunctionTermValue");

    bind_unary_operator<OpSub, Data<FunctionExpression>>(m, "UnaryOperatorSub");
    bind_binary_operator<OpAdd, Data<FunctionExpression>>(m, "BinaryOperatorAdd");
    bind_binary_operator<OpSub, Data<FunctionExpression>>(m, "BinaryOperatorSub");
    bind_binary_operator<OpMul, Data<FunctionExpression>>(m, "BinaryOperatorMul");
    bind_binary_operator<OpDiv, Data<FunctionExpression>>(m, "BinaryOperatorDiv");
    bind_binary_operator<OpEq, Data<FunctionExpression>>(m, "BinaryOperatorEq");
    bind_binary_operator<OpNe, Data<FunctionExpression>>(m, "BinaryOperatorNe");
    bind_binary_operator<OpLe, Data<FunctionExpression>>(m, "BinaryOperatorLe");
    bind_binary_operator<OpLt, Data<FunctionExpression>>(m, "BinaryOperatorLt");
    bind_binary_operator<OpGe, Data<FunctionExpression>>(m, "BinaryOperatorGe");
    bind_binary_operator<OpGt, Data<FunctionExpression>>(m, "BinaryOperatorGt");

    bind_multi_operator<OpAdd, Data<FunctionExpression>>(m, "MultiOperatorAdd");
    bind_multi_operator<OpMul, Data<FunctionExpression>>(m, "MultiOperatorMul");

    bind_arithmethic_operator<Data<FunctionExpression>>(m, "ArithmeticOperator");
    bind_boolean_operator<Data<FunctionExpression>>(m, "BooleanOperator");

    bind_function_expression(m, "FunctionExpression");
    bind_conjunctive_condition(m, "ConjunctiveCondition");

    bind_numeric_effect<OpAssign, FluentTag>(m, "FluentNumericEffectAssign");
    bind_numeric_effect<OpIncrease, FluentTag>(m, "FluentNumericEffectIncrease");
    bind_numeric_effect<OpDecrease, FluentTag>(m, "FluentNumericEffectDecrease");
    bind_numeric_effect<OpScaleUp, FluentTag>(m, "FluentNumericEffectScaleUp");
    bind_numeric_effect<OpScaleDown, FluentTag>(m, "FluentNumericEffectScaleDown");
    bind_numeric_effect<OpIncrease, AuxiliaryTag>(m, "AuxiliaryNumericEffectIncrease");

    bind_numeric_effect_operator<FluentTag>(m, "FluentNumericEffectOperator");
    bind_numeric_effect_operator<AuxiliaryTag>(m, "AuxiliaryNumericEffectOperator");

    bind_conjunctive_effect(m, "ConjunctiveEffect");
    bind_conditional_effect(m, "ConditionalEffect");
    bind_action(m, "Action");
    bind_axiom(m, "Axiom");

    bind_unary_operator<OpSub, Data<GroundFunctionExpression>>(m, "GroundUnaryOperatorSub");
    bind_binary_operator<OpAdd, Data<GroundFunctionExpression>>(m, "GroundBinaryOperatorAdd");
    bind_binary_operator<OpSub, Data<GroundFunctionExpression>>(m, "GroundBinaryOperatorSub");
    bind_binary_operator<OpMul, Data<GroundFunctionExpression>>(m, "GroundBinaryOperatorMul");
    bind_binary_operator<OpDiv, Data<GroundFunctionExpression>>(m, "GroundBinaryOperatorDiv");
    bind_binary_operator<OpEq, Data<GroundFunctionExpression>>(m, "GroundBinaryOperatorEq");
    bind_binary_operator<OpNe, Data<GroundFunctionExpression>>(m, "GroundBinaryOperatorNe");
    bind_binary_operator<OpLe, Data<GroundFunctionExpression>>(m, "GroundBinaryOperatorLe");
    bind_binary_operator<OpLt, Data<GroundFunctionExpression>>(m, "GroundBinaryOperatorLt");
    bind_binary_operator<OpGe, Data<GroundFunctionExpression>>(m, "GroundBinaryOperatorGe");
    bind_binary_operator<OpGt, Data<GroundFunctionExpression>>(m, "GroundBinaryOperatorGt");

    bind_multi_operator<OpAdd, Data<GroundFunctionExpression>>(m, "GroundMultiOperatorAdd");
    bind_multi_operator<OpMul, Data<GroundFunctionExpression>>(m, "GroundMultiOperatorMul");

    bind_arithmethic_operator<Data<GroundFunctionExpression>>(m, "GroundArithmeticOperator");
    bind_boolean_operator<Data<GroundFunctionExpression>>(m, "GroundBooleanOperator");

    bind_ground_function_expression(m, "GroundFunctionExpression");
    bind_ground_conjunctive_condition(m, "GroundConjunctiveCondition");

    bind_ground_numeric_effect<OpAssign, FluentTag>(m, "FluentGroundNumericEffectAssign");
    bind_ground_numeric_effect<OpIncrease, FluentTag>(m, "FluentGroundNumericEffectIncrease");
    bind_ground_numeric_effect<OpDecrease, FluentTag>(m, "FluentGroundNumericEffectDecrease");
    bind_ground_numeric_effect<OpScaleUp, FluentTag>(m, "FluentGroundNumericEffectScaleUp");
    bind_ground_numeric_effect<OpScaleDown, FluentTag>(m, "FluentGroundNumericEffectScaleDown");
    bind_ground_numeric_effect<OpIncrease, AuxiliaryTag>(m, "AuxiliaryGroundNumericEffectIncrease");

    bind_ground_numeric_effect_operator<FluentTag>(m, "FluentGroundNumericEffectOperator");
    bind_ground_numeric_effect_operator<AuxiliaryTag>(m, "AuxiliaryGroundNumericEffectOperator");

    bind_ground_conjunctive_effect(m, "GroundConjunctiveEffect");
    bind_ground_conditional_effect(m, "GroundConditionalEffect");
    bind_ground_action(m, "GroundAction");
    bind_ground_axiom(m, "GroundAxiom");

    bind_metric(m, "Metric");
    bind_domain(m, "Domain");
    bind_lifted_task(m, "LiftedTask");
    bind_ground_task(m, "GroundTask");

    /**
     * RepositoryFactory
     */

    nb::class_<RepositoryFactory>(m, "RepositoryFactory")  //
        .def(nb::new_([]() { return std::make_shared<RepositoryFactory>(); }))
        .def("create_repository", &RepositoryFactory::create_shared, "parent_repository"_a = nullptr);

    /**
     * Repository
     */

    nb::class_<Repository>(m, "Repository")  //
        .def("get_or_create", nb::overload_cast<Data<Object>&>(&Repository::get_or_create<Object>), "builder"_a)
        .def("get_or_create", nb::overload_cast<Data<Variable>&>(&Repository::get_or_create<Variable>), "builder"_a)
        .def("get_or_create",
             nb::overload_cast<PredicateView<StaticTag>, const IndexList<Object>&>(&Repository::get_or_create<Predicate<StaticTag>>),
             "predicate"_a,
             "object_indices"_a)
        .def("get_or_create",
             nb::overload_cast<PredicateView<FluentTag>, const IndexList<Object>&>(&Repository::get_or_create<Predicate<FluentTag>>),
             "predicate"_a,
             "object_indices"_a)
        .def("get_or_create",
             nb::overload_cast<PredicateView<DerivedTag>, const IndexList<Object>&>(&Repository::get_or_create<Predicate<DerivedTag>>),
             "predicate"_a,
             "object_indices"_a)
        .def("get_or_create",
             nb::overload_cast<FunctionView<StaticTag>, const IndexList<Object>&>(&Repository::get_or_create<Function<StaticTag>>),
             "function"_a,
             "object_indices"_a)
        .def("get_or_create",
             nb::overload_cast<FunctionView<FluentTag>, const IndexList<Object>&>(&Repository::get_or_create<Function<FluentTag>>),
             "function"_a,
             "object_indices"_a)
        .def("get_or_create",
             nb::overload_cast<FunctionView<AuxiliaryTag>, const IndexList<Object>&>(&Repository::get_or_create<Function<AuxiliaryTag>>),
             "function"_a,
             "object_indices"_a)
        .def("get_or_create", nb::overload_cast<ActionView, const IndexList<Object>&>(&Repository::get_or_create<Action>), "action"_a, "object_indices"_a)
        .def("get_or_create", nb::overload_cast<AxiomView, const IndexList<Object>&>(&Repository::get_or_create<Axiom>), "axiom"_a, "object_indices"_a)
        .def("get_or_create", nb::overload_cast<Data<Predicate<StaticTag>>&>(&Repository::get_or_create<Predicate<StaticTag>>), "builder"_a)
        .def("get_or_create", nb::overload_cast<Data<Predicate<FluentTag>>&>(&Repository::get_or_create<Predicate<FluentTag>>), "builder"_a)
        .def("get_or_create", nb::overload_cast<Data<Predicate<DerivedTag>>&>(&Repository::get_or_create<Predicate<DerivedTag>>), "builder"_a)
        .def("get_or_create", nb::overload_cast<Data<Atom<StaticTag>>&>(&Repository::get_or_create<Atom<StaticTag>>), "builder"_a)
        .def("get_or_create", nb::overload_cast<Data<Atom<FluentTag>>&>(&Repository::get_or_create<Atom<FluentTag>>), "builder"_a)
        .def("get_or_create", nb::overload_cast<Data<Atom<DerivedTag>>&>(&Repository::get_or_create<Atom<DerivedTag>>), "builder"_a)
        .def("get_or_create", nb::overload_cast<Data<GroundAtom<StaticTag>>&>(&Repository::get_or_create<GroundAtom<StaticTag>>), "builder"_a)
        .def("get_or_create", nb::overload_cast<Data<GroundAtom<FluentTag>>&>(&Repository::get_or_create<GroundAtom<FluentTag>>), "builder"_a)
        .def("get_or_create", nb::overload_cast<Data<GroundAtom<DerivedTag>>&>(&Repository::get_or_create<GroundAtom<DerivedTag>>), "builder"_a)
        .def("get_or_create", nb::overload_cast<Data<Literal<StaticTag>>&>(&Repository::get_or_create<Literal<StaticTag>>), "builder"_a)
        .def("get_or_create", nb::overload_cast<Data<Literal<FluentTag>>&>(&Repository::get_or_create<Literal<FluentTag>>), "builder"_a)
        .def("get_or_create", nb::overload_cast<Data<Literal<DerivedTag>>&>(&Repository::get_or_create<Literal<DerivedTag>>), "builder"_a)
        .def("get_or_create", nb::overload_cast<Data<GroundLiteral<StaticTag>>&>(&Repository::get_or_create<GroundLiteral<StaticTag>>), "builder"_a)
        .def("get_or_create", nb::overload_cast<Data<GroundLiteral<FluentTag>>&>(&Repository::get_or_create<GroundLiteral<FluentTag>>), "builder"_a)
        .def("get_or_create", nb::overload_cast<Data<GroundLiteral<DerivedTag>>&>(&Repository::get_or_create<GroundLiteral<DerivedTag>>), "builder"_a)
        .def("get_or_create", nb::overload_cast<Data<FDRVariable<FluentTag>>&>(&Repository::get_or_create<FDRVariable<FluentTag>>), "builder"_a)
        .def("get_or_create", nb::overload_cast<Data<Function<StaticTag>>&>(&Repository::get_or_create<Function<StaticTag>>), "builder"_a)
        .def("get_or_create", nb::overload_cast<Data<Function<FluentTag>>&>(&Repository::get_or_create<Function<FluentTag>>), "builder"_a)
        .def("get_or_create", nb::overload_cast<Data<Function<AuxiliaryTag>>&>(&Repository::get_or_create<Function<AuxiliaryTag>>), "builder"_a)
        .def("get_or_create", nb::overload_cast<Data<FunctionTerm<StaticTag>>&>(&Repository::get_or_create<FunctionTerm<StaticTag>>), "builder"_a)
        .def("get_or_create", nb::overload_cast<Data<FunctionTerm<FluentTag>>&>(&Repository::get_or_create<FunctionTerm<FluentTag>>), "builder"_a)
        .def("get_or_create", nb::overload_cast<Data<FunctionTerm<AuxiliaryTag>>&>(&Repository::get_or_create<FunctionTerm<AuxiliaryTag>>), "builder"_a)
        .def("get_or_create", nb::overload_cast<Data<GroundFunctionTerm<StaticTag>>&>(&Repository::get_or_create<GroundFunctionTerm<StaticTag>>), "builder"_a)
        .def("get_or_create", nb::overload_cast<Data<GroundFunctionTerm<FluentTag>>&>(&Repository::get_or_create<GroundFunctionTerm<FluentTag>>), "builder"_a)
        .def("get_or_create",
             nb::overload_cast<Data<GroundFunctionTerm<AuxiliaryTag>>&>(&Repository::get_or_create<GroundFunctionTerm<AuxiliaryTag>>),
             "builder"_a)
        .def("get_or_create",
             nb::overload_cast<Data<GroundFunctionTermValue<StaticTag>>&>(&Repository::get_or_create<GroundFunctionTermValue<StaticTag>>),
             "builder"_a)
        .def("get_or_create",
             nb::overload_cast<Data<GroundFunctionTermValue<FluentTag>>&>(&Repository::get_or_create<GroundFunctionTermValue<FluentTag>>),
             "builder"_a)
        .def("get_or_create",
             nb::overload_cast<Data<GroundFunctionTermValue<AuxiliaryTag>>&>(&Repository::get_or_create<GroundFunctionTermValue<AuxiliaryTag>>),
             "builder"_a);

    /**
     * FDRContexts
     */

    nb::class_<BinaryFDRContext>(m, "BinaryFDRContext")  //
        .def(nb::init<Repository&>(), "repository")
        .def("get_fact", nb::overload_cast<GroundAtomView<FluentTag>>(&BinaryFDRContext::get_fact_view), "atom"_a)
        .def("get_fact", nb::overload_cast<GroundLiteralView<FluentTag>>(&BinaryFDRContext::get_fact_view), "literal"_a);

    nb::class_<GeneralFDRContext>(m, "GeneralFDRContext")  //
        .def("get_fact", nb::overload_cast<GroundAtomView<FluentTag>>(&GeneralFDRContext::get_fact_view), "atom"_a)
        .def("get_fact", nb::overload_cast<GroundLiteralView<FluentTag>>(&GeneralFDRContext::get_fact_view), "literal"_a);

    /**
     * PlanningDomain
     */

    {
        nb::class_<PlanningDomain>(m, "PlanningDomain")  //
            .def(nb::init<DomainView, RepositoryPtr, RepositoryFactoryPtr>(), "domain"_a, "repository"_a, "repository_factory"_a)
            .def("get_domain", &PlanningDomain::get_domain)
            .def("get_repository", &PlanningDomain::get_repository)
            .def("get_repository_factory", &PlanningDomain::get_repository_factory);
    }

    {
        nb::class_<PlanningTask>(m, "PlanningTask")  //
            .def(nb::new_([](TaskView task, BinaryFDRContext&& fdr_context, std::shared_ptr<Repository> repository, PlanningDomain planning_domain)
                          { return PlanningTask(task, std::move(fdr_context), std::move(repository), std::move(planning_domain)); }),
                 "task"_a,
                 "fdr_context"_a,
                 "repository"_a,
                 "planning_domain"_a)
            .def("get_task", &PlanningTask::get_task)
            .def("get_repository", &PlanningTask::get_repository)
            .def("get_fdr_context", nb::overload_cast<>(&PlanningTask::get_fdr_context), nb::rv_policy::reference_internal)
            .def("get_domain", &PlanningTask::get_domain);
    }

    {
        nb::class_<PlanningFDRTask>(m, "PlanningFDRTask")  //
            .def("get_task", &PlanningFDRTask::get_task)
            .def("get_repository", &PlanningFDRTask::get_repository)
            .def("get_fdr_context", &PlanningFDRTask::get_fdr_context, nb::rv_policy::reference_internal)
            .def("get_domain", &PlanningFDRTask::get_domain);
    }
}

}