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

#ifndef TYR_FORMALISM_BINDING_VIEW_HPP_
#define TYR_FORMALISM_BINDING_VIEW_HPP_

#include "tyr/common/types.hpp"
#include "tyr/common/vector.hpp"
#include "tyr/formalism/binding_index.hpp"
#include "tyr/formalism/declarations.hpp"
#include "tyr/formalism/function_index.hpp"
#include "tyr/formalism/object_index.hpp"
#include "tyr/formalism/predicate_index.hpp"

namespace tyr
{
template<typename C>
class View<Index<formalism::Binding>, C>
{
private:
    const C* m_context;
    Index<formalism::Binding> m_handle;

public:
    View(Index<formalism::Binding> handle, const C& context) noexcept : m_context(&context), m_handle(handle) {}

    const auto& get_data() const noexcept { return get_repository(*m_context)[m_handle]; }
    const auto& get_context() const noexcept { return *m_context; }
    const auto& get_handle() const noexcept { return m_handle; }

    auto get_index() const noexcept { return m_handle; }
    auto get_objects() const noexcept { return make_view(get_data().objects, *m_context); }

    auto identifying_members() const noexcept { return std::tie(m_handle, m_context); }
};

template<IndexConcept I, typename C>
class View<std::pair<I, Index<formalism::Binding>>, C>
{
private:
    const C* m_context;
    std::pair<I, Index<formalism::Binding>> m_handle;

public:
    View(std::pair<I, Index<formalism::Binding>> handle, const C& context) noexcept : m_context(&context), m_handle(handle) {}

    // This will return an ArrayView aleady
    auto get_data() const noexcept { return get_repository(*m_context)[m_handle]; }
    const auto& get_context() const noexcept { return *m_context; }
    const auto& get_handle() const noexcept { return m_handle; }

    auto get_index() const noexcept { return m_handle; }
    auto get_relation() const noexcept { return make_view(m_handle.first, *m_context); }
    auto get_objects() const noexcept { return make_view(get_data(), *m_context); }

    auto identifying_members() const noexcept { return std::tie(m_handle, m_context); }
};

}

#endif
