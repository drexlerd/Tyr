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
class UnpackedState<LiftedTask> : public UnpackedStateMixin<UnpackedState<LiftedTask>>
{
public:
    using TaskType = LiftedTask;

    UnpackedState() = default;

    StateIndex get_index_impl() const { return m_index; }
    void set_impl(StateIndex index) { m_index = index; }

    // Fluent facts
    formalism::FDRValue get_impl(Index<formalism::FDRVariable<formalism::FluentTag>> index) const
    {
        if (index.get_value() >= m_fluent_atoms.size())
            return formalism::FDRValue { 0 };
        return formalism::FDRValue { m_fluent_atoms.test(index.get_value()) };
    }
    void set_impl(Data<formalism::FDRFact<formalism::FluentTag>> fact)
    {
        assert(uint_t(fact.value) < 2);  // can only handle binary using bitsets
        if (fact.variable.get_value() >= m_fluent_atoms.size())
            m_fluent_atoms.resize(fact.variable.get_value() + 1, false);
        m_fluent_atoms[fact.variable.get_value()] = uint_t(fact.value);
    }

    // Fluent numeric variables
    float_t get_impl(Index<formalism::GroundFunctionTerm<formalism::FluentTag>> index) const
    {
        if (index.get_value() >= m_numeric_variables.size())
            return std::numeric_limits<float_t>::quiet_NaN();
        return m_numeric_variables[index.get_value()];
    }
    void set_impl(Index<formalism::GroundFunctionTerm<formalism::FluentTag>> index, float_t value)
    {
        if (index.get_value() >= m_numeric_variables.size())
            m_numeric_variables.resize(index.get_value() + 1, std::numeric_limits<float_t>::quiet_NaN());
        m_numeric_variables[index.get_value()] = value;
    }

    // Derived atoms
    bool test_impl(Index<formalism::GroundAtom<formalism::DerivedTag>> index) const
    {
        if (index.get_value() >= m_derived_atoms.size())
            return false;
        return m_derived_atoms.test(index.get_value());
    }
    void set_impl(Index<formalism::GroundAtom<formalism::DerivedTag>> index)
    {
        if (index.get_value() >= m_derived_atoms.size())
            m_derived_atoms.resize(index.get_value() + 1, false);
        m_derived_atoms.set(index.get_value());
    }

    void clear_impl()
    {
        m_fluent_atoms.clear();
        m_derived_atoms.clear();
        m_numeric_variables.clear();
    }

    /**
     * For lifted task
     */

    template<formalism::FactKind T>
    boost::dynamic_bitset<>& get_atoms() noexcept
    {
        if constexpr (std::same_as<T, formalism::FluentTag>)
            return m_fluent_atoms;
        else if constexpr (std::same_as<T, formalism::DerivedTag>)
            return m_derived_atoms;
        else
            static_assert(dependent_false<T>::value, "Missing case");
    }

    template<formalism::FactKind T>
    const boost::dynamic_bitset<>& get_atoms() const noexcept
    {
        if constexpr (std::same_as<T, formalism::FluentTag>)
            return m_fluent_atoms;
        else if constexpr (std::same_as<T, formalism::DerivedTag>)
            return m_derived_atoms;
        else
            static_assert(dependent_false<T>::value, "Missing case");
    }

    std::vector<float_t>& get_numeric_variables() noexcept { return m_numeric_variables; }

    const std::vector<float_t>& get_numeric_variables() const noexcept { return m_numeric_variables; }

private:
    StateIndex m_index;
    boost::dynamic_bitset<> m_fluent_atoms;
    boost::dynamic_bitset<> m_derived_atoms;
    std::vector<float_t> m_numeric_variables;
};
}

#endif
