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

#include "tyr/datalog/policies/annotation.hpp"

#include "tyr/common/config.hpp"
#include "tyr/common/vector.hpp"
#include "tyr/datalog/policies/aggregation.hpp"
#include "tyr/formalism/binding_index.hpp"
#include "tyr/formalism/datalog/builder.hpp"
#include "tyr/formalism/datalog/canonicalization.hpp"
#include "tyr/formalism/datalog/declarations.hpp"
#include "tyr/formalism/datalog/formatter.hpp"
#include "tyr/formalism/datalog/ground_atom_index.hpp"
#include "tyr/formalism/datalog/ground_conjunctive_condition_index.hpp"
#include "tyr/formalism/datalog/grounder.hpp"
#include "tyr/formalism/datalog/repository.hpp"
#include "tyr/formalism/datalog/rule_index.hpp"

#include <algorithm>
#include <cassert>
#include <concepts>
#include <limits>
#include <optional>
#include <tuple>
#include <vector>

namespace tyr::datalog
{

/**
 * OrAnnotationPolicy
 */

namespace
{
CostUpdate update_min_cost(Cost& cost, Cost candidate)
{
    const auto old_cost = cost;

    if (candidate < old_cost)
        cost = candidate;

    return CostUpdate(old_cost, cost);
}

std::optional<Witness> fetch_best_head_witness_cost(formalism::datalog::PredicateBindingView<formalism::FluentTag> delta_head,
                                                    const AndAnnotationsMap& delta_and_annot)
{
    if (auto it = delta_and_annot.find(delta_head); it != delta_and_annot.end())
        return it->second;

    return std::nullopt;  // No witness available (not derived yet / skipped / not tracked) -> no update from AND side
}

void resize_or_annot_to_fit(formalism::datalog::PredicateBindingView<formalism::FluentTag> program_head, OrAnnotationsList& or_annot)
{
    const auto g = uint_t(program_head.get_index().relation);
    const auto i = uint_t(program_head.get_index().row);

    assert(g < or_annot.size());

    auto& vec = or_annot[g];
    if (i >= vec.size())
        vec.resize(i + 1, std::numeric_limits<Cost>::max());
}
}

void OrAnnotationPolicy::initialize_annotation(formalism::datalog::PredicateBindingView<formalism::FluentTag> program_head, OrAnnotationsList& or_annot) const
{
    resize_or_annot_to_fit(program_head, or_annot);

    const auto g = uint_t(program_head.get_index().relation);
    const auto i = uint_t(program_head.get_index().row);

    or_annot[g][i] = Cost(0);
}

CostUpdate OrAnnotationPolicy::update_annotation(formalism::datalog::PredicateBindingView<formalism::FluentTag> program_head,
                                                 formalism::datalog::PredicateBindingView<formalism::FluentTag> delta_head,
                                                 OrAnnotationsList& or_annot,
                                                 const AndAnnotationsMap& delta_and_annot,
                                                 AndAnnotationsMap& program_and_annot) const
{
    resize_or_annot_to_fit(program_head, or_annot);

    // Fast path 1: already optimal
    const auto g = uint_t(program_head.get_index().relation);
    const auto i = uint_t(program_head.get_index().row);

    auto& or_cost = or_annot[g][i];
    if (or_cost == Cost(0))
        return CostUpdate(or_cost, or_cost);

    const auto result = fetch_best_head_witness_cost(delta_head, delta_and_annot);

    // Fast path 2: no witness available => no update
    if (!result)
        return CostUpdate(or_cost, or_cost);

    const auto witness = result.value();

    const auto old_cost = or_cost;

    const auto cost_update = update_min_cost(or_cost, witness.get_cost());

    if (or_cost < old_cost)
        program_and_annot.insert_or_assign(program_head, witness);

    return cost_update;
}

/**
 * AndAnnotationPolicy
 */

namespace
{

uint_t fetch_current_best_cost(formalism::datalog::PredicateBindingView<formalism::FluentTag> delta_head, const AndAnnotationsMap& delta_and_annot)
{
    if (auto it = delta_and_annot.find(delta_head); it != delta_and_annot.end())
        return it->second.get_cost();

    return std::numeric_limits<uint_t>::max();
}

uint_t fetch_atom_cost(formalism::datalog::PredicateBindingView<formalism::FluentTag> program_head, const OrAnnotationsList& or_annot)
{
    const auto g = uint_t(program_head.get_index().relation);
    const auto i = uint_t(program_head.get_index().row);

    return tyr::get(i, or_annot[g], std::numeric_limits<uint_t>::max());
}

template<typename AggregationFunction>
std::optional<Witness> try_ground_better_witness(uint_t best_cost,
                                                 formalism::datalog::RuleView rule,
                                                 formalism::datalog::ConjunctiveConditionView witness_condition,
                                                 formalism::datalog::GrounderContext& delta_context,
                                                 formalism::datalog::GrounderContext& iteration_context,
                                                 const OrAnnotationsList& or_annot)
{
    auto body_cost = AggregationFunction::identity();
    const auto rule_cost = rule.get_cost();

    if (best_cost <= body_cost + rule_cost)
        return std::nullopt;  ///< No local or global improvement

    for (const auto literal : witness_condition.get_literals<formalism::FluentTag>())
    {
        assert(literal.get_polarity());

        const auto [program_ground_atom, inserted] = formalism::datalog::ground(literal.get_atom(), iteration_context);
        assert(!inserted);  ///< must exist in program because the precondition is applicable in program fact set.

        const auto program_ground_atom_cost = fetch_atom_cost(program_ground_atom.get_row(), or_annot);
        assert(program_ground_atom_cost != std::numeric_limits<uint_t>::max());

        body_cost = AggregationFunction()(body_cost, program_ground_atom_cost);

        if (best_cost <= body_cost + rule_cost)
            return std::nullopt;  ///< No local or global improvement
    }
    const auto witness_cost = body_cost + rule_cost;

    const auto delta_binding = formalism::datalog::ground_binding(rule, delta_context).first;

    return Witness(delta_binding, witness_cost);
}
}

template<typename AggregationFunction>
void AndAnnotationPolicy<AggregationFunction>::update_annotation(formalism::datalog::PredicateBindingView<formalism::FluentTag> program_head,
                                                                 formalism::datalog::PredicateBindingView<formalism::FluentTag> delta_head,
                                                                 uint_t current_cost,
                                                                 formalism::datalog::RuleView rule,
                                                                 formalism::datalog::ConjunctiveConditionView witness_condition,
                                                                 const OrAnnotationsList& or_annot,
                                                                 AndAnnotationsMap& delta_and_annot,
                                                                 formalism::datalog::GrounderContext& delta_context,
                                                                 formalism::datalog::GrounderContext& iteration_context) const
{
    // Use min among global minimum in cost of last iteration and thread local minimum.
    const auto best_global_cost = fetch_atom_cost(program_head, or_annot);
    const auto best_local_cost = fetch_current_best_cost(delta_head, delta_and_annot);
    const auto best_cost = std::min(best_global_cost, best_local_cost);
    const auto cur_cost_lower_bound = current_cost + rule.get_cost();

    if (best_cost <= cur_cost_lower_bound)
        return;  ///< No local or global improvement

    const auto witness = try_ground_better_witness<AggregationFunction>(best_cost, rule, witness_condition, delta_context, iteration_context, or_annot);
    if (!witness)
        return;  ///< No local or global improvement

    /// Update improved witness and cost annotation
    delta_and_annot.insert_or_assign(delta_head, *witness);
}

template class AndAnnotationPolicy<SumAggregation>;
template class AndAnnotationPolicy<MaxAggregation>;

}
