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

#ifndef TYR_PLANNING_DOMAIN_HPP_
#define TYR_PLANNING_DOMAIN_HPP_

#include "tyr/formalism/declarations.hpp"
#include "tyr/formalism/views.hpp"

namespace tyr::planning
{

class Domain
{
public:
    Domain(std::shared_ptr<formalism::Repository> repository, View<Index<formalism::Domain>, formalism::Repository> domain);

    View<Index<formalism::Domain>, formalism::Repository> get_domain() const noexcept;

private:
    std::shared_ptr<formalism::Repository> m_repository;
    View<Index<formalism::Domain>, formalism::Repository> m_domain;
};

}

#endif
