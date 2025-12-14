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

#ifndef TYR_FORMALISM_PLANNING_FDR_FACT_VIEW_HPP_
#define TYR_FORMALISM_PLANNING_FDR_FACT_VIEW_HPP_

#include "tyr/common/types.hpp"
#include "tyr/formalism/declarations.hpp"
#include "tyr/formalism/object_index.hpp"
#include "tyr/formalism/planning/fdr_value.hpp"

namespace tyr
{
template<formalism::FactKind T, formalism::Context C>
class View<Data<formalism::FDRFact<T>>, C>
{
private:
    const C* m_context;
    Data<formalism::FDRFact<T>> m_handle;

public:
    using Tag = formalism::FDRFact<T>;

    View(Data<formalism::FDRFact<T>> handle, const C& context) noexcept : m_context(&context), m_handle(handle) {}

    const auto& get_data() const noexcept { return m_handle; }
    const auto& get_context() const noexcept { return *m_context; }
    const auto& get_handle() const noexcept { return m_handle; }

    auto get_variable() const noexcept { return make_view(get_data().variable, *m_context); }
    auto get_value() const noexcept { return get_data().value; }
    auto get_atom() const noexcept
    {
        assert(has_value());
        return get_variable().get_atoms()[uint_t(get_value() - 1)];
    }
    auto has_value() const noexcept { return get_value() != formalism::FDRValue::none(); }

    auto identifying_members() const noexcept { return std::tie(m_context, m_handle); }
};
}

#endif
