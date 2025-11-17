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

#ifndef TYR_FORMALISM_RULE_HPP_
#define TYR_FORMALISM_RULE_HPP_

#include "tyr/formalism/declarations.hpp"
#include "tyr/formalism/literal.hpp"

namespace tyr::formalism
{
struct RuleIndex
{
    uint_t value {};

    RuleIndex() = default;
    explicit RuleIndex(uint_t value) : value(value) {}

    uint_t get() const noexcept { return value; }

    auto cista_members() const noexcept { return std::tie(value); }
};

class Rule
{
private:
    RuleIndex m_index;
    LiteralIndexList<StaticTag> m_static_body;
    LiteralIndexList<FluentTag> m_fluent_body;
    AtomIndex<FluentTag> m_head;

public:
    using IndexType = RuleIndex;

    Rule();
    Rule(RuleIndex index, LiteralIndexList<StaticTag> static_body, LiteralIndexList<FluentTag> fluent_body, AtomIndex<FluentTag> head);

    RuleIndex get_index() const noexcept;
    const LiteralIndexList<StaticTag>& get_static_body() const noexcept;
    const LiteralIndexList<FluentTag>& get_fluent_body() const noexcept;
    AtomIndex<FluentTag> get_head() const noexcept;

    auto cista_members() const noexcept { return std::tie(m_index, m_static_body, m_fluent_body, m_head); }
};

using RuleIndexList = cista::offset::vector<RuleIndex>;
}

#endif
