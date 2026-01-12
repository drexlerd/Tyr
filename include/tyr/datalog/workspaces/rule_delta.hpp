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

#ifndef TYR_DATALOG_WORKSPACES_RULE_DELTA_HPP_
#define TYR_DATALOG_WORKSPACES_RULE_DELTA_HPP_

#include "tyr/common/declarations.hpp"
#include "tyr/common/equal_to.hpp"
#include "tyr/common/hash.hpp"
#include "tyr/datalog/policies/annotation.hpp"
#include "tyr/formalism/binding_index.hpp"
#include "tyr/formalism/datalog/ground_atom_index.hpp"
#include "tyr/formalism/datalog/merge.hpp"
#include "tyr/formalism/datalog/repository.hpp"
#include "tyr/formalism/object_index.hpp"

namespace tyr::datalog
{

struct RuleDeltaWorkspace
{
    /// Merge thread into staging area
    formalism::datalog::RepositoryPtr repository;

    /// Results across iterations
    IndexList<formalism::Object> binding;
    formalism::datalog::MergeCache merge_cache;

    UnorderedSet<IndexList<formalism::Object>> seen_bindings_dbg;

    RuleDeltaWorkspace();

    void clear() noexcept;
};
}

#endif
