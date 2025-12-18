/*
 * Copyright (C) 2023 Dominik Drexler and Simon Stahlberg
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

#include "tyr/planning/ground_task/match_tree/match_tree.hpp"

namespace tyr::planning::match_tree
{
/* MatchTree */
template<typename Tag>
template<formalism::Context C>
std::unique_ptr<MatchTree<Tag>> MatchTree<Tag>::create(View<IndexList<Tag>, C> elements)
{
}

template<typename Tag>
void MatchTree<Tag>::generate_applicable_elements_iteratively(const UnpackedState<GroundTask>& state, IndexList<Tag>& out_applicable_elements)
{
}

}
