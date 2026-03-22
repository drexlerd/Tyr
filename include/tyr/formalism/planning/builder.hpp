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

#ifndef TYR_FORMALISM_PLANNING_BUILDER_HPP_
#define TYR_FORMALISM_PLANNING_BUILDER_HPP_

#include "tyr/common/tuple.hpp"
#include "tyr/common/unique_object_pool.hpp"
#include "tyr/formalism/basic_builder.hpp"
#include "tyr/formalism/planning/datas.hpp"
#include "tyr/formalism/planning/declarations.hpp"

namespace tyr::formalism::planning
{
class Builder
{
private:
    using BuilderStorage = std::tuple<BasicBuilder<formalism::Variable>,
                                      BasicBuilder<formalism::Object>,
                                      BasicBuilder<RelationBinding<Predicate<StaticTag>>>,
                                      BasicBuilder<RelationBinding<Predicate<FluentTag>>>,
                                      BasicBuilder<RelationBinding<Predicate<DerivedTag>>>,
                                      BasicBuilder<RelationBinding<Function<StaticTag>>>,
                                      BasicBuilder<RelationBinding<Function<FluentTag>>>,
                                      BasicBuilder<RelationBinding<Function<AuxiliaryTag>>>,
                                      BasicBuilder<RelationBinding<Action>>,
                                      BasicBuilder<RelationBinding<Axiom>>,
                                      BasicBuilder<formalism::Predicate<StaticTag>>,
                                      BasicBuilder<formalism::Predicate<FluentTag>>,
                                      BasicBuilder<formalism::Predicate<DerivedTag>>,
                                      BasicBuilder<Atom<StaticTag>>,
                                      BasicBuilder<Atom<FluentTag>>,
                                      BasicBuilder<Atom<DerivedTag>>,
                                      BasicBuilder<GroundAtom<StaticTag>>,
                                      BasicBuilder<GroundAtom<FluentTag>>,
                                      BasicBuilder<GroundAtom<DerivedTag>>,
                                      BasicBuilder<Literal<StaticTag>>,
                                      BasicBuilder<Literal<FluentTag>>,
                                      BasicBuilder<Literal<DerivedTag>>,
                                      BasicBuilder<GroundLiteral<StaticTag>>,
                                      BasicBuilder<GroundLiteral<FluentTag>>,
                                      BasicBuilder<GroundLiteral<DerivedTag>>,
                                      BasicBuilder<formalism::Function<StaticTag>>,
                                      BasicBuilder<formalism::Function<FluentTag>>,
                                      BasicBuilder<formalism::Function<AuxiliaryTag>>,
                                      BasicBuilder<FunctionTerm<StaticTag>>,
                                      BasicBuilder<FunctionTerm<FluentTag>>,
                                      BasicBuilder<FunctionTerm<AuxiliaryTag>>,
                                      BasicBuilder<GroundFunctionTerm<StaticTag>>,
                                      BasicBuilder<GroundFunctionTerm<FluentTag>>,
                                      BasicBuilder<GroundFunctionTerm<AuxiliaryTag>>,
                                      BasicBuilder<GroundFunctionTermValue<StaticTag>>,
                                      BasicBuilder<GroundFunctionTermValue<FluentTag>>,
                                      BasicBuilder<GroundFunctionTermValue<AuxiliaryTag>>,
                                      BasicBuilder<UnaryOperator<OpSub, Data<FunctionExpression>>>,
                                      BasicBuilder<BinaryOperator<OpAdd, Data<FunctionExpression>>>,
                                      BasicBuilder<BinaryOperator<OpSub, Data<FunctionExpression>>>,
                                      BasicBuilder<BinaryOperator<OpMul, Data<FunctionExpression>>>,
                                      BasicBuilder<BinaryOperator<OpDiv, Data<FunctionExpression>>>,
                                      BasicBuilder<MultiOperator<OpAdd, Data<FunctionExpression>>>,
                                      BasicBuilder<MultiOperator<OpMul, Data<FunctionExpression>>>,
                                      BasicBuilder<BinaryOperator<OpEq, Data<FunctionExpression>>>,
                                      BasicBuilder<BinaryOperator<OpNe, Data<FunctionExpression>>>,
                                      BasicBuilder<BinaryOperator<OpLe, Data<FunctionExpression>>>,
                                      BasicBuilder<BinaryOperator<OpLt, Data<FunctionExpression>>>,
                                      BasicBuilder<BinaryOperator<OpGe, Data<FunctionExpression>>>,
                                      BasicBuilder<BinaryOperator<OpGt, Data<FunctionExpression>>>,
                                      BasicBuilder<UnaryOperator<OpSub, Data<GroundFunctionExpression>>>,
                                      BasicBuilder<BinaryOperator<OpAdd, Data<GroundFunctionExpression>>>,
                                      BasicBuilder<BinaryOperator<OpSub, Data<GroundFunctionExpression>>>,
                                      BasicBuilder<BinaryOperator<OpMul, Data<GroundFunctionExpression>>>,
                                      BasicBuilder<BinaryOperator<OpDiv, Data<GroundFunctionExpression>>>,
                                      BasicBuilder<MultiOperator<OpAdd, Data<GroundFunctionExpression>>>,
                                      BasicBuilder<MultiOperator<OpMul, Data<GroundFunctionExpression>>>,
                                      BasicBuilder<BinaryOperator<OpEq, Data<GroundFunctionExpression>>>,
                                      BasicBuilder<BinaryOperator<OpNe, Data<GroundFunctionExpression>>>,
                                      BasicBuilder<BinaryOperator<OpLe, Data<GroundFunctionExpression>>>,
                                      BasicBuilder<BinaryOperator<OpLt, Data<GroundFunctionExpression>>>,
                                      BasicBuilder<BinaryOperator<OpGe, Data<GroundFunctionExpression>>>,
                                      BasicBuilder<BinaryOperator<OpGt, Data<GroundFunctionExpression>>>,
                                      BasicBuilder<NumericEffect<OpAssign, FluentTag>>,
                                      BasicBuilder<NumericEffect<OpIncrease, FluentTag>>,
                                      BasicBuilder<NumericEffect<OpDecrease, FluentTag>>,
                                      BasicBuilder<NumericEffect<OpScaleUp, FluentTag>>,
                                      BasicBuilder<NumericEffect<OpScaleDown, FluentTag>>,
                                      BasicBuilder<NumericEffect<OpIncrease, AuxiliaryTag>>,
                                      BasicBuilder<GroundNumericEffect<OpAssign, FluentTag>>,
                                      BasicBuilder<GroundNumericEffect<OpIncrease, FluentTag>>,
                                      BasicBuilder<GroundNumericEffect<OpDecrease, FluentTag>>,
                                      BasicBuilder<GroundNumericEffect<OpScaleUp, FluentTag>>,
                                      BasicBuilder<GroundNumericEffect<OpScaleDown, FluentTag>>,
                                      BasicBuilder<GroundNumericEffect<OpIncrease, AuxiliaryTag>>,
                                      BasicBuilder<ConditionalEffect>,
                                      BasicBuilder<GroundConditionalEffect>,
                                      BasicBuilder<ConjunctiveEffect>,
                                      BasicBuilder<GroundConjunctiveEffect>,
                                      BasicBuilder<Action>,
                                      BasicBuilder<GroundAction>,
                                      BasicBuilder<Axiom>,
                                      BasicBuilder<GroundAxiom>,
                                      BasicBuilder<Metric>,
                                      BasicBuilder<Domain>,
                                      BasicBuilder<Task>,
                                      BasicBuilder<FDRVariable<FluentTag>>,
                                      BasicBuilder<FDRVariable<DerivedTag>>,
                                      BasicBuilder<FDRFact<FluentTag>>,
                                      BasicBuilder<FDRFact<DerivedTag>>,
                                      BasicBuilder<ConjunctiveCondition>,
                                      BasicBuilder<GroundConjunctiveCondition>,
                                      BasicBuilder<FDRTask>>;

    BuilderStorage m_builder;

public:
    Builder() = default;

    template<typename T>
    [[nodiscard]] auto get_builder()
    {
        return std::get<BasicBuilder<T>>(m_builder).get_builder();
    }
};
}

namespace tyr::formalism
{
/**
 * Explicit extern template declarations for BasicBuilder
 */

extern template class BasicBuilder<Variable>;
extern template class BasicBuilder<Object>;
extern template class BasicBuilder<RelationBinding<Predicate<StaticTag>>>;
extern template class BasicBuilder<RelationBinding<Predicate<FluentTag>>>;
extern template class BasicBuilder<RelationBinding<Predicate<DerivedTag>>>;
extern template class BasicBuilder<RelationBinding<Function<StaticTag>>>;
extern template class BasicBuilder<RelationBinding<Function<FluentTag>>>;
extern template class BasicBuilder<RelationBinding<Function<AuxiliaryTag>>>;
extern template class BasicBuilder<RelationBinding<planning::Action>>;
extern template class BasicBuilder<RelationBinding<planning::Axiom>>;

extern template class BasicBuilder<formalism::Predicate<StaticTag>>;
extern template class BasicBuilder<formalism::Predicate<FluentTag>>;
extern template class BasicBuilder<formalism::Predicate<DerivedTag>>;

extern template class BasicBuilder<planning::Atom<StaticTag>>;
extern template class BasicBuilder<planning::Atom<FluentTag>>;
extern template class BasicBuilder<planning::Atom<DerivedTag>>;

extern template class BasicBuilder<planning::GroundAtom<StaticTag>>;
extern template class BasicBuilder<planning::GroundAtom<FluentTag>>;
extern template class BasicBuilder<planning::GroundAtom<DerivedTag>>;

extern template class BasicBuilder<planning::Literal<StaticTag>>;
extern template class BasicBuilder<planning::Literal<FluentTag>>;
extern template class BasicBuilder<planning::Literal<DerivedTag>>;

extern template class BasicBuilder<planning::GroundLiteral<StaticTag>>;
extern template class BasicBuilder<planning::GroundLiteral<FluentTag>>;
extern template class BasicBuilder<planning::GroundLiteral<DerivedTag>>;

extern template class BasicBuilder<Function<StaticTag>>;
extern template class BasicBuilder<Function<FluentTag>>;
extern template class BasicBuilder<Function<AuxiliaryTag>>;

extern template class BasicBuilder<planning::FunctionTerm<StaticTag>>;
extern template class BasicBuilder<planning::FunctionTerm<FluentTag>>;
extern template class BasicBuilder<planning::FunctionTerm<AuxiliaryTag>>;

extern template class BasicBuilder<planning::GroundFunctionTerm<StaticTag>>;
extern template class BasicBuilder<planning::GroundFunctionTerm<FluentTag>>;
extern template class BasicBuilder<planning::GroundFunctionTerm<AuxiliaryTag>>;

extern template class BasicBuilder<planning::GroundFunctionTermValue<StaticTag>>;
extern template class BasicBuilder<planning::GroundFunctionTermValue<FluentTag>>;
extern template class BasicBuilder<planning::GroundFunctionTermValue<AuxiliaryTag>>;

extern template class BasicBuilder<planning::UnaryOperator<OpSub, Data<planning::FunctionExpression>>>;

extern template class BasicBuilder<planning::BinaryOperator<OpAdd, Data<planning::FunctionExpression>>>;
extern template class BasicBuilder<planning::BinaryOperator<OpSub, Data<planning::FunctionExpression>>>;
extern template class BasicBuilder<planning::BinaryOperator<OpMul, Data<planning::FunctionExpression>>>;
extern template class BasicBuilder<planning::BinaryOperator<OpDiv, Data<planning::FunctionExpression>>>;

extern template class BasicBuilder<planning::MultiOperator<OpAdd, Data<planning::FunctionExpression>>>;
extern template class BasicBuilder<planning::MultiOperator<OpMul, Data<planning::FunctionExpression>>>;

extern template class BasicBuilder<planning::BinaryOperator<OpEq, Data<planning::FunctionExpression>>>;
extern template class BasicBuilder<planning::BinaryOperator<OpNe, Data<planning::FunctionExpression>>>;
extern template class BasicBuilder<planning::BinaryOperator<OpLe, Data<planning::FunctionExpression>>>;
extern template class BasicBuilder<planning::BinaryOperator<OpLt, Data<planning::FunctionExpression>>>;
extern template class BasicBuilder<planning::BinaryOperator<OpGe, Data<planning::FunctionExpression>>>;
extern template class BasicBuilder<planning::BinaryOperator<OpGt, Data<planning::FunctionExpression>>>;

extern template class BasicBuilder<planning::UnaryOperator<OpSub, Data<planning::GroundFunctionExpression>>>;

extern template class BasicBuilder<planning::BinaryOperator<OpAdd, Data<planning::GroundFunctionExpression>>>;
extern template class BasicBuilder<planning::BinaryOperator<OpSub, Data<planning::GroundFunctionExpression>>>;
extern template class BasicBuilder<planning::BinaryOperator<OpMul, Data<planning::GroundFunctionExpression>>>;
extern template class BasicBuilder<planning::BinaryOperator<OpDiv, Data<planning::GroundFunctionExpression>>>;

extern template class BasicBuilder<planning::MultiOperator<OpAdd, Data<planning::GroundFunctionExpression>>>;
extern template class BasicBuilder<planning::MultiOperator<OpMul, Data<planning::GroundFunctionExpression>>>;

extern template class BasicBuilder<planning::BinaryOperator<OpEq, Data<planning::GroundFunctionExpression>>>;
extern template class BasicBuilder<planning::BinaryOperator<OpNe, Data<planning::GroundFunctionExpression>>>;
extern template class BasicBuilder<planning::BinaryOperator<OpLe, Data<planning::GroundFunctionExpression>>>;
extern template class BasicBuilder<planning::BinaryOperator<OpLt, Data<planning::GroundFunctionExpression>>>;
extern template class BasicBuilder<planning::BinaryOperator<OpGe, Data<planning::GroundFunctionExpression>>>;
extern template class BasicBuilder<planning::BinaryOperator<OpGt, Data<planning::GroundFunctionExpression>>>;

extern template class BasicBuilder<planning::NumericEffect<planning::OpAssign, FluentTag>>;
extern template class BasicBuilder<planning::NumericEffect<planning::OpIncrease, FluentTag>>;
extern template class BasicBuilder<planning::NumericEffect<planning::OpDecrease, FluentTag>>;
extern template class BasicBuilder<planning::NumericEffect<planning::OpScaleUp, FluentTag>>;
extern template class BasicBuilder<planning::NumericEffect<planning::OpScaleDown, FluentTag>>;
extern template class BasicBuilder<planning::NumericEffect<planning::OpIncrease, AuxiliaryTag>>;

extern template class BasicBuilder<planning::GroundNumericEffect<planning::OpAssign, FluentTag>>;
extern template class BasicBuilder<planning::GroundNumericEffect<planning::OpIncrease, FluentTag>>;
extern template class BasicBuilder<planning::GroundNumericEffect<planning::OpDecrease, FluentTag>>;
extern template class BasicBuilder<planning::GroundNumericEffect<planning::OpScaleUp, FluentTag>>;
extern template class BasicBuilder<planning::GroundNumericEffect<planning::OpScaleDown, FluentTag>>;
extern template class BasicBuilder<planning::GroundNumericEffect<planning::OpIncrease, AuxiliaryTag>>;

extern template class BasicBuilder<planning::ConditionalEffect>;
extern template class BasicBuilder<planning::GroundConditionalEffect>;

extern template class BasicBuilder<planning::ConjunctiveEffect>;
extern template class BasicBuilder<planning::GroundConjunctiveEffect>;

extern template class BasicBuilder<planning::Action>;
extern template class BasicBuilder<planning::GroundAction>;

extern template class BasicBuilder<planning::Axiom>;
extern template class BasicBuilder<planning::GroundAxiom>;

extern template class BasicBuilder<planning::Metric>;
extern template class BasicBuilder<planning::Domain>;
extern template class BasicBuilder<planning::Task>;

extern template class BasicBuilder<planning::FDRVariable<FluentTag>>;
extern template class BasicBuilder<planning::FDRVariable<DerivedTag>>;

extern template class BasicBuilder<planning::FDRFact<FluentTag>>;
extern template class BasicBuilder<planning::FDRFact<DerivedTag>>;

extern template class BasicBuilder<planning::ConjunctiveCondition>;
extern template class BasicBuilder<planning::GroundConjunctiveCondition>;

extern template class BasicBuilder<planning::FDRTask>;
}

#endif