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

#ifndef TYR_FORMALISM_PROGRAM_PROXY_HPP_
#define TYR_FORMALISM_PROGRAM_PROXY_HPP_

#include "tyr/common/span.hpp"
#include "tyr/formalism/declarations.hpp"
#include "tyr/formalism/function_index.hpp"
#include "tyr/formalism/ground_atom_index.hpp"
#include "tyr/formalism/ground_function_term_value_index.hpp"
#include "tyr/formalism/predicate_index.hpp"
#include "tyr/formalism/program_index.hpp"
#include "tyr/formalism/repository.hpp"
#include "tyr/formalism/rule_proxy.hpp"

namespace tyr::formalism
{
template<IsContext C>
class ProgramProxy
{
private:
    const C* context;
    ProgramIndex index;

public:
    ProgramProxy(ProgramIndex index, const C& context) : context(&context), index(index) {}

    const C& get_context() const { return *context; }
    const auto& get() const { return get_repository(*context)[index]; }

    auto get_index() const { return index; }
    template<IsStaticOrFluentTag T>
    auto get_predicates() const
    {
        return SpanProxy<PredicateIndex<T>, C>(get().template get_predicates<T>(), *context);
    }
    template<IsStaticOrFluentTag T>
    auto get_functions() const
    {
        return SpanProxy<FunctionIndex<T>, C>(get().template get_functions<T>(), *context);
    }
    auto get_objects() const { return SpanProxy<ObjectIndex, C>(get().objects, *context); }
    template<IsStaticOrFluentTag T>
    auto get_atoms() const
    {
        return SpanProxy<GroundAtomIndex<T>, C>(get().template get_atoms<T>(), *context);
    }
    template<IsStaticOrFluentTag T>
    auto get_function_values() const
    {
        return SpanProxy<GroundFunctionTermValueIndex<T>, C>(get().template get_function_values<T>(), *context);
    }
    auto get_rules() const { return SpanProxy<RuleIndex, C>(get().rules, *context); }
};
}

#endif
