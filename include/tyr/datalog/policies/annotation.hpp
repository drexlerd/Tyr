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
#include "tyr/formalism/datalog/declarations.hpp"
#include "tyr/formalism/datalog/formatter.hpp"
#include "tyr/formalism/datalog/ground_atom_index.hpp"
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
             Index<formalism::datalog::Rule> rule,
             Index<formalism::datalog::ConjunctiveCondition> witness_condition,
             Index<formalism::datalog::GroundAtom<formalism::FluentTag>> delta_head,
             const OrAnnotationsList& or_annot,
             AndAnnotationsMap& and_annot,
             HeadToWitness& delta_head_to_witness,
             formalism::datalog::GrounderContext<formalism::OverlayRepository<formalism::datalog::Repository>>& iteration_context,
             formalism::datalog::GrounderContext<formalism::datalog::Repository>& delta_context,
             formalism::datalog::GrounderContext<formalism::OverlayRepository<formalism::datalog::Repository>>& persistent_context) {
        /// Ground the witness and annotate the cost of it from the given annotations.
        {
            p.update_annotation(rule,
                                witness_condition,
                                delta_head,
                                or_annot,
                                and_annot,
                                delta_head_to_witness,
                                iteration_context,
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
    update_annotation(Index<formalism::datalog::Rule> rule,
                      Index<formalism::datalog::ConjunctiveCondition> witness_condition,
                      Index<formalism::datalog::GroundAtom<formalism::FluentTag>> head,
                      const OrAnnotationsList& or_annot,
                      AndAnnotationsMap& and_annot,
                      HeadToWitness& head_to_witness,
                      formalism::datalog::GrounderContext<formalism::OverlayRepository<formalism::datalog::Repository>>& iteration_context,
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

        const auto [witness, and_cost] = fetch_best_head_witness_cost(delta_head, and_annot, delta_head_to_witness);

        auto& or_cost = or_annot[uint_t(program_head.group)][program_head.value];
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

    static std::pair<Witness, Cost> fetch_best_head_witness_cost(Index<formalism::datalog::GroundAtom<formalism::FluentTag>> delta_head,
                                                                 const AndAnnotationsMap& and_annot,
                                                                 const HeadToWitness& delta_head_to_witness)
    {
        const auto it_w = delta_head_to_witness.find(delta_head);
        assert(it_w != delta_head_to_witness.end());

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

    CostUpdate update_annotation(Index<formalism::datalog::Rule> rule,
                                 Index<formalism::datalog::ConjunctiveCondition> witness_condition,
                                 Index<formalism::datalog::GroundAtom<formalism::FluentTag>> delta_head,
                                 const OrAnnotationsList& or_annot,
                                 AndAnnotationsMap& and_annot,
                                 HeadToWitness& delta_head_to_witness,
                                 formalism::datalog::GrounderContext<formalism::OverlayRepository<formalism::datalog::Repository>>& iteration_context,
                                 formalism::datalog::GrounderContext<formalism::datalog::Repository>& delta_context,
                                 formalism::datalog::GrounderContext<formalism::OverlayRepository<formalism::datalog::Repository>>& persistent_context) const
    {
        assert(iteration_context.binding == delta_context.binding);
        assert(delta_context.binding == persistent_context.binding);

        const auto witness = ground_witness(rule, witness_condition, delta_context, persistent_context);

        auto cost = compute_aggregate_witness_condition_cost(witness.witness_condition, or_annot, persistent_context);

        /// Add cost of rule itself.
        cost += make_view(rule, iteration_context.destination).get_cost();

        auto [it, inserted] = and_annot.try_emplace(witness, std::numeric_limits<Cost>::max());

        const auto cost_update = update_min_cost(it->second, cost);

        update_if_better_witness_for_head(delta_head, witness, it->second, and_annot, delta_head_to_witness);

        return cost_update;
    }

private:
    /// @brief Grounds two components to create the witness:
    /// 1) the binding into the persistent delta context, allowing merge into program when necessary.
    /// 2) the witness condition into the program context, which is sound, be
    static Witness ground_witness(Index<formalism::datalog::Rule> rule,
                                  Index<formalism::datalog::ConjunctiveCondition> witness_condition,
                                  formalism::datalog::GrounderContext<formalism::datalog::Repository>& delta_context,
                                  formalism::datalog::GrounderContext<formalism::OverlayRepository<formalism::datalog::Repository>>& persistent_context)
    {
        return Witness { rule,
                         formalism::datalog::ground(delta_context.binding, delta_context).first,
                         formalism::datalog::ground(make_view(witness_condition, persistent_context.destination), persistent_context).first };
    }

    /// @brief cost(u) = cost(v_1) o ... o cost(v_k)
    static Cost compute_aggregate_witness_condition_cost(
        Index<formalism::datalog::GroundConjunctiveCondition> witness_condition,
        const OrAnnotationsList& or_annot,
        const formalism::datalog::GrounderContext<formalism::OverlayRepository<formalism::datalog::Repository>>& persistent_context)
    {
        auto cost = AggregationFunction::identity();

        for (const auto& literal : make_view(witness_condition, persistent_context.destination).get_literals<formalism::FluentTag>())
        {
            if (!literal.get_polarity())
                continue;

            const auto atom_index = literal.get_atom().get_index();

            assert(uint_t(atom_index.group) < or_annot.size());
            assert(atom_index.value < or_annot[uint_t(atom_index.group)].size());
            assert(or_annot[uint_t(atom_index.group)][atom_index.value] != std::numeric_limits<Cost>::max());

            cost = agg(cost, or_annot[uint_t(atom_index.group)][atom_index.value]);
        }

        return cost;
    }

    /// @brief Update per-witness cost (min)
    static CostUpdate update_min_cost(Cost& slot, Cost candidate)
    {
        const Cost old = slot;
        if (candidate < slot)
            slot = candidate;
        return CostUpdate(old, slot);
    }

    /// @brief Update best witness for head based on updated witness cost
    static void update_if_better_witness_for_head(Index<formalism::datalog::GroundAtom<formalism::FluentTag>> delta_head,
                                                  const Witness& candidate,
                                                  Cost candidate_cost,
                                                  const AndAnnotationsMap& and_annot,
                                                  HeadToWitness& delta_head_to_witness)
    {
        auto [hit, inserted] = delta_head_to_witness.try_emplace(delta_head, candidate);
        if (inserted)
            return;

        const auto it_best = and_annot.find(hit->second);
        assert(it_best != and_annot.end());

        if (candidate_cost < it_best->second)
            hit->second = candidate;
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
