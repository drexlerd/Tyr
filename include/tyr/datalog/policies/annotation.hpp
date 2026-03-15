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

#ifndef TYR_SOLVER_POLICIES_ANNOTATION_HPP_
#define TYR_SOLVER_POLICIES_ANNOTATION_HPP_

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
 * Annotations
 */

using OrAnnotationsList = std::vector<std::vector<Cost>>;

/// @brief `Witness` is the rule and binding in the rule delta repository whose ground rule is the witness for its ground atom in the head.
/// The witness lives in the rule delta repository.
struct Witness
{
public:
    Witness(Index<formalism::datalog::Rule> rule, formalism::datalog::BindingView binding, Cost cost) : m_rule(rule), m_cost(cost), m_binding(binding) {}

    auto get_rule() const noexcept { return m_rule; }

    auto get_binding() const noexcept { return m_binding; }
    auto get_cost() const noexcept { return m_cost; }

    auto identifying_members() const noexcept { return std::tie(m_binding, m_rule); }

private:
    Index<formalism::datalog::Rule> m_rule;
    Cost m_cost;
    formalism::datalog::BindingView m_binding;
};

using AndAnnotationsMap = UnorderedMap<Index<formalism::datalog::GroundAtom<formalism::FluentTag>>, Witness>;

static_assert(sizeof(AndAnnotationsMap::value_type) == 32);

struct CostUpdate
{
    std::optional<Cost> old_cost;
    Cost new_cost;

    CostUpdate() noexcept : old_cost(std::nullopt), new_cost(Cost(0)) { assert(is_monoton()); }
    CostUpdate(std::optional<Cost> old_cost, Cost new_cost) noexcept : old_cost(old_cost), new_cost(new_cost) { assert(is_monoton()); }
    CostUpdate(Cost old_cost, Cost new_cost) noexcept :
        old_cost(old_cost == std::numeric_limits<Cost>::max() ? std::nullopt : std::optional<Cost>(old_cost)),
        new_cost(new_cost)
    {
        assert(is_monoton());
    }

    bool is_monoton() const noexcept { return !old_cost || new_cost <= old_cost.value(); }
};

/**
 * Policies
 */

// circle "or"-node
template<typename T>
concept OrAnnotationPolicyConcept = requires(const T& p,
                                             formalism::datalog::GroundAtomView<formalism::FluentTag> program_head,
                                             Index<formalism::datalog::GroundAtom<formalism::FluentTag>> delta_head,
                                             OrAnnotationsList& or_annot,
                                             const AndAnnotationsMap& delta_and_annot,
                                             AndAnnotationsMap& program_and_annot) {
    /// Annotate the initial cost of the atom.
    { p.initialize_annotation(program_head, or_annot) } -> std::same_as<void>;
    /// Annotate the cost of the atom from the given witness and annotations.
    /// `delta_head` indexes into the rule-local delta repository; `head` indexes into the global program repository.
    { p.update_annotation(program_head, delta_head, or_annot, delta_and_annot, program_and_annot) } -> std::same_as<CostUpdate>;
};

// rectangular "and"-node
template<typename T>
concept AndAnnotationPolicyConcept = requires(const T& p,
                                              formalism::datalog::GroundAtomView<formalism::FluentTag> program_head,
                                              Index<formalism::datalog::GroundAtom<formalism::FluentTag>> delta_head,
                                              uint_t current_cost,
                                              formalism::datalog::RuleView rule,
                                              formalism::datalog::ConjunctiveConditionView witness_condition,
                                              const OrAnnotationsList& or_annot,
                                              AndAnnotationsMap& delta_and_annot,
                                              formalism::datalog::GrounderContext& delta_context,
                                              formalism::datalog::GrounderContext& iteration_context) {
    /// Ground the witness and annotate the cost of it from the given annotations.
    {
        p.update_annotation(program_head, delta_head, current_cost, rule, witness_condition, or_annot, delta_and_annot, delta_context, iteration_context)
    } -> std::same_as<void>;
};

class NoOrAnnotationPolicy
{
public:
    static constexpr bool ShouldAnnotate = false;

    void initialize_annotation(formalism::datalog::GroundAtomView<formalism::FluentTag> head, OrAnnotationsList& or_annot) const noexcept {}

    CostUpdate update_annotation(formalism::datalog::GroundAtomView<formalism::FluentTag> program_head,
                                 Index<formalism::datalog::GroundAtom<formalism::FluentTag>> delta_head,
                                 OrAnnotationsList& or_annot,
                                 const AndAnnotationsMap& delta_and_annot,
                                 AndAnnotationsMap& program_and_annot) const noexcept
    {
        return CostUpdate();
    }
};

class NoAndAnnotationPolicy
{
public:
    static constexpr bool ShouldAnnotate = false;

    void update_annotation(formalism::datalog::GroundAtomView<formalism::FluentTag> program_head,
                           Index<formalism::datalog::GroundAtom<formalism::FluentTag>> delta_head,
                           uint_t current_cost,
                           formalism::datalog::RuleView rule,
                           formalism::datalog::ConjunctiveConditionView witness_condition,
                           const OrAnnotationsList& or_annot,
                           AndAnnotationsMap& delta_and_annot,
                           formalism::datalog::GrounderContext& delta_context,
                           formalism::datalog::GrounderContext& iteration_context) const noexcept
    {
    }
};

class OrAnnotationPolicy
{
public:
    static constexpr bool ShouldAnnotate = true;

    void initialize_annotation(formalism::datalog::GroundAtomView<formalism::FluentTag> program_head, OrAnnotationsList& or_annot) const
    {
        resize_or_annot_to_fit(program_head, or_annot);

        or_annot[uint_t(program_head.get_predicate().get_index())][uint_t(program_head.get_row().get_index().second)] = Cost(0);
    }

    CostUpdate update_annotation(formalism::datalog::GroundAtomView<formalism::FluentTag> program_head,
                                 Index<formalism::datalog::GroundAtom<formalism::FluentTag>> delta_head,
                                 OrAnnotationsList& or_annot,
                                 const AndAnnotationsMap& delta_and_annot,
                                 AndAnnotationsMap& program_and_annot) const
    {
        resize_or_annot_to_fit(program_head, or_annot);

        // Fast path 1: already optimal
        auto& or_cost = or_annot[uint_t(program_head.get_predicate().get_index())][uint_t(program_head.get_row().get_index().second)];
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
            program_and_annot.insert_or_assign(program_head.get_index(), witness);

        return cost_update;
    }

private:
    static CostUpdate update_min_cost(Cost& cost, Cost candidate)
    {
        const auto old_cost = cost;

        if (candidate < old_cost)
            cost = candidate;

        return CostUpdate(old_cost, cost);
    }

    static std::optional<Witness> fetch_best_head_witness_cost(Index<formalism::datalog::GroundAtom<formalism::FluentTag>> delta_head,
                                                               const AndAnnotationsMap& delta_and_annot)
    {
        if (auto it = delta_and_annot.find(delta_head); it != delta_and_annot.end())
            return it->second;

        return std::nullopt;  // No witness available (not derived yet / skipped / not tracked) -> no update from AND side
    }

    static void resize_or_annot_to_fit(formalism::datalog::GroundAtomView<formalism::FluentTag> program_head, OrAnnotationsList& or_annot)
    {
        assert(uint_t(program_head.get_predicate().get_index()) < or_annot.size());

        auto& vec = or_annot[uint_t(program_head.get_predicate().get_index())];
        if (uint_t(program_head.get_row().get_index().second) >= vec.size())
            vec.resize(uint_t(program_head.get_row().get_index().second) + 1, std::numeric_limits<Cost>::max());
    }
};

template<typename AggregationFunction>
class AndAnnotationPolicy
{
public:
    static constexpr bool ShouldAnnotate = true;
    static constexpr AggregationFunction agg = AggregationFunction {};

    void update_annotation(formalism::datalog::GroundAtomView<formalism::FluentTag> program_head,
                           Index<formalism::datalog::GroundAtom<formalism::FluentTag>> delta_head,
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

        const auto witness = try_ground_better_witness(best_cost, rule, witness_condition, delta_context, iteration_context, or_annot);
        if (!witness)
            return;  ///< No local or global improvement

        /// Update improved witness and cost annotation
        delta_and_annot.insert_or_assign(delta_head, *witness);
    }

private:
    static uint_t fetch_current_best_cost(Index<formalism::datalog::GroundAtom<formalism::FluentTag>> delta_head, const AndAnnotationsMap& delta_and_annot)
    {
        if (auto it = delta_and_annot.find(delta_head); it != delta_and_annot.end())
            return it->second.get_cost();

        return std::numeric_limits<uint_t>::max();
    }

    static uint_t fetch_atom_cost(formalism::datalog::GroundAtomView<formalism::FluentTag> atom, const OrAnnotationsList& or_annot)
    {
        return tyr::get(uint_t(atom.get_row().get_index().second), or_annot[uint_t(atom.get_predicate().get_index())], std::numeric_limits<uint_t>::max());
    }

    static std::optional<Witness> try_ground_better_witness(uint_t best_cost,
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

            const auto program_ground_atom_cost = fetch_atom_cost(program_ground_atom, or_annot);
            assert(program_ground_atom_cost != std::numeric_limits<uint_t>::max());

            body_cost = agg(body_cost, program_ground_atom_cost);

            if (best_cost <= body_cost + rule_cost)
                return std::nullopt;  ///< No local or global improvement
        }
        const auto witness_cost = body_cost + rule_cost;

        const auto delta_binding = formalism::datalog::ground(delta_context.binding, delta_context).first;

        return Witness(rule.get_index(), delta_binding, witness_cost);
    }
};

}

#endif
