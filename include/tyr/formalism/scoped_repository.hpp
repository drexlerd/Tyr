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

#ifndef TYR_FORMALISM_SCOPED_REPOSITORY_HPP_
#define TYR_FORMALISM_SCOPED_REPOSITORY_HPP_

// Include specialization headers first
#include "tyr/formalism/data_traits.hpp"
#include "tyr/formalism/index_traits.hpp"
#include "tyr/formalism/proxy_traits.hpp"
//
#include "tyr/formalism/declarations.hpp"
#include "tyr/formalism/repository.hpp"

namespace tyr::formalism
{

class ScopedRepository
{
private:
    const Repository& global;
    Repository& local;

public:
    ScopedRepository(const Repository& global, Repository& local) : global(global), local(local) {}

    // nullptr signals that the object does not exist.
    template<IsGroupRepository T>
    const T* find(const T& builder) const
    {
        if (auto ptr = global.find(builder))
            return ptr;

        return local.find(builder);
    }

    // nullptr signals that the object does not exist.
    template<IsFlatRepository T>
    const T* find(const T& builder) const
    {
        if (auto ptr = global.find(builder))
            return ptr;

        return local.find(builder);
    }

    // const T* always points to a valid instantiation of the class.
    // We return const T* here to avoid bugs when using structured bindings.
    template<IsGroupRepository T, bool AssignIndex = true>
    std::pair<const T*, bool> get_or_create(T& builder, cista::Buffer& buf)
    {
        if (auto ptr = global.find(builder))
            return std::make_pair(ptr, false);

        // Manually assign index to continue indexing.
        builder.index.value = global.size(builder.index) + local.size(builder.index);

        return local.get_or_create<T, false>(builder, buf);
    }

    // const T* always points to a valid instantiation of the class.
    // We return const T* here to avoid bugs when using structured bindings.
    template<IsFlatRepository T, bool AssignIndex = true>
    std::pair<const T*, bool> get_or_create(T& builder, cista::Buffer& buf)
    {
        if (auto ptr = global.find(builder))
            return std::make_pair(ptr, false);

        // Manually assign index to continue indexing.
        builder.index.value = global.size() + local.size();

        return local.get_or_create<T, false>(builder, buf);
    }

    template<IsIndexType T>
        requires IsGroupRepository<typename IndexTraits<T>::DataType>
    const typename IndexTraits<T>::DataType& operator[](T index) const
    {
        if (index.value < global.size(index))
            return global[index];

        return local[index];
    }

    template<IsIndexType T>
        requires IsFlatRepository<typename IndexTraits<T>::DataType>
    const typename IndexTraits<T>::DataType& operator[](T index) const
    {
        if (index.value < global.size())
            return global[index];

        return local[index];
    }
};

static_assert(IsRepository<ScopedRepository>);
static_assert(IsContext<ScopedRepository>);

}

#endif
