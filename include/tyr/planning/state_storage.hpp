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
#include "tyr/planning/task.hpp"

#include <concepts>
#include <vector>

namespace tyr::planning
{

/**
 * Context
 */

template<TaskKind Kind, typename Tag>
struct StateStorageContext;

/**
 * Backend
 */

template<TaskKind Kind, typename Tag>
struct AtomStorageBackend;

template<TaskKind Kind, typename Tag>
struct FactStorageBackend;

template<TaskKind Kind, typename Tag>
struct NumericStorageBackend;

/**
 * Packed
 */

template<TaskKind Kind, typename Tag>
struct AtomPackedStorage;

template<TaskKind Kind, typename Tag>
struct FactPackedStorage;

template<TaskKind Kind, typename Tag>
struct NumericPackedStorage;

/**
 * Unpacked
 */

template<TaskKind Kind>
struct AtomUnpackedStorage;

template<TaskKind Kind>
struct FactUnpackedStorage;

template<TaskKind Kind>
struct NumericUnpackedStorage
{
    std::vector<float_t> values;
};

}

#endif
