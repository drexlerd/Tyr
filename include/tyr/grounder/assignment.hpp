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

#ifndef TYR_GROUNDER_ASSIGNMENT_HPP_
#define TYR_GROUNDER_ASSIGNMENT_HPP_

#include "tyr/common/config.hpp"
#include "tyr/common/declarations.hpp"

#include <limits>

namespace tyr::grounder
{

/**
 * EmptyAssignment
 */

struct EmptyAssignment
{
    static constexpr uint_t rank = 0;
};

/**
 * VertexAssignment
 */

struct VertexAssignment
{
    uint_t index;
    uint_t object;

    VertexAssignment() : index(std::numeric_limits<uint_t>::max()), object(std::numeric_limits<uint_t>::max()) {}

    VertexAssignment(uint_t index, uint_t object) : index(index), object(object) {}

    inline bool is_valid() const noexcept { return index != std::numeric_limits<uint_t>::max() && object != std::numeric_limits<uint_t>::max(); }
};

/**
 * EdgeAssignment
 */

/// @brief Encapsulate assignment of objects to variables of atoms.
struct EdgeAssignment
{
    uint_t first_index;
    uint_t first_object;
    uint_t second_index;
    uint_t second_object;

    EdgeAssignment() :
        first_index(std::numeric_limits<uint_t>::max()),
        first_object(std::numeric_limits<uint_t>::max()),
        second_index(std::numeric_limits<uint_t>::max()),
        second_object(std::numeric_limits<uint_t>::max())
    {
    }

    EdgeAssignment(uint_t first_index, uint_t first_object, uint_t second_index, uint_t second_object) :
        first_index(first_index),
        first_object(first_object),
        second_index(second_index),
        second_object(second_object)
    {
    }

    inline bool is_valid() const noexcept
    {
        return (first_index < second_index) && (first_index != std::numeric_limits<uint_t>::max()) && (second_index != std::numeric_limits<uint_t>::max())
               && (first_object != std::numeric_limits<uint_t>::max()) && (second_object != std::numeric_limits<uint_t>::max());
    }
};

inline std::ostream& operator<<(std::ostream& os, const VertexAssignment& assignment)
{
    os << "[" << assignment.index << "/" << assignment.object << "]";
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const EdgeAssignment& assignment)
{
    os << "[" << assignment.first_index << "/" << assignment.first_object << ", " << assignment.second_index << "/" << assignment.second_object << "]";
    return os;
}
}

#endif