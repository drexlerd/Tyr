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

#include "indices.hpp"

namespace tyr::formalism::planning
{
namespace
{
template<typename Tag>
void bind_relation_binding_index(nb::module_& m, const std::string& name)
{
    using V = Index<RelationBinding<Tag>>;

    auto cls = nb::class_<V>(m, name.c_str())  //
                   .def_ro("relation_index", &V::relation)
                   .def_ro("row_index", &V::row);
    add_print(cls);
    add_hash(cls);
}

}

/**
 * bind_indices
 */

void bind_indices(nb::module_& m)
{
    bind_index<Index<Row>>(m, "RowIndex");
    bind_index<Index<Object>>(m, "ObjectIndex");
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
}

}
