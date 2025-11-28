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

#ifndef TYR_FORMALISM_TERM_VIEW_HPP_
#define TYR_FORMALISM_TERM_VIEW_HPP_

#include "tyr/common/types.hpp"
#include "tyr/common/variant.hpp"
#include "tyr/formalism/repository.hpp"
#include "tyr/formalism/term_data.hpp"

namespace tyr
{
template<formalism::IsContext C>
class View<Data<formalism::Term>, C>
{
private:
    const C* m_context;
    Data<formalism::Term> m_data;

public:
    using Tag = formalism::Term;

    auto get() const { return View<typename Data<formalism::Term>::Variant, C>(m_data.value, *m_context); }
    const auto& get_context() const noexcept { return *m_context; }
    const auto& get_data() const noexcept { return m_data; }

    View(Data<formalism::Term> data, const C& context) : m_context(&context), m_data(data) {}

    auto identifying_members() const noexcept { return std::tie(m_context, m_data); }
};
}

#endif
