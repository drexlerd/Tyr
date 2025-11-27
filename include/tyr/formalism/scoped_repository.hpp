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

#include "tyr/formalism/declarations.hpp"
#include "tyr/formalism/repository.hpp"

namespace tyr::formalism
{
template<IsContext C>
class ScopedRepository
{
private:
    const C& global;
    C& local;

public:
    ScopedRepository(const C& global, C& local) : global(global), local(local) {}

    template<IsGroupType T>
    std::optional<View<Index<T>, Repository>> find(const Data<T>& builder) const
    {
        if (auto ptr = global.find(builder))
            return ptr;

        return local.find(builder);
    }

    template<IsFlatType T>
    std::optional<View<Index<T>, Repository>> find(const Data<T>& builder) const
    {
        if (auto ptr = global.find(builder))
            return ptr;

        return local.find(builder);
    }

    template<IsGroupType T, bool AssignIndex = true>
    std::pair<View<Index<T>, Repository>, bool> get_or_create(Data<T>& builder, cista::Buffer& buf)
    {
        if (auto ptr = global.find(builder))
            return std::make_pair(ptr, false);

        // Manually assign index to continue indexing.
        builder.index.value = global.size(builder.index) + local.size(builder.index);

        return local.template get_or_create<T, false>(builder, buf);
    }

    template<IsFlatType T, bool AssignIndex = true>
    std::pair<View<Index<T>, Repository>, bool> get_or_create(Data<T>& builder, cista::Buffer& buf)
    {
        if (auto ptr = global.find(builder))
            return std::make_pair(ptr, false);

        // Manually assign index to continue indexing.
        builder.index.value = global.template size<T>() + local.template size<T>();

        return local.template get_or_create<T, false>(builder, buf);
    }

    template<IsGroupType T>
    const Data<T>& operator[](Index<T> index) const
    {
        if (index.value < global.size(index))
            return global[index];

        return local[index];
    }

    template<IsFlatType T>
    const Data<T>& operator[](Index<T> index) const
    {
        if (index.value < global.template size<T>())
            return global[index];

        return local[index];
    }
};

/// @brief Make ScopedRepository a trivial context.
template<IsContext C>
inline const ScopedRepository<C>& get_repository(const ScopedRepository<C>& context) noexcept
{
    return context;
}

static_assert(IsRepository<ScopedRepository<Repository>>);
static_assert(IsContext<ScopedRepository<Repository>>);

}

#endif
