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

#ifndef TYR_GROUNDER_FACTS_VIEW_HPP_
#define TYR_GROUNDER_FACTS_VIEW_HPP_

#include "tyr/common/config.hpp"
#include "tyr/formalism/declarations.hpp"
#include "tyr/formalism/views.hpp"
#include "tyr/grounder/fact_sets.hpp"

#include <boost/dynamic_bitset.hpp>
#include <vector>

namespace tyr::grounder
{

/// TODO: Find a better name
/// Its job is to group the randomized access low level fact set or state information together,
/// allowing us to check applicability on actions or rules on state or fact sets.
struct FactsView
{
    const boost::dynamic_bitset<>& static_atoms;
    const boost::dynamic_bitset<>& fluent_atoms;
    const boost::dynamic_bitset<>& derived_atoms;
    const std::vector<float_t>& static_numeric_variables;
    const std::vector<float_t>& fluent_numeric_variables;

    // programs dont have derived atoms
    static const boost::dynamic_bitset<> empty_derived_atoms;

    FactsView(const boost::dynamic_bitset<>& static_atoms,
              const boost::dynamic_bitset<>& fluent_atoms,
              const boost::dynamic_bitset<>& derived_atoms,
              const std::vector<float_t>& static_numeric_variables,
              const std::vector<float_t>& fluent_numeric_variables) :
        static_atoms(static_atoms),
        fluent_atoms(fluent_atoms),
        derived_atoms(derived_atoms),
        static_numeric_variables(static_numeric_variables),
        fluent_numeric_variables(fluent_numeric_variables)
    {
    }

    FactsView(const FactSets& fact_sets) :
        FactsView(fact_sets.static_sets.predicate.get_bitset(),
                  fact_sets.fluent_sets.predicate.get_bitset(),
                  empty_derived_atoms,
                  fact_sets.static_sets.function.get_values(),
                  fact_sets.fluent_sets.function.get_values())
    {
    }

    template<formalism::FactKind T>
    bool contains(Index<formalism::GroundAtom<T>> index) const noexcept
    {
        const auto& atoms = get_atoms<T>();
        if (index.get_value() >= atoms.size())
            return false;
        return atoms.test(index.get_value());
    }

    template<formalism::FactKind T>
    bool contains(Index<formalism::GroundFunctionTerm<T>> index) const noexcept
    {
        const auto& numeric_variables = get_numeric_variables<T>();
        if (index.get_value() >= numeric_variables.size())
            return false;
        return !std::isnan(numeric_variables[index.get_value()]);
    }

    template<formalism::FactKind T>
    float_t operator[](Index<formalism::GroundFunctionTerm<T>> index) const noexcept
    {
        assert(index.get_value() < get_numeric_variables<T>().size());
        return get_numeric_variables<T>()[index.get_value()];
    }

    template<formalism::FactKind T>
    const auto& get_atoms() const noexcept
    {
        if constexpr (std::is_same_v<T, formalism::StaticTag>)
            return static_atoms;
        else if constexpr (std::is_same_v<T, formalism::FluentTag>)
            return fluent_atoms;
        else if constexpr (std::is_same_v<T, formalism::DerivedTag>)
            return derived_atoms;
        else
            static_assert(dependent_false<T>::value, "Missing case");
    }

    template<formalism::FactKind T>
    const auto& get_numeric_variables() const noexcept
    {
        if constexpr (std::is_same_v<T, formalism::StaticTag>)
            return static_numeric_variables;
        else if constexpr (std::is_same_v<T, formalism::FluentTag>)
            return fluent_numeric_variables;
        else
            static_assert(dependent_false<T>::value, "Missing case");
    }
};

}

#endif