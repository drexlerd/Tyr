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

#ifndef TYR_FORMALISM_PLANNING_MERGE_PLANNING_HPP_
#define TYR_FORMALISM_PLANNING_MERGE_PLANNING_HPP_

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

class MergePlanningCache
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
        MapEntryType<formalism::datalog::Atom<StaticTag>, Atom<StaticTag>>,
        MapEntryType<formalism::datalog::Atom<FluentTag>, Atom<FluentTag>>,
        MapEntryType<formalism::datalog::Atom<DerivedTag>, Atom<DerivedTag>>,
        MapEntryType<formalism::datalog::Atom<FluentTag>, Atom<DerivedTag>>,
        MapEntryType<formalism::datalog::Atom<DerivedTag>, Atom<FluentTag>>,
        MapEntryType<formalism::datalog::GroundAtom<StaticTag>, GroundAtom<StaticTag>>,
        MapEntryType<formalism::datalog::GroundAtom<FluentTag>, GroundAtom<FluentTag>>,
        MapEntryType<formalism::datalog::GroundAtom<DerivedTag>, GroundAtom<DerivedTag>>,
        MapEntryType<formalism::datalog::GroundAtom<FluentTag>, GroundAtom<DerivedTag>>,
        MapEntryType<formalism::datalog::GroundAtom<DerivedTag>, GroundAtom<FluentTag>>,
        MapEntryType<formalism::datalog::Literal<StaticTag>, Literal<StaticTag>>,
        MapEntryType<formalism::datalog::Literal<FluentTag>, Literal<FluentTag>>,
        MapEntryType<formalism::datalog::Literal<DerivedTag>, Literal<DerivedTag>>,
        MapEntryType<formalism::datalog::Literal<FluentTag>, Literal<DerivedTag>>,
        MapEntryType<formalism::datalog::Literal<DerivedTag>, Literal<FluentTag>>,
        MapEntryType<formalism::datalog::GroundLiteral<StaticTag>, GroundLiteral<StaticTag>>,
        MapEntryType<formalism::datalog::GroundLiteral<FluentTag>, GroundLiteral<FluentTag>>,
        MapEntryType<formalism::datalog::GroundLiteral<DerivedTag>, GroundLiteral<DerivedTag>>,
        MapEntryType<formalism::datalog::GroundLiteral<FluentTag>, GroundLiteral<DerivedTag>>,
        MapEntryType<formalism::datalog::GroundLiteral<DerivedTag>, GroundLiteral<FluentTag>>,
        MapEntryType<formalism::Function<StaticTag>, formalism::Function<StaticTag>>,
        MapEntryType<formalism::Function<FluentTag>, formalism::Function<FluentTag>>,
        MapEntryType<formalism::Function<AuxiliaryTag>, formalism::Function<AuxiliaryTag>>,
        MapEntryType<formalism::datalog::FunctionTerm<StaticTag>, FunctionTerm<StaticTag>>,
        MapEntryType<formalism::datalog::FunctionTerm<FluentTag>, FunctionTerm<FluentTag>>,
        MapEntryType<formalism::datalog::FunctionTerm<AuxiliaryTag>, FunctionTerm<AuxiliaryTag>>,
        MapEntryType<formalism::datalog::GroundFunctionTerm<StaticTag>, GroundFunctionTerm<StaticTag>>,
        MapEntryType<formalism::datalog::GroundFunctionTerm<FluentTag>, GroundFunctionTerm<FluentTag>>,
        MapEntryType<formalism::datalog::GroundFunctionTerm<AuxiliaryTag>, GroundFunctionTerm<AuxiliaryTag>>,
        MapEntryType<formalism::datalog::GroundFunctionTermValue<StaticTag>, GroundFunctionTermValue<StaticTag>>,
        MapEntryType<formalism::datalog::GroundFunctionTermValue<FluentTag>, GroundFunctionTermValue<FluentTag>>,
        MapEntryType<formalism::datalog::GroundFunctionTermValue<AuxiliaryTag>, GroundFunctionTermValue<AuxiliaryTag>>,
        MapEntryType<formalism::datalog::UnaryOperator<OpSub, Data<formalism::datalog::FunctionExpression>>, UnaryOperator<OpSub, Data<FunctionExpression>>>,
        MapEntryType<formalism::datalog::BinaryOperator<OpAdd, Data<formalism::datalog::FunctionExpression>>, BinaryOperator<OpAdd, Data<FunctionExpression>>>,
        MapEntryType<formalism::datalog::BinaryOperator<OpSub, Data<formalism::datalog::FunctionExpression>>, BinaryOperator<OpSub, Data<FunctionExpression>>>,
        MapEntryType<formalism::datalog::BinaryOperator<OpMul, Data<formalism::datalog::FunctionExpression>>, BinaryOperator<OpMul, Data<FunctionExpression>>>,
        MapEntryType<formalism::datalog::BinaryOperator<OpDiv, Data<formalism::datalog::FunctionExpression>>, BinaryOperator<OpDiv, Data<FunctionExpression>>>,
        MapEntryType<formalism::datalog::MultiOperator<OpAdd, Data<formalism::datalog::FunctionExpression>>, MultiOperator<OpAdd, Data<FunctionExpression>>>,
        MapEntryType<formalism::datalog::MultiOperator<OpMul, Data<formalism::datalog::FunctionExpression>>, MultiOperator<OpMul, Data<FunctionExpression>>>,
        MapEntryType<formalism::datalog::BinaryOperator<OpEq, Data<formalism::datalog::FunctionExpression>>, BinaryOperator<OpEq, Data<FunctionExpression>>>,
        MapEntryType<formalism::datalog::BinaryOperator<OpNe, Data<formalism::datalog::FunctionExpression>>, BinaryOperator<OpNe, Data<FunctionExpression>>>,
        MapEntryType<formalism::datalog::BinaryOperator<OpLe, Data<formalism::datalog::FunctionExpression>>, BinaryOperator<OpLe, Data<FunctionExpression>>>,
        MapEntryType<formalism::datalog::BinaryOperator<OpLt, Data<formalism::datalog::FunctionExpression>>, BinaryOperator<OpLt, Data<FunctionExpression>>>,
        MapEntryType<formalism::datalog::BinaryOperator<OpGe, Data<formalism::datalog::FunctionExpression>>, BinaryOperator<OpGe, Data<FunctionExpression>>>,
        MapEntryType<formalism::datalog::BinaryOperator<OpGt, Data<formalism::datalog::FunctionExpression>>, BinaryOperator<OpGt, Data<FunctionExpression>>>,
        MapEntryType<formalism::datalog::UnaryOperator<OpSub, Data<formalism::datalog::GroundFunctionExpression>>,
                     UnaryOperator<OpSub, Data<GroundFunctionExpression>>>,
        MapEntryType<formalism::datalog::BinaryOperator<OpAdd, Data<formalism::datalog::GroundFunctionExpression>>,
                     BinaryOperator<OpAdd, Data<GroundFunctionExpression>>>,
        MapEntryType<formalism::datalog::BinaryOperator<OpSub, Data<formalism::datalog::GroundFunctionExpression>>,
                     BinaryOperator<OpSub, Data<GroundFunctionExpression>>>,
        MapEntryType<formalism::datalog::BinaryOperator<OpMul, Data<formalism::datalog::GroundFunctionExpression>>,
                     BinaryOperator<OpMul, Data<GroundFunctionExpression>>>,
        MapEntryType<formalism::datalog::BinaryOperator<OpDiv, Data<formalism::datalog::GroundFunctionExpression>>,
                     BinaryOperator<OpDiv, Data<GroundFunctionExpression>>>,
        MapEntryType<formalism::datalog::MultiOperator<OpAdd, Data<formalism::datalog::GroundFunctionExpression>>,
                     MultiOperator<OpAdd, Data<GroundFunctionExpression>>>,
        MapEntryType<formalism::datalog::MultiOperator<OpMul, Data<formalism::datalog::GroundFunctionExpression>>,
                     MultiOperator<OpMul, Data<GroundFunctionExpression>>>,
        MapEntryType<formalism::datalog::BinaryOperator<OpEq, Data<formalism::datalog::GroundFunctionExpression>>,
                     BinaryOperator<OpEq, Data<GroundFunctionExpression>>>,
        MapEntryType<formalism::datalog::BinaryOperator<OpNe, Data<formalism::datalog::GroundFunctionExpression>>,
                     BinaryOperator<OpNe, Data<GroundFunctionExpression>>>,
        MapEntryType<formalism::datalog::BinaryOperator<OpLe, Data<formalism::datalog::GroundFunctionExpression>>,
                     BinaryOperator<OpLe, Data<GroundFunctionExpression>>>,
        MapEntryType<formalism::datalog::BinaryOperator<OpLt, Data<formalism::datalog::GroundFunctionExpression>>,
                     BinaryOperator<OpLt, Data<GroundFunctionExpression>>>,
        MapEntryType<formalism::datalog::BinaryOperator<OpGe, Data<formalism::datalog::GroundFunctionExpression>>,
                     BinaryOperator<OpGe, Data<GroundFunctionExpression>>>,
        MapEntryType<formalism::datalog::BinaryOperator<OpGt, Data<formalism::datalog::GroundFunctionExpression>>,
                     BinaryOperator<OpGt, Data<GroundFunctionExpression>>>>;

    MergeStorage m_maps;

public:
    MergePlanningCache() = default;

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

template<Context C>
struct MergePlanningContext
{
    Builder& builder;
    C& destination;
    MergePlanningCache& cache;
};

/**
 * Forward declarations
 */

// Common

template<Context C_SRC, Context C_DST>
auto merge_d2p(View<Index<formalism::Variable>, C_SRC> element, MergePlanningContext<C_DST>& context);

template<formalism::datalog::Context C_SRC, Context C_DST>
auto merge_d2p(View<Index<formalism::Object>, C_SRC> element, MergePlanningContext<C_DST>& context);

template<formalism::datalog::Context C_SRC, Context C_DST>
auto merge_d2p(View<Index<formalism::Binding>, C_SRC> element, MergePlanningContext<C_DST>& context);

template<formalism::datalog::Context C_SRC, Context C_DST>
auto merge_d2p(View<Data<formalism::Term>, C_SRC> element, MergePlanningContext<C_DST>& context);

// Propositional

template<FactKind T_SRC, formalism::datalog::Context C_SRC, Context C_DST, FactKind T_DST = T_SRC>
auto merge_d2p(View<Index<formalism::Predicate<T_SRC>>, C_SRC> element, MergePlanningContext<C_DST>& context);

template<FactKind T_SRC, formalism::datalog::Context C_SRC, Context C_DST, FactKind T_DST = T_SRC>
auto merge_d2p(View<Index<formalism::datalog::Atom<T_SRC>>, C_SRC> element, MergePlanningContext<C_DST>& context);

template<FactKind T_SRC, formalism::datalog::Context C_SRC, Context C_DST, FactKind T_DST = T_SRC>
auto merge_d2p(View<Index<formalism::datalog::GroundAtom<T_SRC>>, C_SRC> element, MergePlanningContext<C_DST>& context);

template<FactKind T_SRC, formalism::datalog::Context C_SRC, Context C_DST, FactKind T_DST = T_SRC>
auto merge_d2p(View<Index<formalism::datalog::Literal<T_SRC>>, C_SRC> element, MergePlanningContext<C_DST>& context);

template<FactKind T_SRC, formalism::datalog::Context C_SRC, Context C_DST, FactKind T_DST = T_SRC>
auto merge_d2p(View<Index<formalism::datalog::GroundLiteral<T_SRC>>, C_SRC> element, MergePlanningContext<C_DST>& context);

// Numeric

template<FactKind T, formalism::datalog::Context C_SRC, Context C_DST>
auto merge_d2p(View<Index<formalism::Function<T>>, C_SRC> element, MergePlanningContext<C_DST>& context);

template<FactKind T, formalism::datalog::Context C_SRC, Context C_DST>
auto merge_d2p(View<Index<formalism::datalog::FunctionTerm<T>>, C_SRC> element, MergePlanningContext<C_DST>& context);

template<FactKind T, formalism::datalog::Context C_SRC, Context C_DST>
auto merge_d2p(View<Index<formalism::datalog::GroundFunctionTerm<T>>, C_SRC> element, MergePlanningContext<C_DST>& context);

template<FactKind T, formalism::datalog::Context C_SRC, Context C_DST>
auto merge_d2p(View<Index<formalism::datalog::GroundFunctionTermValue<T>>, C_SRC> element, MergePlanningContext<C_DST>& context);

template<formalism::datalog::Context C_SRC, Context C_DST>
auto merge_d2p(View<Data<formalism::datalog::FunctionExpression>, C_SRC> element, MergePlanningContext<C_DST>& context);

template<formalism::datalog::Context C_SRC, Context C_DST>
auto merge_d2p(View<Data<formalism::datalog::GroundFunctionExpression>, C_SRC> element, MergePlanningContext<C_DST>& context);

template<OpKind O, typename T, formalism::datalog::Context C_SRC, Context C_DST>
auto merge_d2p(View<Index<formalism::datalog::UnaryOperator<O, T>>, C_SRC> element, MergePlanningContext<C_DST>& context);

template<OpKind O, typename T, formalism::datalog::Context C_SRC, Context C_DST>
auto merge_d2p(View<Index<formalism::datalog::BinaryOperator<O, T>>, C_SRC> element, MergePlanningContext<C_DST>& context);

template<OpKind O, typename T, formalism::datalog::Context C_SRC, Context C_DST>
auto merge_d2p(View<Index<formalism::datalog::MultiOperator<O, T>>, C_SRC> element, MergePlanningContext<C_DST>& context);

template<typename T, formalism::datalog::Context C_SRC, Context C_DST>
auto merge_d2p(View<Data<formalism::datalog::ArithmeticOperator<T>>, C_SRC> element, MergePlanningContext<C_DST>& context);

template<typename T, formalism::datalog::Context C_SRC, Context C_DST>
auto merge_d2p(View<Data<formalism::datalog::BooleanOperator<T>>, C_SRC> element, MergePlanningContext<C_DST>& context);

/**
 * Implementations
 */

template<typename T_SRC, typename T_DST, Context C_SRC, typename F>
auto with_cache(View<Index<T_SRC>, C_SRC> element, MergePlanningCache& cache, F&& compute)
{
    auto& m = cache.template get<T_SRC, T_DST>();

    if (auto it = m.find(element.get_index()); it != m.end())
        return std::make_pair(it->second, false);

    auto result = compute();  // compute the merge_d2pd element

    m.emplace(element.get_index(), result.first);

    return result;
}

template<typename T>
struct to_planning_payload
{
    using type = T;  // default: unchanged
};

template<>
struct to_planning_payload<Data<formalism::datalog::FunctionExpression>>
{
    using type = Data<formalism::planning::FunctionExpression>;
};

template<>
struct to_planning_payload<Data<formalism::datalog::GroundFunctionExpression>>
{
    using type = Data<formalism::planning::GroundFunctionExpression>;
};

template<typename T>
using to_planning_payload_t = typename to_planning_payload<T>::type;

// Common

template<formalism::datalog::Context C_SRC, Context C_DST>
auto merge_d2p(View<Index<formalism::Variable>, C_SRC> element, MergePlanningContext<C_DST>& context)
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

template<formalism::datalog::Context C_SRC, Context C_DST>
auto merge_d2p(View<Index<formalism::Object>, C_SRC> element, MergePlanningContext<C_DST>& context)
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

template<formalism::datalog::Context C_SRC, Context C_DST>
auto merge_d2p(View<Index<formalism::Binding>, C_SRC> element, MergePlanningContext<C_DST>& context)
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

template<formalism::datalog::Context C_SRC, Context C_DST>
auto merge_d2p(View<Data<formalism::Term>, C_SRC> element, MergePlanningContext<C_DST>& context)
{
    return visit(
        [&](auto&& arg)
        {
            using Alternative = std::decay_t<decltype(arg)>;

            if constexpr (std::is_same_v<Alternative, formalism::ParameterIndex>)
                return Data<formalism::Term>(arg);
            else if constexpr (std::is_same_v<Alternative, View<Index<formalism::Object>, C_SRC>>)
                return Data<formalism::Term>(merge_d2p(arg, context).first);
            else
                static_assert(dependent_false<Alternative>::value, "Missing case");
        },
        element.get_variant());
}

// Propositional

template<FactKind T_SRC, formalism::datalog::Context C_SRC, Context C_DST, FactKind T_DST>
auto merge_d2p(View<Index<formalism::Predicate<T_SRC>>, C_SRC> element, MergePlanningContext<C_DST>& context)
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

template<FactKind T_SRC, formalism::datalog::Context C_SRC, Context C_DST, FactKind T_DST>
auto merge_d2p(View<Index<formalism::datalog::Atom<T_SRC>>, C_SRC> element, MergePlanningContext<C_DST>& context)
{
    return with_cache<formalism::datalog::Atom<T_SRC>, Atom<T_DST>>(element,
                                                                    context.cache,
                                                                    [&]()
                                                                    {
                                                                        auto atom_ptr = context.builder.template get_builder<Atom<T_DST>>();
                                                                        auto& atom = *atom_ptr;
                                                                        atom.clear();

                                                                        atom.predicate =
                                                                            merge_d2p<T_SRC, C_SRC, C_DST, T_DST>(element.get_predicate(), context).first;
                                                                        for (const auto term : element.get_terms())
                                                                            atom.terms.push_back(merge_d2p(term, context));

                                                                        canonicalize(atom);
                                                                        return context.destination.get_or_create(atom, context.builder.get_buffer());
                                                                    });
}

template<FactKind T_SRC, formalism::datalog::Context C_SRC, Context C_DST, FactKind T_DST>
auto merge_d2p(View<Index<formalism::datalog::GroundAtom<T_SRC>>, C_SRC> element, MergePlanningContext<C_DST>& context)
{
    return with_cache<formalism::datalog::GroundAtom<T_SRC>, GroundAtom<T_DST>>(
        element,
        context.cache,
        [&]()
        {
            auto atom_ptr = context.builder.template get_builder<GroundAtom<T_DST>>();
            auto& atom = *atom_ptr;
            atom.clear();

            atom.predicate = merge_d2p<T_SRC, C_SRC, C_DST, T_DST>(element.get_predicate(), context).first;
            atom.binding = merge_d2p(element.get_binding(), context).first;

            canonicalize(atom);
            return context.destination.get_or_create(atom, context.builder.get_buffer());
        });
}

template<FactKind T_SRC, formalism::datalog::Context C_SRC, Context C_DST, FactKind T_DST>
auto merge_d2p(View<Index<formalism::datalog::Literal<T_SRC>>, C_SRC> element, MergePlanningContext<C_DST>& context)
{
    return with_cache<formalism::datalog::Literal<T_SRC>, Literal<T_DST>>(element,
                                                                          context.cache,
                                                                          [&]()
                                                                          {
                                                                              auto literal_ptr = context.builder.template get_builder<Literal<T_DST>>();
                                                                              auto& literal = *literal_ptr;
                                                                              literal.clear();

                                                                              literal.polarity = element.get_polarity();
                                                                              literal.atom =
                                                                                  merge_d2p<T_SRC, C_SRC, C_DST, T_DST>(element.get_atom(), context).first;

                                                                              canonicalize(literal);
                                                                              return context.destination.get_or_create(literal, context.builder.get_buffer());
                                                                          });
}

template<FactKind T_SRC, formalism::datalog::Context C_SRC, Context C_DST, FactKind T_DST>
auto merge_d2p(View<Index<formalism::datalog::GroundLiteral<T_SRC>>, C_SRC> element, MergePlanningContext<C_DST>& context)
{
    return with_cache<formalism::datalog::GroundLiteral<T_SRC>, GroundLiteral<T_DST>>(
        element,
        context.cache,
        [&]()
        {
            auto literal_ptr = context.builder.template get_builder<GroundLiteral<T_DST>>();
            auto& literal = *literal_ptr;
            literal.clear();

            literal.polarity = element.get_polarity();
            literal.atom = merge_d2p<T_SRC, C_SRC, C_DST, T_DST>(element.get_atom(), context).first;

            canonicalize(literal);
            return context.destination.get_or_create(literal, context.builder.get_buffer());
        });
}

// Numeric

template<FactKind T, formalism::datalog::Context C_SRC, Context C_DST>
auto merge_d2p(View<Index<formalism::Function<T>>, C_SRC> element, MergePlanningContext<C_DST>& context)
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

template<FactKind T, formalism::datalog::Context C_SRC, Context C_DST>
auto merge_d2p(View<Index<formalism::datalog::FunctionTerm<T>>, C_SRC> element, MergePlanningContext<C_DST>& context)
{
    return with_cache<formalism::datalog::FunctionTerm<T>, FunctionTerm<T>>(element,
                                                                            context.cache,
                                                                            [&]()
                                                                            {
                                                                                auto fterm_ptr = context.builder.template get_builder<FunctionTerm<T>>();
                                                                                auto& fterm = *fterm_ptr;
                                                                                fterm.clear();

                                                                                fterm.function = element.get_function().get_index();
                                                                                for (const auto term : element.get_terms())
                                                                                    fterm.terms.push_back(merge_d2p(term, context));

                                                                                canonicalize(fterm);
                                                                                return context.destination.get_or_create(fterm, context.builder.get_buffer());
                                                                            });
}

template<FactKind T, formalism::datalog::Context C_SRC, Context C_DST>
auto merge_d2p(View<Index<formalism::datalog::GroundFunctionTerm<T>>, C_SRC> element, MergePlanningContext<C_DST>& context)
{
    return with_cache<formalism::datalog::GroundFunctionTerm<T>, GroundFunctionTerm<T>>(
        element,
        context.cache,
        [&]()
        {
            auto fterm_ptr = context.builder.template get_builder<GroundFunctionTerm<T>>();
            auto& fterm = *fterm_ptr;
            fterm.clear();

            fterm.function = element.get_function().get_index();
            fterm.binding = merge_d2p(element.get_binding(), context).first;

            canonicalize(fterm);
            return context.destination.get_or_create(fterm, context.builder.get_buffer());
        });
}

template<FactKind T, formalism::datalog::Context C_SRC, Context C_DST>
auto merge_d2p(View<Index<formalism::datalog::GroundFunctionTermValue<T>>, C_SRC> element, MergePlanningContext<C_DST>& context)
{
    return with_cache<formalism::datalog::GroundFunctionTermValue<T>, GroundFunctionTermValue<T>>(
        element,
        context.cache,
        [&]()
        {
            auto fterm_value_ptr = context.builder.template get_builder<GroundFunctionTermValue<T>>();
            auto& fterm_value = *fterm_value_ptr;
            fterm_value.clear();

            fterm_value.fterm = merge_d2p(element.get_fterm(), context).first;
            fterm_value.value = element.get_value();

            canonicalize(fterm_value);
            return context.destination.get_or_create(fterm_value, context.builder.get_buffer());
        });
}

template<formalism::datalog::Context C_SRC, Context C_DST>
auto merge_d2p(View<Data<formalism::datalog::FunctionExpression>, C_SRC> element, MergePlanningContext<C_DST>& context)
{
    return visit(
        [&](auto&& arg)
        {
            using Alternative = std::decay_t<decltype(arg)>;

            if constexpr (std::is_same_v<Alternative, float_t>)
                return Data<FunctionExpression>(arg);
            else if constexpr (std::is_same_v<Alternative, View<Data<ArithmeticOperator<Data<FunctionExpression>>>, C_SRC>>)
                return Data<FunctionExpression>(merge_d2p(arg, context));
            else
                return Data<FunctionExpression>(merge_d2p(arg, context).first);
        },
        element.get_variant());
}

template<formalism::datalog::Context C_SRC, Context C_DST>
auto merge_d2p(View<Data<formalism::datalog::GroundFunctionExpression>, C_SRC> element, MergePlanningContext<C_DST>& context)
{
    return visit(
        [&](auto&& arg)
        {
            using Alternative = std::decay_t<decltype(arg)>;

            if constexpr (std::is_same_v<Alternative, float_t>)
                return Data<GroundFunctionExpression>(arg);
            else if constexpr (std::is_same_v<Alternative, View<Data<ArithmeticOperator<Data<GroundFunctionExpression>>>, C_SRC>>)
                return Data<GroundFunctionExpression>(merge_d2p(arg, context));
            else
                return Data<GroundFunctionExpression>(merge_d2p(arg, context).first);
        },
        element.get_variant());
}

template<OpKind O, typename T, formalism::datalog::Context C_SRC, Context C_DST>
auto merge_d2p(View<Index<formalism::datalog::UnaryOperator<O, T>>, C_SRC> element, MergePlanningContext<C_DST>& context)
{
    using T_DST = to_planning_payload_t<T>;

    return with_cache<formalism::datalog::UnaryOperator<O, T>, UnaryOperator<O, T_DST>>(
        element,
        context.cache,
        [&]()
        {
            auto unary_ptr = context.builder.template get_builder<UnaryOperator<O, T_DST>>();
            auto& unary = *unary_ptr;
            unary.clear();

            unary.arg = merge_d2p(element.get_arg(), context);

            canonicalize(unary);
            return context.destination.get_or_create(unary, context.builder.get_buffer());
        });
}

template<OpKind O, typename T, formalism::datalog::Context C_SRC, Context C_DST>
auto merge_d2p(View<Index<formalism::datalog::BinaryOperator<O, T>>, C_SRC> element, MergePlanningContext<C_DST>& context)
{
    using T_DST = to_planning_payload_t<T>;

    return with_cache<formalism::datalog::BinaryOperator<O, T>, BinaryOperator<O, T_DST>>(
        element,
        context.cache,
        [&]()
        {
            auto binary_ptr = context.builder.template get_builder<BinaryOperator<O, T_DST>>();
            auto& binary = *binary_ptr;
            binary.clear();

            binary.lhs = merge_d2p(element.get_lhs(), context);
            binary.rhs = merge_d2p(element.get_rhs(), context);

            canonicalize(binary);
            return context.destination.get_or_create(binary, context.builder.get_buffer());
        });
}

template<OpKind O, typename T, formalism::datalog::Context C_SRC, Context C_DST>
auto merge_d2p(View<Index<formalism::datalog::MultiOperator<O, T>>, C_SRC> element, MergePlanningContext<C_DST>& context)
{
    using T_DST = to_planning_payload_t<T>;

    return with_cache<formalism::datalog::MultiOperator<O, T>, MultiOperator<O, T_DST>>(
        element,
        context.cache,
        [&]()
        {
            auto multi_ptr = context.builder.template get_builder<MultiOperator<O, T_DST>>();
            auto& multi = *multi_ptr;
            multi.clear();

            for (const auto arg : element.get_args())
                multi.args.push_back(merge_d2p(arg, context));

            canonicalize(multi);
            return context.destination.get_or_create(multi, context.builder.get_buffer());
        });
}

template<typename T, formalism::datalog::Context C_SRC, Context C_DST>
auto merge_d2p(View<Data<formalism::datalog::ArithmeticOperator<T>>, C_SRC> element, MergePlanningContext<C_DST>& context)
{
    using T_DST = to_planning_payload_t<T>;

    return visit([&](auto&& arg) { return Data<ArithmeticOperator<T_DST>>(merge_d2p(arg, context).first); }, element.get_variant());
}

template<typename T, formalism::datalog::Context C_SRC, Context C_DST>
auto merge_d2p(View<Data<formalism::datalog::BooleanOperator<T>>, C_SRC> element, MergePlanningContext<C_DST>& context)
{
    using T_DST = to_planning_payload_t<T>;

    return visit([&](auto&& arg) { return Data<BooleanOperator<T_DST>>(merge_d2p(arg, context).first); }, element.get_variant());
}

}

#endif