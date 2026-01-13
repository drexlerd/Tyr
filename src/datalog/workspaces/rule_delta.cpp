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

#include "tyr/datalog/workspaces/rule_delta.hpp"

namespace fd = tyr::formalism::datalog;

namespace tyr::datalog
{

RuleDeltaWorkspace::RuleDeltaWorkspace() :
    repository(std::make_shared<fd::Repository>()),
    binding(),
    merge_cache(),
    seen_bindings_dbg(),
    applicability_check_pool(),
    pending_rules()
{
}

void RuleDeltaWorkspace::clear() noexcept
{
    repository->clear();
    merge_cache.clear();
    seen_bindings_dbg.clear();
    pending_rules.clear();
}

}
