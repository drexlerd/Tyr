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
#include "tyr/formalism/binding_index.hpp"
#include "tyr/formalism/datalog/declarations.hpp"
#include "tyr/formalism/datalog/ground_atom_index.hpp"
#include "tyr/formalism/datalog/grounder.hpp"
#include "tyr/formalism/datalog/repository.hpp"
#include "tyr/formalism/datalog/rule_index.hpp"
#include "tyr/formalism/overlay_repository.hpp"

#include <concepts>
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
concept OrAnnotationPolicyConcept = requires(T& p,
                                             Index<formalism::datalog::Rule> rule,
                                             Index<formalism::datalog::GroundAtom<formalism::FluentTag>> head,
                                             OrAnnotationsList& or_annot,
                                             const AndAnnotationsMap& and_annot,
                                             const HeadToWitness& head_to_witness) {
    /// Annotate the initial cost of the atom.
    { p.annotate(head, or_annot) } -> std::same_as<void>;
    /// Annotate the cost of the atom from the given witness and annotations.
    { p.annotate(rule, head, or_annot, and_annot, head_to_witness) } -> std::same_as<void>;
    /// Clear the policy for reuse.
    { p.clear() } -> std::same_as<void>;
};

// rectangular "and"-node
template<typename T>
concept AndAnnotationPolicyConcept = requires(T& p,
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
    { p.annotate(rule, fluent_rule, objects, head, or_annot, and_annot, head_to_witness, rule_context, delta_context) } -> std::same_as<void>;
    /// Clear the policy for reuse.
    { p.clear() } -> std::same_as<void>;
};

class NoOrAnnotationPolicy
{
public:
    static constexpr bool ShouldAnnotate = false;

    void annotate(Index<formalism::datalog::GroundAtom<formalism::FluentTag>> head, OrAnnotationsList& or_annot) noexcept {}

    void annotate(Index<formalism::datalog::Rule> rule,
                  Index<formalism::datalog::GroundAtom<formalism::FluentTag>> head,
                  OrAnnotationsList& or_annot,
                  const AndAnnotationsMap& and_annot,
                  const HeadToWitness& head_to_witness) noexcept
    {
    }

    void clear() noexcept {}
};

class NoAndAnnotationPolicy
{
public:
    static constexpr bool ShouldAnnotate = false;

    void annotate(Index<formalism::datalog::Rule> rule,
                  Index<formalism::datalog::Rule> fluent_rule,
                  const IndexList<formalism::Object>& objects,
                  Index<formalism::datalog::GroundAtom<formalism::FluentTag>> head,
                  const OrAnnotationsList& or_annot,
                  AndAnnotationsMap& and_annot,
                  HeadToWitness& head_to_witness,
                  formalism::datalog::GrounderContext<formalism::OverlayRepository<formalism::datalog::Repository>>& rule_context,
                  formalism::datalog::GrounderContext<formalism::datalog::Repository>& delta_context) noexcept
    {
    }

    void clear() noexcept {}
};

class OrAnnotationPolicy
{
public:
    static constexpr bool ShouldAnnotate = true;

    explicit OrAnnotationPolicy(const formalism::datalog::Repository& repository) : m_repository(repository) {}

    void annotate(Index<formalism::datalog::GroundAtom<formalism::FluentTag>> head, OrAnnotationsList& or_annot)
    {
        resize_or_annot_to_fit(head, or_annot);

        or_annot[uint_t(head.group)][head.value] = Cost(0);
    }

    void annotate(Index<formalism::datalog::Rule> rule,
                  Index<formalism::datalog::GroundAtom<formalism::FluentTag>> head,
                  OrAnnotationsList& or_annot,
                  const AndAnnotationsMap& and_annot,
                  const HeadToWitness& head_to_witness) noexcept
    {
        resize_or_annot_to_fit(head, or_annot);

        const auto rule_cost = make_view(rule, m_repository).get_cost();
        const auto annot_cost = and_annot.at(head_to_witness.at(head));

        /// cost(u) = min({ cost(v_1), ..., cost(v_k) })
        auto& annot = or_annot[uint_t(head.group)][head.value];
        annot = std::min(annot, rule_cost + annot_cost);
    }

    void clear() noexcept {}

private:
    void resize_or_annot_to_fit(Index<formalism::datalog::GroundAtom<formalism::FluentTag>> head, OrAnnotationsList& or_annot)
    {
        if (uint_t(head.group) >= or_annot.size())
            or_annot.resize(uint_t(head.group) + 1);
        if (head.value >= or_annot[uint_t(head.group)].size())
            or_annot[uint_t(head.group)].resize(head.value + 1, std::numeric_limits<Cost>::max());
    }

private:
    const formalism::datalog::Repository& m_repository;
};

struct SumAggregation
{
    static constexpr Cost identity() noexcept { return Cost(0); }
    constexpr Cost operator()(Cost acc, Cost x) const noexcept { return acc + x; }
};

struct MaxAggregation
{
    static constexpr Cost identity() noexcept { return std::numeric_limits<Cost>::min(); }
    constexpr Cost operator()(Cost acc, Cost x) const noexcept { return std::max(acc, x); }
};

template<typename AggregationFunction>
class AndAnnotationPolicy
{
public:
    static constexpr bool ShouldAnnotate = true;

    void annotate(Index<formalism::datalog::Rule> rule,
                  Index<formalism::datalog::Rule> fluent_rule,
                  const IndexList<formalism::Object>& objects,
                  Index<formalism::datalog::GroundAtom<formalism::FluentTag>> head,
                  const OrAnnotationsList& or_annot,
                  AndAnnotationsMap& and_annot,
                  HeadToWitness& head_to_witness,
                  formalism::datalog::GrounderContext<formalism::OverlayRepository<formalism::datalog::Repository>>& rule_context,
                  formalism::datalog::GrounderContext<formalism::datalog::Repository>& delta_context) noexcept
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
                cost = agg(cost, or_annot[uint_t(atom_index.group)][atom_index.value]);
            }
        }

        const auto witness = Witness { rule, binding };

        // Update per-witness cost
        auto [it, inserted] = and_annot.try_emplace(witness, cost);
        if (!inserted && cost < it->second)
            it->second = cost;

        // Update best witness for head (if better than current best)
        auto hit = head_to_witness.find(head);
        if (hit == head_to_witness.end() || it->second < and_annot.at(hit->second))
            head_to_witness[head] = witness;
    }

    void clear() noexcept {}
};

template<OrAnnotationPolicyConcept OrAP, AndAnnotationPolicyConcept AndAP>
struct AnnotationPolicies
{
    OrAP or_ap;
    std::vector<AndAP> and_aps;

    OrAnnotationsList or_annot;
    std::vector<AndAnnotationsMap> and_annots;

    std::vector<HeadToWitness> head_to_witnesss;

    AnnotationPolicies(OrAP or_ap,
                       std::vector<AndAP> and_aps,
                       OrAnnotationsList or_annot,
                       std::vector<AndAnnotationsMap> and_annots,
                       std::vector<HeadToWitness> head_to_witnesss) :
        or_ap(std::move(or_ap)),
        and_aps(std::move(and_aps)),
        or_annot(std::move(or_annot)),
        and_annots(std::move(and_annots)),
        head_to_witnesss(std::move(head_to_witnesss))
    {
    }

    void clear() noexcept
    {
        or_ap.clear();
        for (auto& and_ap : and_aps)
            and_ap.clear();

        for (auto& pred_or_annot : or_annot)
            pred_or_annot.clear();
        for (auto& and_annot : and_annots)
            and_annot.clear();

        head_to_witnesss.clear();
    }
};

}

#endif
