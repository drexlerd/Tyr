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

#ifndef TYR_PLANNING_GROUND_TASK_STATE_ITERATORS_HPP_
#define TYR_PLANNING_GROUND_TASK_STATE_ITERATORS_HPP_

#include "tyr/common/types.hpp"
#include "tyr/formalism/planning/declarations.hpp"
#include "tyr/formalism/planning/indices.hpp"
#include "tyr/planning/declarations.hpp"
#include "tyr/planning/state_iterators.hpp"

#include <boost/dynamic_bitset.hpp>
#include <cassert>
#include <cmath>
#include <iterator>
#include <utility>
#include <vector>

namespace tyr::planning
{
/**
 * FDRFact
 */

template<class Tag>
class FDRFactIterator<GroundTag, Tag>
{
public:
    using value_type = Data<formalism::planning::FDRFact<Tag>>;
    using reference = value_type;
    using difference_type = std::ptrdiff_t;
    using iterator_category = std::input_iterator_tag;
    using iterator_concept = std::input_iterator_tag;
    using pointer = void;

    FDRFactIterator() noexcept : m_data(nullptr), m_i(0) {}
    FDRFactIterator(const std::vector<uint_t>& data, bool begin) noexcept : m_data(&data), m_i(begin ? 0 : m_data->size())
    {
        if (begin)
            while (m_i < m_data->size() && formalism::planning::FDRValue((*m_data)[m_i]).is_none())
                ++m_i;
    }

    value_type operator*() const noexcept
    {
        assert(m_data);
        return Data<formalism::planning::FDRFact<Tag>> { Index<formalism::planning::FDRVariable<Tag>> { static_cast<uint_t>(m_i) },
                                                         formalism::planning::FDRValue((*m_data)[m_i]) };
    }

    FDRFactIterator& operator++() noexcept
    {
        ++m_i;
        while (m_i < m_data->size() && formalism::planning::FDRValue((*m_data)[m_i]).is_none())
            ++m_i;
        return *this;
    }
    FDRFactIterator operator++(int) noexcept
    {
        FDRFactIterator tmp = *this;
        ++(*this);
        return tmp;
    }

    friend bool operator==(const FDRFactIterator& lhs, const FDRFactIterator& rhs) noexcept { return lhs.m_data == rhs.m_data && lhs.m_i == rhs.m_i; }
    friend bool operator!=(const FDRFactIterator& lhs, const FDRFactIterator& rhs) noexcept { return !(lhs == rhs); }

private:
    const std::vector<uint_t>* m_data;
    size_t m_i = 0;
};

template<typename Tag>
class FDRFactRange<GroundTag, Tag> : public std::ranges::view_interface<FDRFactRange<GroundTag, Tag>>
{
public:
    FDRFactRange() = default;
    explicit FDRFactRange(const std::vector<uint_t>& values) : m_data(&values) {}

    auto begin() const { return FDRFactIterator<GroundTag, Tag>(*m_data, true); }
    auto end() const { return FDRFactIterator<GroundTag, Tag>(*m_data, false); }

private:
    const std::vector<uint_t>* m_data;
};
}

#endif