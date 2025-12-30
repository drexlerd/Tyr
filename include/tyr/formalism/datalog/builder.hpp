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

// Include specialization headers first
#include "tyr/formalism/datalog/datas.hpp"
//
#include "tyr/buffer/declarations.hpp"
#include "tyr/common/tuple.hpp"
#include "tyr/common/unique_object_pool.hpp"
#include "tyr/formalism/datalog/declarations.hpp"

namespace tyr::formalism::datalog
{
class Builder
{
public:
    Builder() = default;

    template<typename T>
    [[nodiscard]] auto get_builder()
    {
        return get_container<T>(m_builder).get_or_allocate();
    }

    auto& get_buffer() noexcept { return buffer; }

private:
    /**
     * Datalog
     */

    template<typename T>
    struct BuilderEntry
    {
        using value_type = T;
        using container_type = UniqueObjectPool<Data<T>>;

        container_type container;

        BuilderEntry() = default;
    };

    using BuilderStorage = std::tuple<BuilderEntry<formalism::Variable>,
                                      BuilderEntry<formalism::Object>,
                                      BuilderEntry<formalism::Binding>,
                                      BuilderEntry<formalism::Predicate<StaticTag>>,
                                      BuilderEntry<formalism::Predicate<FluentTag>>,
                                      BuilderEntry<Atom<StaticTag>>,
                                      BuilderEntry<Atom<FluentTag>>,
                                      BuilderEntry<GroundAtom<StaticTag>>,
                                      BuilderEntry<GroundAtom<FluentTag>>,
                                      BuilderEntry<Literal<StaticTag>>,
                                      BuilderEntry<Literal<FluentTag>>,
                                      BuilderEntry<GroundLiteral<StaticTag>>,
                                      BuilderEntry<GroundLiteral<FluentTag>>,
                                      BuilderEntry<formalism::Function<StaticTag>>,
                                      BuilderEntry<formalism::Function<FluentTag>>,
                                      BuilderEntry<FunctionTerm<StaticTag>>,
                                      BuilderEntry<FunctionTerm<FluentTag>>,
                                      BuilderEntry<GroundFunctionTerm<StaticTag>>,
                                      BuilderEntry<GroundFunctionTerm<FluentTag>>,
                                      BuilderEntry<GroundFunctionTermValue<StaticTag>>,
                                      BuilderEntry<GroundFunctionTermValue<FluentTag>>,
                                      BuilderEntry<UnaryOperator<OpSub, Data<FunctionExpression>>>,
                                      BuilderEntry<BinaryOperator<OpAdd, Data<FunctionExpression>>>,
                                      BuilderEntry<BinaryOperator<OpSub, Data<FunctionExpression>>>,
                                      BuilderEntry<BinaryOperator<OpMul, Data<FunctionExpression>>>,
                                      BuilderEntry<BinaryOperator<OpDiv, Data<FunctionExpression>>>,
                                      BuilderEntry<MultiOperator<OpAdd, Data<FunctionExpression>>>,
                                      BuilderEntry<MultiOperator<OpMul, Data<FunctionExpression>>>,
                                      BuilderEntry<BinaryOperator<OpEq, Data<FunctionExpression>>>,
                                      BuilderEntry<BinaryOperator<OpNe, Data<FunctionExpression>>>,
                                      BuilderEntry<BinaryOperator<OpLe, Data<FunctionExpression>>>,
                                      BuilderEntry<BinaryOperator<OpLt, Data<FunctionExpression>>>,
                                      BuilderEntry<BinaryOperator<OpGe, Data<FunctionExpression>>>,
                                      BuilderEntry<BinaryOperator<OpGt, Data<FunctionExpression>>>,
                                      BuilderEntry<UnaryOperator<OpSub, Data<GroundFunctionExpression>>>,
                                      BuilderEntry<BinaryOperator<OpAdd, Data<GroundFunctionExpression>>>,
                                      BuilderEntry<BinaryOperator<OpSub, Data<GroundFunctionExpression>>>,
                                      BuilderEntry<BinaryOperator<OpMul, Data<GroundFunctionExpression>>>,
                                      BuilderEntry<BinaryOperator<OpDiv, Data<GroundFunctionExpression>>>,
                                      BuilderEntry<MultiOperator<OpAdd, Data<GroundFunctionExpression>>>,
                                      BuilderEntry<MultiOperator<OpMul, Data<GroundFunctionExpression>>>,
                                      BuilderEntry<BinaryOperator<OpEq, Data<GroundFunctionExpression>>>,
                                      BuilderEntry<BinaryOperator<OpNe, Data<GroundFunctionExpression>>>,
                                      BuilderEntry<BinaryOperator<OpLe, Data<GroundFunctionExpression>>>,
                                      BuilderEntry<BinaryOperator<OpLt, Data<GroundFunctionExpression>>>,
                                      BuilderEntry<BinaryOperator<OpGe, Data<GroundFunctionExpression>>>,
                                      BuilderEntry<BinaryOperator<OpGt, Data<GroundFunctionExpression>>>,
                                      BuilderEntry<ConjunctiveCondition>,
                                      BuilderEntry<Rule>,
                                      BuilderEntry<GroundConjunctiveCondition>,
                                      BuilderEntry<GroundRule>,
                                      BuilderEntry<Program>>;

    BuilderStorage m_builder;

    buffer::Buffer buffer;
};

}

#endif
