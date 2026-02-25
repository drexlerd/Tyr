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

#ifndef TYR_PLANNING_STATE_ITERATORS_HPP_
#define TYR_PLANNING_STATE_ITERATORS_HPP_

#include "tyr/common/types.hpp"
#include "tyr/formalism/planning/declarations.hpp"
#include "tyr/formalism/planning/indices.hpp"

#include <cassert>
#include <cmath>
#include <iterator>
#include <utility>
#include <vector>

namespace tyr::planning
{
template<class Tag>
class NumericIterator
{
public:
    using value_type = std::pair<Index<formalism::planning::GroundFunctionTerm<Tag>>, float_t>;
    using difference_type = std::ptrdiff_t;
    using iterator_category = std::input_iterator_tag;
    using iterator_concept = std::input_iterator_tag;

    void skip_nan() noexcept
    {
        assert(m_data);
        while (m_i < m_data->size() && std::isnan((*m_data)[m_i]))
            ++m_i;
    }

    NumericIterator() noexcept : m_data(nullptr), m_i(0) {}
    NumericIterator(const std::vector<float_t>& data, bool begin) noexcept : m_data(&data), m_i(begin ? 0 : m_data->size())
    {
        if (begin)
            skip_nan();
    }

    value_type operator*() const noexcept
    {
        assert(m_data);
        return { Index<formalism::planning::GroundFunctionTerm<Tag>> { m_i }, (*m_data)[m_i] };
    }

    NumericIterator& operator++() noexcept
    {
        ++m_i;
        skip_nan();
        return *this;
    }
    NumericIterator operator++(int) noexcept
    {
        NumericIterator tmp = *this;
        ++(*this);
        return tmp;
    }

    friend bool operator==(const NumericIterator& lhs, const NumericIterator& rhs) noexcept { return lhs.m_data == rhs.m_data && lhs.m_i == rhs.m_i; }
    friend bool operator!=(const NumericIterator& lhs, const NumericIterator& rhs) noexcept { return !(lhs == rhs); }

private:
    const std::vector<float_t>* m_data;
    std::size_t m_i = 0;
};

template<class Tag>
class NumericRange
{
public:
    explicit NumericRange(const std::vector<float_t>& values) : m_data(values) {}

    auto begin() const { return NumericIterator<Tag>(m_data, true); }
    auto end() const { return NumericIterator<Tag>(m_data, false); }

private:
    const std::vector<float_t>& m_data;
};
}

#endif