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

#ifndef TYR_COMMON_OPTIONAL_HPP_
#define TYR_COMMON_OPTIONAL_HPP_

#include "tyr/common/declarations.hpp"
#include "tyr/common/types.hpp"
#include "tyr/common/types_utils.hpp"

#include <cista/containers/optional.h>

namespace tyr
{

template<typename C, typename T>
class View<::cista::optional<T>, C>
{
public:
    using Optional = ::cista::optional<T>;

    View(const Optional& handle, const C& context) noexcept : m_context(&context), m_handle(&handle) {}

    const auto& get_data() const noexcept { return *m_handle; }
    const auto& get_context() const noexcept { return *m_context; }
    const auto& get_handle() const noexcept { return m_handle; }

    bool has_value() const noexcept { return m_handle->has_value(); }

    explicit operator bool() const noexcept { return m_handle->has_value(); }

    decltype(auto) value() const
    {
        if constexpr (ViewConcept<T, C>)
            return make_view(**m_handle, *m_context);
        else
            return m_handle->value();
    }

    decltype(auto) operator*() const noexcept { return value(); }

    auto operator->() const
    {
        if constexpr (ViewConcept<T, C>)
            static_assert(!ViewConcept<T, C>,
                          "operator-> is not supported when T is viewable; "
                          "call .value() first to get a make_view.");
        else
            return &m_handle->value();
    }

private:
    const C* m_context;
    const Optional* m_handle;
};

}
#endif
