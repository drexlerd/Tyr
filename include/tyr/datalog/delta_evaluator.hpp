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

#ifndef TYR_DATALOG_DELTA_EVALUATOR_HPP_
#define TYR_DATALOG_DELTA_EVALUATOR_HPP_

#include "tyr/common/declarations.hpp"
#include "tyr/formalism/binding_view.hpp"
#include "tyr/formalism/datalog/atom_index.hpp"
#include "tyr/formalism/datalog/declarations.hpp"
#include "tyr/formalism/datalog/rule_index.hpp"

namespace tyr::datalog::evaluator
{

/**
 * AtomProxy and GroundAtomProxy define hash and comparison
 * such that an AtomProxy P1 and a GroundAtomProxy get the same hash and compare equally
 * if and only if the ground atom corresponding to the AtomProxy hashes
 * and compares equally with the ground atom in the GroundAtomProxy.
 */

template<formalism::datalog::Context C>
struct AtomProxy
{
    size_t index;
    Index<formalism::datalog::Atom<formalism::FluentTag>> atom;
    View<Index<formalism::Binding>, C> binding;
    size_t hash;
};

template<formalism::datalog::Context C>
struct GroundAtomProxy
{
    View<Index<formalism::datalog::GroundAtom<formalism::FluentTag>>, C> atom;
};

/**
 * Witness is a rule that we have to ground for the given binding.
 */

template<formalism::datalog::Context C>
struct Witness
{
    Index<formalism::datalog::Rule> rule;
    View<Index<formalism::Binding>, C> binding;  ///< This binding way be different than the projected atom.
    size_t num_unsatisfied;                      ///< when this reaches 0, ground the rule using the `groundings` container
};

template<formalism::datalog::Context C>
class DeltaEvaluator
{
public:
    void insert(const Witness<C>& witness);

    /// @brief Insert an atom
    /// TODO: how to hash/compare GroundAtom and ProjectedAtom?
    /// @tparam Callback
    /// @param atom
    /// @param callback
    template<typename Callback>
    void insert(View<Index<formalism::datalog::GroundAtom<formalism::FluentTag>>, C> atom, Callback&& callback);

private:
    UnorderedMap<AtomProxy<C>, std::vector<Witness<C>>> subscribers;

    UnorderedMap<AtomProxy<C>, Index<formalism::datalog::GroundAtom<formalism::FluentTag>>> groundings;
};

}

#endif