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

#ifndef TYR_PLANNING_LIFTED_TASK_UNPACKED_STATE_HPP_
#define TYR_PLANNING_LIFTED_TASK_UNPACKED_STATE_HPP_

#include "tyr/common/config.hpp"
#include "tyr/common/dynamic_bitset.hpp"
#include "tyr/common/vector.hpp"
#include "tyr/formalism/planning/declarations.hpp"
#include "tyr/formalism/planning/repository.hpp"
#include "tyr/formalism/planning/views.hpp"
#include "tyr/planning/declarations.hpp"
#include "tyr/planning/lifted_task/state_iterators.hpp"
#include "tyr/planning/state_index.hpp"
#include "tyr/planning/state_iterators.hpp"
#include "tyr/planning/unpacked_state.hpp"
//
#include "tyr/planning/lifted_task/state_storage/tree_compression/atom.hpp"
#include "tyr/planning/lifted_task/state_storage/tree_compression/fact.hpp"
#include "tyr/planning/state_storage/tree_compression/numeric.hpp"

#include <boost/dynamic_bitset.hpp>
#include <vector>

namespace tyr::planning
{
template<>
class UnpackedState<LiftedTag>
{
public:
    using TaskType = Task<LiftedTag>;

    UnpackedState() = default;

    Index<State<LiftedTag>> get_index() const;
    void set(Index<State<LiftedTag>> index);

    void clear();
    void clear_unextended_part();
    void clear_extended_part();
    void assign_unextended_part(const UnpackedState<LiftedTag>& other);

    /**
     * UnpackedStateConcept
     */

    formalism::planning::FDRValue get(Index<formalism::planning::FDRVariable<formalism::FluentTag>> index) const;
    void set(Data<formalism::planning::FDRFact<formalism::FluentTag>> fact);
    float_t get(Index<formalism::planning::GroundFunctionTerm<formalism::FluentTag>> index) const;
    void set(Index<formalism::planning::GroundFunctionTerm<formalism::FluentTag>> index, float_t value);
    bool test(Index<formalism::planning::GroundAtom<formalism::DerivedTag>> index) const;
    void set(Index<formalism::planning::GroundAtom<formalism::DerivedTag>> index);

    formalism::planning::FDRValue get(formalism::planning::FDRVariableView<formalism::FluentTag> view) const;
    void set(formalism::planning::FDRFactView<formalism::FluentTag> view);
    float_t get(formalism::planning::GroundFunctionTermView<formalism::FluentTag> view) const;
    void set(formalism::planning::GroundFunctionTermView<formalism::FluentTag> view, float_t value);
    bool test(formalism::planning::GroundAtomView<formalism::DerivedTag> view) const;
    void set(formalism::planning::GroundAtomView<formalism::DerivedTag> view);

    auto get_fluent_facts() const noexcept;
    auto get_derived_atoms() const noexcept;
    auto get_fluent_fterm_values() const noexcept;

    auto get_fluent_facts_view(const formalism::planning::Repository& repository) const noexcept;
    auto get_derived_atoms_view(const formalism::planning::Repository& repository) const noexcept;
    auto get_fluent_fterm_values_view(const formalism::planning::Repository& repository) const noexcept;

    template<formalism::FactKind T>
    auto& get_atoms() noexcept;
    template<formalism::FactKind T>
    const auto& get_atoms() const noexcept;

    auto& get_numeric_variables() noexcept;
    const auto& get_numeric_variables() const noexcept;

private:
    Index<State<LiftedTag>> m_index;

    planning::FactUnpackedStorage<LiftedTag> m_fact_storage;
    planning::AtomUnpackedStorage<LiftedTag> m_atom_storage;
    planning::NumericUnpackedStorage<LiftedTag> m_numeric_storage;
};

static_assert(UnpackedStateConcept<UnpackedState<LiftedTag>, LiftedTag>);

/**
 * Implementations
 */

inline Index<State<LiftedTag>> UnpackedState<LiftedTag>::get_index() const { return m_index; }

inline void UnpackedState<LiftedTag>::set(Index<State<LiftedTag>> index) { m_index = index; }

// Fluent facts
inline formalism::planning::FDRValue UnpackedState<LiftedTag>::get(Index<formalism::planning::FDRVariable<formalism::FluentTag>> index) const
{
    return formalism::planning::FDRValue(tyr::test(uint_t(index), m_fact_storage.indices));
}

inline void UnpackedState<LiftedTag>::set(Data<formalism::planning::FDRFact<formalism::FluentTag>> fact)
{
    assert(uint_t(fact.value) < 2);  // can only handle binary using bitsets
    tyr::set(uint_t(fact.variable), bool(uint_t(fact.value)), m_fact_storage.indices);
}

// Fluent numeric variables
inline float_t UnpackedState<LiftedTag>::get(Index<formalism::planning::GroundFunctionTerm<formalism::FluentTag>> index) const
{
    return tyr::get(uint_t(index), m_numeric_storage.values, std::numeric_limits<float_t>::quiet_NaN());
}

inline void UnpackedState<LiftedTag>::set(Index<formalism::planning::GroundFunctionTerm<formalism::FluentTag>> index, float_t value)
{
    tyr::set(uint_t(index), value, m_numeric_storage.values, std::numeric_limits<float_t>::quiet_NaN());
}

// Derived atoms
inline bool UnpackedState<LiftedTag>::test(Index<formalism::planning::GroundAtom<formalism::DerivedTag>> index) const
{
    return tyr::test(uint_t(index), m_atom_storage.indices);
}

inline void UnpackedState<LiftedTag>::set(Index<formalism::planning::GroundAtom<formalism::DerivedTag>> index)
{
    tyr::set(uint_t(index), true, m_atom_storage.indices);
}

// Fluent facts
inline formalism::planning::FDRValue UnpackedState<LiftedTag>::get(formalism::planning::FDRVariableView<formalism::FluentTag> view) const
{
    return get(view.get_index());
}

inline void UnpackedState<LiftedTag>::set(formalism::planning::FDRFactView<formalism::FluentTag> view) { set(view.get_data()); }

// Fluent numeric variables
inline float_t UnpackedState<LiftedTag>::get(formalism::planning::GroundFunctionTermView<formalism::FluentTag> view) const { return get(view.get_index()); }

inline void UnpackedState<LiftedTag>::set(formalism::planning::GroundFunctionTermView<formalism::FluentTag> view, float_t value)
{
    set(view.get_index(), value);
}

// Derived atoms
inline bool UnpackedState<LiftedTag>::test(formalism::planning::GroundAtomView<formalism::DerivedTag> view) const { return test(view.get_index()); }

inline void UnpackedState<LiftedTag>::set(formalism::planning::GroundAtomView<formalism::DerivedTag> view) { set(view.get_index()); }

inline void UnpackedState<LiftedTag>::clear()
{
    clear_unextended_part();
    clear_extended_part();
}

inline void UnpackedState<LiftedTag>::clear_unextended_part()
{
    m_fact_storage.indices.clear();
    m_numeric_storage.values.clear();
}

inline void UnpackedState<LiftedTag>::clear_extended_part() { m_atom_storage.indices.clear(); }

inline void UnpackedState<LiftedTag>::assign_unextended_part(const UnpackedState<LiftedTag>& other)
{
    m_fact_storage = other.m_fact_storage;
    m_numeric_storage = other.m_numeric_storage;
}

inline auto UnpackedState<LiftedTag>::get_fluent_facts() const noexcept { return FDRFactRange<LiftedTag, formalism::FluentTag>(m_fact_storage.indices); }

inline auto UnpackedState<LiftedTag>::get_derived_atoms() const noexcept { return AtomRange<formalism::DerivedTag>(m_atom_storage.indices); }

inline auto UnpackedState<LiftedTag>::get_fluent_fterm_values() const noexcept
{
    return FunctionTermValueRange<formalism::FluentTag>(m_numeric_storage.values);
}

inline auto UnpackedState<LiftedTag>::get_fluent_facts_view(const formalism::planning::Repository& repository_) const noexcept
{
    return get_fluent_facts() | std::views::transform([repository = &repository_](auto id) { return make_view(id, *repository); });
}

inline auto UnpackedState<LiftedTag>::get_derived_atoms_view(const formalism::planning::Repository& repository_) const noexcept
{
    return get_derived_atoms() | std::views::transform([repository = &repository_](auto id) { return make_view(id, *repository); });
}

inline auto UnpackedState<LiftedTag>::get_fluent_fterm_values_view(const formalism::planning::Repository& repository_) const noexcept
{
    return get_fluent_fterm_values()
           | std::views::transform([repository = &repository_](auto&& pair) { return std::make_pair(make_view(pair.first, *repository), pair.second); });
}

template<formalism::FactKind T>
inline auto& UnpackedState<LiftedTag>::get_atoms() noexcept
{
    if constexpr (std::same_as<T, formalism::FluentTag>)
        return m_fact_storage;
    else if constexpr (std::same_as<T, formalism::DerivedTag>)
        return m_atom_storage;
    else
        static_assert(dependent_false<T>::value, "Missing case");
}

template<formalism::FactKind T>
inline const auto& UnpackedState<LiftedTag>::get_atoms() const noexcept
{
    if constexpr (std::same_as<T, formalism::FluentTag>)
        return m_fact_storage;
    else if constexpr (std::same_as<T, formalism::DerivedTag>)
        return m_atom_storage;
    else
        static_assert(dependent_false<T>::value, "Missing case");
}

inline auto& UnpackedState<LiftedTag>::get_numeric_variables() noexcept { return m_numeric_storage; }

inline const auto& UnpackedState<LiftedTag>::get_numeric_variables() const noexcept { return m_numeric_storage; }
}

#endif
