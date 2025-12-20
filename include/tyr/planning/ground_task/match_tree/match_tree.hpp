/*
 * Copyright (C) 2023 Dominik Drexler and Simon Stahlberg
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

#ifndef TYR_PLANNING_GROUND_TASK_MATCH_TREE_MATCH_TREE_HPP_
#define TYR_PLANNING_GROUND_TASK_MATCH_TREE_MATCH_TREE_HPP_

#include "tyr/formalism/declarations.hpp"
#include "tyr/planning/declarations.hpp"
#include "tyr/planning/ground_task/match_tree/builder.hpp"
#include "tyr/planning/ground_task/match_tree/declarations.hpp"
#include "tyr/planning/ground_task/match_tree/nodes/node_data.hpp"
#include "tyr/planning/ground_task/match_tree/repository.hpp"
#include "tyr/planning/ground_task/unpacked_state.hpp"

#include <deque>

namespace tyr::planning::match_tree
{

using PreconditionVariant = std::variant<Index<formalism::GroundAtom<formalism::DerivedTag>>,
                                         Index<formalism::FDRVariable<formalism::FluentTag>>,
                                         Data<formalism::BooleanOperator<Data<formalism::GroundFunctionExpression>>>>;

template<typename Tag>
struct Occurrence
{
    Index<Tag> element;
    std::variant<std::monostate, bool, formalism::FDRValue> detail;
};

template<typename Tag>
using PostingList = std::vector<Occurrence<Tag>>;

template<typename Tag>
using PreconditionOccurences = UnorderedMap<PreconditionVariant, PostingList<Tag>>;

template<typename Tag>
using PreconditionDetails = UnorderedMap<Index<Tag>, UnorderedMap<PreconditionVariant, std::variant<bool, formalism::FDRValue>>>;

template<typename Tag>
struct BaseEntry
{
    size_t depth;
    std::span<Index<Tag>> elements;

    BaseEntry(size_t depth, std::span<Index<Tag>> elements) : depth(depth), elements(elements) {}
};

template<typename Tag>
struct AtomStackEntry
{
    BaseEntry<Tag> base;

    Index<formalism::GroundAtom<formalism::DerivedTag>> atom;
    std::span<Index<Tag>> true_elements;
    std::span<Index<Tag>> false_elements;
    std::span<Index<Tag>> dontcare_elements;

    std::optional<Data<Node<Tag>>> true_child;
    std::optional<Data<Node<Tag>>> false_child;
    std::optional<Data<Node<Tag>>> dontcare_child;

    AtomStackEntry(BaseEntry<Tag> base,
                   Index<formalism::GroundAtom<formalism::DerivedTag>> atom,
                   std::span<Index<Tag>> true_elements,
                   std::span<Index<Tag>> false_elements,
                   std::span<Index<Tag>> dontcare_elements) :
        base(base),
        atom(atom),
        true_elements(true_elements),
        false_elements(false_elements),
        dontcare_elements(dontcare_elements),
        true_child(std::nullopt),
        false_child(std::nullopt),
        dontcare_child(std::nullopt)
    {
    }
};

template<typename Tag>
struct FactStackEntry
{
    BaseEntry<Tag> base;

    Index<formalism::FDRVariable<formalism::FluentTag>> fact;
    std::vector<uint_t> fact_to_child;
    std::vector<std::span<Index<Tag>>> child_elements;
    std::span<Index<Tag>> dontcare_elements;

    std::vector<Data<Node<Tag>>> children;
    std::optional<Data<Node<Tag>>> dontcare_child;

    FactStackEntry(BaseEntry<Tag> base,
                   std::vector<uint_t> fact_to_child,
                   std::vector<std::span<Index<Tag>>> child_elements,
                   Index<formalism::FDRVariable<formalism::FluentTag>> fact) :
        base(base),
        fact(fact),
        fact_to_child(std::move(fact_to_child)),
        child_elements(std::move(child_elements)),
        children(),
        dontcare_child(std::nullopt)
    {
    }
};

template<typename Tag>
struct ConstraintStackEntry
{
    BaseEntry<Tag> base;

    Data<formalism::BooleanOperator<Data<formalism::GroundFunctionExpression>>> constraint;
    std::span<Index<Tag>> true_elements;
    std::span<Index<Tag>> dontcare_elements;

    std::optional<Data<Node<Tag>>> true_child;
    std::optional<Data<Node<Tag>>> dontcare_child;

    bool need_true;
    bool need_dontcare;

    ConstraintStackEntry(BaseEntry<Tag> base,
                         Data<formalism::BooleanOperator<Data<formalism::GroundFunctionExpression>>> constraint,
                         std::span<Index<Tag>> true_elements,
                         std::span<Index<Tag>> dontcare_elements) :
        base(base),
        constraint(constraint),
        true_elements(true_elements),
        dontcare_elements(dontcare_elements),
        true_child(std::nullopt),
        dontcare_child(std::nullopt)
    {
    }
};

template<typename Tag>
struct GeneratorStackEntry
{
    BaseEntry<Tag> base;

    std::span<Index<Tag>> elements;

    std::optional<Data<Node<Tag>>> node;

    explicit GeneratorStackEntry(BaseEntry<Tag> base, std::span<Index<Tag>> elements) : base(base), elements(elements), node(std::nullopt) {}
};

template<typename Tag>
using StackEntry = std::variant<AtomStackEntry<Tag>, FactStackEntry<Tag>, ConstraintStackEntry<Tag>, GeneratorStackEntry<Tag>>;

template<typename Tag>
bool explored(const AtomStackEntry<Tag>& el) noexcept
{
    return (el.true_elements.empty() || el.true_child.has_value()) && (el.false_elements.empty() || el.false_child.has_value())
           && (el.dontcare_elements.empty() || el.dontcare_child.has_value());
}

template<typename Tag>
std::optional<StackEntry<Tag>> next_entry(const AtomStackEntry<Tag>& element)
{
}

template<typename Tag>
Data<Node<Tag>> get_result(const AtomStackEntry<Tag>& element)
{
}

template<typename Tag>
void push_result(AtomStackEntry<Tag>& element, Data<Node<Tag>> node)
{
}

template<typename Tag>
bool explored(const FactStackEntry<Tag>& el) noexcept
{
    return el.children.size() == el.fact_to_child.size() && (el.dontcare_elements.empty() || el.dontcare_child.has_value());
}

template<typename Tag>
std::optional<StackEntry<Tag>> next_entry(const FactStackEntry<Tag>& element)
{
}

template<typename Tag>
Data<Node<Tag>> get_result(const FactStackEntry<Tag>& element)
{
}

template<typename Tag>
void push_result(FactStackEntry<Tag>& element, Data<Node<Tag>> node)
{
}

template<typename Tag>
bool explored(const ConstraintStackEntry<Tag>& el) noexcept
{
    return (el.true_elements.empty() || el.true_child.has_value()) && (el.dontcare_elements.empty() || el.dontcare_child.has_value());
}

template<typename Tag>
std::optional<StackEntry<Tag>> next_entry(const ConstraintStackEntry<Tag>& element)
{
}

template<typename Tag>
Data<Node<Tag>> get_result(const ConstraintStackEntry<Tag>& element)
{
}

template<typename Tag>
void push_result(ConstraintStackEntry<Tag>& element, Data<Node<Tag>> node)
{
}

template<typename Tag>
bool explored(const GeneratorStackEntry<Tag>& el) noexcept
{
    return el.node.has_value();
}

template<typename Tag>
std::optional<StackEntry<Tag>> next_entry(const GeneratorStackEntry<Tag>& element)
{
    return std::nullopt;
}

template<typename Tag>
Data<Node<Tag>> get_result(const GeneratorStackEntry<Tag>& element)
{
}

template<typename Tag>
void push_result(GeneratorStackEntry<Tag>& element, Data<Node<Tag>> node)
{
}

template<formalism::Context C>
auto get_condition(View<Index<formalism::GroundAxiom>, C> element)
{
    return element.get_body();
}

template<formalism::Context C>
auto get_condition(View<Index<formalism::GroundAction>, C> element)
{
    return element.get_condition();
}

template<typename Tag>
static std::optional<StackEntry<Tag>>
try_create_atom_stack_entry(Index<formalism::GroundAtom<formalism::DerivedTag>> atom, BaseEntry<Tag> base, const PreconditionDetails<Tag>& details)
{
}

template<typename Tag>
static std::optional<StackEntry<Tag>>
try_create_fact_stack_entry(Index<formalism::FDRVariable<formalism::FluentTag>> variable, BaseEntry<Tag> base, const PreconditionDetails<Tag>& details)
{
}

template<typename Tag>
static std::optional<StackEntry<Tag>> try_create_constraint_stack_entry(Data<formalism::BooleanOperator<Data<formalism::GroundFunctionExpression>>> constraint,
                                                                        BaseEntry<Tag> base,
                                                                        const PreconditionDetails<Tag>& details)
{
}

template<typename Tag>
static StackEntry<Tag> create_generator_stack_entry(BaseEntry<Tag> base)
{
}

template<typename Tag>
static std::optional<StackEntry<Tag>> try_create_selector_stack_entry(BaseEntry<Tag> base,
                                                                      const std::vector<std::pair<PreconditionVariant, PostingList<Tag>>>& sorted_preconditions,
                                                                      const PreconditionDetails<Tag>& details)
{
    return std::visit(
        [&](auto&& arg)
        {
            using Alternative = std::decay_t<decltype(arg)>;

            if constexpr (std::same_as<Alternative, Index<formalism::FDRVariable<formalism::FluentTag>>>)
                return try_create_fact_stack_entry(arg, base, details);
            else if constexpr (std::same_as<Alternative, Index<formalism::GroundAtom<formalism::DerivedTag>>>)
                return try_create_atom_stack_entry(arg, base, details);
            else if constexpr (std::same_as<Alternative, Data<formalism::BooleanOperator<Data<formalism::GroundFunctionExpression>>>>)
                return try_create_constraint_stack_entry(arg, base, details);
            else
                static_assert(dependent_false<Alternative>::value, "Missing case");
        },
        sorted_preconditions[base.depth].first);
}

template<typename Tag>
static StackEntry<Tag> create_stack_entry(BaseEntry<Tag> base,
                                          const std::vector<std::pair<PreconditionVariant, PostingList<Tag>>>& sorted_preconditions,
                                          const PreconditionDetails<Tag>& details)
{
    if (!base.elements.empty())
        for (; base.depth < sorted_preconditions.size(); ++base.depth)
            if (auto entry = try_create_selector_stack_entry(base, sorted_preconditions, details))
                return entry.value();

    return create_generator_stack_entry(base);
}

template<typename Tag>
class MatchTree
{
private:
    IndexList<Tag> m_elements;

    RepositoryPtr<formalism::OverlayRepository<formalism::Repository>, Tag> m_context;

    Data<Node<Tag>> m_root;

    std::vector<Data<Node<Tag>>> m_evaluate_stack;  ///< temporary during evaluation.

public:
    template<formalism::Context C>
    MatchTree(View<IndexList<Tag>, C> elements) :
        m_elements(elements.get_data()),
        m_context(std::make_unique<Repository<formalism::OverlayRepository<formalism::Repository>, Tag>>(elements.get_context())),
        m_root(),
        m_evaluate_stack()
    {
        auto occurences = PreconditionOccurences<Tag> {};
        auto details = PreconditionDetails<Tag> {};

        for (const auto element : elements)
        {
            const auto condition = get_condition(element);

            for (const auto fact : condition.template get_facts<formalism::FluentTag>())
            {
                const auto key = fact.get_variable().get_index();
                occurences[key].push_back({ element.get_index(), fact.get_value() });
            }

            for (const auto literal : condition.template get_facts<formalism::DerivedTag>())
            {
                const auto key = literal.get_atom().get_index();
                occurences[key].push_back({ element.get_index(), literal.get_polarity() });
            }

            for (const auto constraint : condition.get_numeric_constraints())
            {
                const auto key = constraint.get_data();
                occurences[key].push_back({ element.get_index(), std::monostate {} });
            }
        }

        std::vector<std::pair<PreconditionVariant, PostingList<Tag>>> sorted_preconditions(occurences.begin(), occurences.end());

        std::sort(sorted_preconditions.begin(), sorted_preconditions.end(), [](const auto& a, const auto& b) { return a.second.size() > b.second.size(); });

        auto stack = std::deque<StackEntry<Tag>> {};
        stack.emplace_back(create_stack_entry(BaseEntry<Tag>(size_t(0), std::span(m_elements.begin(), m_elements.end())), sorted_preconditions, details));

        while (!stack.empty())
        {
            auto& entry = stack.back();

            std::optional<Data<Node<Tag>>> produced;
            std::optional<StackEntry<Tag>> next;

            std::visit(
                [&](auto& frame)
                {
                    if (!explored(frame))
                        next = next_entry(frame);
                    else
                        produced = get_result(frame);
                },
                entry);

            if (next)
            {
                stack.push_back(std::move(next.value()));
                continue;
            }

            stack.pop_back();

            if (stack.empty())
                m_root = std::move(*produced);
            else
                std::visit([&](auto& parent) { push_result(parent, std::move(produced.value())); }, stack.back());
        }
    }

    template<formalism::Context C>
    static MatchTreePtr<Tag> create(View<IndexList<Tag>, C> elements)
    {
        return std::make_unique<MatchTree<Tag>>(elements);
    }

    // Uncopieable and unmoveable to prohibit invalidating spans on m_elements.
    MatchTree(const MatchTree& other) = delete;
    MatchTree& operator=(const MatchTree& other) = delete;
    MatchTree(MatchTree&& other) = delete;
    MatchTree& operator=(MatchTree&& other) = delete;

    void generate_applicable_elements_iteratively(const UnpackedState<GroundTask>& state, IndexList<Tag>& out_applicable_elements);
};

}

#endif
