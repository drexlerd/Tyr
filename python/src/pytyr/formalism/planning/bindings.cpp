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
    nb::class_<View<Index<Object>, Repository>>(m, "Object")  //
        .def("__str__", [](View<Index<Object>, Repository> self) { return to_string(self); })
        .def("get_name", &View<Index<Object>, Repository>::get_name);

    nb::class_<View<Index<Binding>, Repository>>(m, "Binding")  //
        .def("__str__", [](View<Index<Binding>, Repository> self) { return to_string(self); });

    nb::class_<View<Index<Variable>, Repository>>(m, "Variable")  //
        .def("__str__", [](View<Index<Variable>, Repository> self) { return to_string(self); })
        .def("get_name", &View<Index<Variable>, Repository>::get_name);

    bind_fixed_uint<ParameterIndex>(m, "ParameterIndex");

    nb::class_<View<Data<Term>, Repository>>(m, "Term")  //
        .def("__str__", [](View<Data<Term>, Repository> self) { return to_string(self); })
        .def("get_variant", &View<Data<Term>, Repository>::get_variant);

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
    bind_fixed_uint<FDRValue>(m, "FDRValue");
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

    bind_arithmethic_operator<Data<FunctionExpression>>(m, "ArithmeticOperator");
    bind_boolean_operator<Data<FunctionExpression>>(m, "BooleanOperator");

    nb::class_<View<Data<FunctionExpression>, Repository>>(m, "FunctionExpression");

    nb::class_<View<Index<ConjunctiveCondition>, Repository>>(m, "ConjunctiveCondition");

    bind_numeric_effect<OpAssign, FluentTag>(m, "FluentNumericEffectAssign");
    bind_numeric_effect<OpIncrease, FluentTag>(m, "FluentNumericEffectIncrease");
    bind_numeric_effect<OpDecrease, FluentTag>(m, "FluentNumericEffectDecrease");
    bind_numeric_effect<OpScaleUp, FluentTag>(m, "FluentNumericEffectScaleUp");
    bind_numeric_effect<OpScaleDown, FluentTag>(m, "FluentNumericEffectScaleDown");
    bind_numeric_effect<OpIncrease, AuxiliaryTag>(m, "AuxiliaryNumericEffectIncrease");

    bind_numeric_effect_operator<FluentTag>(m, "FluentNumericEffectOperator");
    bind_numeric_effect_operator<AuxiliaryTag>(m, "AuxiliaryNumericEffectOperator");

    nb::class_<View<Index<ConjunctiveEffect>, Repository>>(m, "ConjunctiveEffect");

    nb::class_<View<Index<ConditionalEffect>, Repository>>(m, "ConditionalEffect");

    nb::class_<View<Index<Action>, Repository>>(m, "Action")  //
        .def("__str__", [](View<Index<Action>, Repository> self) { return to_string(self); })
        .def("get_name", &View<Index<Action>, Repository>::get_name)
        .def("get_original_arity", &View<Index<Action>, Repository>::get_original_arity)
        .def("get_arity", &View<Index<Action>, Repository>::get_arity)
        .def("get_variables", &View<Index<Action>, Repository>::get_variables)
        .def("get_condition", &View<Index<Action>, Repository>::get_condition)
        .def("get_effects", &View<Index<Action>, Repository>::get_effects);

    nb::class_<View<Index<Axiom>, Repository>>(m, "Axiom");

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

    bind_arithmethic_operator<Data<GroundFunctionExpression>>(m, "GroundArithmeticOperator");
    bind_boolean_operator<Data<GroundFunctionExpression>>(m, "GroundBooleanOperator");

    nb::class_<View<Data<GroundFunctionExpression>, Repository>>(m, "GroundFunctionExpression");

    nb::class_<View<Index<GroundConjunctiveCondition>, Repository>>(m, "GroundConjunctiveCondition");

    bind_ground_numeric_effect<OpAssign, FluentTag>(m, "FluentGroundNumericEffectAssign");
    bind_ground_numeric_effect<OpIncrease, FluentTag>(m, "FluentGroundNumericEffectIncrease");
    bind_ground_numeric_effect<OpDecrease, FluentTag>(m, "FluentGroundNumericEffectDecrease");
    bind_ground_numeric_effect<OpScaleUp, FluentTag>(m, "FluentGroundNumericEffectScaleUp");
    bind_ground_numeric_effect<OpScaleDown, FluentTag>(m, "FluentGroundNumericEffectScaleDown");
    bind_ground_numeric_effect<OpIncrease, AuxiliaryTag>(m, "AuxiliaryGroundNumericEffectIncrease");

    bind_ground_numeric_effect_operator<FluentTag>(m, "FluentGroundNumericEffectOperator");
    bind_ground_numeric_effect_operator<AuxiliaryTag>(m, "AuxiliaryGroundNumericEffectOperator");

    nb::class_<View<Index<GroundConjunctiveEffect>, Repository>>(m, "GroundConjunctiveEffect");

    nb::class_<View<Index<GroundConditionalEffect>, Repository>>(m, "GroundConditionalEffect");

    nb::class_<View<Index<GroundAction>, Repository>>(m, "GroundAction")  //
        .def("__str__", [](View<Index<GroundAction>, Repository> self) { return to_string(self); })
        .def("get_action", &View<Index<GroundAction>, Repository>::get_action)
        .def("get_binding", &View<Index<GroundAction>, Repository>::get_binding)
        .def("get_condition", &View<Index<GroundAction>, Repository>::get_condition)
        .def("get_effects", &View<Index<GroundAction>, Repository>::get_effects);

    nb::class_<View<Index<GroundAxiom>, Repository>>(m, "GroundAxiom");

    nb::class_<View<Index<Metric>, Repository>>(m, "Metric");

    nb::class_<View<Index<Domain>, Repository>>(m, "Domain");

    nb::class_<View<Index<Task>, Repository>>(m, "LiftedTask");

    nb::class_<View<Index<FDRTask>, Repository>>(m, "GroundTask");

    nb::class_<Repository>(m, "Repository");
}

}