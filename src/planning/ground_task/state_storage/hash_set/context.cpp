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

#include "tyr/planning/ground_task/state_storage/hash_set/context.hpp"

#include "tyr/planning/ground_task.hpp"

namespace tyr::planning
{
namespace
{
template<std::unsigned_integral Block>
auto compute_layout_data(const Task<GroundTag>& task) -> StateStorageContext<GroundTag, HashSet>::LayoutData
{
    using Context = StateStorageContext<GroundTag, HashSet>;
    using VariableInfo = Context::VariableInfo;
    using LayoutData = Context::LayoutData;

    constexpr uint_t bits_per_block = static_cast<uint_t>(bit::bits_per_block_v<Block>);

    auto layout = LayoutData {};

    uint_t current_bit = 0;

    for (const auto variable : task.get_formalism_task().get_task().get_fluent_variables())
    {
        const auto domain_size = static_cast<uint_t>(variable.get_atoms().size() + 1);
        const auto length = static_cast<uint_t>(bit::bits_needed(domain_size));

        layout.fluent_infos.push_back(VariableInfo {
            .begin = current_bit / bits_per_block,
            .offset = static_cast<uint8_t>(current_bit % bits_per_block),
            .length = static_cast<uint8_t>(length),
        });

        current_bit += length;
    }

    layout.fluent_array_size = bit::ceil_div(current_bit, bits_per_block);
    layout.derived_num_bits = static_cast<uint_t>(task.get_formalism_task().get_task().get_atoms<formalism::DerivedTag>().size());
    layout.derived_array_size = bit::ceil_div(layout.derived_num_bits, bits_per_block);

    return layout;
}

}

StateStorageContext<GroundTag, HashSet>::StateStorageContext(const Task<GroundTag>& task) : StateStorageContext(compute_layout_data<uint_t>(task)) {}

StateStorageContext<GroundTag, HashSet>::StateStorageContext(LayoutData&& layout_data) :
    fluent_infos(layout_data.fluent_infos),
    fluent_array_set(layout_data.fluent_array_size),
    derived_num_bits(layout_data.derived_num_bits),
    derived_array_set(layout_data.derived_array_size)
{
}

}
