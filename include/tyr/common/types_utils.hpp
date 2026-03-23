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
 *<
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef TYR_COMMON_TYPES_UTILS_HPP_
#define TYR_COMMON_TYPES_UTILS_HPP_

#include "tyr/common/types.hpp"

namespace tyr
{
template<typename T>
concept Clearable = requires(T& a) { a.clear(); };

template<typename T>
struct is_cista_variant_helper : std::false_type
{
};

template<typename... Ts>
struct is_cista_variant_helper<::cista::offset::variant<Ts...>> : std::true_type
{
};

template<typename T>
concept CistaVariantConcept = is_cista_variant_helper<T>::value;

template<typename T>
struct is_optional_variant_helper : std::false_type
{
};

template<typename T>
struct is_optional_variant_helper<::cista::optional<T>> : std::true_type
{
};

template<typename T>
concept CistaOptionalConcept = is_optional_variant_helper<std::remove_cvref_t<T>>::value;

template<typename T>
void clear(T& element)
{
    if constexpr (Clearable<T>)
        element.clear();
    else if constexpr (CistaVariantConcept<T>)
    {
        static_assert(std::is_default_constructible_v<T>, "Cannot clear variant: not default constructible.");
        // hard reset to default alternative.
        element.destruct();
        new (&element) T {};
    }
    else if constexpr (CistaOptionalConcept<T>)
        element = std::nullopt;
    else if constexpr (std::is_default_constructible_v<T> && std::is_assignable_v<T&, T&&>)
        element = T {};
    else if constexpr (std::is_default_constructible_v<T> && std::is_move_constructible_v<T> && std::is_swappable_v<T>)
    {
        T tmp {};
        using std::swap;
        swap(element, tmp);
    }
    else
        static_assert(dependent_false<T>::value, "Cannot clear T.");
}

template<typename T, typename C>
void append(View<Index<T>, C> view, IndexList<T>& ref_list)
{
    ref_list.push_back(view.get_index());
}

template<typename T, typename C>
void append(View<Data<T>, C> view, DataList<T>& ref_list)
{
    ref_list.push_back(view.get_data());
}

template<typename T, typename C>
void extend(const std::vector<View<Index<T>, C>>& views, IndexList<T>& ref_list)
{
    for (const auto& view : views)
        append(view, ref_list);
}

template<typename T, typename C>
void extend(const std::vector<View<Data<T>, C>>& views, DataList<T>& ref_list)
{
    for (const auto& view : views)
        append(view, ref_list);
}

template<typename T, typename C>
void set(View<Index<T>, C> view, Index<T>& index)
{
    index = view.get_index();
}

template<typename T, typename C>
void set(View<Data<T>, C> view, Data<T>& data)
{
    data = view.get_data();
}

template<typename T, typename C>
void set(const std::optional<View<Index<T>, C>>& view, ::cista::optional<Index<T>>& data)
{
    if (view)
    {
        data = view->get_index();
    }
    else
    {
        data = std::nullopt;
    }
}

template<typename T, typename C>
void set(const std::optional<View<Data<T>, C>>& view, ::cista::optional<Data<T>>& data)
{
    if (view)
    {
        data = view->get_data();
    }
    else
    {
        data = std::nullopt;
    }
}

template<typename T, typename C>
void set(const std::vector<View<Index<T>, C>>& views, IndexList<T>& out_list)
{
    out_list.clear();
    out_list.reserve(views.size());
    extend(views, out_list);
}

template<typename T, typename C>
void set(const std::vector<View<Data<T>, C>>& views, DataList<T>& out_list)
{
    out_list.clear();
    out_list.reserve(views.size());
    extend(views, out_list);
}

}

#endif