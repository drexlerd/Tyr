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

#include "tyr/datalog/policies/termination.hpp"

#include "tyr/common/config.hpp"
#include "tyr/common/dynamic_bitset.hpp"
#include "tyr/datalog/fact_sets.hpp"
#include "tyr/datalog/policies/aggregation.hpp"
#include "tyr/formalism/datalog/declarations.hpp"
#include "tyr/formalism/datalog/ground_atom_index.hpp"
#include "tyr/formalism/datalog/repository.hpp"
#include "tyr/formalism/datalog/views.hpp"

#include <boost/dynamic_bitset.hpp>
#include <concepts>

namespace tyr::datalog
{

template<typename AggregationFunction>
TerminationPolicy<AggregationFunction>::TerminationPolicy(size_t num_fluent_predicates) : unsat_goals(num_fluent_predicates), num_unsat_goals(0), bindings()
{
}

template<typename AggregationFunction>
void TerminationPolicy<AggregationFunction>::set_goals(const PredicateFactSets<formalism::FluentTag>& goals)
{
    clear();

    num_unsat_goals = 0;
    for (const auto& set : goals.get_sets())
    {
        for (const auto& binding : set.get_bindings())
        {
            const auto g = uint_t(binding.get_index().relation);
            const auto i = uint_t(binding.get_index().row);

            tyr::set(i, true, unsat_goals[g]);
            ++num_unsat_goals;
            bindings.push_back(binding);
        }
    }
}

template<typename AggregationFunction>
void TerminationPolicy<AggregationFunction>::achieve(formalism::datalog::PredicateBindingView<formalism::FluentTag> binding) noexcept
{
    const auto g = uint_t(binding.get_index().relation);
    const auto i = uint_t(binding.get_index().row);

    if (tyr::test(i, unsat_goals[g]))
    {
        --num_unsat_goals;
        tyr::set(i, false, unsat_goals[g]);
    }
}

template<typename AggregationFunction>
bool TerminationPolicy<AggregationFunction>::check() const noexcept
{
    return num_unsat_goals == 0;
}

template<typename AggregationFunction>
Cost TerminationPolicy<AggregationFunction>::get_total_cost(const OrAnnotationsList& or_annot) const noexcept
{
    auto cost = AggregationFunction::identity();

    for (const auto binding : bindings)
    {
        const auto g = uint_t(binding.get_index().relation);
        const auto i = uint_t(binding.get_index().row);
        assert(g < or_annot.size());
        assert(i < or_annot[g].size());

        cost = agg(cost, or_annot[g][i]);
    }

    return cost;
}

template<typename AggregationFunction>
void TerminationPolicy<AggregationFunction>::reset() noexcept
{
    num_unsat_goals = 0;
    for (auto& bitset : unsat_goals)
        bitset.reset();

    for (const auto& binding : bindings)
    {
        const auto g = uint_t(binding.get_index().relation);
        const auto i = uint_t(binding.get_index().row);

        tyr::set(i, true, unsat_goals[g]);
    }
}

template<typename AggregationFunction>
void TerminationPolicy<AggregationFunction>::clear() noexcept
{
    num_unsat_goals = 0;
    for (auto& bitset : unsat_goals)
        bitset.reset();

    bindings.clear();
}

template class TerminationPolicy<SumAggregation>;
template class TerminationPolicy<MaxAggregation>;

}
