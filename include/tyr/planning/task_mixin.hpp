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

#ifndef TYR_PLANNING_TASK_MIXIN_HPP_
#define TYR_PLANNING_TASK_MIXIN_HPP_

#include "tyr/common/indexed_hash_set.hpp"
#include "tyr/common/shared_object_pool.hpp"
#include "tyr/formalism/formalism.hpp"
#include "tyr/planning/node.hpp"
#include "tyr/planning/packed_state.hpp"
#include "tyr/planning/state.hpp"
#include "tyr/planning/state_index.hpp"
#include "tyr/planning/unpacked_state.hpp"

#include <gtl/phmap.hpp>
#include <memory>
#include <valla/valla.hpp>

namespace tyr::planning
{

static void fill_atoms(valla::Slot<uint_t> slot,
                       const valla::IndexedHashSet<valla::Slot<uint_t>, uint_t>& uint_nodes,
                       std::vector<uint_t>& buffer,
                       boost::dynamic_bitset<>& atoms)
{
    buffer.clear();

    valla::read_sequence(slot, uint_nodes, std::back_inserter(buffer));

    if (!buffer.empty())
    {
        assert(std::is_sorted(buffer.begin(), buffer.end()));
        atoms.resize(buffer.back() + 1, false);
        for (const auto& atom_index : buffer)
            atoms.set(atom_index);
    }
}

static void fill_numeric_variables(valla::Slot<uint_t> slot,
                                   const valla::IndexedHashSet<valla::Slot<uint_t>, uint_t>& uint_nodes,
                                   const valla::IndexedHashSet<float_t, uint_t>& float_nodes,
                                   std::vector<uint_t>& buffer,
                                   std::vector<float_t>& numeric_variables)
{
    buffer.clear();

    valla::read_sequence(slot, uint_nodes, std::back_inserter(buffer));

    if (!buffer.empty())
    {
        valla::decode_from_unsigned_integrals(buffer, float_nodes, std::back_inserter(numeric_variables));
    }
}

static valla::Slot<uint_t>
create_atoms_slot(const boost::dynamic_bitset<>& atoms, std::vector<uint_t>& buffer, valla::IndexedHashSet<valla::Slot<uint_t>, uint_t>& uint_nodes)
{
    buffer.clear();

    const auto& bits = atoms;
    for (uint_t i = bits.find_first(); i != boost::dynamic_bitset<>::npos; i = bits.find_next(i))
    {
        buffer.push_back(i);
    }

    return valla::insert_sequence(buffer, uint_nodes);
}

static valla::Slot<uint_t> create_numeric_variables_slot(const std::vector<float_t>& numeric_variables,
                                                         std::vector<uint_t>& buffer,
                                                         valla::IndexedHashSet<valla::Slot<uint_t>, uint_t>& uint_nodes,
                                                         valla::IndexedHashSet<float_t, uint_t>& float_nodes)
{
    buffer.clear();

    valla::encode_as_unsigned_integrals(numeric_variables, float_nodes, std::back_inserter(buffer));

    return valla::insert_sequence(buffer, uint_nodes);
}

template<typename Task>
class TaskMixin
{
private:
    /// @brief Helper to cast to Task.
    constexpr const auto& self() const { return static_cast<const Task&>(*this); }
    constexpr auto& self() { return static_cast<Task&>(*this); }

public:
    TaskMixin(std::shared_ptr<formalism::Repository> repository, Index<formalism::planning::Task> task_index) :
        m_repository(std::move(repository)),
        m_task_index(task_index)
    {
    }

    State<Task> get_state(StateIndex state_index)
    {
        const auto& packed_state = m_packed_states[state_index];

        auto unpacked_state = m_unpacked_state_pool.get_or_allocate();
        unpacked_state->clear();

        thread_local auto buffer = std::vector<uint_t> {};

        unpacked_state->get_index() = state_index;
        fill_atoms(packed_state.get_atoms<formalism::FluentTag>(), m_uint_nodes, buffer, unpacked_state->get_atoms<formalism::FluentTag>());
        fill_atoms(packed_state.get_atoms<formalism::DerivedTag>(), m_uint_nodes, buffer, unpacked_state->get_atoms<formalism::DerivedTag>());
        fill_numeric_variables(packed_state.get_numeric_variables(), m_uint_nodes, m_float_nodes, buffer, unpacked_state->get_numeric_variables());

        return State(*this, std::move(unpacked_state));
    }

    StateIndex register_state(const UnpackedState& state)
    {
        thread_local auto buffer = std::vector<uint_t> {};

        auto fluent_atoms = create_atoms_slot(state.get_atoms<formalism::FluentTag>(), buffer, m_uint_nodes);
        auto derived_atoms = create_atoms_slot(state.get_atoms<formalism::DerivedTag>(), buffer, m_uint_nodes);
        auto numeric_variables = create_numeric_variables_slot(state.get_numeric_variables(), buffer, m_uint_nodes, m_float_nodes);

        return m_packed_states.insert(PackedState(StateIndex(m_packed_states.size()), fluent_atoms, derived_atoms, numeric_variables));
    }

    View<Index<formalism::planning::Task>, formalism::Repository> get_task() const;

    Node<Task> get_initial_node() { return m_initial_node; }

    std::vector<std::pair<View<Index<formalism::planning::GroundAction>, formalism::Repository>, Node<Task>>> get_successor_nodes(const Node<Task>& node)
    {
        return self().get_successor_nodes_impl(node);
    }

    void get_successor_nodes(const Node<Task>& node,
                             std::vector<std::pair<View<Index<formalism::planning::GroundAction>, formalism::Repository>, Node<Task>>>& out_nodes)
    {
        self().get_successor_nodes_impl(node, out_nodes);
    }

protected:
    std::shared_ptr<formalism::Repository> m_repository;
    Index<formalism::planning::Task> m_task_index;

    // States
    valla::IndexedHashSet<valla::Slot<uint_t>, uint_t> m_uint_nodes;
    valla::IndexedHashSet<float_t, uint_t> m_float_nodes;
    IndexedHashSet<PackedState, StateIndex> m_packed_states;
    SharedObjectPool<UnpackedState> m_unpacked_state_pool;

    // TODO: initial node
    Node<Task> m_initial_node;
};

}

#endif
