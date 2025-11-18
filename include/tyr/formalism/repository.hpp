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

#ifndef TYR_FORMALISM_REPOSITORY_HPP_
#define TYR_FORMALISM_REPOSITORY_HPP_

#include "tyr/cista/declarations.hpp"
#include "tyr/cista/indexed_hash_set.hpp"
#include "tyr/formalism/atom.hpp"
#include "tyr/formalism/declarations.hpp"
#include "tyr/formalism/function.hpp"
#include "tyr/formalism/function_term.hpp"
#include "tyr/formalism/ground_atom.hpp"
#include "tyr/formalism/ground_literal.hpp"
#include "tyr/formalism/ground_rule.hpp"
#include "tyr/formalism/literal.hpp"
#include "tyr/formalism/object.hpp"
#include "tyr/formalism/predicate.hpp"
#include "tyr/formalism/program.hpp"
#include "tyr/formalism/rule.hpp"
#include "tyr/formalism/term.hpp"
#include "tyr/formalism/variable.hpp"

#include <boost/hana.hpp>

namespace tyr::formalism
{

template<typename T>
using MappedType = boost::hana::pair<boost::hana::type<T>, cista::IndexedHashSet<T>>;

template<typename T>
using MappedTypePerIndex = boost::hana::pair<boost::hana::type<T>, cista::IndexedHashSetList<T>>;

template<typename T>
struct TypeProperties
{
    using PairType = MappedType<T>;
};

template<IsStaticOrFluentTag T>
struct TypeProperties<Atom<T>>
{
    using PairType = MappedTypePerIndex<Atom<T>>;
    static auto get_index(AtomIndex<T>& self) noexcept { return self.predicate_index; }
};

template<IsStaticOrFluentTag T>
struct TypeProperties<Literal<T>>
{
    using PairType = MappedTypePerIndex<Literal<T>>;
    static auto get_index(LiteralIndex<T> self) noexcept { return self.predicate_index; }
};

template<IsStaticOrFluentTag T>
struct TypeProperties<GroundAtom<T>>
{
    using PairType = MappedTypePerIndex<GroundAtom<T>>;
    static auto get_index(GroundAtomIndex<T> self) noexcept { return self.predicate_index; }
};

template<IsStaticOrFluentTag T>
struct TypeProperties<GroundLiteral<T>>
{
    using PairType = MappedTypePerIndex<GroundLiteral<T>>;
    static auto get_index(GroundLiteralIndex<T> self) noexcept { return self.predicate_index; }
};

template<>
struct TypeProperties<GroundRule>
{
    using PairType = MappedTypePerIndex<GroundRule>;
    static auto get_index(GroundRuleIndex self) noexcept { return self.rule_index; }
};

template<typename T>
concept IsMappedType = std::same_as<typename TypeProperties<T>::PairType, MappedType<T>>;

template<typename T>
concept IsMappedTypePerIndex = std::same_as<typename TypeProperties<T>::PairType, MappedTypePerIndex<T>>;

class Repository
{
private:
    using HanaRepository = boost::hana::map<TypeProperties<Atom<StaticTag>>::PairType,
                                            TypeProperties<Atom<FluentTag>>::PairType,
                                            TypeProperties<Literal<StaticTag>>::PairType,
                                            TypeProperties<Literal<FluentTag>>::PairType,
                                            TypeProperties<GroundAtom<StaticTag>>::PairType,
                                            TypeProperties<GroundAtom<FluentTag>>::PairType,
                                            TypeProperties<GroundLiteral<StaticTag>>::PairType,
                                            TypeProperties<GroundLiteral<FluentTag>>::PairType,
                                            TypeProperties<Predicate<StaticTag>>::PairType,
                                            TypeProperties<Predicate<FluentTag>>::PairType,
                                            TypeProperties<Function<StaticTag>>::PairType,
                                            TypeProperties<Function<FluentTag>>::PairType,
                                            TypeProperties<FunctionTerm<StaticTag>>::PairType,
                                            TypeProperties<FunctionTerm<FluentTag>>::PairType,
                                            TypeProperties<Variable>::PairType,
                                            TypeProperties<Object>::PairType,
                                            TypeProperties<Rule>::PairType,
                                            TypeProperties<GroundRule>::PairType,
                                            TypeProperties<Program>::PairType>;

    HanaRepository m_repository;

public:
    Repository() = default;

    template<IsMappedTypePerIndex T>
    auto get_or_create(T& builder, cista::Buffer& buf)
    {
        auto& list = boost::hana::at_key(m_repository, boost::hana::type<T> {});

        const auto i = TypeProperties<T>::get_index(builder.index).get();

        if (i >= list.size())
            list.resize(i + 1);

        auto& repository = list[i];

        return repository.insert(builder, buf);
    }

    template<IsMappedType T>
    auto get_or_create(T& builder, cista::Buffer& buf)
    {
        auto& repository = boost::hana::at_key(m_repository, boost::hana::type<T> {});

        return repository.insert(builder, buf);
    }

    template<IsMappedTypePerIndex T>
    const auto& operator[](typename T::IndexType index) const
    {
        auto& list = boost::hana::at_key(m_repository, boost::hana::type<T> {});

        const auto i = TypeProperties<T>::get_index(index).get();

        assert(i < list.size());

        auto& repository = list[i];

        return *repository[index];
    }

    template<IsMappedType T>
    const auto& operator[](typename T::IndexType index) const
    {
        auto& repository = boost::hana::at_key(m_repository, boost::hana::type<T> {});

        return *repository[index];
    }
};
}

#endif
