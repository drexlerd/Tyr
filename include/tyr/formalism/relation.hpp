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

#ifndef TYR_FORMALISM_RELATION_HPP_
#define TYR_FORMALISM_RELATION_HPP_

#include "tyr/formalism/declarations.hpp"
#include "tyr/formalism/variable.hpp"

namespace tyr::formalism
{
template<IsStaticOrFluentTag T>
struct RelationIndex
{
    uint_t value {};

    RelationIndex() = default;
    explicit RelationIndex(uint_t value) : value(value) {}

    uint_t get() const noexcept { return value; }

    auto cista_members() const noexcept { return std::tie(value); }
};

template<IsStaticOrFluentTag T>
class Relation
{
private:
    RelationIndex m_index;
    std::string m_name;
    uint_t m_arity;

public:
    Relation();
    Relation(RelationIndex index, std::string name, uint_t arity);

    RelationIndex get_index() const noexcept;
    const std::string& get_name() const noexcept;
    uint_t get_arity() const noexcept;

    auto cista_members() const noexcept { return std::tie(m_index, m_name, m_arity); }
};
}

#endif
