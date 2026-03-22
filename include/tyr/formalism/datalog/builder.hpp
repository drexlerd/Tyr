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

#ifndef TYR_FORMALISM_DATALOG_BUILDER_HPP_
#define TYR_FORMALISM_DATALOG_BUILDER_HPP_

#include "tyr/common/tuple.hpp"
#include "tyr/common/unique_object_pool.hpp"
#include "tyr/formalism/basic_builder.hpp"
#include "tyr/formalism/datalog/datas.hpp"
#include "tyr/formalism/datalog/declarations.hpp"

namespace tyr::formalism::datalog
{
class Builder
{
private:
    using BuilderStorage = std::tuple<BasicBuilder<Variable>,
                                      BasicBuilder<Object>,
                                      BasicBuilder<RelationBinding<Predicate<StaticTag>>>,
                                      BasicBuilder<RelationBinding<Predicate<FluentTag>>>,
                                      BasicBuilder<RelationBinding<Function<StaticTag>>>,
                                      BasicBuilder<RelationBinding<Function<FluentTag>>>,
                                      BasicBuilder<RelationBinding<Rule>>,
                                      BasicBuilder<Predicate<StaticTag>>,
                                      BasicBuilder<Predicate<FluentTag>>,
                                      BasicBuilder<Atom<StaticTag>>,
                                      BasicBuilder<Atom<FluentTag>>,
                                      BasicBuilder<GroundAtom<StaticTag>>,
                                      BasicBuilder<GroundAtom<FluentTag>>,
                                      BasicBuilder<Literal<StaticTag>>,
                                      BasicBuilder<Literal<FluentTag>>,
                                      BasicBuilder<GroundLiteral<StaticTag>>,
                                      BasicBuilder<GroundLiteral<FluentTag>>,
                                      BasicBuilder<Function<StaticTag>>,
                                      BasicBuilder<Function<FluentTag>>,
                                      BasicBuilder<FunctionTerm<StaticTag>>,
                                      BasicBuilder<FunctionTerm<FluentTag>>,
                                      BasicBuilder<GroundFunctionTerm<StaticTag>>,
                                      BasicBuilder<GroundFunctionTerm<FluentTag>>,
                                      BasicBuilder<GroundFunctionTermValue<StaticTag>>,
                                      BasicBuilder<GroundFunctionTermValue<FluentTag>>,
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
                                      BasicBuilder<ConjunctiveCondition>,
                                      BasicBuilder<Rule>,
                                      BasicBuilder<GroundConjunctiveCondition>,
                                      BasicBuilder<GroundRule>,
                                      BasicBuilder<Program>>;

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
extern template class BasicBuilder<RelationBinding<Function<StaticTag>>>;
extern template class BasicBuilder<RelationBinding<Function<FluentTag>>>;
extern template class BasicBuilder<RelationBinding<datalog::Rule>>;

extern template class BasicBuilder<Predicate<StaticTag>>;
extern template class BasicBuilder<Predicate<FluentTag>>;

extern template class BasicBuilder<datalog::Atom<StaticTag>>;
extern template class BasicBuilder<datalog::Atom<FluentTag>>;
extern template class BasicBuilder<datalog::GroundAtom<StaticTag>>;
extern template class BasicBuilder<datalog::GroundAtom<FluentTag>>;

extern template class BasicBuilder<datalog::Literal<StaticTag>>;
extern template class BasicBuilder<datalog::Literal<FluentTag>>;
extern template class BasicBuilder<datalog::GroundLiteral<StaticTag>>;
extern template class BasicBuilder<datalog::GroundLiteral<FluentTag>>;

extern template class BasicBuilder<Function<StaticTag>>;
extern template class BasicBuilder<Function<FluentTag>>;

extern template class BasicBuilder<datalog::FunctionTerm<StaticTag>>;
extern template class BasicBuilder<datalog::FunctionTerm<FluentTag>>;
extern template class BasicBuilder<datalog::GroundFunctionTerm<StaticTag>>;
extern template class BasicBuilder<datalog::GroundFunctionTerm<FluentTag>>;

extern template class BasicBuilder<datalog::GroundFunctionTermValue<StaticTag>>;
extern template class BasicBuilder<datalog::GroundFunctionTermValue<FluentTag>>;

extern template class BasicBuilder<datalog::UnaryOperator<OpSub, Data<datalog::FunctionExpression>>>;

extern template class BasicBuilder<datalog::BinaryOperator<OpAdd, Data<datalog::FunctionExpression>>>;
extern template class BasicBuilder<datalog::BinaryOperator<OpSub, Data<datalog::FunctionExpression>>>;
extern template class BasicBuilder<datalog::BinaryOperator<OpMul, Data<datalog::FunctionExpression>>>;
extern template class BasicBuilder<datalog::BinaryOperator<OpDiv, Data<datalog::FunctionExpression>>>;

extern template class BasicBuilder<datalog::MultiOperator<OpAdd, Data<datalog::FunctionExpression>>>;
extern template class BasicBuilder<datalog::MultiOperator<OpMul, Data<datalog::FunctionExpression>>>;

extern template class BasicBuilder<datalog::BinaryOperator<OpEq, Data<datalog::FunctionExpression>>>;
extern template class BasicBuilder<datalog::BinaryOperator<OpNe, Data<datalog::FunctionExpression>>>;
extern template class BasicBuilder<datalog::BinaryOperator<OpLe, Data<datalog::FunctionExpression>>>;
extern template class BasicBuilder<datalog::BinaryOperator<OpLt, Data<datalog::FunctionExpression>>>;
extern template class BasicBuilder<datalog::BinaryOperator<OpGe, Data<datalog::FunctionExpression>>>;
extern template class BasicBuilder<datalog::BinaryOperator<OpGt, Data<datalog::FunctionExpression>>>;

extern template class BasicBuilder<datalog::UnaryOperator<OpSub, Data<datalog::GroundFunctionExpression>>>;

extern template class BasicBuilder<datalog::BinaryOperator<OpAdd, Data<datalog::GroundFunctionExpression>>>;
extern template class BasicBuilder<datalog::BinaryOperator<OpSub, Data<datalog::GroundFunctionExpression>>>;
extern template class BasicBuilder<datalog::BinaryOperator<OpMul, Data<datalog::GroundFunctionExpression>>>;
extern template class BasicBuilder<datalog::BinaryOperator<OpDiv, Data<datalog::GroundFunctionExpression>>>;

extern template class BasicBuilder<datalog::MultiOperator<OpAdd, Data<datalog::GroundFunctionExpression>>>;
extern template class BasicBuilder<datalog::MultiOperator<OpMul, Data<datalog::GroundFunctionExpression>>>;

extern template class BasicBuilder<datalog::BinaryOperator<OpEq, Data<datalog::GroundFunctionExpression>>>;
extern template class BasicBuilder<datalog::BinaryOperator<OpNe, Data<datalog::GroundFunctionExpression>>>;
extern template class BasicBuilder<datalog::BinaryOperator<OpLe, Data<datalog::GroundFunctionExpression>>>;
extern template class BasicBuilder<datalog::BinaryOperator<OpLt, Data<datalog::GroundFunctionExpression>>>;
extern template class BasicBuilder<datalog::BinaryOperator<OpGe, Data<datalog::GroundFunctionExpression>>>;
extern template class BasicBuilder<datalog::BinaryOperator<OpGt, Data<datalog::GroundFunctionExpression>>>;

extern template class BasicBuilder<datalog::ConjunctiveCondition>;
extern template class BasicBuilder<datalog::Rule>;
extern template class BasicBuilder<datalog::GroundConjunctiveCondition>;
extern template class BasicBuilder<datalog::GroundRule>;
extern template class BasicBuilder<datalog::Program>;

}

#endif
