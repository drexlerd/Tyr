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
#include "tyr/formalism/datalog/canonicalization.hpp"
#include "tyr/formalism/datalog/declarations.hpp"
#include "tyr/formalism/datalog/formatter.hpp"
#include "tyr/formalism/datalog/ground_atom_index.hpp"
#include "tyr/formalism/datalog/ground_conjunctive_condition_index.hpp"
#include "tyr/formalism/datalog/grounder.hpp"
#include "tyr/formalism/datalog/repository.hpp"
#include "tyr/formalism/datalog/rule_index.hpp"
#include "tyr/formalism/overlay_repository.hpp"

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
    Index<formalism::datalog::Rule> rule;
    Index<formalism::Binding> binding;
    Index<formalism::datalog::GroundConjunctiveCondition> witness_condition;

    auto identifying_members() const noexcept { return std::tie(rule, binding); }
};

using AndAnnotationsMap = UnorderedMap<Witness, Cost>;
using HeadToWitness = UnorderedMap<Index<formalism::datalog::GroundAtom<formalism::FluentTag>>, Witness>;

/// @brief `CostAnnotations` encapsulates the cost of nodes.
struct CostAnnotations
{
    OrAnnotationsList or_annot;
    std::vector<AndAnnotationsMap> and_annots;

    CostAnnotations(OrAnnotationsList or_annot, std::vector<AndAnnotationsMap> and_annots) : or_annot(std::move(or_annot)), and_annots(std::move(and_annots)) {}

    void clear() noexcept
    {
        or_annot.clear();
        for (auto& and_annot : and_annots)
            and_annot.clear();
    }
};

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
                                             Index<formalism::datalog::GroundAtom<formalism::FluentTag>> program_head,
                                             Index<formalism::datalog::GroundAtom<formalism::FluentTag>> delta_head,
                                             OrAnnotationsList& or_annot,
                                             const AndAnnotationsMap& and_annot,
                                             const HeadToWitness& delta_head_to_witness,
                                             HeadToWitness& program_head_to_witness) {
    /// Annotate the initial cost of the atom.
    { p.initialize_annotation(program_head, or_annot) } -> std::same_as<void>;
    /// Annotate the cost of the atom from the given witness and annotations.
    /// `delta_head` indexes into the rule-local delta repository; `head` indexes into the global program repository.
    { p.update_annotation(program_head, delta_head, or_annot, and_annot, delta_head_to_witness, program_head_to_witness) } -> std::same_as<CostUpdate>;
};

// rectangular "and"-node
template<typename T>
concept AndAnnotationPolicyConcept =
    requires(const T& p,
             uint_t current_cost,
             Index<formalism::datalog::Rule> rule,
             Index<formalism::datalog::ConjunctiveCondition> witness_condition,
             Index<formalism::datalog::GroundAtom<formalism::FluentTag>> program_head,
             Index<formalism::datalog::GroundAtom<formalism::FluentTag>> delta_head,
             const OrAnnotationsList& or_annot,
             AndAnnotationsMap& and_annot,
             HeadToWitness& delta_head_to_witness,
             formalism::datalog::GrounderContext<formalism::datalog::Repository>& delta_context,
             formalism::datalog::GrounderContext<formalism::OverlayRepository<formalism::datalog::Repository>>& persistent_context) {
        /// Ground the witness and annotate the cost of it from the given annotations.
        {
            p.update_annotation(current_cost,
                                rule,
                                witness_condition,
                                program_head,
                                delta_head,
                                or_annot,
                                and_annot,
                                delta_head_to_witness,
                                delta_context,
                                persistent_context)
        } -> std::same_as<CostUpdate>;
    };

class NoOrAnnotationPolicy
{
public:
    static constexpr bool ShouldAnnotate = false;

    void initialize_annotation(Index<formalism::datalog::GroundAtom<formalism::FluentTag>> head, OrAnnotationsList& or_annot) const noexcept {}

    CostUpdate update_annotation(Index<formalism::datalog::GroundAtom<formalism::FluentTag>> program_head,
                                 Index<formalism::datalog::GroundAtom<formalism::FluentTag>> delta_head,
                                 OrAnnotationsList& or_annot,
                                 const AndAnnotationsMap& and_annot,
                                 const HeadToWitness& delta_head_to_witness,
                                 HeadToWitness& program_head_to_witness) const noexcept
    {
        return CostUpdate();
    }
};

class NoAndAnnotationPolicy
{
public:
    static constexpr bool ShouldAnnotate = false;

    CostUpdate
    update_annotation(uint_t current_cost,
                      Index<formalism::datalog::Rule> rule,
                      Index<formalism::datalog::ConjunctiveCondition> witness_condition,
                      Index<formalism::datalog::GroundAtom<formalism::FluentTag>> program_head,
                      Index<formalism::datalog::GroundAtom<formalism::FluentTag>> delta_head,
                      const OrAnnotationsList& or_annot,
                      AndAnnotationsMap& and_annot,
                      HeadToWitness& head_to_witness,
                      formalism::datalog::GrounderContext<formalism::datalog::Repository>& delta_context,
                      formalism::datalog::GrounderContext<formalism::OverlayRepository<formalism::datalog::Repository>>& persistent_context) const noexcept
    {
        return CostUpdate();
    }
};

class OrAnnotationPolicy
{
public:
    static constexpr bool ShouldAnnotate = true;

    void initialize_annotation(Index<formalism::datalog::GroundAtom<formalism::FluentTag>> program_head, OrAnnotationsList& or_annot) const
    {
        resize_or_annot_to_fit(program_head, or_annot);

        or_annot[uint_t(program_head.group)][program_head.value] = Cost(0);
    }

    CostUpdate update_annotation(Index<formalism::datalog::GroundAtom<formalism::FluentTag>> program_head,
                                 Index<formalism::datalog::GroundAtom<formalism::FluentTag>> delta_head,
                                 OrAnnotationsList& or_annot,
                                 const AndAnnotationsMap& and_annot,
                                 const HeadToWitness& delta_head_to_witness,
                                 HeadToWitness& program_head_to_witness) const
    {
        resize_or_annot_to_fit(program_head, or_annot);

        // Fast path 1: already optimal
        auto& or_cost = or_annot[uint_t(program_head.group)][program_head.value];
        if (or_cost == Cost(0))
            return CostUpdate(or_cost, or_cost);

        const auto result = fetch_best_head_witness_cost(delta_head, and_annot, delta_head_to_witness);

        // Fast path 2: no witness available => no update
        if (!result)
            return CostUpdate(or_cost, or_cost);

        const auto [witness, and_cost] = result.value();

        const auto old_cost = or_cost;

        const auto cost_update = update_min_cost(or_cost, and_cost);

        if (or_cost < old_cost)
            program_head_to_witness[program_head] = witness;

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

    static std::optional<std::pair<Witness, Cost>> fetch_best_head_witness_cost(Index<formalism::datalog::GroundAtom<formalism::FluentTag>> delta_head,
                                                                                const AndAnnotationsMap& and_annot,
                                                                                const HeadToWitness& delta_head_to_witness)
    {
        const auto it_w = delta_head_to_witness.find(delta_head);
        if (it_w == delta_head_to_witness.end())
            return std::nullopt;  // No witness available (not derived yet / skipped / not tracked) -> no update from AND side

        const auto it_a = and_annot.find(it_w->second);
        assert(it_a != and_annot.end());

        return *it_a;
    }

    static void resize_or_annot_to_fit(Index<formalism::datalog::GroundAtom<formalism::FluentTag>> program_head, OrAnnotationsList& or_annot)
    {
        assert(uint_t(program_head.group) < or_annot.size());

        auto& vec = or_annot[uint_t(program_head.group)];
        if (program_head.value >= vec.size())
            vec.resize(program_head.value + 1, std::numeric_limits<Cost>::max());
    }
};

template<typename AggregationFunction>
class AndAnnotationPolicy
{
public:
    static constexpr bool ShouldAnnotate = true;
    static constexpr AggregationFunction agg = AggregationFunction {};

    CostUpdate update_annotation(uint_t current_cost,
                                 Index<formalism::datalog::Rule> rule,
                                 Index<formalism::datalog::ConjunctiveCondition> witness_condition,
                                 Index<formalism::datalog::GroundAtom<formalism::FluentTag>> program_head,
                                 Index<formalism::datalog::GroundAtom<formalism::FluentTag>> delta_head,
                                 const OrAnnotationsList& or_annot,
                                 AndAnnotationsMap& and_annot,
                                 HeadToWitness& delta_head_to_witness,
                                 formalism::datalog::GrounderContext<formalism::datalog::Repository>& delta_context,
                                 formalism::datalog::GrounderContext<formalism::OverlayRepository<formalism::datalog::Repository>>& persistent_context) const
    {
        assert(delta_context.binding == persistent_context.binding);

        const auto old_cost = fetch_atom_cost(program_head, or_annot);
        const auto LB = current_cost + make_view(rule, persistent_context.destination).get_cost();
        if (old_cost <= LB)
            return CostUpdate(old_cost, old_cost);

        const auto [cost_update, witness] = try_ground_better_witness(rule, witness_condition, program_head, delta_context, persistent_context, or_annot);
        if (!witness)
            return cost_update;
        if (cost_update.old_cost && cost_update.old_cost <= cost_update.new_cost)
            return cost_update;  ///< no improvement

        /// Update witness-cost mappings
        and_annot.insert_or_assign(*witness, cost_update.new_cost);
        delta_head_to_witness.insert_or_assign(delta_head, *witness);

        return cost_update;
    }

private:
    static uint_t fetch_atom_cost(Index<formalism::datalog::GroundAtom<formalism::FluentTag>> atom, const OrAnnotationsList& or_annot)
    {
        return tyr::get(atom.value, or_annot[uint_t(atom.group)], std::numeric_limits<uint_t>::max());
    }

    static std::pair<CostUpdate, std::optional<Witness>>
    try_ground_better_witness(Index<formalism::datalog::Rule> rule,
                              Index<formalism::datalog::ConjunctiveCondition> witness_condition,
                              Index<formalism::datalog::GroundAtom<formalism::FluentTag>> program_head,
                              formalism::datalog::GrounderContext<formalism::datalog::Repository>& delta_context,
                              formalism::datalog::GrounderContext<formalism::OverlayRepository<formalism::datalog::Repository>>& persistent_context,
                              const OrAnnotationsList& or_annot)
    {
        const auto old_cost = fetch_atom_cost(program_head, or_annot);

        auto new_cost = AggregationFunction::identity();
        new_cost = agg(new_cost, make_view(rule, persistent_context.destination).get_cost());

        if (new_cost >= old_cost)
            return std::make_pair(CostUpdate(old_cost, old_cost), std::nullopt);  ///< Cannot improve

        auto ground_conj_cond_ptr = persistent_context.builder.get_builder<formalism::datalog::GroundConjunctiveCondition>();
        auto& ground_conj_cond = *ground_conj_cond_ptr;
        ground_conj_cond.clear();

        for (const auto& literal : make_view(witness_condition, persistent_context.destination).get_literals<formalism::FluentTag>())
        {
            assert(literal.get_polarity());
            // TODO: we could get rid of grounding literals by having strictly positive rules
            const auto ground_literal = formalism::datalog::ground(literal, persistent_context).first;
            const auto ground_atom = make_view(ground_literal, persistent_context.destination).get_atom().get_index();
            const auto ground_atom_cost = fetch_atom_cost(ground_atom, or_annot);
            assert(ground_atom_cost != std::numeric_limits<uint_t>::max());

            new_cost = agg(new_cost, ground_atom_cost);

            if (new_cost >= old_cost)
                return std::make_pair(CostUpdate(old_cost, old_cost), std::nullopt);  ///< Cannot improve

            ground_conj_cond.fluent_literals.push_back(ground_literal);
        }

        canonicalize(ground_conj_cond);
        const auto new_ground_conj_cond = persistent_context.destination.get_or_create(ground_conj_cond, persistent_context.builder.get_buffer()).first;

        const auto delta_binding = formalism::datalog::ground(delta_context.binding, delta_context).first;

        return std::make_pair(CostUpdate(old_cost, new_cost), Witness { rule, delta_binding, new_ground_conj_cond });
    }
};

template<OrAnnotationPolicyConcept OrAP, AndAnnotationPolicyConcept AndAP>
struct AnnotationPolicies
{
    OrAP or_ap;
    std::vector<AndAP> and_aps;

    OrAnnotationsList or_annot;
    std::vector<AndAnnotationsMap> and_annots;

    std::vector<HeadToWitness> delta_head_to_witness;
    HeadToWitness program_head_to_witness;

    AnnotationPolicies(OrAP or_ap,
                       std::vector<AndAP> and_aps,
                       OrAnnotationsList or_annot,
                       std::vector<AndAnnotationsMap> and_annots,
                       std::vector<HeadToWitness> delta_head_to_witness) :
        or_ap(std::move(or_ap)),
        and_aps(std::move(and_aps)),
        or_annot(std::move(or_annot)),
        and_annots(std::move(and_annots)),
        delta_head_to_witness(std::move(delta_head_to_witness)),
        program_head_to_witness()
    {
    }

    void clear() noexcept
    {
        for (auto& vec : or_annot)
            vec.clear();
        for (auto& map : and_annots)
            map.clear();
        for (auto& map : delta_head_to_witness)
            map.clear();
        program_head_to_witness.clear();
    }
};

}

#endif
