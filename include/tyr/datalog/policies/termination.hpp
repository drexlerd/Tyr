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

#ifndef TYR_SOLVER_POLICIES_TERMINATION_HPP_
#define TYR_SOLVER_POLICIES_TERMINATION_HPP_

#include "tyr/common/config.hpp"
#include "tyr/common/dynamic_bitset.hpp"
#include "tyr/datalog/fact_sets.hpp"
#include "tyr/datalog/policies/aggregation.hpp"
#include "tyr/datalog/policies/annotation_types.hpp"
#include "tyr/formalism/datalog/declarations.hpp"
#include "tyr/formalism/datalog/ground_atom_index.hpp"
#include "tyr/formalism/datalog/repository.hpp"
#include "tyr/formalism/datalog/views.hpp"

#include <boost/dynamic_bitset.hpp>
#include <concepts>

namespace tyr::datalog
{

template<typename T>
concept TerminationPolicyConcept = requires(T& p,
                                            const T& cp,
                                            formalism::datalog::PredicateBindingView<formalism::FluentTag> binding,
                                            const PredicateFactSets<formalism::FluentTag>& goals,
                                            const OrAnnotationsList& or_annot) {
    { p.set_goals(goals) } -> std::same_as<void>;
    { p.achieve(binding) } -> std::same_as<void>;
    { cp.check() } -> std::same_as<bool>;
    { cp.get_total_cost(or_annot) } -> std::same_as<Cost>;
    { p.reset() } -> std::same_as<void>;
    { p.clear() } -> std::same_as<void>;
};

class NoTerminationPolicy
{
public:
    NoTerminationPolicy() = default;

    void set_goals(const PredicateFactSets<formalism::FluentTag>& goals) {}
    void achieve(formalism::datalog::PredicateBindingView<formalism::FluentTag> binding) noexcept {}
    bool check() const noexcept { return false; }
    Cost get_total_cost(const OrAnnotationsList& or_annot) const noexcept { return Cost(0); }
    void reset() noexcept {}
    void clear() noexcept {}
};

template<typename AggregationFunction>
class TerminationPolicy
{
public:
    explicit TerminationPolicy(size_t num_fluent_predicates);

    void set_goals(const PredicateFactSets<formalism::FluentTag>& goals);

    void achieve(formalism::datalog::PredicateBindingView<formalism::FluentTag> binding) noexcept;

    bool check() const noexcept;

    Cost get_total_cost(const OrAnnotationsList& or_annot) const noexcept;

    void reset() noexcept;

    void clear() noexcept;

    const std::vector<formalism::datalog::PredicateBindingView<formalism::FluentTag>>& get_bindings() const noexcept { return bindings; }

private:
    std::vector<boost::dynamic_bitset<>> unsat_goals;
    size_t num_unsat_goals;

    std::vector<formalism::datalog::PredicateBindingView<formalism::FluentTag>> bindings;

    AggregationFunction agg;
};
}

#endif
