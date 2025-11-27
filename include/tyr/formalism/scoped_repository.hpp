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
    std::optional<View<Index<T>, ScopedRepository<C>>> find(const Data<T>& builder) const
    {
        if (auto ptr = global.find(builder))
            return View<Index<T>, ScopedRepository<C>>(ptr->get_data(), *this);

        if (auto ptr = local.find(builder))
            return View<Index<T>, ScopedRepository<C>>(ptr->get_data(), *this);

        return std::nullopt;
    }

    template<IsFlatType T>
    std::optional<View<Index<T>, ScopedRepository<C>>> find(const Data<T>& builder) const
    {
        if (auto ptr = global.find(builder))
            return View<Index<T>, ScopedRepository<C>>(ptr->get_data(), *this);

        if (auto ptr = local.find(builder))
            return View<Index<T>, ScopedRepository<C>>(ptr->get_data(), *this);

        return std::nullopt;
    }

    template<IsGroupType T, bool AssignIndex = true>
    std::pair<View<Index<T>, ScopedRepository<C>>, bool> get_or_create(Data<T>& builder, cista::Buffer& buf)
    {
        if (auto ptr = global.find(builder))
            return std::make_pair(View<Index<T>, ScopedRepository<C>>(ptr->get_data(), *this), false);

        // Manually assign index to continue indexing.
        builder.index.value = global.size(builder.index) + local.size(builder.index);

        return std::make_pair(View<Index<T>, ScopedRepository<C>>(local.template get_or_create<T, false>(builder, buf).first.get_data(), *this), true);
    }

    template<IsFlatType T, bool AssignIndex = true>
    std::pair<View<Index<T>, ScopedRepository<C>>, bool> get_or_create(Data<T>& builder, cista::Buffer& buf)
    {
        if (auto ptr = global.find(builder))
            return std::make_pair(View<Index<T>, ScopedRepository<C>>(ptr->get_data(), *this), false);

        // Manually assign index to continue indexing.
        builder.index.value = global.template size<T>() + local.template size<T>();

        return std::make_pair(View<Index<T>, ScopedRepository<C>>(local.template get_or_create<T, false>(builder, buf).first.get_data(), *this), true);
    }

    template<IsGroupType T>
    const Data<T>& operator[](Index<T> index) const
    {
        const auto global_size = global.size(index);

        if (index.value < global_size)
            return global[index];

        // Subtract global size to get position in local storage
        index.value -= global_size;
        return local[index];
    }

    template<IsFlatType T>
    const Data<T>& operator[](Index<T> index) const
    {
        const auto global_size = global.template size<T>();

        if (index.value < global_size)
            return global[index];

        // Subtract global size to get position in local storage
        index.value -= global_size;
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
