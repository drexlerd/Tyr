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

#ifndef TYR_FORMALISM_OVERLAY_REPOSITORY_HPP_
#define TYR_FORMALISM_OVERLAY_REPOSITORY_HPP_

#include "tyr/buffer/declarations.hpp"
#include "tyr/common/index_mixins.hpp"
#include "tyr/common/types.hpp"

namespace tyr::formalism
{
template<typename C>
class OverlayRepository
{
private:
    const C& parent_scope;
    C& local_scope;

public:
    OverlayRepository(const C& parent_scope, C& local_scope) : parent_scope(parent_scope), local_scope(local_scope) {}

    template<typename T>
    auto find(const Data<T>& builder) const noexcept
    {
        if (auto ptr = parent_scope.find(builder))
            return ptr;

        if (auto ptr = local_scope.find(builder))
            return ptr;

        return std::nullopt;
    }

    template<typename T, bool AssignIndex = true>
        requires(IndexConcept<Index<T>> && !GroupIndexConcept<Index<T>>)
    std::pair<Index<T>, bool> get_or_create(Data<T>& builder, buffer::Buffer& buf)
    {
        if (auto ptr = parent_scope.find(builder))
            return std::make_pair(*ptr, false);

        // Manually assign index to continue indexing.
        builder.index.value = parent_scope.template size<T>() + local_scope.template size<T>();

        return std::make_pair(local_scope.template get_or_create<T, false>(builder, buf).first, true);
    }

    template<typename T, bool AssignIndex = true>
        requires(GroupIndexConcept<Index<T>>)
    std::pair<Index<T>, bool> get_or_create(Data<T>& builder, buffer::Buffer& buf)
    {
        if (auto ptr = parent_scope.find(builder))
            return std::make_pair(*ptr, false);

        // Manually assign index to continue indexing.
        builder.index.value = parent_scope.template size<T>(builder.index) + local_scope.template size<T>(builder.index);

        return std::make_pair(local_scope.template get_or_create<T, false>(builder, buf).first, true);
    }

    template<typename T>
        requires(IndexConcept<Index<T>> && !GroupIndexConcept<Index<T>>)
    const Data<T>& operator[](Index<T> index) const noexcept
    {
        assert(index != Index<T>::max() && "Unassigned index.");

        // Guard against accidental overlap from incorrect merging.
        assert(local_scope.template size<T>() == 0 || local_scope.template front<T>().index.value >= parent_scope.template size<T>());

        const auto parent_scope_size = parent_scope.template size<T>();

        if (index.value < parent_scope_size)
            return parent_scope[index];

        // Subtract parent_scope size to get position in local_scope storage
        index.value -= parent_scope_size;
        return local_scope[index];
    }

    template<typename T>
        requires(GroupIndexConcept<Index<T>>)
    const Data<T>& operator[](Index<T> index) const noexcept
    {
        assert(index != Index<T>::max() && "Unassigned index.");

        // Guard against accidental overlap from incorrect merging.
        assert(local_scope.template size<T>(index) == 0 || local_scope.template front<T>(index).index.value >= parent_scope.template size<T>(index));

        const auto parent_scope_size = parent_scope.template size<T>(index);

        if (index.value < parent_scope_size)
            return parent_scope[index];

        // Subtract parent_scope size to get position in local_scope storage
        index.value -= parent_scope_size;
        return local_scope[index];
    }

    template<typename T>
    size_t size() const noexcept
    {
        return parent_scope.template size<T>() + local_scope.template size<T>();
    }

    template<typename T>
        requires(GroupIndexConcept<Index<T>>)
    size_t size(Index<T> index) const noexcept
    {
        return parent_scope.template size<T>(index) + local_scope.template size<T>(index);
    }

    const C& get_parent_scope() const noexcept { return parent_scope; }
    const C& get_local_scope() const noexcept { return local_scope; }
};

}

#endif
