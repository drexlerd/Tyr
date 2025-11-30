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

#ifndef TYR_PLANNING_SPARSE_UNPACKED_STATE_HPP_
#define TYR_PLANNING_SPARSE_UNPACKED_STATE_HPP_

#include "tyr/common/config.hpp"
#include "tyr/formalism/declarations.hpp"
#include "tyr/planning/state_index.hpp"

#include <boost/dynamic_bitset.hpp>
#include <vector>

namespace tyr::planning
{
class SparseUnpackedState
{
public:
    SparseUnpackedState() = default;

    StateIndex& get_index() noexcept;

    StateIndex get_index() const noexcept;

    template<formalism::IsFactTag T>
    boost::dynamic_bitset<>& get_atoms() noexcept;

    template<formalism::IsFactTag T>
    const boost::dynamic_bitset<>& get_atoms() const noexcept;

    std::vector<float_t>& get_numeric_variables() noexcept;

    const std::vector<float_t>& get_numeric_variables() const noexcept;

    void clear();

private:
    StateIndex m_index;
    boost::dynamic_bitset<> m_fluent_atoms;
    boost::dynamic_bitset<> m_derived_atoms;
    std::vector<float_t> m_numeric_variables;
};
}

#endif
