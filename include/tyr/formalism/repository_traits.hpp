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

#ifndef TYR_FORMALISM_REPOSITORY_TRAITS_HPP_
#define TYR_FORMALISM_REPOSITORY_TRAITS_HPP_

#include "tyr/cista/declarations.hpp"
#include "tyr/formalism/declarations.hpp"

#include <boost/hana.hpp>

namespace tyr::formalism
{

/// @brief `FlatRepositoryEntry` is the mapping from data type to an indexed hash set.
template<typename T>
using FlatRepositoryEntry = boost::hana::pair<boost::hana::type<T>, cista::IndexedHashSet<T>>;

/// @brief `GroupRepositoryEntry` is the mapping from data type to a list of indexed hash sets.
template<typename T>
using GroupRepositoryEntry = boost::hana::pair<boost::hana::type<T>, cista::IndexedHashSetList<T>>;

template<typename T>
struct RepositoryTraits
{
    using EntryType = FlatRepositoryEntry<T>;
};

template<IsStaticOrFluentTag T>
struct RepositoryTraits<Atom<T>>
{
    using EntryType = GroupRepositoryEntry<Atom<T>>;
};

template<IsStaticOrFluentTag T>
struct RepositoryTraits<GroundAtom<T>>
{
    using EntryType = GroupRepositoryEntry<GroundAtom<T>>;
};

template<IsStaticOrFluentTag T>
struct RepositoryTraits<Literal<T>>
{
    using EntryType = GroupRepositoryEntry<Literal<T>>;
};

template<IsStaticOrFluentTag T>
struct RepositoryTraits<FunctionTerm<T>>
{
    using EntryType = GroupRepositoryEntry<FunctionTerm<T>>;
};

template<IsStaticOrFluentTag T>
struct RepositoryTraits<GroundFunctionTerm<T>>
{
    using EntryType = GroupRepositoryEntry<GroundFunctionTerm<T>>;
};

template<IsStaticOrFluentTag T>
struct RepositoryTraits<GroundFunctionTermValue<T>>
{
    using EntryType = GroupRepositoryEntry<GroundFunctionTermValue<T>>;
};

template<>
struct RepositoryTraits<GroundRule>
{
    using EntryType = GroupRepositoryEntry<GroundRule>;
};

/// Checks whether T is defined to use a single indexed hash set.
template<typename T>
concept IsFlatRepository = std::same_as<typename RepositoryTraits<T>::EntryType, FlatRepositoryEntry<T>>;

/// Checks whether T is defined to use a list of indexed hash sets.
template<typename T>
concept IsGroupRepository = std::same_as<typename RepositoryTraits<T>::EntryType, GroupRepositoryEntry<T>>;

}

#endif