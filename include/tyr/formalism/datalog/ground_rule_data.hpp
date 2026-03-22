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

#ifndef TYR_FORMALISM_DATALOG_GROUND_RULE_DATA_HPP_
#define TYR_FORMALISM_DATALOG_GROUND_RULE_DATA_HPP_

#include "tyr/common/types.hpp"
#include "tyr/common/types_utils.hpp"
#include "tyr/common/vector.hpp"
#include "tyr/formalism/binding_index.hpp"
#include "tyr/formalism/datalog/boolean_operator_data.hpp"
#include "tyr/formalism/datalog/declarations.hpp"
#include "tyr/formalism/datalog/ground_atom_index.hpp"
#include "tyr/formalism/datalog/ground_conjunctive_condition_index.hpp"
#include "tyr/formalism/datalog/ground_literal_index.hpp"
#include "tyr/formalism/datalog/ground_rule_index.hpp"
#include "tyr/formalism/datalog/rule_index.hpp"

namespace tyr
{
template<>
struct Data<formalism::datalog::GroundRule>
{
    Index<formalism::datalog::GroundRule> index;
    Index<formalism::RelationBinding<formalism::datalog::Rule>> binding;
    Index<formalism::datalog::GroundConjunctiveCondition> body;
    Index<formalism::datalog::GroundAtom<formalism::FluentTag>> head;

    Data() = default;
    Data(Index<formalism::datalog::GroundRule> index,
         Index<formalism::RelationBinding<formalism::datalog::Rule>> binding,
         Index<formalism::datalog::GroundConjunctiveCondition> body,
         Index<formalism::datalog::GroundAtom<formalism::FluentTag>> head) :
        index(index),
        binding(binding),
        body(body),
        head(head)
    {
    }
    Data(const Data& other) = default;
    Data& operator=(const Data& other) = default;
    Data(Data&& other) = default;
    Data& operator=(Data&& other) = default;

    void clear() noexcept
    {
        tyr::clear(index);
        tyr::clear(binding);
        tyr::clear(body);
        tyr::clear(head);
    }

    auto cista_members() const noexcept { return std::tie(index, binding, body, head); }
    auto identifying_members() const noexcept { return std::tie(binding); }
};

static_assert(uses_trivial_storage_v<formalism::datalog::GroundRule>);

}

#endif
