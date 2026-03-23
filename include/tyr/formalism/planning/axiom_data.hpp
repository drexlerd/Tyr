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

#ifndef TYR_FORMALISM_PLANNING_AXIOM_DATA_HPP_
#define TYR_FORMALISM_PLANNING_AXIOM_DATA_HPP_

#include "tyr/common/types.hpp"
#include "tyr/common/types_utils.hpp"
#include "tyr/formalism/planning/atom_index.hpp"
#include "tyr/formalism/planning/axiom_index.hpp"
#include "tyr/formalism/planning/conjunctive_condition_index.hpp"
#include "tyr/formalism/planning/declarations.hpp"
#include "tyr/formalism/variable_index.hpp"

namespace tyr
{

template<>
struct Data<formalism::planning::Axiom>
{
    Index<formalism::planning::Axiom> index;
    IndexList<formalism::Variable> variables;
    Index<formalism::planning::ConjunctiveCondition> body;
    Index<formalism::planning::Atom<formalism::DerivedTag>> head;

    Data() = default;
    Data(IndexList<formalism::Variable> variables_,
         Index<formalism::planning::ConjunctiveCondition> body_,
         Index<formalism::planning::Atom<formalism::DerivedTag>> head_) :
        index(),
        variables(std::move(variables_)),
        body(body_),
        head(head_)
    {
    }
    // Python constructor
    template<typename C>
    Data(const std::vector<View<Index<formalism::Variable>, C>>& variables_,
         View<Index<formalism::planning::ConjunctiveCondition>, C> body_,
         View<Index<formalism::planning::Atom<formalism::DerivedTag>>, C> head_) :
        index(),
        variables(),
        body(),
        head()
    {
        set(variables_, variables);
        set(body_, body);
        set(head_, head);
    }
    Data(const Data& other) = delete;
    Data& operator=(const Data& other) = delete;
    Data(Data&& other) = default;
    Data& operator=(Data&& other) = default;

    void clear() noexcept
    {
        tyr::clear(index);
        tyr::clear(variables);
        tyr::clear(body);
        tyr::clear(head);
    }

    auto cista_members() const noexcept { return std::tie(index, variables, body, head); }
    auto identifying_members() const noexcept { return std::tie(variables, body, head); }
};

static_assert(!uses_trivial_storage_v<formalism::planning::Axiom>);
}

#endif
