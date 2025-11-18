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

#ifndef TYR_FORMALISM_SYMBOL_HPP_
#define TYR_FORMALISM_SYMBOL_HPP_

#include "tyr/formalism/declarations.hpp"

namespace tyr::formalism
{
struct SymbolIndex
{
    uint_t value {};

    SymbolIndex() = default;
    explicit SymbolIndex(uint_t value) : value(value) {}

    friend bool operator==(const SymbolIndex& lhs, const SymbolIndex& rhs) { return EqualTo<uint_t> {}(lhs.value, rhs.value); }

    uint_t get() const noexcept { return value; }

    auto cista_members() const noexcept { return std::tie(value); }
    auto identifying_members() const noexcept { return std::tie(value); }
};

struct SymbolImpl
{
    SymbolIndex index;
    ::cista::offset::string name;

    using IndexType = SymbolIndex;

    SymbolImpl() = default;
    SymbolImpl(SymbolIndex index, ::cista::offset::string name) : index(index), name(std::move(name)) {}

    auto cista_members() const noexcept { return std::tie(index, name); }
    auto identifying_members() const noexcept { return std::tie(name); }
};

static_assert(HasIdentifyingMembers<SymbolImpl>);
}

#endif
