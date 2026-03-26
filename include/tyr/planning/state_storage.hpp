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

#ifndef TYR_PLANNING_STATE_STORAGE_HPP_
#define TYR_PLANNING_STATE_STORAGE_HPP_

#include "tyr/common/config.hpp"

#include <concepts>
#include <vector>

namespace tyr::planning
{

/**
 * Backend
 */

template<typename Task, typename Tag>
struct AtomStorageBackend;

template<typename Task, typename Tag>
struct FactStorageBackend;

template<typename Task, typename Tag>
struct NumericStorageBackend;

/**
 * Packed
 */

template<typename Task, typename Tag>
struct AtomPackedStorage;

template<typename Task, typename Tag>
struct FactPackedStorage;

template<typename Task, typename Tag>
struct NumericPackedStorage;

/**
 * Unpacked
 */

template<typename Task>
struct AtomUnpackedStorage;

template<typename Task>
struct FactUnpackedStorage;

template<typename Task>
struct NumericUnpackedStorage
{
    std::vector<float_t> values;
};

}

#endif
