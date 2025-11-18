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

#ifndef TYR_FORMALISM_PROGRAM_HPP_
#define TYR_FORMALISM_PROGRAM_HPP_

#include "tyr/formalism/declarations.hpp"
#include "tyr/formalism/ground_atom.hpp"
#include "tyr/formalism/rule.hpp"

namespace tyr::formalism
{
struct ProgramIndex
{
    uint_t value {};

    ProgramIndex() = default;
    explicit ProgramIndex(uint_t value) : value(value) {}

    friend bool operator==(const ProgramIndex& lhs, const ProgramIndex& rhs) { return EqualTo<uint_t> {}(lhs.value, rhs.value); }

    uint_t get() const noexcept { return value; }

    auto cista_members() const noexcept { return std::tie(value); }
};

using ProgramIndexList = ::cista::offset::vector<ProgramIndex>;

struct ProgramImpl
{
    ProgramIndex index;
    GroundAtomIndexList<StaticTag> static_atoms;
    GroundAtomIndexList<FluentTag> fluent_atoms;
    RuleIndexList rules;

    using IndexType = ProgramIndex;

    ProgramImpl() = default;
    ProgramImpl(ProgramIndex index, GroundAtomIndexList<StaticTag> static_atoms, GroundAtomIndexList<FluentTag> fluent_atoms, RuleIndexList rules) :
        index(index),
        static_atoms(std::move(static_atoms)),
        fluent_atoms(std::move(fluent_atoms)),
        rules(std::move(rules))
    {
    }

    auto cista_members() const noexcept { return std::tie(index, static_atoms, fluent_atoms, rules); }
    auto identifying_members() const noexcept { return std::tie(static_atoms, fluent_atoms, rules); }
};

}

#endif
