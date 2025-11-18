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

    friend bool operator==(const RuleIndex& lhs, const RuleIndex& rhs) { return EqualTo<uint_t> {}(lhs.value, rhs.value); }

    uint_t get() const noexcept { return value; }

    auto cista_members() const noexcept { return std::tie(value); }
};

using RuleIndexList = ::cista::offset::vector<RuleIndex>;

struct RuleImpl
{
    RuleIndex index;
    LiteralIndexList<StaticTag> static_body;
    LiteralIndexList<FluentTag> fluent_body;
    AtomIndex<FluentTag> head;

    using IndexType = RuleIndex;

    RuleImpl() = default;
    RuleImpl(RuleIndex index, LiteralIndexList<StaticTag> static_body, LiteralIndexList<FluentTag> fluent_body, AtomIndex<FluentTag> head) :
        index(index),
        static_body(std::move(static_body)),
        fluent_body(std::move(fluent_body)),
        head(head)
    {
    }

    auto cista_members() const noexcept { return std::tie(index, static_body, fluent_body, head); }
    auto identifying_members() const noexcept { return std::tie(static_body, fluent_body, head); }
};

}

#endif
