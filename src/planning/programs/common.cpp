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

#include "common.hpp"

namespace f = tyr::formalism;
namespace fp = tyr::formalism::planning;
namespace fd = tyr::formalism::datalog;

namespace tyr::planning
{

::cista::offset::string create_applicability_name(View<Index<fp::Action>, fp::Repository> action)
{
    return ::cista::offset::string { std::string { "@" } + action.get_name().str() + std::string("_") + std::to_string(action.get_index().get_value()) };
}

::cista::offset::string create_triggered_name(View<Index<fp::Action>, fp::Repository> action, View<Index<fp::ConditionalEffect>, fp::Repository> cond_eff)
{
    return ::cista::offset::string { create_applicability_name(action).str() + std::string("_") + std::to_string(cond_eff.get_index().get_value()) };
}

::cista::offset::string create_applicability_name(View<Index<fp::Axiom>, fp::Repository> axiom)
{
    return ::cista::offset::string { std::string { "@" } + axiom.get_head().get_predicate().get_name().str() + std::string("_")
                                     + std::to_string(axiom.get_index().get_value()) };
}

}
