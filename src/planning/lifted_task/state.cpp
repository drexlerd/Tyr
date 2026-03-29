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

#include "tyr/planning/lifted_task.hpp"
#include "tyr/planning/lifted_task/state_repository.hpp"
#include "tyr/planning/lifted_task/state_view.hpp"

namespace tyr
{

bool LiftedStateView::test(Index<formalism::planning::GroundAtom<formalism::StaticTag>> index) const { return m_state_repository->get_task()->test(index); }

float_t LiftedStateView::get(Index<formalism::planning::GroundFunctionTerm<formalism::StaticTag>> index) const
{
    return m_state_repository->get_task()->get(index);
}

template<formalism::FactKind T>
const boost::dynamic_bitset<>& LiftedStateView::get_atoms() const noexcept
{
    if constexpr (std::is_same_v<T, formalism::StaticTag>)
        return m_state_repository->get_task()->get_static_atoms_bitset();
    else if constexpr (std::is_same_v<T, formalism::FluentTag> || std::is_same_v<T, formalism::DerivedTag>)
        return m_unpacked->template get_atoms<T>().indices;
    else
        static_assert(dependent_false<T>::value, "Missing case");
}

template const boost::dynamic_bitset<>& LiftedStateView::get_atoms<formalism::StaticTag>() const noexcept;
template const boost::dynamic_bitset<>& LiftedStateView::get_atoms<formalism::FluentTag>() const noexcept;
template const boost::dynamic_bitset<>& LiftedStateView::get_atoms<formalism::DerivedTag>() const noexcept;

template<formalism::FactKind T>
const std::vector<float_t>& LiftedStateView::get_numeric_variables() const noexcept
{
    if constexpr (std::is_same_v<T, formalism::StaticTag>)
        return m_state_repository->get_task()->get_static_numeric_variables();
    else if constexpr (std::is_same_v<T, formalism::FluentTag>)
        return m_unpacked->get_numeric_variables().values;
    else
        static_assert(dependent_false<T>::value, "Missing case");
}

template const std::vector<float_t>& LiftedStateView::get_numeric_variables<formalism::StaticTag>() const noexcept;
template const std::vector<float_t>& LiftedStateView::get_numeric_variables<formalism::FluentTag>() const noexcept;

planning::AtomRange<formalism::StaticTag> LiftedStateView::get_static_atoms() const noexcept
{
    return planning::AtomRange<formalism::StaticTag>(m_state_repository->get_task()->get_static_atoms_bitset());
}

planning::FDRFactRange<planning::LiftedTag, formalism::FluentTag> LiftedStateView::get_fluent_facts() const noexcept
{
    return planning::FDRFactRange<planning::LiftedTag, formalism::FluentTag>(get_atoms<formalism::FluentTag>());
}

planning::AtomRange<formalism::DerivedTag> LiftedStateView::get_derived_atoms() const noexcept
{
    return planning::AtomRange<formalism::DerivedTag>(get_atoms<formalism::DerivedTag>());
}

planning::FunctionTermValueRange<formalism::StaticTag> LiftedStateView::get_static_fterm_values() const noexcept
{
    return planning::FunctionTermValueRange<formalism::StaticTag>(m_state_repository->get_task()->get_static_numeric_variables());
}

planning::FunctionTermValueRange<formalism::FluentTag> LiftedStateView::get_fluent_fterm_values() const noexcept
{
    return planning::FunctionTermValueRange<formalism::FluentTag>(get_numeric_variables<formalism::FluentTag>());
}

const std::shared_ptr<formalism::planning::Repository>& LiftedStateView::get_repository() const noexcept
{
    return m_state_repository->get_task()->get_repository();
}

static_assert(planning::IterableStateConcept<LiftedStateView>);
static_assert(planning::IterableViewStateConcept<LiftedStateView>);
static_assert(planning::IndexableStateConcept<LiftedStateView, planning::LiftedTag>);
static_assert(planning::IndexableViewStateConcept<LiftedStateView, planning::LiftedTag>);

}
