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
 *<
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef TYR_CISTA_DECLARATIONS_HPP_
#define TYR_CISTA_DECLARATIONS_HPP_

#include "tyr/common/declarations.hpp"
#include "tyr/common/types.hpp"

#include <iostream>

namespace cista
{
template<typename Buf>
struct buf;
}

namespace tyr::cista
{
using Buffer = ::cista::buf<std::vector<uint8_t>>;

template<typename Tag, typename H = Hash<ObserverPtr<const Data<Tag>>>, typename E = EqualTo<ObserverPtr<const Data<Tag>>>>
class IndexedHashSet;

template<typename Tag, typename H = Hash<ObserverPtr<const Data<Tag>>>, typename E = EqualTo<ObserverPtr<const Data<Tag>>>>

using IndexedHashSetList = std::vector<IndexedHashSet<Tag, H, E>>;
}

#endif
