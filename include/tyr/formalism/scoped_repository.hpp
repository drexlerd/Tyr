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
    const Repository& parent;
    Repository& local;

public:
    template<IsIndexType T>
        requires IsMappedTypePerIndex<typename IndexTraits<T>::DataType>
    const typename IndexTraits<T>::DataType& operator[](T index) const
    {
    }

    template<IsIndexType T>
        requires IsMappedType<typename IndexTraits<T>::DataType>
    const typename IndexTraits<T>::DataType& operator[](T index) const
    {
    }
};

static_assert(IsRepository<ScopedRepository>);
static_assert(IsContext<ScopedRepository>);

}

#endif
