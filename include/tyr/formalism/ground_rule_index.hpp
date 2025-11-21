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

#ifndef TYR_FORMALISM_GROUND_RULE_INDEX_HPP_
#define TYR_FORMALISM_GROUND_RULE_INDEX_HPP_

#include "tyr/common/equal_to.hpp"
#include "tyr/formalism/declarations.hpp"
#include "tyr/formalism/rule_index.hpp"

namespace tyr::formalism
{
struct GroundRuleIndex
{
    RuleIndex rule_index;
    uint_t value {};

    GroundRuleIndex() = default;
    explicit GroundRuleIndex(RuleIndex rule_index, uint_t value) : rule_index(rule_index), value(value) {}

    friend bool operator==(const GroundRuleIndex& lhs, const GroundRuleIndex& rhs)
    {
        return EqualTo<RuleIndex> {}(lhs.rule_index, rhs.rule_index) && EqualTo<uint_t> {}(lhs.value, rhs.value);
    }

    uint_t get() const noexcept { return value; }

    auto cista_members() const noexcept { return std::tie(rule_index, value); }
    auto identifying_members() const noexcept { return std::tie(rule_index, value); }
};

using GroundRuleIndexList = ::cista::offset::vector<GroundRuleIndex>;

}

#endif
