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

    uint_t get() const noexcept { return value; }

    auto cista_members() const noexcept { return std::tie(value); }
};

class Program
{
private:
    ProgramIndex m_index;
    GroundAtomIndexList<StaticTag> m_static_atoms;
    GroundAtomIndexList<FluentTag> m_fluent_atoms;
    RuleIndexList m_rules;

public:
    using IndexType = ProgramIndex;

    Program();
    Program(ProgramIndex index, GroundAtomIndexList<StaticTag> static_atoms, GroundAtomIndexList<FluentTag> fluent_atoms, RuleIndexList rules);

    ProgramIndex get_index() const noexcept;
    const GroundAtomIndexList<StaticTag>& get_static_atoms() const noexcept;
    const GroundAtomIndexList<FluentTag>& get_fluent_atoms() const noexcept;
    const RuleIndexList& get_rules() const noexcept;

    auto cista_members() const noexcept { return std::tie(m_index, m_static_atoms, m_fluent_atoms, m_rules); }
};

using ProgramIndexList = cista::offset::vector<ProgramIndex>;
}

#endif
