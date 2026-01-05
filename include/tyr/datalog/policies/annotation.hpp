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
#include "tyr/formalism/binding_index.hpp"
#include "tyr/formalism/datalog/declarations.hpp"
#include "tyr/formalism/datalog/formatter.hpp"
#include "tyr/formalism/datalog/ground_atom_index.hpp"
#include "tyr/formalism/datalog/grounder.hpp"
#include "tyr/formalism/datalog/repository.hpp"
#include "tyr/formalism/datalog/rule_index.hpp"
#include "tyr/formalism/overlay_repository.hpp"

#include <concepts>
#include <optional>
#include <tuple>
#include <vector>

namespace tyr::datalog
{
/**
 * Annotations
 */

using Cost = uint_t;

using OrAnnotationsList = std::vector<std::vector<Cost>>;

/// @brief `Witness` is the rule and binding in the rule delta repository whose ground rule is the witness for its ground atom in the head.
/// The witness lives in the rule delta repository.
struct Witness
{
    Index<formalism::datalog::Rule> rule;
    Index<formalism::Binding> binding;

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

/**
 * Policies
 */

// circle "or"-node
template<typename T>
concept OrAnnotationPolicyConcept = requires(const T& p,
                                             Index<formalism::datalog::Rule> rule,
                                             Index<formalism::datalog::GroundAtom<formalism::FluentTag>> head,
                                             OrAnnotationsList& or_annot,
                                             const AndAnnotationsMap& and_annot,
                                             const HeadToWitness& head_to_witness) {
    /// Annotate the initial cost of the atom.
    { p.initialize_annotation(head, or_annot) } -> std::same_as<void>;
    /// Annotate the cost of the atom from the given witness and annotations.
    { p.update_annotation(rule, head, or_annot, and_annot, head_to_witness) } -> std::same_as<std::pair<std::optional<Cost>, Cost>>;
};

// rectangular "and"-node
template<typename T>
concept AndAnnotationPolicyConcept = requires(const T& p,
                                              Index<formalism::datalog::Rule> rule,
                                              Index<formalism::datalog::Rule> fluent_rule,
                                              const IndexList<formalism::Object>& objects,
                                              Index<formalism::datalog::GroundAtom<formalism::FluentTag>> head,
                                              const OrAnnotationsList& or_annot,
                                              AndAnnotationsMap& and_annot,
                                              HeadToWitness& head_to_witness,
                                              formalism::datalog::GrounderContext<formalism::OverlayRepository<formalism::datalog::Repository>>& rule_context,
                                              formalism::datalog::GrounderContext<formalism::datalog::Repository>& delta_context) {
    /// Ground the witness and annotate the cost of it from the given annotations.
    {
        p.update_annotation(rule, fluent_rule, objects, head, or_annot, and_annot, head_to_witness, rule_context, delta_context)
    } -> std::same_as<std::pair<std::optional<Cost>, Cost>>;
};

class NoOrAnnotationPolicy
{
public:
    static constexpr bool ShouldAnnotate = false;

    void initialize_annotation(Index<formalism::datalog::GroundAtom<formalism::FluentTag>> head, OrAnnotationsList& or_annot) const noexcept {}

    std::pair<std::optional<Cost>, Cost> update_annotation(Index<formalism::datalog::Rule> rule,
                                                           Index<formalism::datalog::GroundAtom<formalism::FluentTag>> head,
                                                           OrAnnotationsList& or_annot,
                                                           const AndAnnotationsMap& and_annot,
                                                           const HeadToWitness& head_to_witness) const noexcept
    {
        return { std::nullopt, Cost(0) };
    }
};

class NoAndAnnotationPolicy
{
public:
    static constexpr bool ShouldAnnotate = false;

    std::pair<std::optional<Cost>, Cost>
    update_annotation(Index<formalism::datalog::Rule> rule,
                      Index<formalism::datalog::Rule> fluent_rule,
                      const IndexList<formalism::Object>& objects,
                      Index<formalism::datalog::GroundAtom<formalism::FluentTag>> head,
                      const OrAnnotationsList& or_annot,
                      AndAnnotationsMap& and_annot,
                      HeadToWitness& head_to_witness,
                      formalism::datalog::GrounderContext<formalism::OverlayRepository<formalism::datalog::Repository>>& rule_context,
                      formalism::datalog::GrounderContext<formalism::datalog::Repository>& delta_context) const noexcept
    {
        return { std::nullopt, Cost(0) };
    }
};

class OrAnnotationPolicy
{
public:
    static constexpr bool ShouldAnnotate = true;

    void initialize_annotation(Index<formalism::datalog::GroundAtom<formalism::FluentTag>> head, OrAnnotationsList& or_annot) const
    {
        resize_or_annot_to_fit(head, or_annot);

        or_annot[uint_t(head.group)][head.value] = Cost(0);
    }

    std::pair<std::optional<Cost>, Cost> update_annotation(Index<formalism::datalog::Rule> rule,
                                                           Index<formalism::datalog::GroundAtom<formalism::FluentTag>> head,
                                                           OrAnnotationsList& or_annot,
                                                           const AndAnnotationsMap& and_annot,
                                                           const HeadToWitness& head_to_witness) const
    {
        resize_or_annot_to_fit(head, or_annot);
        const auto cost = and_annot.at(head_to_witness.at(head));

        /// cost(u) = min({ cost(v_1), ..., cost(v_k) })
        assert(uint_t(head.group) < or_annot.size());
        auto& new_cost = or_annot[uint_t(head.group)][head.value];
        const auto old_cost = new_cost;

        new_cost = std::min(new_cost, cost);

        return { old_cost == std::numeric_limits<Cost>::max() ? std::nullopt : std::optional<Cost>(old_cost), new_cost };
    }

private:
    void resize_or_annot_to_fit(Index<formalism::datalog::GroundAtom<formalism::FluentTag>> head, OrAnnotationsList& or_annot) const
    {
        assert(uint_t(head.group) < or_annot.size());
        if (head.value >= or_annot[uint_t(head.group)].size())
            or_annot[uint_t(head.group)].resize(head.value + 1, std::numeric_limits<Cost>::max());
    }
};

struct SumAggregation
{
    static constexpr Cost identity() noexcept { return Cost(0); }
    constexpr Cost operator()(Cost acc, Cost x) const noexcept { return acc + x; }
};

struct MaxAggregation
{
    static constexpr Cost identity() noexcept { return Cost(0); }
    constexpr Cost operator()(Cost acc, Cost x) const noexcept { return std::max(acc, x); }
};

template<typename AggregationFunction>
class AndAnnotationPolicy
{
public:
    static constexpr bool ShouldAnnotate = true;

    std::pair<std::optional<Cost>, Cost>
    update_annotation(Index<formalism::datalog::Rule> rule,
                      Index<formalism::datalog::Rule> fluent_rule,
                      const IndexList<formalism::Object>& objects,
                      Index<formalism::datalog::GroundAtom<formalism::FluentTag>> head,
                      const OrAnnotationsList& or_annot,
                      AndAnnotationsMap& and_annot,
                      HeadToWitness& head_to_witness,
                      formalism::datalog::GrounderContext<formalism::OverlayRepository<formalism::datalog::Repository>>& rule_context,
                      formalism::datalog::GrounderContext<formalism::datalog::Repository>& delta_context) const
    {
        /// Ground binding in delta to remain persistent across iterations.
        const auto binding = formalism::datalog::ground(objects, delta_context).first;
        /// Ground fluent rule in rule to use ground atom identifies to fetch costs.
        const auto ground_fluent_rule = formalism::datalog::ground(make_view(fluent_rule, rule_context.destination), rule_context).first;
        const auto ground_fluent_rule_view = make_view(ground_fluent_rule, rule_context.destination);

        /// cost(u) = cost(v_1) o ... o cost(v_k)
        AggregationFunction agg {};
        auto cost = AggregationFunction::identity();

        for (const auto& literal : ground_fluent_rule_view.get_body().get_literals<formalism::FluentTag>())
        {
            if (literal.get_polarity())
            {
                const auto atom_index = literal.get_atom().get_index();
                assert(uint_t(atom_index.group) < or_annot.size());
                assert(atom_index.value < or_annot[uint_t(atom_index.group)].size());
                assert(or_annot[uint_t(atom_index.group)][atom_index.value] != std::numeric_limits<Cost>::max());
                cost = agg(cost, or_annot[uint_t(atom_index.group)][atom_index.value]);
            }
        }
        /// Add cost of rule itself.
        cost += make_view(rule, rule_context.destination).get_cost();

        const auto witness = Witness { rule, binding };

        auto [it, inserted] = and_annot.try_emplace(witness, std::numeric_limits<Cost>::max());
        const auto old_cost = it->second;

        // Update per-witness cost
        if (cost < it->second)
            it->second = cost;

        // Update best witness for head (if better than current best)
        auto hit = head_to_witness.find(head);
        if (hit == head_to_witness.end() || it->second < and_annot.at(hit->second))
            head_to_witness[head] = witness;

        return { inserted ? std::nullopt : std::optional<Cost>(old_cost), it->second };
    }
};

template<OrAnnotationPolicyConcept OrAP, AndAnnotationPolicyConcept AndAP>
struct AnnotationPolicies
{
    OrAP or_ap;
    std::vector<AndAP> and_aps;

    OrAnnotationsList or_annot;
    std::vector<AndAnnotationsMap> and_annots;

    std::vector<HeadToWitness> head_to_witness;

    AnnotationPolicies(OrAP or_ap,
                       std::vector<AndAP> and_aps,
                       OrAnnotationsList or_annot,
                       std::vector<AndAnnotationsMap> and_annots,
                       std::vector<HeadToWitness> head_to_witness) :
        or_ap(std::move(or_ap)),
        and_aps(std::move(and_aps)),
        or_annot(std::move(or_annot)),
        and_annots(std::move(and_annots)),
        head_to_witness(std::move(head_to_witness))
    {
    }

    void clear() noexcept
    {
        for (auto& pred_or_annot : or_annot)
            pred_or_annot.clear();
        for (auto& and_annot : and_annots)
            and_annot.clear();

        head_to_witness.clear();
    }
};

}

#endif
