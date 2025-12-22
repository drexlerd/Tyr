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
#include "tyr/formalism/declarations.hpp"
#include "tyr/formalism/ground_atom_index.hpp"
#include "tyr/formalism/ground_function_term_index.hpp"
#include "tyr/formalism/planning/fdr_fact_data.hpp"
#include "tyr/formalism/planning/fdr_variable_index.hpp"
#include "tyr/planning/declarations.hpp"
#include "tyr/planning/state_index.hpp"
#include "tyr/planning/unpacked_state.hpp"

#include <boost/dynamic_bitset.hpp>
#include <vector>

namespace tyr::planning
{
template<>
class UnpackedState<LiftedTask>
{
public:
    using TaskType = LiftedTask;

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
    void assign_unextended_part(const UnpackedState<LiftedTask>& other);

    /**
     * For LiftedTask
     */

    template<formalism::FactKind T>
    boost::dynamic_bitset<>& get_atoms() noexcept;

    template<formalism::FactKind T>
    const boost::dynamic_bitset<>& get_atoms() const noexcept;

    std::vector<float_t>& get_numeric_variables() noexcept;
    const std::vector<float_t>& get_numeric_variables() const noexcept;

private:
    StateIndex m_index;
    boost::dynamic_bitset<> m_fluent_atoms;
    boost::dynamic_bitset<> m_derived_atoms;
    std::vector<float_t> m_numeric_variables;
};

static_assert(UnpackedStateConcept<UnpackedState<LiftedTask>>);

/**
 * Implementations
 */

inline StateIndex UnpackedState<LiftedTask>::get_index() const { return m_index; }

inline void UnpackedState<LiftedTask>::set(StateIndex index) { m_index = index; }

// Fluent facts
inline formalism::FDRValue UnpackedState<LiftedTask>::get(Index<formalism::FDRVariable<formalism::FluentTag>> index) const
{
    return formalism::FDRValue(tyr::test(uint_t(index), m_fluent_atoms));
}

inline void UnpackedState<LiftedTask>::set(Data<formalism::FDRFact<formalism::FluentTag>> fact)
{
    assert(uint_t(fact.value) < 2);  // can only handle binary using bitsets
    tyr::set(uint_t(fact.variable), bool(uint_t(fact.value)), m_fluent_atoms);
}

// Fluent numeric variables
inline float_t UnpackedState<LiftedTask>::get(Index<formalism::GroundFunctionTerm<formalism::FluentTag>> index) const
{
    return tyr::get(uint_t(index), m_numeric_variables, std::numeric_limits<float_t>::quiet_NaN());
}

inline void UnpackedState<LiftedTask>::set(Index<formalism::GroundFunctionTerm<formalism::FluentTag>> index, float_t value)
{
    tyr::set(uint_t(index), value, m_numeric_variables, std::numeric_limits<float_t>::quiet_NaN());
}

// Derived atoms
inline bool UnpackedState<LiftedTask>::test(Index<formalism::GroundAtom<formalism::DerivedTag>> index) const
{
    return tyr::test(uint_t(index), m_derived_atoms);
}

inline void UnpackedState<LiftedTask>::set(Index<formalism::GroundAtom<formalism::DerivedTag>> index) { tyr::set(uint_t(index), true, m_derived_atoms); }

inline void UnpackedState<LiftedTask>::clear()
{
    clear_unextended_part();
    clear_extended_part();
}

inline void UnpackedState<LiftedTask>::clear_unextended_part()
{
    m_fluent_atoms.clear();
    m_numeric_variables.clear();
}

inline void UnpackedState<LiftedTask>::clear_extended_part() { m_derived_atoms.clear(); }

inline void UnpackedState<LiftedTask>::assign_unextended_part(const UnpackedState<LiftedTask>& other)
{
    m_fluent_atoms = other.m_fluent_atoms;
    m_numeric_variables = other.m_numeric_variables;
}

/**
 * For lifted task
 */

template<formalism::FactKind T>
inline boost::dynamic_bitset<>& UnpackedState<LiftedTask>::get_atoms() noexcept
{
    if constexpr (std::same_as<T, formalism::FluentTag>)
        return m_fluent_atoms;
    else if constexpr (std::same_as<T, formalism::DerivedTag>)
        return m_derived_atoms;
    else
        static_assert(dependent_false<T>::value, "Missing case");
}

template<formalism::FactKind T>
inline const boost::dynamic_bitset<>& UnpackedState<LiftedTask>::get_atoms() const noexcept
{
    if constexpr (std::same_as<T, formalism::FluentTag>)
        return m_fluent_atoms;
    else if constexpr (std::same_as<T, formalism::DerivedTag>)
        return m_derived_atoms;
    else
        static_assert(dependent_false<T>::value, "Missing case");
}

inline std::vector<float_t>& UnpackedState<LiftedTask>::get_numeric_variables() noexcept { return m_numeric_variables; }

inline const std::vector<float_t>& UnpackedState<LiftedTask>::get_numeric_variables() const noexcept { return m_numeric_variables; }
}

#endif
