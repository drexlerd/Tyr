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

#include "tyr/buffer/declarations.hpp"
#include "tyr/formalism/declarations.hpp"
#include "tyr/formalism/formatter.hpp"
#include "tyr/planning/declarations.hpp"
#include "tyr/planning/ground_task/match_tree/canonicalization.hpp"
#include "tyr/planning/ground_task/match_tree/declarations.hpp"
#include "tyr/planning/ground_task/match_tree/nodes/node_data.hpp"
#include "tyr/planning/ground_task/match_tree/repository.hpp"
#include "tyr/planning/ground_task/unpacked_state.hpp"

#include <deque>
#include <iostream>

namespace tyr::planning::match_tree
{

using PreconditionVariant = std::variant<Index<formalism::GroundAtom<formalism::DerivedTag>>,
                                         Index<formalism::FDRVariable<formalism::FluentTag>>,
                                         Data<formalism::BooleanOperator<Data<formalism::GroundFunctionExpression>>>>;

template<typename Tag>
using PreconditionOccurences = UnorderedMap<PreconditionVariant, IndexList<Tag>>;

template<typename Tag>
using PreconditionDetails = UnorderedMap<Index<Tag>, UnorderedMap<PreconditionVariant, std::variant<std::monostate, bool, formalism::FDRValue>>>;

template<typename Tag>
struct BaseEntry
{
    size_t depth;
    std::span<Index<Tag>> elements;

    BaseEntry(size_t depth, std::span<Index<Tag>> elements) : depth(depth), elements(elements) {}
};

template<typename Tag>
struct AtomStackEntry;

template<typename Tag>
struct VariableStackEntry;

template<typename Tag>
struct ConstraintStackEntry;

template<typename Tag>
struct GeneratorStackEntry;

template<typename Tag>
using StackEntry = std::variant<AtomStackEntry<Tag>, VariableStackEntry<Tag>, ConstraintStackEntry<Tag>, GeneratorStackEntry<Tag>>;

template<typename Tag, formalism::Context C>
static std::optional<StackEntry<Tag>> try_create_stack_entry(BaseEntry<Tag> base,
                                                             const std::vector<std::pair<PreconditionVariant, IndexList<Tag>>>& sorted_preconditions,
                                                             const PreconditionDetails<Tag>& details,
                                                             const C& context);

template<typename Tag>
struct GetResultContext
{
    Repository<formalism::OverlayRepository<formalism::Repository>, Tag>& destination;
    buffer::Buffer& buffer;
};

template<typename Tag>
struct AtomStackEntry
{
    BaseEntry<Tag> base;

    std::span<Index<Tag>> true_elements;
    std::span<Index<Tag>> false_elements;
    std::span<Index<Tag>> dontcare_elements;

    Data<AtomSelectorNode<Tag>> result;

    AtomStackEntry(BaseEntry<Tag> base,
                   Index<formalism::GroundAtom<formalism::DerivedTag>> atom,
                   std::span<Index<Tag>> true_elements,
                   std::span<Index<Tag>> false_elements,
                   std::span<Index<Tag>> dontcare_elements) :
        base(base),
        true_elements(true_elements),
        false_elements(false_elements),
        dontcare_elements(dontcare_elements),
        result()
    {
        result.atom = atom;
    }

    bool explored_true_child() const noexcept { return (true_elements.empty() || result.true_child.has_value()); }
    bool explored_false_child() const noexcept { return (false_elements.empty() || result.false_child.has_value()); }
    bool explored_dontcare_child() const noexcept { return (dontcare_elements.empty() || result.dontcare_child.has_value()); }
};

template<typename Tag>
struct VariableStackEntry
{
    BaseEntry<Tag> base;

    std::vector<std::span<Index<Tag>>> domain_elements;
    std::vector<uint_t> forward;
    std::span<Index<Tag>> dontcare_elements;
    size_t forward_pos;

    Data<VariableSelectorNode<Tag>> result;

    VariableStackEntry(BaseEntry<Tag> base,
                       Index<formalism::FDRVariable<formalism::FluentTag>> variable,
                       std::vector<std::span<Index<Tag>>> domain_elements_,
                       std::vector<uint_t> forward_,
                       std::span<Index<Tag>> dontcare_elements) :
        base(base),
        domain_elements(std::move(domain_elements_)),
        forward(std::move(forward_)),
        dontcare_elements(dontcare_elements),
        forward_pos(0),
        result()
    {
        result.variable = variable;
        result.domain_children.resize(domain_elements.size());
    }

    bool explored_children() const noexcept { return forward_pos == forward.size(); }
    bool explored_dontcare_child() const noexcept { return (dontcare_elements.empty() || result.dontcare_child.has_value()); }
};

template<typename Tag>
struct ConstraintStackEntry
{
    BaseEntry<Tag> base;

    Data<formalism::BooleanOperator<Data<formalism::GroundFunctionExpression>>> constraint;
    std::span<Index<Tag>> true_elements;
    std::span<Index<Tag>> dontcare_elements;

    Data<NumericConstraintSelectorNode<Tag>> result;

    ConstraintStackEntry(BaseEntry<Tag> base,
                         Data<formalism::BooleanOperator<Data<formalism::GroundFunctionExpression>>> constraint,
                         std::span<Index<Tag>> true_elements,
                         std::span<Index<Tag>> dontcare_elements) :
        base(base),
        constraint(constraint),
        true_elements(true_elements),
        dontcare_elements(dontcare_elements),
        result()
    {
        result.constraint = constraint;
    }

    bool explored_true_child() const noexcept { return (true_elements.empty() || result.true_child.has_value()); }
    bool explored_dontcare_child() const noexcept { return (dontcare_elements.empty() || result.dontcare_child.has_value()); }
};

template<typename Tag>
struct GeneratorStackEntry
{
    BaseEntry<Tag> base;

    Data<planning::match_tree::ElementGeneratorNode<Tag>> result;

    explicit GeneratorStackEntry(BaseEntry<Tag> base) : base(base), result()
    {
        result.elements.insert(result.elements.end(), base.elements.begin(), base.elements.end());

        std::cout << "Num elements in generator node: " << result.elements.size() << std::endl;
    }
};

template<typename Tag>
bool explored(const AtomStackEntry<Tag>& el) noexcept
{
    return el.explored_true_child() && el.explored_false_child() && el.explored_dontcare_child();
}

template<typename Tag, formalism::Context C>
std::optional<StackEntry<Tag>> next_entry(const AtomStackEntry<Tag>& el,
                                          const std::vector<std::pair<PreconditionVariant, IndexList<Tag>>>& sorted_preconditions,
                                          const PreconditionDetails<Tag>& details,
                                          const C& context)
{
    if (!el.explored_true_child())
        return try_create_stack_entry(BaseEntry<Tag> { el.base.depth + 1, el.true_elements }, sorted_preconditions, details, context);
    else if (!el.explored_false_child())
        return try_create_stack_entry(BaseEntry<Tag> { el.base.depth + 1, el.false_elements }, sorted_preconditions, details, context);
    else if (!el.explored_dontcare_child())
        return try_create_stack_entry(BaseEntry<Tag> { el.base.depth + 1, el.dontcare_elements }, sorted_preconditions, details, context);
    else
        throw std::logic_error("Unexpected case.");
}

template<typename Tag>
Data<Node<Tag>> get_result(AtomStackEntry<Tag>& el, GetResultContext<Tag>& context)
{
    canonicalize(el.result);
    return Data<Node<Tag>>(context.destination.get_or_create(el.result, context.buffer).first);
}

template<typename Tag>
void push_result(AtomStackEntry<Tag>& el, Data<Node<Tag>> node)
{
    if (!el.explored_true_child())
        el.result.true_child = node;
    else if (!el.explored_false_child())
        el.result.false_child = node;
    else if (!el.explored_dontcare_child())
        el.result.dontcare_child = node;
    else
        throw std::logic_error("Unexpected case.");
}

template<typename Tag>
bool explored(const VariableStackEntry<Tag>& el) noexcept
{
    return el.explored_children() && el.explored_dontcare_child();
}

template<typename Tag, formalism::Context C>
std::optional<StackEntry<Tag>> next_entry(const VariableStackEntry<Tag>& el,
                                          const std::vector<std::pair<PreconditionVariant, IndexList<Tag>>>& sorted_preconditions,
                                          const PreconditionDetails<Tag>& details,
                                          const C& context)
{
    if (!el.explored_children())
        return try_create_stack_entry(BaseEntry<Tag> { el.base.depth + 1, el.domain_elements.at(el.forward.at(el.forward_pos)) },
                                      sorted_preconditions,
                                      details,
                                      context);
    else if (!el.explored_dontcare_child())
        return try_create_stack_entry(BaseEntry<Tag> { el.base.depth + 1, el.dontcare_elements }, sorted_preconditions, details, context);
    else
        throw std::logic_error("Unexpected case.");
}

template<typename Tag>
Data<Node<Tag>> get_result(VariableStackEntry<Tag>& el, GetResultContext<Tag>& context)
{
    canonicalize(el.result);
    return Data<Node<Tag>>(context.destination.get_or_create(el.result, context.buffer).first);
}

template<typename Tag>
void push_result(VariableStackEntry<Tag>& el, Data<Node<Tag>> node)
{
    if (!el.explored_children())
    {
        el.result.domain_children.at(el.forward.at(el.forward_pos)) = node;
        ++el.forward_pos;
    }
    else if (!el.explored_dontcare_child())
        el.result.dontcare_child = node;
    else
        throw std::logic_error("Unexpected case.");
}

template<typename Tag>
bool explored(const ConstraintStackEntry<Tag>& el) noexcept
{
    return el.explored_true_child() && el.explored_dontcare_child();
}

template<typename Tag, formalism::Context C>
std::optional<StackEntry<Tag>> next_entry(const ConstraintStackEntry<Tag>& el,
                                          const std::vector<std::pair<PreconditionVariant, IndexList<Tag>>>& sorted_preconditions,
                                          const PreconditionDetails<Tag>& details,
                                          const C& context)
{
    if (!el.explored_true_child())
        return try_create_stack_entry(BaseEntry<Tag> { el.base.depth + 1, el.true_elements }, sorted_preconditions, details, context);
    else if (!el.explored_dontcare_child())
        return try_create_stack_entry(BaseEntry<Tag> { el.base.depth + 1, el.dontcare_elements }, sorted_preconditions, details, context);
    else
        throw std::logic_error("Unexpected case.");
}

template<typename Tag>
Data<Node<Tag>> get_result(ConstraintStackEntry<Tag>& el, GetResultContext<Tag>& context)
{
    canonicalize(el.result);
    return Data<Node<Tag>>(context.destination.get_or_create(el.result, context.buffer).first);
}

template<typename Tag>
void push_result(ConstraintStackEntry<Tag>& el, Data<Node<Tag>> node)
{
    if (!el.result.true_child && !el.true_elements.empty())
        el.result.true_child = node;
    else if (!el.result.dontcare_child && !el.dontcare_elements.empty())
        el.result.dontcare_child = node;
    else
        throw std::logic_error("Unexpected case.");
}

template<typename Tag>
bool explored(const GeneratorStackEntry<Tag>& el) noexcept
{
    return true;
}

template<typename Tag, formalism::Context C>
std::optional<StackEntry<Tag>> next_entry(const GeneratorStackEntry<Tag>& el,
                                          const std::vector<std::pair<PreconditionVariant, IndexList<Tag>>>& sorted_preconditions,
                                          const PreconditionDetails<Tag>& details,
                                          const C& context)
{
    return std::nullopt;
}

template<typename Tag>
Data<Node<Tag>> get_result(GeneratorStackEntry<Tag>& el, GetResultContext<Tag>& context)
{
    canonicalize(el.result);
    return Data<Node<Tag>>(context.destination.get_or_create(el.result, context.buffer).first);
}

template<typename Tag>
void push_result(GeneratorStackEntry<Tag>& el, Data<Node<Tag>> node)
{
    throw std::logic_error("Unexpected case.");
}

template<formalism::Context C>
auto get_condition(View<Index<formalism::GroundAxiom>, C> el)
{
    return el.get_body();
}

template<formalism::Context C>
auto get_condition(View<Index<formalism::GroundAction>, C> el)
{
    return el.get_condition();
}

template<typename Tag>
static std::optional<StackEntry<Tag>>
try_create_atom_stack_entry(Index<formalism::GroundAtom<formalism::DerivedTag>> atom, BaseEntry<Tag> base, const PreconditionDetails<Tag>& details)
{
    assert(!base.elements.empty());

    auto category = [&](Index<Tag> e) -> uint_t
    {
        if (!details.at(e).contains(atom))
            return 2;  // dontcare

        const auto polarity = std::get<bool>(details.at(e).at(atom));
        return polarity ? 0 : 1;  // true first, then false
    };

    std::sort(base.elements.begin(),
              base.elements.end(),
              [&](auto&& lhs, auto&& rhs)
              {
                  const auto lhs_cat = category(lhs);
                  const auto rhs_cat = category(rhs);
                  if (lhs_cat != rhs_cat)
                      return lhs_cat < rhs_cat;  // 0 < 1 < 2
                  return lhs < rhs;
              });

    const auto mid1 = std::find_if(base.elements.begin(), base.elements.end(), [&](Index<Tag> e) { return category(e) >= 1; });
    const auto mid2 = std::find_if(mid1, base.elements.end(), [&](Index<Tag> e) { return category(e) >= 2; });

    const auto true_elements = std::span<Index<Tag>>(base.elements.begin(), mid1);
    const auto false_elements = std::span<Index<Tag>>(mid1, mid2);
    const auto dontcare_elements = std::span<Index<Tag>>(mid2, base.elements.end());

    if (true_elements.empty() && false_elements.empty())
        return std::nullopt;  ///< no element cares about the atom

    return AtomStackEntry<Tag>(base, atom, true_elements, false_elements, dontcare_elements);
}

template<typename Tag, formalism::Context C>
static std::optional<StackEntry<Tag>> try_create_variable_stack_entry(Index<formalism::FDRVariable<formalism::FluentTag>> variable,
                                                                      BaseEntry<Tag> base,
                                                                      const PreconditionDetails<Tag>& details,
                                                                      const C& context)
{
    assert(!base.elements.empty());

    const auto domain_size = make_view(variable, context).get_domain_size();

    auto category = [&](Index<Tag> e) -> uint_t
    {
        if (!details.at(e).contains(variable))
            return domain_size;  // dontcare

        const auto value = std::get<formalism::FDRValue>(details.at(e).at(variable));
        return uint_t(value);
    };

    std::sort(base.elements.begin(),
              base.elements.end(),
              [&](auto&& lhs, auto&& rhs)
              {
                  const auto lhs_cat = category(lhs);
                  const auto rhs_cat = category(rhs);
                  if (lhs_cat != rhs_cat)
                      return lhs_cat < rhs_cat;  // 0 < 1 < ... < domain_size (dontcare)
                  return lhs < rhs;
              });

    auto children_elements = std::vector<std::span<Index<Tag>>> {};
    children_elements.reserve(domain_size);

    auto it = base.elements.begin();
    for (uint_t i = 0; i < domain_size; ++i)
    {
        const auto mid = std::find_if(it, base.elements.end(), [&](Index<Tag> e) { return category(e) > i; });
        children_elements.push_back(std::span<Index<Tag>>(it, mid));
        it = mid;
    }

    auto forward = std::vector<uint_t>();
    for (uint_t i = 0; i < domain_size; ++i)
    {
        if (!children_elements[i].empty())
            forward.push_back(i);
    }

    const auto dontcare_elements = std::span<Index<Tag>>(it, base.elements.end());

    if (forward.empty())
        return std::nullopt;  ///< no element cares about the atom

    return VariableStackEntry<Tag>(base, variable, children_elements, forward, dontcare_elements);
}

template<typename Tag>
static std::optional<StackEntry<Tag>> try_create_constraint_stack_entry(Data<formalism::BooleanOperator<Data<formalism::GroundFunctionExpression>>> constraint,
                                                                        BaseEntry<Tag> base,
                                                                        const PreconditionDetails<Tag>& details)
{
    assert(!base.elements.empty());

    std::sort(base.elements.begin(),
              base.elements.end(),
              [&](auto&& lhs, auto&& rhs)
              {
                  const auto lhs_has = details.at(lhs).contains(constraint);
                  const auto rhs_has = details.at(rhs).contains(constraint);
                  if (lhs_has == rhs_has)
                      return lhs < rhs;
                  return lhs_has > rhs_has;  // true < dontcare
              });

    const auto mid = std::find_if(base.elements.begin(), base.elements.end(), [&](auto&& e) { return !details.at(e).contains(constraint); });

    const auto true_elements = std::span<Index<Tag>>(base.elements.begin(), mid);
    const auto dontcare_elements = std::span<Index<Tag>>(mid, base.elements.end());

    if (true_elements.empty())
        return std::nullopt;  ///< no element cares about the constraint

    return ConstraintStackEntry<Tag>(base, constraint, true_elements, dontcare_elements);
}

template<typename Tag>
static StackEntry<Tag> create_generator_stack_entry(BaseEntry<Tag> base)
{
    assert(!base.elements.empty());
    return GeneratorStackEntry(base);
}

template<typename Tag, formalism::Context C>
static std::optional<StackEntry<Tag>> try_create_selector_stack_entry(BaseEntry<Tag> base,
                                                                      const std::vector<std::pair<PreconditionVariant, IndexList<Tag>>>& sorted_preconditions,
                                                                      const PreconditionDetails<Tag>& details,
                                                                      const C& context)
{
    return std::visit(
        [&](auto&& arg)
        {
            using Alternative = std::decay_t<decltype(arg)>;

            if constexpr (std::same_as<Alternative, Index<formalism::FDRVariable<formalism::FluentTag>>>)
                return try_create_variable_stack_entry(arg, base, details, context);
            else if constexpr (std::same_as<Alternative, Index<formalism::GroundAtom<formalism::DerivedTag>>>)
                return try_create_atom_stack_entry(arg, base, details);
            else if constexpr (std::same_as<Alternative, Data<formalism::BooleanOperator<Data<formalism::GroundFunctionExpression>>>>)
                return try_create_constraint_stack_entry(arg, base, details);
            else
                static_assert(dependent_false<Alternative>::value, "Missing case");
        },
        sorted_preconditions[base.depth].first);
}

template<typename Tag, formalism::Context C>
static std::optional<StackEntry<Tag>> try_create_stack_entry(BaseEntry<Tag> base,
                                                             const std::vector<std::pair<PreconditionVariant, IndexList<Tag>>>& sorted_preconditions,
                                                             const PreconditionDetails<Tag>& details,
                                                             const C& context)
{
    if (!base.elements.empty())
    {
        for (; base.depth < sorted_preconditions.size(); ++base.depth)
            if (auto entry = try_create_selector_stack_entry(base, sorted_preconditions, details, context))
                return std::move(entry.value());
    }
    else
        return std::nullopt;

    return create_generator_stack_entry(base);
}

template<typename Tag>
class MatchTree
{
private:
    IndexList<Tag> m_elements;

    RepositoryPtr<formalism::OverlayRepository<formalism::Repository>, Tag> m_context;

    std::optional<Data<Node<Tag>>> m_root;

    std::vector<Data<Node<Tag>>> m_evaluate_stack;  ///< temporary during evaluation.

public:
    template<formalism::Context C>
    MatchTree(IndexList<Tag> elements_, const C& context_) :
        m_elements(std::move(elements_)),
        m_context(std::make_unique<Repository<formalism::OverlayRepository<formalism::Repository>, Tag>>(context_)),
        m_root(),
        m_evaluate_stack()
    {
        auto occurences = PreconditionOccurences<Tag> {};
        auto details = PreconditionDetails<Tag> {};

        std::cout << "Num elements: " << m_elements.size() << std::endl;

        for (const auto element : m_elements)
        {
            const auto condition = get_condition(make_view(element, context_));

            details.try_emplace(element);  //

            for (const auto fact : condition.template get_facts<formalism::FluentTag>())
            {
                const auto key = fact.get_variable().get_index();
                occurences[key].push_back(element);
                details[element][key] = fact.get_value();
            }

            for (const auto literal : condition.template get_facts<formalism::DerivedTag>())
            {
                const auto key = literal.get_atom().get_index();
                occurences[key].push_back(element);
                details[element][key] = literal.get_polarity();
            }

            for (const auto constraint : condition.get_numeric_constraints())
            {
                const auto key = constraint.get_data();
                occurences[key].push_back(element);
                details[element][key] = std::monostate {};
            }
        }

        std::vector<std::pair<PreconditionVariant, IndexList<Tag>>> sorted_preconditions(occurences.begin(), occurences.end());

        std::sort(sorted_preconditions.begin(), sorted_preconditions.end(), [](const auto& a, const auto& b) { return a.second.size() > b.second.size(); });

        // std::cout << details << std::endl;
        // std::cout << sorted_preconditions << std::endl;

        auto stack = std::deque<StackEntry<Tag>> {};
        auto initial_entry =
            try_create_stack_entry(BaseEntry<Tag>(size_t(0), std::span(m_elements.begin(), m_elements.end())), sorted_preconditions, details, context_);
        if (!initial_entry)
            return;

        stack.emplace_back(std::move(initial_entry.value()));

        auto buffer = buffer::Buffer {};
        auto result_context = GetResultContext { *m_context, buffer };

        auto num_nodes = size_t(0);

        // iterative post-order dfs
        while (!stack.empty())
        {
            auto& entry = stack.back();

            std::optional<Data<Node<Tag>>> produced;
            std::optional<StackEntry<Tag>> next;
            bool entry_explored = false;

            std::visit(
                [&](auto& frame)
                {
                    if (!explored(frame))
                    {
                        // std::cout << "next_entry" << std::endl;
                        next = next_entry(frame, sorted_preconditions, details, context_);
                    }
                    else
                    {
                        // std::cout << "get_result" << std::endl;
                        produced = get_result(frame, result_context);
                        entry_explored = true;
                    }
                },
                entry);

            if (next)
            {
                // std::cout << "push next" << std::endl;
                stack.push_back(std::move(next.value()));
                continue;
            }

            assert(produced);

            ++num_nodes;

            stack.pop_back();

            if (stack.empty())
            {
                m_root = std::move(produced.value());
                break;
            }
            else
            {
                // std::cout << "push result" << std::endl;
                std::visit([&](auto& parent) { push_result(parent, std::move(produced.value())); }, stack.back());
            }
        }

        std::cout << "Num nodes: " << num_nodes << std::endl;
    }

    template<formalism::Context C>
    static MatchTreePtr<Tag> create(IndexList<Tag> elements, const C& context)
    {
        return std::make_unique<MatchTree<Tag>>(elements, context);
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
