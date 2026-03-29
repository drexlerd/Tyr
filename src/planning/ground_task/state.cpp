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

#include "tyr/planning/ground_task.hpp"
#include "tyr/planning/ground_task/state_repository.hpp"
#include "tyr/planning/ground_task/state_view.hpp"

namespace f = tyr::formalism;
namespace fp = tyr::formalism::planning;

namespace tyr
{

bool GroundStateView::test(Index<fp::GroundAtom<f::StaticTag>> index) const { return m_state_repository->get_task()->test(index); }

float_t GroundStateView::get(Index<fp::GroundFunctionTerm<f::StaticTag>> index) const { return m_state_repository->get_task()->get(index); }

template<f::FactKind T>
const boost::dynamic_bitset<>& GroundStateView::get_atoms() const noexcept
{
    if constexpr (std::is_same_v<T, f::StaticTag>)
        return m_state_repository->get_task()->get_static_atoms_bitset();
    else if constexpr (std::is_same_v<T, f::DerivedTag>)
        return m_unpacked->template get_atoms<f::DerivedTag>().indices;
    else
        static_assert(dependent_false<T>::value, "Missing case");
}

template const boost::dynamic_bitset<>& GroundStateView::get_atoms<f::StaticTag>() const noexcept;
template const boost::dynamic_bitset<>& GroundStateView::get_atoms<f::DerivedTag>() const noexcept;

template<f::FactKind T>
const std::vector<float_t>& GroundStateView::get_numeric_variables() const noexcept
{
    if constexpr (std::is_same_v<T, f::StaticTag>)
        return m_state_repository->get_task()->get_static_numeric_variables();
    else if constexpr (std::is_same_v<T, f::FluentTag>)
        return m_unpacked->get_numeric_variables().values;
    else
        static_assert(dependent_false<T>::value, "Missing case");
}

template const std::vector<float_t>& GroundStateView::get_numeric_variables<f::StaticTag>() const noexcept;
template const std::vector<float_t>& GroundStateView::get_numeric_variables<f::FluentTag>() const noexcept;

planning::AtomRange<formalism::StaticTag> GroundStateView::get_static_atoms() const noexcept
{
    return planning::AtomRange<formalism::StaticTag>(m_state_repository->get_task()->get_static_atoms_bitset());
}

planning::FDRFactRange<planning::GroundTag, formalism::FluentTag> GroundStateView::get_fluent_facts() const noexcept
{
    return planning::FDRFactRange<planning::GroundTag, formalism::FluentTag>(get_fluent_values());
}

planning::AtomRange<formalism::DerivedTag> GroundStateView::get_derived_atoms() const noexcept
{
    return planning::AtomRange<formalism::DerivedTag>(get_atoms<formalism::DerivedTag>());
}

planning::FunctionTermValueRange<formalism::StaticTag> GroundStateView::get_static_fterm_values() const noexcept
{
    return planning::FunctionTermValueRange<formalism::StaticTag>(m_state_repository->get_task()->get_static_numeric_variables());
}

planning::FunctionTermValueRange<formalism::FluentTag> GroundStateView::get_fluent_fterm_values() const noexcept
{
    return planning::FunctionTermValueRange<formalism::FluentTag>(get_numeric_variables<formalism::FluentTag>());
}

const std::shared_ptr<formalism::planning::Repository>& GroundStateView::get_repository() const noexcept
{
    return m_state_repository->get_task()->get_repository();
}

static_assert(planning::IterableStateConcept<GroundStateView>);
static_assert(planning::IterableViewStateConcept<GroundStateView>);
static_assert(planning::IndexableStateConcept<GroundStateView, planning::GroundTag>);
static_assert(planning::IndexableViewStateConcept<GroundStateView, planning::GroundTag>);
}
