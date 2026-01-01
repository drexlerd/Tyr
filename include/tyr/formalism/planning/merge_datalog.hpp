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

#ifndef TYR_FORMALISM_PLANNING_MERGE_DATALOG_HPP_
#define TYR_FORMALISM_PLANNING_MERGE_DATALOG_HPP_

#include "tyr/common/tuple.hpp"
#include "tyr/formalism/datalog/builder.hpp"
#include "tyr/formalism/datalog/canonicalization.hpp"
#include "tyr/formalism/datalog/declarations.hpp"
#include "tyr/formalism/datalog/indices.hpp"
#include "tyr/formalism/planning/declarations.hpp"
#include "tyr/formalism/planning/indices.hpp"
#include "tyr/formalism/planning/views.hpp"

namespace tyr::formalism::planning
{

class MergeDatalogCache
{
private:
    template<typename T_SRC, typename T_DST>
    struct MapEntryType
    {
        using value_type = std::pair<T_SRC, T_DST>;
        using container_type = UnorderedMap<Index<T_SRC>, Index<T_DST>>;

        container_type container;
    };

    using MergeStorage = std::tuple<
        MapEntryType<formalism::Variable, formalism::Variable>,
        MapEntryType<formalism::Object, formalism::Object>,
        MapEntryType<formalism::Binding, formalism::Binding>,
        MapEntryType<formalism::Predicate<StaticTag>, formalism::Predicate<StaticTag>>,
        MapEntryType<formalism::Predicate<FluentTag>, formalism::Predicate<FluentTag>>,
        MapEntryType<formalism::Predicate<DerivedTag>, formalism::Predicate<DerivedTag>>,
        MapEntryType<formalism::Predicate<FluentTag>, formalism::Predicate<DerivedTag>>,
        MapEntryType<formalism::Predicate<DerivedTag>, formalism::Predicate<FluentTag>>,
        MapEntryType<Atom<StaticTag>, formalism::datalog::Atom<StaticTag>>,
        MapEntryType<Atom<FluentTag>, formalism::datalog::Atom<FluentTag>>,
        MapEntryType<Atom<DerivedTag>, formalism::datalog::Atom<DerivedTag>>,
        MapEntryType<Atom<FluentTag>, formalism::datalog::Atom<DerivedTag>>,
        MapEntryType<Atom<DerivedTag>, formalism::datalog::Atom<FluentTag>>,
        MapEntryType<GroundAtom<StaticTag>, formalism::datalog::GroundAtom<StaticTag>>,
        MapEntryType<GroundAtom<FluentTag>, formalism::datalog::GroundAtom<FluentTag>>,
        MapEntryType<GroundAtom<DerivedTag>, formalism::datalog::GroundAtom<DerivedTag>>,
        MapEntryType<GroundAtom<FluentTag>, formalism::datalog::GroundAtom<DerivedTag>>,
        MapEntryType<GroundAtom<DerivedTag>, formalism::datalog::GroundAtom<FluentTag>>,
        MapEntryType<Literal<StaticTag>, formalism::datalog::Literal<StaticTag>>,
        MapEntryType<Literal<FluentTag>, formalism::datalog::Literal<FluentTag>>,
        MapEntryType<Literal<DerivedTag>, formalism::datalog::Literal<DerivedTag>>,
        MapEntryType<Literal<FluentTag>, formalism::datalog::Literal<DerivedTag>>,
        MapEntryType<Literal<DerivedTag>, formalism::datalog::Literal<FluentTag>>,
        MapEntryType<GroundLiteral<StaticTag>, formalism::datalog::GroundLiteral<StaticTag>>,
        MapEntryType<GroundLiteral<FluentTag>, formalism::datalog::GroundLiteral<FluentTag>>,
        MapEntryType<GroundLiteral<DerivedTag>, formalism::datalog::GroundLiteral<DerivedTag>>,
        MapEntryType<GroundLiteral<FluentTag>, formalism::datalog::GroundLiteral<DerivedTag>>,
        MapEntryType<GroundLiteral<DerivedTag>, formalism::datalog::GroundLiteral<FluentTag>>,
        MapEntryType<formalism::Function<StaticTag>, formalism::Function<StaticTag>>,
        MapEntryType<formalism::Function<FluentTag>, formalism::Function<FluentTag>>,
        MapEntryType<formalism::Function<AuxiliaryTag>, formalism::Function<AuxiliaryTag>>,
        MapEntryType<FunctionTerm<StaticTag>, formalism::datalog::FunctionTerm<StaticTag>>,
        MapEntryType<FunctionTerm<FluentTag>, formalism::datalog::FunctionTerm<FluentTag>>,
        MapEntryType<FunctionTerm<AuxiliaryTag>, formalism::datalog::FunctionTerm<AuxiliaryTag>>,
        MapEntryType<GroundFunctionTerm<StaticTag>, formalism::datalog::GroundFunctionTerm<StaticTag>>,
        MapEntryType<GroundFunctionTerm<FluentTag>, formalism::datalog::GroundFunctionTerm<FluentTag>>,
        MapEntryType<GroundFunctionTerm<AuxiliaryTag>, formalism::datalog::GroundFunctionTerm<AuxiliaryTag>>,
        MapEntryType<GroundFunctionTermValue<StaticTag>, formalism::datalog::GroundFunctionTermValue<StaticTag>>,
        MapEntryType<GroundFunctionTermValue<FluentTag>, formalism::datalog::GroundFunctionTermValue<FluentTag>>,
        MapEntryType<GroundFunctionTermValue<AuxiliaryTag>, formalism::datalog::GroundFunctionTermValue<AuxiliaryTag>>,
        MapEntryType<UnaryOperator<OpSub, Data<FunctionExpression>>, formalism::datalog::UnaryOperator<OpSub, Data<formalism::datalog::FunctionExpression>>>,
        MapEntryType<BinaryOperator<OpAdd, Data<FunctionExpression>>, formalism::datalog::BinaryOperator<OpAdd, Data<formalism::datalog::FunctionExpression>>>,
        MapEntryType<BinaryOperator<OpSub, Data<FunctionExpression>>, formalism::datalog::BinaryOperator<OpSub, Data<formalism::datalog::FunctionExpression>>>,
        MapEntryType<BinaryOperator<OpMul, Data<FunctionExpression>>, formalism::datalog::BinaryOperator<OpMul, Data<formalism::datalog::FunctionExpression>>>,
        MapEntryType<BinaryOperator<OpDiv, Data<FunctionExpression>>, formalism::datalog::BinaryOperator<OpDiv, Data<formalism::datalog::FunctionExpression>>>,
        MapEntryType<MultiOperator<OpAdd, Data<FunctionExpression>>, formalism::datalog::MultiOperator<OpAdd, Data<formalism::datalog::FunctionExpression>>>,
        MapEntryType<MultiOperator<OpMul, Data<FunctionExpression>>, formalism::datalog::MultiOperator<OpMul, Data<formalism::datalog::FunctionExpression>>>,
        MapEntryType<BinaryOperator<OpEq, Data<FunctionExpression>>, formalism::datalog::BinaryOperator<OpEq, Data<formalism::datalog::FunctionExpression>>>,
        MapEntryType<BinaryOperator<OpNe, Data<FunctionExpression>>, formalism::datalog::BinaryOperator<OpNe, Data<formalism::datalog::FunctionExpression>>>,
        MapEntryType<BinaryOperator<OpLe, Data<FunctionExpression>>, formalism::datalog::BinaryOperator<OpLe, Data<formalism::datalog::FunctionExpression>>>,
        MapEntryType<BinaryOperator<OpLt, Data<FunctionExpression>>, formalism::datalog::BinaryOperator<OpLt, Data<formalism::datalog::FunctionExpression>>>,
        MapEntryType<BinaryOperator<OpGe, Data<FunctionExpression>>, formalism::datalog::BinaryOperator<OpGe, Data<formalism::datalog::FunctionExpression>>>,
        MapEntryType<BinaryOperator<OpGt, Data<FunctionExpression>>, formalism::datalog::BinaryOperator<OpGt, Data<formalism::datalog::FunctionExpression>>>,
        MapEntryType<UnaryOperator<OpSub, Data<GroundFunctionExpression>>,
                     formalism::datalog::UnaryOperator<OpSub, Data<formalism::datalog::GroundFunctionExpression>>>,
        MapEntryType<BinaryOperator<OpAdd, Data<GroundFunctionExpression>>,
                     formalism::datalog::BinaryOperator<OpAdd, Data<formalism::datalog::GroundFunctionExpression>>>,
        MapEntryType<BinaryOperator<OpSub, Data<GroundFunctionExpression>>,
                     formalism::datalog::BinaryOperator<OpSub, Data<formalism::datalog::GroundFunctionExpression>>>,
        MapEntryType<BinaryOperator<OpMul, Data<GroundFunctionExpression>>,
                     formalism::datalog::BinaryOperator<OpMul, Data<formalism::datalog::GroundFunctionExpression>>>,
        MapEntryType<BinaryOperator<OpDiv, Data<GroundFunctionExpression>>,
                     formalism::datalog::BinaryOperator<OpDiv, Data<formalism::datalog::GroundFunctionExpression>>>,
        MapEntryType<MultiOperator<OpAdd, Data<GroundFunctionExpression>>,
                     formalism::datalog::MultiOperator<OpAdd, Data<formalism::datalog::GroundFunctionExpression>>>,
        MapEntryType<MultiOperator<OpMul, Data<GroundFunctionExpression>>,
                     formalism::datalog::MultiOperator<OpMul, Data<formalism::datalog::GroundFunctionExpression>>>,
        MapEntryType<BinaryOperator<OpEq, Data<GroundFunctionExpression>>,
                     formalism::datalog::BinaryOperator<OpEq, Data<formalism::datalog::GroundFunctionExpression>>>,
        MapEntryType<BinaryOperator<OpNe, Data<GroundFunctionExpression>>,
                     formalism::datalog::BinaryOperator<OpNe, Data<formalism::datalog::GroundFunctionExpression>>>,
        MapEntryType<BinaryOperator<OpLe, Data<GroundFunctionExpression>>,
                     formalism::datalog::BinaryOperator<OpLe, Data<formalism::datalog::GroundFunctionExpression>>>,
        MapEntryType<BinaryOperator<OpLt, Data<GroundFunctionExpression>>,
                     formalism::datalog::BinaryOperator<OpLt, Data<formalism::datalog::GroundFunctionExpression>>>,
        MapEntryType<BinaryOperator<OpGe, Data<GroundFunctionExpression>>,
                     formalism::datalog::BinaryOperator<OpGe, Data<formalism::datalog::GroundFunctionExpression>>>,
        MapEntryType<BinaryOperator<OpGt, Data<GroundFunctionExpression>>,
                     formalism::datalog::BinaryOperator<OpGt, Data<formalism::datalog::GroundFunctionExpression>>>>;

    MergeStorage m_maps;

public:
    MergeDatalogCache() = default;

    template<typename T_SRC, typename T_DST = T_SRC>
    auto& get() noexcept
    {
        using Key = std::pair<T_SRC, T_DST>;
        return get_container<Key>(m_maps);
    }
    template<typename T_SRC, typename T_DST = T_SRC>
    const auto& get() const noexcept
    {
        using Key = std::pair<T_SRC, T_DST>;
        return get_container<Key>(m_maps);
    }

    void clear() noexcept
    {
        std::apply([](auto&... slots) { (slots.container.clear(), ...); }, m_maps);
    }
};

template<formalism::datalog::Context C>
struct MergeDatalogContext
{
    formalism::datalog::Builder& builder;
    C& destination;
    MergeDatalogCache& cache;
};

/**
 * Forward declarations
 */

// Common

template<Context C_SRC, formalism::datalog::Context C_DST>
auto merge_p2d(View<Index<formalism::Variable>, C_SRC> element, MergeDatalogContext<C_DST>& context);

template<Context C_SRC, formalism::datalog::Context C_DST>
auto merge_p2d(View<Index<formalism::Object>, C_SRC> element, MergeDatalogContext<C_DST>& context);

template<Context C_SRC, formalism::datalog::Context C_DST>
auto merge_p2d(View<Index<formalism::Binding>, C_SRC> element, MergeDatalogContext<C_DST>& context);

template<Context C_SRC, formalism::datalog::Context C_DST>
auto merge_p2d(View<Data<formalism::Term>, C_SRC> element, MergeDatalogContext<C_DST>& context);

// Propositional

template<FactKind T_SRC, Context C_SRC, formalism::datalog::Context C_DST, FactKind T_DST = T_SRC>
auto merge_p2d(View<Index<formalism::Predicate<T_SRC>>, C_SRC> element, MergeDatalogContext<C_DST>& context);

template<FactKind T_SRC, Context C_SRC, formalism::datalog::Context C_DST, FactKind T_DST = T_SRC>
auto merge_p2d(View<Index<Atom<T_SRC>>, C_SRC> element, MergeDatalogContext<C_DST>& context);

template<FactKind T_SRC, Context C_SRC, formalism::datalog::Context C_DST, FactKind T_DST = T_SRC>
auto merge_p2d(View<Index<GroundAtom<T_SRC>>, C_SRC> element, MergeDatalogContext<C_DST>& context);

template<FactKind T_SRC, Context C_SRC, formalism::datalog::Context C_DST, FactKind T_DST = T_SRC>
auto merge_p2d(View<Index<Literal<T_SRC>>, C_SRC> element, MergeDatalogContext<C_DST>& context);

template<FactKind T_SRC, Context C_SRC, formalism::datalog::Context C_DST, FactKind T_DST = T_SRC>
auto merge_p2d(View<Index<GroundLiteral<T_SRC>>, C_SRC> element, MergeDatalogContext<C_DST>& context);

// Numeric

template<FactKind T, Context C_SRC, formalism::datalog::Context C_DST>
auto merge_p2d(View<Index<formalism::Function<T>>, C_SRC> element, MergeDatalogContext<C_DST>& context);

template<FactKind T, Context C_SRC, formalism::datalog::Context C_DST>
auto merge_p2d(View<Index<FunctionTerm<T>>, C_SRC> element, MergeDatalogContext<C_DST>& context);

template<FactKind T, Context C_SRC, formalism::datalog::Context C_DST>
auto merge_p2d(View<Index<GroundFunctionTerm<T>>, C_SRC> element, MergeDatalogContext<C_DST>& context);

template<FactKind T, Context C_SRC, formalism::datalog::Context C_DST>
auto merge_p2d(View<Index<GroundFunctionTermValue<T>>, C_SRC> element, MergeDatalogContext<C_DST>& context);

template<Context C_SRC, formalism::datalog::Context C_DST>
auto merge_p2d(View<Data<FunctionExpression>, C_SRC> element, MergeDatalogContext<C_DST>& context);

template<Context C_SRC, formalism::datalog::Context C_DST>
auto merge_p2d(View<Data<GroundFunctionExpression>, C_SRC> element, MergeDatalogContext<C_DST>& context);

template<OpKind O, typename T, Context C_SRC, formalism::datalog::Context C_DST>
auto merge_p2d(View<Index<UnaryOperator<O, T>>, C_SRC> element, MergeDatalogContext<C_DST>& context);

template<OpKind O, typename T, Context C_SRC, formalism::datalog::Context C_DST>
auto merge_p2d(View<Index<BinaryOperator<O, T>>, C_SRC> element, MergeDatalogContext<C_DST>& context);

template<OpKind O, typename T, Context C_SRC, formalism::datalog::Context C_DST>
auto merge_p2d(View<Index<MultiOperator<O, T>>, C_SRC> element, MergeDatalogContext<C_DST>& context);

template<typename T, Context C_SRC, formalism::datalog::Context C_DST>
auto merge_p2d(View<Data<ArithmeticOperator<T>>, C_SRC> element, MergeDatalogContext<C_DST>& context);

template<typename T, Context C_SRC, formalism::datalog::Context C_DST>
auto merge_p2d(View<Data<BooleanOperator<T>>, C_SRC> element, MergeDatalogContext<C_DST>& context);

/**
 * Implementations
 */

template<typename T_SRC, typename T_DST, Context C_SRC, typename F>
auto with_cache(View<Index<T_SRC>, C_SRC> element, MergeDatalogCache& cache, F&& compute)
{
    auto& m = cache.template get<T_SRC, T_DST>();

    if (auto it = m.find(element.get_index()); it != m.end())
        return std::make_pair(it->second, false);

    auto result = compute();  // compute the merge_p2dd element

    m.emplace(element.get_index(), result.first);

    return result;
}

template<typename T>
struct to_datalog_payload
{
    using type = T;  // default: unchanged
};

template<>
struct to_datalog_payload<Data<formalism::planning::FunctionExpression>>
{
    using type = Data<formalism::datalog::FunctionExpression>;
};

template<>
struct to_datalog_payload<Data<formalism::planning::GroundFunctionExpression>>
{
    using type = Data<formalism::datalog::GroundFunctionExpression>;
};

template<typename T>
using to_datalog_payload_t = typename to_datalog_payload<T>::type;

// Common

template<Context C_SRC, formalism::datalog::Context C_DST>
auto merge_p2d(View<Index<formalism::Variable>, C_SRC> element, MergeDatalogContext<C_DST>& context)
{
    return with_cache<formalism::Variable, formalism::Variable>(element,
                                                                context.cache,
                                                                [&]()
                                                                {
                                                                    auto variable_ptr = context.builder.template get_builder<formalism::Variable>();
                                                                    auto& variable = *variable_ptr;
                                                                    variable.clear();

                                                                    variable.name = element.get_name();

                                                                    canonicalize(variable);
                                                                    return context.destination.get_or_create(variable, context.builder.get_buffer());
                                                                });
}

template<Context C_SRC, formalism::datalog::Context C_DST>
auto merge_p2d(View<Index<formalism::Object>, C_SRC> element, MergeDatalogContext<C_DST>& context)
{
    return with_cache<formalism::Object, formalism::Object>(element,
                                                            context.cache,
                                                            [&]()
                                                            {
                                                                auto object_ptr = context.builder.template get_builder<formalism::Object>();
                                                                auto& object = *object_ptr;
                                                                object.clear();

                                                                object.name = element.get_name();

                                                                canonicalize(object);
                                                                return context.destination.get_or_create(object, context.builder.get_buffer());
                                                            });
}

template<Context C_SRC, formalism::datalog::Context C_DST>
auto merge_p2d(View<Index<formalism::Binding>, C_SRC> element, MergeDatalogContext<C_DST>& context)
{
    return with_cache<formalism::Binding, formalism::Binding>(element,
                                                              context.cache,
                                                              [&]()
                                                              {
                                                                  auto binding_ptr = context.builder.template get_builder<formalism::Binding>();
                                                                  auto& binding = *binding_ptr;
                                                                  binding.clear();

                                                                  binding.objects = element.get_data().objects;

                                                                  canonicalize(binding);
                                                                  return context.destination.get_or_create(binding, context.builder.get_buffer());
                                                              });
}

template<Context C_SRC, formalism::datalog::Context C_DST>
auto merge_p2d(View<Data<Term>, C_SRC> element, MergeDatalogContext<C_DST>& context)
{
    return visit(
        [&](auto&& arg)
        {
            using Alternative = std::decay_t<decltype(arg)>;

            if constexpr (std::is_same_v<Alternative, ParameterIndex>)
                return Data<Term>(arg);
            else if constexpr (std::is_same_v<Alternative, View<Index<Object>, C_SRC>>)
                return Data<Term>(merge_p2d(arg, context).first);
            else
                static_assert(dependent_false<Alternative>::value, "Missing case");
        },
        element.get_variant());
}

// Propositional

template<FactKind T_SRC, Context C_SRC, formalism::datalog::Context C_DST, FactKind T_DST>
auto merge_p2d(View<Index<formalism::Predicate<T_SRC>>, C_SRC> element, MergeDatalogContext<C_DST>& context)
{
    return with_cache<formalism::Predicate<T_SRC>, formalism::Predicate<T_DST>>(
        element,
        context.cache,
        [&]()
        {
            auto predicate_ptr = context.builder.template get_builder<formalism::Predicate<T_DST>>();
            auto& predicate = *predicate_ptr;
            predicate.clear();

            predicate.name = element.get_name();
            predicate.arity = element.get_arity();

            canonicalize(predicate);
            return context.destination.get_or_create(predicate, context.builder.get_buffer());
        });
}

template<FactKind T_SRC, Context C_SRC, formalism::datalog::Context C_DST, FactKind T_DST>
auto merge_p2d(View<Index<Atom<T_SRC>>, C_SRC> element, MergeDatalogContext<C_DST>& context)
{
    return with_cache<Atom<T_SRC>, formalism::datalog::Atom<T_DST>>(element,
                                                                    context.cache,
                                                                    [&]()
                                                                    {
                                                                        auto atom_ptr = context.builder.template get_builder<formalism::datalog::Atom<T_DST>>();
                                                                        auto& atom = *atom_ptr;
                                                                        atom.clear();

                                                                        atom.predicate =
                                                                            merge_p2d<T_SRC, C_SRC, C_DST, T_DST>(element.get_predicate(), context).first;
                                                                        for (const auto term : element.get_terms())
                                                                            atom.terms.push_back(merge_p2d(term, context));

                                                                        canonicalize(atom);
                                                                        return context.destination.get_or_create(atom, context.builder.get_buffer());
                                                                    });
}

template<FactKind T_SRC, Context C_SRC, formalism::datalog::Context C_DST, FactKind T_DST>
auto merge_p2d(View<Index<GroundAtom<T_SRC>>, C_SRC> element, MergeDatalogContext<C_DST>& context)
{
    return with_cache<GroundAtom<T_SRC>, formalism::datalog::GroundAtom<T_DST>>(
        element,
        context.cache,
        [&]()
        {
            auto atom_ptr = context.builder.template get_builder<formalism::datalog::GroundAtom<T_DST>>();
            auto& atom = *atom_ptr;
            atom.clear();

            atom.index.group = merge_p2d<T_SRC, C_SRC, C_DST, T_DST>(element.get_predicate(), context).first;
            atom.binding = merge_p2d(element.get_binding(), context).first;

            canonicalize(atom);
            return context.destination.get_or_create(atom, context.builder.get_buffer());
        });
}

template<FactKind T_SRC, Context C_SRC, formalism::datalog::Context C_DST, FactKind T_DST>
auto merge_p2d(View<Index<Literal<T_SRC>>, C_SRC> element, MergeDatalogContext<C_DST>& context)
{
    return with_cache<Literal<T_SRC>, formalism::datalog::Literal<T_DST>>(element,
                                                                          context.cache,
                                                                          [&]()
                                                                          {
                                                                              auto literal_ptr =
                                                                                  context.builder.template get_builder<formalism::datalog::Literal<T_DST>>();
                                                                              auto& literal = *literal_ptr;
                                                                              literal.clear();

                                                                              literal.polarity = element.get_polarity();
                                                                              literal.atom =
                                                                                  merge_p2d<T_SRC, C_SRC, C_DST, T_DST>(element.get_atom(), context).first;

                                                                              canonicalize(literal);
                                                                              return context.destination.get_or_create(literal, context.builder.get_buffer());
                                                                          });
}

template<FactKind T_SRC, Context C_SRC, formalism::datalog::Context C_DST, FactKind T_DST>
auto merge_p2d(View<Index<GroundLiteral<T_SRC>>, C_SRC> element, MergeDatalogContext<C_DST>& context)
{
    return with_cache<GroundLiteral<T_SRC>, formalism::datalog::GroundLiteral<T_DST>>(
        element,
        context.cache,
        [&]()
        {
            auto literal_ptr = context.builder.template get_builder<formalism::datalog::GroundLiteral<T_DST>>();
            auto& literal = *literal_ptr;
            literal.clear();

            literal.polarity = element.get_polarity();
            literal.atom = merge_p2d<T_SRC, C_SRC, C_DST, T_DST>(element.get_atom(), context).first;

            canonicalize(literal);
            return context.destination.get_or_create(literal, context.builder.get_buffer());
        });
}

// Numeric

template<FactKind T, Context C_SRC, formalism::datalog::Context C_DST>
auto merge_p2d(View<Index<formalism::Function<T>>, C_SRC> element, MergeDatalogContext<C_DST>& context)
{
    return with_cache<formalism::Function<T>, formalism::Function<T>>(element,
                                                                      context.cache,
                                                                      [&]()
                                                                      {
                                                                          auto function_ptr = context.builder.template get_builder<formalism::Function<T>>();
                                                                          auto& function = *function_ptr;
                                                                          function.clear();

                                                                          function.name = element.get_name();
                                                                          function.arity = element.get_arity();

                                                                          canonicalize(function);
                                                                          return context.destination.get_or_create(function, context.builder.get_buffer());
                                                                      });
}

template<FactKind T, Context C_SRC, formalism::datalog::Context C_DST>
auto merge_p2d(View<Index<FunctionTerm<T>>, C_SRC> element, MergeDatalogContext<C_DST>& context)
{
    return with_cache<FunctionTerm<T>, formalism::datalog::FunctionTerm<T>>(element,
                                                                            context.cache,
                                                                            [&]()
                                                                            {
                                                                                auto fterm_ptr =
                                                                                    context.builder.template get_builder<formalism::datalog::FunctionTerm<T>>();
                                                                                auto& fterm = *fterm_ptr;
                                                                                fterm.clear();

                                                                                fterm.function = element.get_function().get_index();
                                                                                for (const auto term : element.get_terms())
                                                                                    fterm.terms.push_back(merge_p2d(term, context));

                                                                                canonicalize(fterm);
                                                                                return context.destination.get_or_create(fterm, context.builder.get_buffer());
                                                                            });
}

template<FactKind T, Context C_SRC, formalism::datalog::Context C_DST>
auto merge_p2d(View<Index<GroundFunctionTerm<T>>, C_SRC> element, MergeDatalogContext<C_DST>& context)
{
    return with_cache<GroundFunctionTerm<T>, formalism::datalog::GroundFunctionTerm<T>>(
        element,
        context.cache,
        [&]()
        {
            auto fterm_ptr = context.builder.template get_builder<formalism::datalog::GroundFunctionTerm<T>>();
            auto& fterm = *fterm_ptr;
            fterm.clear();

            fterm.index.group = element.get_function().get_index();
            fterm.binding = merge_p2d(element.get_binding(), context).first;

            canonicalize(fterm);
            return context.destination.get_or_create(fterm, context.builder.get_buffer());
        });
}

template<FactKind T, Context C_SRC, formalism::datalog::Context C_DST>
auto merge_p2d(View<Index<GroundFunctionTermValue<T>>, C_SRC> element, MergeDatalogContext<C_DST>& context)
{
    return with_cache<GroundFunctionTermValue<T>, formalism::datalog::GroundFunctionTermValue<T>>(
        element,
        context.cache,
        [&]()
        {
            auto fterm_value_ptr = context.builder.template get_builder<formalism::datalog::GroundFunctionTermValue<T>>();
            auto& fterm_value = *fterm_value_ptr;
            fterm_value.clear();

            fterm_value.fterm = merge_p2d(element.get_fterm(), context).first;
            fterm_value.value = element.get_value();

            canonicalize(fterm_value);
            return context.destination.get_or_create(fterm_value, context.builder.get_buffer());
        });
}

template<Context C_SRC, formalism::datalog::Context C_DST>
auto merge_p2d(View<Data<FunctionExpression>, C_SRC> element, MergeDatalogContext<C_DST>& context)
{
    return visit(
        [&](auto&& arg)
        {
            using Alternative = std::decay_t<decltype(arg)>;

            if constexpr (std::is_same_v<Alternative, float_t>)
                return Data<formalism::datalog::FunctionExpression>(arg);
            else if constexpr (std::is_same_v<Alternative, View<Data<ArithmeticOperator<Data<FunctionExpression>>>, C_SRC>>)
                return Data<formalism::datalog::FunctionExpression>(merge_p2d(arg, context));
            else
                return Data<formalism::datalog::FunctionExpression>(merge_p2d(arg, context).first);
        },
        element.get_variant());
}

template<Context C_SRC, formalism::datalog::Context C_DST>
auto merge_p2d(View<Data<GroundFunctionExpression>, C_SRC> element, MergeDatalogContext<C_DST>& context)
{
    return visit(
        [&](auto&& arg)
        {
            using Alternative = std::decay_t<decltype(arg)>;

            if constexpr (std::is_same_v<Alternative, float_t>)
                return Data<formalism::datalog::GroundFunctionExpression>(arg);
            else if constexpr (std::is_same_v<Alternative, View<Data<ArithmeticOperator<Data<GroundFunctionExpression>>>, C_SRC>>)
                return Data<formalism::datalog::GroundFunctionExpression>(merge_p2d(arg, context));
            else
                return Data<formalism::datalog::GroundFunctionExpression>(merge_p2d(arg, context).first);
        },
        element.get_variant());
}

template<OpKind O, typename T, Context C_SRC, formalism::datalog::Context C_DST>
auto merge_p2d(View<Index<UnaryOperator<O, T>>, C_SRC> element, MergeDatalogContext<C_DST>& context)
{
    using T_DST = to_datalog_payload_t<T>;

    return with_cache<UnaryOperator<O, T>, formalism::datalog::UnaryOperator<O, T_DST>>(
        element,
        context.cache,
        [&]()
        {
            auto unary_ptr = context.builder.template get_builder<formalism::datalog::UnaryOperator<O, T_DST>>();
            auto& unary = *unary_ptr;
            unary.clear();

            unary.arg = merge_p2d(element.get_arg(), context);

            canonicalize(unary);
            return context.destination.get_or_create(unary, context.builder.get_buffer());
        });
}

template<OpKind O, typename T, Context C_SRC, formalism::datalog::Context C_DST>
auto merge_p2d(View<Index<BinaryOperator<O, T>>, C_SRC> element, MergeDatalogContext<C_DST>& context)
{
    using T_DST = to_datalog_payload_t<T>;

    return with_cache<BinaryOperator<O, T>, formalism::datalog::BinaryOperator<O, T_DST>>(
        element,
        context.cache,
        [&]()
        {
            auto binary_ptr = context.builder.template get_builder<formalism::datalog::BinaryOperator<O, T_DST>>();
            auto& binary = *binary_ptr;
            binary.clear();

            binary.lhs = merge_p2d(element.get_lhs(), context);
            binary.rhs = merge_p2d(element.get_rhs(), context);

            canonicalize(binary);
            return context.destination.get_or_create(binary, context.builder.get_buffer());
        });
}

template<OpKind O, typename T, Context C_SRC, formalism::datalog::Context C_DST>
auto merge_p2d(View<Index<MultiOperator<O, T>>, C_SRC> element, MergeDatalogContext<C_DST>& context)
{
    using T_DST = to_datalog_payload_t<T>;

    return with_cache<MultiOperator<O, T>, formalism::datalog::MultiOperator<O, T_DST>>(
        element,
        context.cache,
        [&]()
        {
            auto multi_ptr = context.builder.template get_builder<formalism::datalog::MultiOperator<O, T_DST>>();
            auto& multi = *multi_ptr;
            multi.clear();

            for (const auto arg : element.get_args())
                multi.args.push_back(merge_p2d(arg, context));

            canonicalize(multi);
            return context.destination.get_or_create(multi, context.builder.get_buffer());
        });
}

template<typename T, Context C_SRC, formalism::datalog::Context C_DST>
auto merge_p2d(View<Data<ArithmeticOperator<T>>, C_SRC> element, MergeDatalogContext<C_DST>& context)
{
    using T_DST = to_datalog_payload_t<T>;

    return visit([&](auto&& arg) { return Data<formalism::datalog::ArithmeticOperator<T_DST>>(merge_p2d(arg, context).first); }, element.get_variant());
}

template<typename T, Context C_SRC, formalism::datalog::Context C_DST>
auto merge_p2d(View<Data<BooleanOperator<T>>, C_SRC> element, MergeDatalogContext<C_DST>& context)
{
    using T_DST = to_datalog_payload_t<T>;

    return visit([&](auto&& arg) { return Data<formalism::datalog::BooleanOperator<T_DST>>(merge_p2d(arg, context).first); }, element.get_variant());
}

}

#endif