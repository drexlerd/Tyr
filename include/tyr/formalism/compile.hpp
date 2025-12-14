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

#ifndef TYR_FORMALISM_COMPILE_HPP_
#define TYR_FORMALISM_COMPILE_HPP_

#include "tyr/common/tuple.hpp"
#include "tyr/formalism/builder.hpp"
#include "tyr/formalism/canonicalization.hpp"
#include "tyr/formalism/declarations.hpp"
#include "tyr/formalism/merge.hpp"

namespace tyr::formalism
{
template<Context C_SRC, Context C_DST>
class CompileCache
{
private:
    template<typename T_SRC, typename T_DST>
    struct MapEntryType
    {
        using value_type = std::pair<T_SRC, T_DST>;
        using container_type = UnorderedMap<View<Index<T_SRC>, C_SRC>, View<Index<T_DST>, C_DST>>;

        container_type container;
    };

    using CompileStorage = std::tuple<MapEntryType<Predicate<DerivedTag>, Predicate<FluentTag>>,
                                      MapEntryType<Atom<DerivedTag>, Atom<FluentTag>>,
                                      MapEntryType<GroundAtom<DerivedTag>, GroundAtom<FluentTag>>,
                                      MapEntryType<Literal<DerivedTag>, Literal<FluentTag>>,
                                      MapEntryType<GroundLiteral<DerivedTag>, GroundLiteral<FluentTag>>,
                                      MapEntryType<Function<AuxiliaryTag>, Function<FluentTag>>,
                                      MapEntryType<FunctionTerm<AuxiliaryTag>, FunctionTerm<FluentTag>>,
                                      MapEntryType<GroundFunctionTerm<AuxiliaryTag>, GroundFunctionTerm<FluentTag>>,
                                      MapEntryType<GroundFunctionTermValue<AuxiliaryTag>, GroundFunctionTermValue<FluentTag>>,
                                      MapEntryType<Predicate<FluentTag>, Predicate<DerivedTag>>,
                                      MapEntryType<Atom<FluentTag>, Atom<DerivedTag>>,
                                      MapEntryType<GroundAtom<FluentTag>, GroundAtom<DerivedTag>>,
                                      MapEntryType<Literal<FluentTag>, Literal<DerivedTag>>,
                                      MapEntryType<GroundLiteral<FluentTag>, GroundLiteral<DerivedTag>>,
                                      MapEntryType<Function<FluentTag>, Function<AuxiliaryTag>>,
                                      MapEntryType<FunctionTerm<FluentTag>, FunctionTerm<AuxiliaryTag>>,
                                      MapEntryType<GroundFunctionTerm<FluentTag>, GroundFunctionTerm<AuxiliaryTag>>,
                                      MapEntryType<GroundFunctionTermValue<FluentTag>, GroundFunctionTermValue<AuxiliaryTag>>>;

    CompileStorage m_maps;

public:
    CompileCache() = default;

    template<typename T_SRC, typename T_DST>
    auto& get() noexcept
    {
        using Key = std::pair<T_SRC, T_DST>;
        return get_container<Key>(m_maps);
    }

    template<typename T_SRC, typename T_DST>
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

/**
 * Forward declarations
 */

template<FactKind T_SRC, FactKind T_DST, Context C_SRC, Context C_DST>
auto compile(View<Index<Predicate<T_SRC>>, C_SRC> element,
             Builder& builder,
             C_DST& destination,
             CompileCache<C_SRC, C_DST>& compile_cache,
             MergeCache<C_SRC, C_DST>& merge_cache);

template<FactKind T_SRC, FactKind T_DST, Context C_SRC, Context C_DST>
auto compile(View<Index<Atom<T_SRC>>, C_SRC> element,
             Builder& builder,
             C_DST& destination,
             CompileCache<C_SRC, C_DST>& compile_cache,
             MergeCache<C_SRC, C_DST>& merge_cache);

template<FactKind T_SRC, FactKind T_DST, Context C_SRC, Context C_DST>
auto compile(View<Index<GroundAtom<T_SRC>>, C_SRC> element,
             Builder& builder,
             C_DST& destination,
             CompileCache<C_SRC, C_DST>& compile_cache,
             MergeCache<C_SRC, C_DST>& merge_cache);

template<FactKind T_SRC, FactKind T_DST, Context C_SRC, Context C_DST>
auto compile(View<Index<Literal<T_SRC>>, C_SRC> element,
             Builder& builder,
             C_DST& destination,
             CompileCache<C_SRC, C_DST>& compile_cache,
             MergeCache<C_SRC, C_DST>& merge_cache);

template<FactKind T_SRC, FactKind T_DST, Context C_SRC, Context C_DST>
auto compile(View<Index<GroundLiteral<T_SRC>>, C_SRC> element,
             Builder& builder,
             C_DST& destination,
             CompileCache<C_SRC, C_DST>& compile_cache,
             MergeCache<C_SRC, C_DST>& merge_cache);

template<FactKind T_SRC, FactKind T_DST, Context C_SRC, Context C_DST>
auto compile(View<Index<Function<T_SRC>>, C_SRC> element,
             Builder& builder,
             C_DST& destination,
             CompileCache<C_SRC, C_DST>& compile_cache,
             MergeCache<C_SRC, C_DST>& merge_cache);

template<FactKind T_SRC, FactKind T_DST, Context C_SRC, Context C_DST>
auto compile(View<Index<FunctionTerm<T_SRC>>, C_SRC> element,
             Builder& builder,
             C_DST& destination,
             CompileCache<C_SRC, C_DST>& compile_cache,
             MergeCache<C_SRC, C_DST>& merge_cache);

template<FactKind T_SRC, FactKind T_DST, Context C_SRC, Context C_DST>
auto compile(View<Index<GroundFunctionTerm<T_SRC>>, C_SRC> element,
             Builder& builder,
             C_DST& destination,
             CompileCache<C_SRC, C_DST>& compile_cache,
             MergeCache<C_SRC, C_DST>& merge_cache);

template<FactKind T_SRC, FactKind T_DST, Context C_SRC, Context C_DST>
auto compile(View<Index<GroundFunctionTermValue<T_SRC>>, C_SRC> element,
             Builder& builder,
             C_DST& destination,
             CompileCache<C_SRC, C_DST>& compile_cache,
             MergeCache<C_SRC, C_DST>& merge_cache);

/**
 * Implementations
 */

template<typename T_SRC, typename T_DST, Context C_SRC, Context C_DST, typename F>
auto with_cache(View<Index<T_SRC>, C_SRC> element, CompileCache<C_SRC, C_DST>& cache, F&& compute)
{
    auto& m = cache.template get<T_SRC, T_DST>();

    if (auto it = m.find(element); it != m.end())
        return it->second;

    auto result = compute();  // compute the merged element

    m.emplace(element, result);

    return result;
}

template<FactKind T_SRC, FactKind T_DST, Context C_SRC, Context C_DST>
auto compile(View<Index<Predicate<T_SRC>>, C_SRC> element,
             Builder& builder,
             C_DST& destination,
             CompileCache<C_SRC, C_DST>& compile_cache,
             MergeCache<C_SRC, C_DST>& merge_cache)
{
    return with_cache<Predicate<T_SRC>, Predicate<T_DST>>(element,
                                                          compile_cache,
                                                          [&]()
                                                          {
                                                              auto predicate_ptr = builder.template get_builder<formalism::Predicate<T_DST>>();
                                                              auto& predicate = *predicate_ptr;
                                                              predicate.clear();

                                                              predicate.name = element.get_name();
                                                              predicate.arity = element.get_arity();

                                                              canonicalize(predicate);
                                                              return destination.get_or_create(predicate, builder.get_buffer()).first;
                                                          });
}

template<FactKind T_SRC, FactKind T_DST, Context C_SRC, Context C_DST>
auto compile(View<Index<Atom<T_SRC>>, C_SRC> element,
             Builder& builder,
             C_DST& destination,
             CompileCache<C_SRC, C_DST>& compile_cache,
             MergeCache<C_SRC, C_DST>& merge_cache)
{
    return with_cache<Atom<T_SRC>, Atom<T_DST>>(
        element,
        compile_cache,
        [&]()
        {
            auto atom_ptr = builder.template get_builder<formalism::Atom<T_DST>>();
            auto& atom = *atom_ptr;
            atom.clear();

            atom.predicate = compile<T_SRC, T_DST>(element.get_predicate(), builder, destination, compile_cache, merge_cache).get_index();
            for (const auto term : element.get_terms())
                atom.terms.push_back(merge(term, builder, destination, merge_cache).get_data());

            canonicalize(atom);
            return destination.get_or_create(atom, builder.get_buffer()).first;
        });
}

template<FactKind T_SRC, FactKind T_DST, Context C_SRC, Context C_DST>
auto compile(View<Index<GroundAtom<T_SRC>>, C_SRC> element,
             Builder& builder,
             C_DST& destination,
             CompileCache<C_SRC, C_DST>& compile_cache,
             MergeCache<C_SRC, C_DST>& merge_cache)
{
    return with_cache<GroundAtom<T_SRC>, GroundAtom<T_DST>>(
        element,
        compile_cache,
        [&]()
        {
            auto atom_ptr = builder.template get_builder<formalism::GroundAtom<T_DST>>();
            auto& atom = *atom_ptr;
            atom.clear();

            atom.predicate = compile<T_SRC, T_DST>(element.get_predicate(), builder, destination, compile_cache, merge_cache).get_index();
            atom.binding = merge(element.get_binding(), builder, destination, merge_cache).get_index();

            canonicalize(atom);
            return destination.get_or_create(atom, builder.get_buffer()).first;
        });
}

template<FactKind T_SRC, FactKind T_DST, Context C_SRC, Context C_DST>
auto compile(View<Index<Literal<T_SRC>>, C_SRC> element,
             Builder& builder,
             C_DST& destination,
             CompileCache<C_SRC, C_DST>& compile_cache,
             MergeCache<C_SRC, C_DST>& merge_cache)
{
    return with_cache<Literal<T_SRC>, Literal<T_DST>>(
        element,
        compile_cache,
        [&]()
        {
            auto literal_ptr = builder.template get_builder<formalism::Literal<T_DST>>();
            auto& literal = *literal_ptr;
            literal.clear();

            literal.polarity = element.get_polarity();
            literal.atom = compile<T_SRC, T_DST>(element.get_atom(), builder, destination, compile_cache, merge_cache).get_index();

            canonicalize(literal);
            return destination.get_or_create(literal, builder.get_buffer()).first;
        });
}

template<FactKind T_SRC, FactKind T_DST, Context C_SRC, Context C_DST>
auto compile(View<Index<GroundLiteral<T_SRC>>, C_SRC> element,
             Builder& builder,
             C_DST& destination,
             CompileCache<C_SRC, C_DST>& compile_cache,
             MergeCache<C_SRC, C_DST>& merge_cache)
{
    return with_cache<GroundLiteral<T_SRC>, GroundLiteral<T_DST>>(
        element,
        compile_cache,
        [&]()
        {
            auto literal_ptr = builder.template get_builder<formalism::GroundLiteral<T_DST>>();
            auto& literal = *literal_ptr;
            literal.clear();

            literal.polarity = element.get_polarity();
            literal.atom = compile<T_SRC, T_DST>(element.get_atom(), builder, destination, compile_cache, merge_cache).get_index();

            canonicalize(literal);
            return destination.get_or_create(literal, builder.get_buffer()).first;
        });
}

template<FactKind T_SRC, FactKind T_DST, Context C_SRC, Context C_DST>
auto compile(View<Index<Function<T_SRC>>, C_SRC> element,
             Builder& builder,
             C_DST& destination,
             CompileCache<C_SRC, C_DST>& compile_cache,
             MergeCache<C_SRC, C_DST>& merge_cache)
{
    return with_cache<Function<T_SRC>, Function<T_DST>>(element,
                                                        compile_cache,
                                                        [&]()
                                                        {
                                                            auto function_ptr = builder.template get_builder<formalism::Function<T_DST>>();
                                                            auto& function = *function_ptr;
                                                            function.clear();

                                                            function.name = element.get_name();
                                                            function.arity = element.get_arity();

                                                            canonicalize(function);
                                                            return destination.get_or_create(function, builder.get_buffer()).first;
                                                        });
}

template<FactKind T_SRC, FactKind T_DST, Context C_SRC, Context C_DST>
auto compile(View<Index<FunctionTerm<T_SRC>>, C_SRC> element,
             Builder& builder,
             C_DST& destination,
             CompileCache<C_SRC, C_DST>& compile_cache,
             MergeCache<C_SRC, C_DST>& merge_cache)
{
    return with_cache<FunctionTerm<T_SRC>, FunctionTerm<T_DST>>(
        element,
        compile_cache,
        [&]()
        {
            auto fterm_ptr = builder.template get_builder<formalism::FunctionTerm<T_DST>>();
            auto& fterm = *fterm_ptr;
            fterm.clear();

            fterm.function = compile<T_SRC, T_DST>(element.get_function(), builder, destination, compile_cache, merge_cache).get_index();
            for (const auto object : element.get_objects())
                fterm.objects.push_back(merge(object, builder, destination, merge_cache).get_index());

            canonicalize(fterm);
            return destination.get_or_create(fterm, builder.get_buffer()).first;
        });
}

template<FactKind T_SRC, FactKind T_DST, Context C_SRC, Context C_DST>
auto compile(View<Index<GroundFunctionTerm<T_SRC>>, C_SRC> element,
             Builder& builder,
             C_DST& destination,
             CompileCache<C_SRC, C_DST>& compile_cache,
             MergeCache<C_SRC, C_DST>& merge_cache)
{
    return with_cache<GroundFunctionTerm<T_SRC>, GroundFunctionTerm<T_DST>>(
        element,
        compile_cache,
        [&]()
        {
            auto fterm_ptr = builder.template get_builder<formalism::GroundFunctionTerm<T_DST>>();
            auto& fterm = *fterm_ptr;
            fterm.clear();

            fterm.function = compile<T_SRC, T_DST>(element.get_function(), builder, destination, compile_cache, merge_cache).get_index();
            fterm.binding = merge(element.get_binding(), builder, destination, merge_cache).get_index();

            canonicalize(fterm);
            return destination.get_or_create(fterm, builder.get_buffer()).first;
        });
}

template<FactKind T_SRC, FactKind T_DST, Context C_SRC, Context C_DST>
auto compile(View<Index<GroundFunctionTermValue<T_SRC>>, C_SRC> element,
             Builder& builder,
             C_DST& destination,
             CompileCache<C_SRC, C_DST>& compile_cache,
             MergeCache<C_SRC, C_DST>& merge_cache)
{
    return with_cache<GroundFunctionTermValue<T_SRC>, GroundFunctionTermValue<T_DST>>(
        element,
        compile_cache,
        [&]()
        {
            auto fterm_value_ptr = builder.template get_builder<formalism::GroundFunctionTermValue<T_DST>>();
            auto& fterm_value = *fterm_value_ptr;
            fterm_value.clear();

            fterm_value.fterm = compile<T_SRC, T_DST>(element.get_fterm(), builder, destination, compile_cache, merge_cache).get_index();
            fterm_value.value = element.get_value();

            canonicalize(fterm_value);
            return destination.get_or_create(fterm_value, builder.get_buffer()).first;
        });
}

}

#endif