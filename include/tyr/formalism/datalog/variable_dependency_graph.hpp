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

#ifndef TYR_FORMALISM_DATALOG_VARIABLE_DEPENDENCY_GRAPH_HPP_
#define TYR_FORMALISM_DATALOG_VARIABLE_DEPENDENCY_GRAPH_HPP_

#include "tyr/common/types.hpp"
#include "tyr/common/variant.hpp"
#include "tyr/common/vector.hpp"
#include "tyr/formalism/datalog/declarations.hpp"
#include "tyr/formalism/datalog/repository.hpp"
#include "tyr/formalism/datalog/views.hpp"

namespace tyr::formalism::datalog
{

class VariableDependencyGraph
{
private:
    template<FactKind T, PolarityKind P>
    const auto& get_dependency() const noexcept
    {
        if constexpr (std::is_same_v<T, StaticTag>)
            if constexpr (std::is_same_v<P, PositiveTag>)
                return m_static_positive_dependencies;
            else if constexpr (std::is_same_v<P, NegativeTag>)
                return m_static_negative_dependencies;
            else
                static_assert(dependent_false<P>::value, "Missing case");
        else if constexpr (std::is_same_v<T, FluentTag>)
            if constexpr (std::is_same_v<P, PositiveTag>)
                return m_fluent_positive_dependencies;
            else if constexpr (std::is_same_v<P, NegativeTag>)
                return m_fluent_negative_dependencies;
            else
                static_assert(dependent_false<P>::value, "Missing case");
        else
            static_assert(dependent_false<T>::value, "Missing case");
    }

public:
    explicit VariableDependencyGraph(ConjunctiveConditionView condition);

    static constexpr uint_t get_index(uint_t pi, uint_t pj, uint_t k) noexcept
    {
        assert(pi < k && pj < k);
        return pi * k + pj;
    }

    template<FactKind T, PolarityKind P>
    bool has_dependency(uint_t pi, uint_t pj) const noexcept
    {
        return get_dependency<T, P>().test(get_index(pi, pj, m_k));
    }

    template<FactKind T>
    bool has_dependency(uint_t pi, uint_t pj) const noexcept
    {
        return has_dependency<T, PositiveTag>(pi, pj) || has_dependency<T, NegativeTag>(pi, pj);
    }

    template<PolarityKind P>
    bool has_dependency(uint_t pi, uint_t pj) const noexcept
    {
        return has_dependency<StaticTag, P>(pi, pj) || has_dependency<FluentTag, P>(pi, pj);
    }

    bool has_dependency(uint_t pi, uint_t pj) const noexcept { return has_dependency<StaticTag>(pi, pj) || has_dependency<FluentTag>(pi, pj); }

    auto k() const noexcept { return m_k; }

private:
    uint_t m_k;
    boost::dynamic_bitset<> m_static_positive_dependencies;
    boost::dynamic_bitset<> m_static_negative_dependencies;
    boost::dynamic_bitset<> m_fluent_positive_dependencies;
    boost::dynamic_bitset<> m_fluent_negative_dependencies;
};
}

#endif
