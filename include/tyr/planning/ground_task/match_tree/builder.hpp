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

#ifndef TYR_PLANNING_GROUND_TASK_MATCH_TREE_BUILDER_HPP_
#define TYR_PLANNING_GROUND_TASK_MATCH_TREE_BUILDER_HPP_

// Include specialization headers first
#include "tyr/planning/ground_task/match_tree/nodes/atom_data.hpp"
#include "tyr/planning/ground_task/match_tree/nodes/constraint_data.hpp"
#include "tyr/planning/ground_task/match_tree/nodes/fact_data.hpp"
#include "tyr/planning/ground_task/match_tree/nodes/generator_data.hpp"
#include "tyr/planning/ground_task/match_tree/nodes/node_data.hpp"
//
#include "tyr/buffer/declarations.hpp"
#include "tyr/common/tuple.hpp"
#include "tyr/common/unique_object_pool.hpp"
#include "tyr/planning/ground_task/match_tree/declarations.hpp"

namespace tyr::planning::match_tree
{
template<typename Tag>
struct Builder
{
    Builder() = default;

    /**
     * Datalog
     */

    template<typename T>
    struct BuilderEntry
    {
        using value_type = T;
        using container_type = UniqueObjectPool<Data<T>>;

        container_type container;

        BuilderEntry() = default;
    };

    using BuilderStorage = std::tuple<BuilderEntry<AtomSelectorNode<Tag>>,
                                      BuilderEntry<FactSelectorNode<Tag>>,
                                      BuilderEntry<NumericConstraintSelectorNode<Tag>>,
                                      BuilderEntry<ElementGeneratorNode<Tag>>,
                                      BuilderEntry<Node<Tag>>,
                                      BuilderEntry<AtomSelectorNode<Tag>>,
                                      BuilderEntry<FactSelectorNode<Tag>>,
                                      BuilderEntry<NumericConstraintSelectorNode<Tag>>,
                                      BuilderEntry<ElementGeneratorNode<Tag>>,
                                      BuilderEntry<Node<Tag>>>;

    BuilderStorage m_builder;

    template<typename T>
    [[nodiscard]] auto get_builder()
    {
        return get_container<T>(m_builder).get_or_allocate();
    }

    buffer::Buffer buffer;

    auto& get_buffer() noexcept { return buffer; }
};

}

#endif
