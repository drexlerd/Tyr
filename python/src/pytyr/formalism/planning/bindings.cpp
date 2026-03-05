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
        using V = View<Index<Object>, Repository>;

        nb::class_<V>(m, "Object")  //
            .def("__str__", [](const V& self) { return to_string(self); })
            .def("__repr__", [](const V& self) { return to_string(self); })
            .def("__eq__", [](const V& self, const V& other) { return EqualTo<V> {}(self, other); })
            .def("__hash__", [](const V& self) { return Hash<V> {}(self); })
            .def("get_name", &V::get_name);
    }

    {
        using V = View<Index<Binding>, Repository>;

        nb::class_<V>(m, "Binding")  //
            .def("__str__", [](const V& self) { return to_string(self); })
            .def("__repr__", [](const V& self) { return to_string(self); })
            .def("__eq__", [](const V& self, const V& other) { return EqualTo<V> {}(self, other); })
            .def("__hash__", [](const V& self) { return Hash<V> {}(self); })
            .def("get_objects", &V::get_objects);
    }

    {
        using V = View<Index<Variable>, Repository>;

        nb::class_<V>(m, "Variable")  //
            .def("__str__", [](const V& self) { return to_string(self); })
            .def("__repr__", [](const V& self) { return to_string(self); })
            .def("__eq__", [](const V& self, const V& other) { return EqualTo<V> {}(self, other); })
            .def("__hash__", [](const V& self) { return Hash<V> {}(self); })
            .def("get_name", &V::get_name);
    }

    bind_fixed_uint<ParameterIndex>(m, "ParameterIndex");

    {
        using V = View<Data<Term>, Repository>;

        nb::class_<V>(m, "Term")  //
            .def("__str__", [](const V& self) { return to_string(self); })
            .def("__repr__", [](const V& self) { return to_string(self); })
            .def("__eq__", [](const V& self, const V& other) { return EqualTo<V> {}(self, other); })
            .def("__hash__", [](const V& self) { return Hash<V> {}(self); })
            .def("get_variant", &V::get_variant);
    }

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

    bind_multi_operator<OpAdd, Data<FunctionExpression>>(m, "MultiOperatorAdd");
    bind_multi_operator<OpMul, Data<FunctionExpression>>(m, "MultiOperatorMul");

    bind_arithmethic_operator<Data<FunctionExpression>>(m, "ArithmeticOperator");
    bind_boolean_operator<Data<FunctionExpression>>(m, "BooleanOperator");

    {
        using V = View<Data<FunctionExpression>, Repository>;

        nb::class_<V>(m, "FunctionExpression")
            .def("__str__", [](const V& self) { return to_string(self); })
            .def("__repr__", [](const V& self) { return to_string(self); })
            .def("__eq__", [](const V& self, const V& other) { return EqualTo<V> {}(self, other); })
            .def("__hash__", [](const V& self) { return Hash<V> {}(self); })
            .def("get_variant", &V::get_variant);
    }

    {
        using V = View<Index<ConjunctiveCondition>, Repository>;

        nb::class_<V>(m, "ConjunctiveCondition")
            .def("__str__", [](const V& self) { return to_string(self); })
            .def("__repr__", [](const V& self) { return to_string(self); })
            .def("__eq__", [](const V& self, const V& other) { return EqualTo<V> {}(self, other); })
            .def("__hash__", [](const V& self) { return Hash<V> {}(self); })
            .def("get_variables", &V::get_variables)
            .def("get_arity", &V::get_arity)
            .def("get_static_literals", &V::get_literals<StaticTag>)
            .def("get_fluent_literals", &V::get_literals<FluentTag>)
            .def("get_derived_literals", &V::get_literals<DerivedTag>)
            .def("get_numeric_constraints", &V::get_numeric_constraints);
    }

    bind_numeric_effect<OpAssign, FluentTag>(m, "FluentNumericEffectAssign");
    bind_numeric_effect<OpIncrease, FluentTag>(m, "FluentNumericEffectIncrease");
    bind_numeric_effect<OpDecrease, FluentTag>(m, "FluentNumericEffectDecrease");
    bind_numeric_effect<OpScaleUp, FluentTag>(m, "FluentNumericEffectScaleUp");
    bind_numeric_effect<OpScaleDown, FluentTag>(m, "FluentNumericEffectScaleDown");
    bind_numeric_effect<OpIncrease, AuxiliaryTag>(m, "AuxiliaryNumericEffectIncrease");

    bind_numeric_effect_operator<FluentTag>(m, "FluentNumericEffectOperator");
    bind_numeric_effect_operator<AuxiliaryTag>(m, "AuxiliaryNumericEffectOperator");

    {
        using V = View<Index<ConjunctiveEffect>, Repository>;

        nb::class_<V>(m, "ConjunctiveEffect")
            .def("__str__", [](const V& self) { return to_string(self); })
            .def("__repr__", [](const V& self) { return to_string(self); })
            .def("__eq__", [](const V& self, const V& other) { return EqualTo<V> {}(self, other); })
            .def("__hash__", [](const V& self) { return Hash<V> {}(self); })
            .def("get_literals", &V::get_literals)
            .def("get_numeric_effects", &V::get_numeric_effects)
            .def("get_auxiliary_numeric_effect", &V::get_auxiliary_numeric_effect);
    }

    {
        using V = View<Index<ConditionalEffect>, Repository>;

        nb::class_<V>(m, "ConditionalEffect")
            .def("__str__", [](const V& self) { return to_string(self); })
            .def("__repr__", [](const V& self) { return to_string(self); })
            .def("__eq__", [](const V& self, const V& other) { return EqualTo<V> {}(self, other); })
            .def("__hash__", [](const V& self) { return Hash<V> {}(self); })
            .def("get_variables", &V::get_variables)
            .def("get_arity", &V::get_arity)
            .def("get_condition", &V::get_condition)
            .def("get_effect", &V::get_effect);
    }

    {
        using V = View<Index<Action>, Repository>;

        nb::class_<V>(m, "Action")  //
            .def("__str__", [](const V& self) { return to_string(self); })
            .def("__repr__", [](const V& self) { return to_string(self); })
            .def("__eq__", [](const V& self, const V& other) { return EqualTo<V> {}(self, other); })
            .def("__hash__", [](const V& self) { return Hash<V> {}(self); })
            .def("get_name", &V::get_name)
            .def("get_original_arity", &V::get_original_arity)
            .def("get_arity", &V::get_arity)
            .def("get_variables", &V::get_variables)
            .def("get_condition", &V::get_condition)
            .def("get_effects", &V::get_effects);
    }

    {
        using V = View<Index<Axiom>, Repository>;

        nb::class_<V>(m, "Axiom")
            .def("__str__", [](const V& self) { return to_string(self); })
            .def("__repr__", [](const V& self) { return to_string(self); })
            .def("__eq__", [](const V& self, const V& other) { return EqualTo<V> {}(self, other); })
            .def("__hash__", [](const V& self) { return Hash<V> {}(self); })
            .def("get_arity", &V::get_arity)
            .def("get_variables", &V::get_variables)
            .def("get_body", &V::get_body)
            .def("get_head", &V::get_head);
    }

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

    {
        using V = View<Data<GroundFunctionExpression>, Repository>;

        nb::class_<V>(m, "GroundFunctionExpression")
            .def("__str__", [](const V& self) { return to_string(self); })
            .def("__repr__", [](const V& self) { return to_string(self); })
            .def("__eq__", [](const V& self, const V& other) { return EqualTo<V> {}(self, other); })
            .def("__hash__", [](const V& self) { return Hash<V> {}(self); })
            .def("get_variant", &V::get_variant);
    }

    {
        using V = View<Index<GroundConjunctiveCondition>, Repository>;

        nb::class_<V>(m, "GroundConjunctiveCondition")
            .def("__str__", [](const V& self) { return to_string(self); })
            .def("__repr__", [](const V& self) { return to_string(self); })
            .def("__eq__", [](const V& self, const V& other) { return EqualTo<V> {}(self, other); })
            .def("__hash__", [](const V& self) { return Hash<V> {}(self); })
            .def("get_static_facts", &V::get_facts<StaticTag>)
            .def("get_fluent_facts", &V::get_facts<FluentTag>)
            .def("get_derived_facts", &V::get_facts<DerivedTag>)
            .def("get_numeric_constraints", &V::get_numeric_constraints);
    }

    bind_ground_numeric_effect<OpAssign, FluentTag>(m, "FluentGroundNumericEffectAssign");
    bind_ground_numeric_effect<OpIncrease, FluentTag>(m, "FluentGroundNumericEffectIncrease");
    bind_ground_numeric_effect<OpDecrease, FluentTag>(m, "FluentGroundNumericEffectDecrease");
    bind_ground_numeric_effect<OpScaleUp, FluentTag>(m, "FluentGroundNumericEffectScaleUp");
    bind_ground_numeric_effect<OpScaleDown, FluentTag>(m, "FluentGroundNumericEffectScaleDown");
    bind_ground_numeric_effect<OpIncrease, AuxiliaryTag>(m, "AuxiliaryGroundNumericEffectIncrease");

    bind_ground_numeric_effect_operator<FluentTag>(m, "FluentGroundNumericEffectOperator");
    bind_ground_numeric_effect_operator<AuxiliaryTag>(m, "AuxiliaryGroundNumericEffectOperator");

    {
        using V = View<Index<GroundConjunctiveEffect>, Repository>;

        nb::class_<V>(m, "GroundConjunctiveEffect")
            .def("__str__", [](const V& self) { return to_string(self); })
            .def("__repr__", [](const V& self) { return to_string(self); })
            .def("__eq__", [](const V& self, const V& other) { return EqualTo<V> {}(self, other); })
            .def("__hash__", [](const V& self) { return Hash<V> {}(self); })
            .def("get_facts", &V::get_facts)
            .def("get_numeric_effects", &V::get_numeric_effects)
            .def("get_auxiliary_numeric_effect", &V::get_auxiliary_numeric_effect);
    }

    {
        using V = View<Index<GroundConditionalEffect>, Repository>;

        nb::class_<V>(m, "GroundConditionalEffect")
            .def("__str__", [](const V& self) { return to_string(self); })
            .def("__repr__", [](const V& self) { return to_string(self); })
            .def("__eq__", [](const V& self, const V& other) { return EqualTo<V> {}(self, other); })
            .def("__hash__", [](const V& self) { return Hash<V> {}(self); })
            .def("get_condition", &V::get_condition)
            .def("get_effect", &V::get_effect);
    }

    {
        using V = View<Index<GroundAction>, Repository>;

        nb::class_<V>(m, "GroundAction")  //
            .def("__str__", [](const V& self) { return to_string(self); })
            .def("__repr__", [](const V& self) { return to_string(self); })
            .def("__eq__", [](const V& self, const V& other) { return EqualTo<V> {}(self, other); })
            .def("__hash__", [](const V& self) { return Hash<V> {}(self); })
            .def("get_action", &V::get_action)
            .def("get_binding", &V::get_binding)
            .def("get_condition", &V::get_condition)
            .def("get_effects", &V::get_effects);
    }

    {
        using V = View<Index<GroundAxiom>, Repository>;

        nb::class_<V>(m, "GroundAxiom")
            .def("__str__", [](const V& self) { return to_string(self); })
            .def("__repr__", [](const V& self) { return to_string(self); })
            .def("__eq__", [](const V& self, const V& other) { return EqualTo<V> {}(self, other); })
            .def("__hash__", [](const V& self) { return Hash<V> {}(self); })
            .def("get_axiom", &V::get_axiom)
            .def("get_binding", &V::get_binding)
            .def("get_body", &V::get_body)
            .def("get_head", &V::get_head);
    }

    nb::class_<Minimize>(m, "Minimize")
        .def("__str__", [](const Minimize& self) { return to_string(self); })
        .def("__repr__", [](const Minimize& self) { return to_string(self); });

    nb::class_<Maximize>(m, "Maximize")
        .def("__str__", [](const Maximize& self) { return to_string(self); })
        .def("__repr__", [](const Maximize& self) { return to_string(self); });

    {
        using V = View<Index<Metric>, Repository>;

        nb::class_<V>(m, "Metric")
            .def("__str__", [](const V& self) { return to_string(self); })
            .def("__repr__", [](const V& self) { return to_string(self); })
            .def("__eq__", [](const V& self, const V& other) { return EqualTo<V> {}(self, other); })
            .def("__hash__", [](const V& self) { return Hash<V> {}(self); })
            .def("get_objective", &V::get_objective)
            .def("get_fexpr", &V::get_fexpr);
    }

    {
        using V = View<Index<Domain>, Repository>;

        nb::class_<V>(m, "Domain")
            .def("__str__", [](const V& self) { return to_string(self); })
            .def("__repr__", [](const V& self) { return to_string(self); })
            .def("__eq__", [](const V& self, const V& other) { return EqualTo<V> {}(self, other); })
            .def("__hash__", [](const V& self) { return Hash<V> {}(self); })
            .def("get_name", &V::get_name)
            .def("get_static_predicates", &V::get_predicates<StaticTag>)
            .def("get_fluent_predicates", &V::get_predicates<FluentTag>)
            .def("get_derived_predicates", &V::get_predicates<DerivedTag>)
            .def("get_static_functions", &V::get_functions<StaticTag>)
            .def("get_fluent_functions", &V::get_functions<FluentTag>)
            .def("get_auxiliary_function", &V::get_auxiliary_function)
            .def("get_constants", &V::get_constants)
            .def("get_actions", &V::get_actions)
            .def("get_axioms", &V::get_axioms);
    }

    {
        using V = View<Index<Task>, Repository>;

        nb::class_<V>(m, "LiftedTask")
            .def("__str__", [](const V& self) { return to_string(self); })
            .def("__repr__", [](const V& self) { return to_string(self); })
            .def("__eq__", [](const V& self, const V& other) { return EqualTo<V> {}(self, other); })
            .def("__hash__", [](const V& self) { return Hash<V> {}(self); })
            .def("get_name", &V::get_name)
            .def("get_domain", &V::get_domain)
            .def("get_derived_predicates", &V::get_derived_predicates)
            .def("get_objects", &V::get_objects)
            .def("get_static_atoms", &V::get_atoms<StaticTag>)
            .def("get_fluent_atoms", &V::get_atoms<FluentTag>)
            .def("get_static_fterm_values", &V::get_fterm_values<StaticTag>)
            .def("get_fluent_fterm_values", &V::get_fterm_values<FluentTag>)
            .def("get_auxiliary_fterm_value", &V::get_auxiliary_fterm_value)
            .def("get_goal", &V::get_goal)
            .def("get_metric", &V::get_metric)
            .def("get_axioms", &V::get_axioms);
    }

    {
        using V = View<Index<FDRTask>, Repository>;

        nb::class_<V>(m, "GroundTask")
            .def("__str__", [](const V& self) { return to_string(self); })
            .def("__repr__", [](const V& self) { return to_string(self); })
            .def("__eq__", [](const V& self, const V& other) { return EqualTo<V> {}(self, other); })
            .def("__hash__", [](const V& self) { return Hash<V> {}(self); })
            .def("get_name", &V::get_name)
            .def("get_domain", &V::get_domain)
            .def("get_derived_predicates", &V::get_derived_predicates)
            .def("get_objects", &V::get_objects)
            .def("get_static_atoms", &V::get_atoms<StaticTag>)
            .def("get_fluent_atoms", &V::get_atoms<FluentTag>)
            .def("get_derived_atoms", &V::get_atoms<DerivedTag>)
            .def("get_static_fterm", &V::get_fterms<StaticTag>)
            .def("get_fluent_fterm", &V::get_fterms<FluentTag>)
            .def("get_auxiliary_fterm", &V::get_auxiliary_fterm)
            .def("get_static_fterm_values", &V::get_fterm_values<StaticTag>)
            .def("get_fluent_fterm_values", &V::get_fterm_values<FluentTag>)
            .def("get_auxiliary_fterm_value", &V::get_auxiliary_fterm_value)
            .def("get_goal", &V::get_goal)
            .def("get_metric", &V::get_metric)
            .def("get_axioms", &V::get_axioms)
            .def("get_fluent_variables", &V::get_fluent_variables)
            .def("get_fluent_facts", &V::get_fluent_facts)
            .def("get_ground_actions", &V::get_ground_actions)
            .def("get_ground_axioms", &V::get_ground_axioms);
    }

    nb::class_<Repository>(m, "Repository");
}

}