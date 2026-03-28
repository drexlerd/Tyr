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

#ifndef TYR_PLANNING_GROUND_TASK_UNPACKED_STATE_HPP_
#define TYR_PLANNING_GROUND_TASK_UNPACKED_STATE_HPP_

#include "tyr/common/config.hpp"
#include "tyr/common/dynamic_bitset.hpp"
#include "tyr/common/vector.hpp"
#include "tyr/formalism/planning/fdr_fact_data.hpp"
#include "tyr/formalism/planning/fdr_variable_index.hpp"
#include "tyr/formalism/planning/ground_atom_index.hpp"
#include "tyr/formalism/planning/ground_function_term_index.hpp"
#include "tyr/planning/declarations.hpp"
#include "tyr/planning/unpacked_state.hpp"
//
#include "tyr/planning/ground_task/state_storage/tree_compression/atom.hpp"
#include "tyr/planning/ground_task/state_storage/tree_compression/fact.hpp"
#include "tyr/planning/state_storage/tree_compression/numeric.hpp"

#include <boost/dynamic_bitset.hpp>
#include <cassert>
#include <limits>
#include <vector>

namespace tyr::planning
{
template<>
class UnpackedState<GroundTask>
{
public:
    using TaskType = GroundTask;

    UnpackedState() = default;

    /**
     * UnpackedStateConcept
     */

    Index<State<GroundTask>> get_index() const;
    void set(Index<State<GroundTask>> index);

    formalism::planning::FDRValue get(Index<formalism::planning::FDRVariable<formalism::FluentTag>> index) const;
    void set(Data<formalism::planning::FDRFact<formalism::FluentTag>> fact);

    float_t get(Index<formalism::planning::GroundFunctionTerm<formalism::FluentTag>> index) const;
    void set(Index<formalism::planning::GroundFunctionTerm<formalism::FluentTag>> index, float_t value);

    bool test(Index<formalism::planning::GroundAtom<formalism::DerivedTag>> index) const;
    void set(Index<formalism::planning::GroundAtom<formalism::DerivedTag>> index);

    void clear();
    void clear_unextended_part();
    void clear_extended_part();
    void assign_unextended_part(const UnpackedState<GroundTask>& other);

    /**
     * For GroundTask
     */

    void resize_fluent_facts(size_t num_fluent_facts);
    void resize_derived_atoms(size_t num_derived_atoms);

    template<formalism::FactKind T>
    auto& get_atoms() noexcept;
    template<formalism::FactKind T>
    const auto& get_atoms() const noexcept;

    auto& get_numeric_variables() noexcept;
    const auto& get_numeric_variables() const noexcept;

private:
    Index<State<GroundTask>> m_index;

    planning::FactUnpackedStorage<TaskType> m_fact_storage;
    planning::AtomUnpackedStorage<TaskType> m_atom_storage;
    planning::NumericUnpackedStorage<TaskType> m_numeric_storage;
};

static_assert(UnpackedStateConcept<UnpackedState<GroundTask>, GroundTask>);

/**
 * Implementations
 */

inline Index<State<GroundTask>> UnpackedState<GroundTask>::get_index() const { return m_index; }

inline void UnpackedState<GroundTask>::set(Index<State<GroundTask>> index) { m_index = index; }

// Fluent facts
inline formalism::planning::FDRValue UnpackedState<GroundTask>::get(Index<formalism::planning::FDRVariable<formalism::FluentTag>> index) const
{
    assert(uint_t(index) < m_fact_storage.values.size());
    return formalism::planning::FDRValue(m_fact_storage.values[uint_t(index)]);
}

inline void UnpackedState<GroundTask>::set(Data<formalism::planning::FDRFact<formalism::FluentTag>> fact)
{
    assert(uint_t(fact.variable) < m_fact_storage.values.size());
    m_fact_storage.values[uint_t(fact.variable)] = uint_t(fact.value);
}

// Fluent numeric variables
inline float_t UnpackedState<GroundTask>::get(Index<formalism::planning::GroundFunctionTerm<formalism::FluentTag>> index) const
{
    return tyr::get(uint_t(index), m_numeric_storage.values, std::numeric_limits<float_t>::quiet_NaN());
}

inline void UnpackedState<GroundTask>::set(Index<formalism::planning::GroundFunctionTerm<formalism::FluentTag>> index, float_t value)
{
    tyr::set(uint_t(index), value, m_numeric_storage.values, std::numeric_limits<float_t>::quiet_NaN());
}

// Derived atoms
inline bool UnpackedState<GroundTask>::test(Index<formalism::planning::GroundAtom<formalism::DerivedTag>> index) const
{
    assert(uint_t(index) < m_atom_storage.indices.size());
    return m_atom_storage.indices.test(uint_t(index));
}

inline void UnpackedState<GroundTask>::set(Index<formalism::planning::GroundAtom<formalism::DerivedTag>> index)
{
    assert(uint_t(index) < m_atom_storage.indices.size());
    m_atom_storage.indices.set(uint_t(index));
}

inline void UnpackedState<GroundTask>::clear()
{
    clear_unextended_part();
    clear_extended_part();
}

inline void UnpackedState<GroundTask>::clear_unextended_part()
{
    m_fact_storage.values.clear();
    m_numeric_storage.values.clear();
}

inline void UnpackedState<GroundTask>::clear_extended_part() { m_atom_storage.indices.clear(); }

inline void UnpackedState<GroundTask>::assign_unextended_part(const UnpackedState<GroundTask>& other)
{
    m_fact_storage = other.m_fact_storage;
    m_numeric_storage = other.m_numeric_storage;
}

inline void UnpackedState<GroundTask>::resize_fluent_facts(size_t num_fluent_facts) { m_fact_storage.values.resize(num_fluent_facts, 0); }

inline void UnpackedState<GroundTask>::resize_derived_atoms(size_t num_derived_atoms) { m_atom_storage.indices.resize(num_derived_atoms, false); }

template<formalism::FactKind T>
inline auto& UnpackedState<GroundTask>::get_atoms() noexcept
{
    if constexpr (std::same_as<T, formalism::FluentTag>)
        return m_fact_storage;
    else if constexpr (std::same_as<T, formalism::DerivedTag>)
        return m_atom_storage;
    else
        static_assert(dependent_false<T>::value, "Missing case");
}

template<formalism::FactKind T>
inline const auto& UnpackedState<GroundTask>::get_atoms() const noexcept
{
    if constexpr (std::same_as<T, formalism::FluentTag>)
        return m_fact_storage;
    else if constexpr (std::same_as<T, formalism::DerivedTag>)
        return m_atom_storage;
    else
        static_assert(dependent_false<T>::value, "Missing case");
}

inline auto& UnpackedState<GroundTask>::get_numeric_variables() noexcept { return m_numeric_storage; }

inline const auto& UnpackedState<GroundTask>::get_numeric_variables() const noexcept { return m_numeric_storage; }

}

#endif
