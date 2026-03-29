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

#ifndef TYR_PLANNING_LIFTED_TASK_STATE_STORAGE_HASH_SET_CONTEXT_HPP_
#define TYR_PLANNING_LIFTED_TASK_STATE_STORAGE_HASH_SET_CONTEXT_HPP_

#include "tyr/common/config.hpp"
#include "tyr/common/raw_vector_set.hpp"
#include "tyr/planning/declarations.hpp"
#include "tyr/planning/state_storage.hpp"
#include "tyr/planning/state_storage/tags.hpp"

#include <concepts>
#include <valla/valla.hpp>
#include <vector>

namespace tyr::planning
{

/**
 * Context
 */

template<>
struct StateStorageContext<LiftedTag, HashSet>
{
    RawVectorSet<uint_t, uint_t> uint_vec_set;
    RawVectorSet<uint_t, float_t> float_vec_set;

    size_t memory_usage() const noexcept
    {
        size_t bytes = 0;
        bytes += uint_vec_set.memory_usage();
        bytes += float_vec_set.memory_usage();
        return bytes;
    }
};

}

#endif