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
#include "tyr/formalism/ground_atom_index.hpp"
#include "tyr/formalism/ground_function_term_index.hpp"
#include "tyr/formalism/planning/fdr_fact_data.hpp"
#include "tyr/formalism/planning/fdr_variable_index.hpp"
#include "tyr/planning/declarations.hpp"
#include "tyr/planning/unpacked_state.hpp"

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

    StateIndex get_index() const;
    void set(StateIndex index);

    formalism::FDRValue get(Index<formalism::FDRVariable<formalism::FluentTag>> index) const;
    void set(Data<formalism::FDRFact<formalism::FluentTag>> fact);

    float_t get(Index<formalism::GroundFunctionTerm<formalism::FluentTag>> index) const;
    void set(Index<formalism::GroundFunctionTerm<formalism::FluentTag>> index, float_t value);

    bool test(Index<formalism::GroundAtom<formalism::DerivedTag>> index) const;
    void set(Index<formalism::GroundAtom<formalism::DerivedTag>> index);

    void clear();
    void clear_unextended_part();
    void clear_extended_part();
    void assign_unextended_part(const UnpackedState<GroundTask>& other);

    /**
     * For GroundTask
     */

    void resize_fluent_facts(size_t num_fluent_facts);
    void resize_derived_atoms(size_t num_derived_atoms);

    std::vector<formalism::FDRValue>& get_fluent_values() noexcept;
    const std::vector<formalism::FDRValue>& get_fluent_values() const noexcept;

    boost::dynamic_bitset<>& get_derived_atoms() noexcept;
    const boost::dynamic_bitset<>& get_derived_atoms() const noexcept;

    std::vector<float_t>& get_numeric_variables() noexcept;
    const std::vector<float_t>& get_numeric_variables() const noexcept;

private:
    StateIndex m_index;
    std::vector<formalism::FDRValue> m_fluent_values;
    boost::dynamic_bitset<> m_derived_atoms;
    std::vector<float_t> m_numeric_variables;
};

static_assert(UnpackedStateConcept<UnpackedState<GroundTask>>);

/**
 * Implementations
 */

inline StateIndex UnpackedState<GroundTask>::get_index() const { return m_index; }

inline void UnpackedState<GroundTask>::set(StateIndex index) { m_index = index; }

// Fluent facts
inline formalism::FDRValue UnpackedState<GroundTask>::get(Index<formalism::FDRVariable<formalism::FluentTag>> index) const
{
    assert(uint_t(index) < m_fluent_values.size());
    return m_fluent_values[uint_t(index)];
}

inline void UnpackedState<GroundTask>::set(Data<formalism::FDRFact<formalism::FluentTag>> fact)
{
    assert(uint_t(fact.variable) < m_fluent_values.size());
    m_fluent_values[uint_t(fact.variable)] = fact.value;
}

// Fluent numeric variables
inline float_t UnpackedState<GroundTask>::get(Index<formalism::GroundFunctionTerm<formalism::FluentTag>> index) const
{
    return tyr::get(uint_t(index), m_numeric_variables, std::numeric_limits<float_t>::quiet_NaN());
}

inline void UnpackedState<GroundTask>::set(Index<formalism::GroundFunctionTerm<formalism::FluentTag>> index, float_t value)
{
    tyr::set(uint_t(index), value, m_numeric_variables, std::numeric_limits<float_t>::quiet_NaN());
}

// Derived atoms
inline bool UnpackedState<GroundTask>::test(Index<formalism::GroundAtom<formalism::DerivedTag>> index) const
{
    assert(uint_t(index) < m_derived_atoms.size());
    return m_derived_atoms.test(uint_t(index));
}

inline void UnpackedState<GroundTask>::set(Index<formalism::GroundAtom<formalism::DerivedTag>> index)
{
    assert(uint_t(index) < m_derived_atoms.size());
    m_derived_atoms.set(uint_t(index));
}

inline void UnpackedState<GroundTask>::clear()
{
    clear_unextended_part();
    clear_extended_part();
}

inline void UnpackedState<GroundTask>::clear_unextended_part()
{
    m_fluent_values.clear();
    m_numeric_variables.clear();
}

inline void UnpackedState<GroundTask>::clear_extended_part() { m_derived_atoms.clear(); }

inline void UnpackedState<GroundTask>::assign_unextended_part(const UnpackedState<GroundTask>& other)
{
    m_fluent_values = other.m_fluent_values;
    m_numeric_variables = other.m_numeric_variables;
}

inline void UnpackedState<GroundTask>::resize_fluent_facts(size_t num_fluent_facts) { m_fluent_values.resize(num_fluent_facts, formalism::FDRValue::none()); }

inline void UnpackedState<GroundTask>::resize_derived_atoms(size_t num_derived_atoms) { m_derived_atoms.resize(num_derived_atoms, false); }

inline std::vector<formalism::FDRValue>& UnpackedState<GroundTask>::get_fluent_values() noexcept { return m_fluent_values; }

inline const std::vector<formalism::FDRValue>& UnpackedState<GroundTask>::get_fluent_values() const noexcept { return m_fluent_values; }

inline boost::dynamic_bitset<>& UnpackedState<GroundTask>::get_derived_atoms() noexcept { return m_derived_atoms; }

inline const boost::dynamic_bitset<>& UnpackedState<GroundTask>::get_derived_atoms() const noexcept { return m_derived_atoms; }

inline std::vector<float_t>& UnpackedState<GroundTask>::get_numeric_variables() noexcept { return m_numeric_variables; }

inline const std::vector<float_t>& UnpackedState<GroundTask>::get_numeric_variables() const noexcept { return m_numeric_variables; }

}

#endif
