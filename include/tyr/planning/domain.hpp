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

#include "tyr/formalism/planning/declarations.hpp"
#include "tyr/formalism/planning/repository.hpp"
#include "tyr/formalism/planning/views.hpp"

namespace tyr::planning
{

class Domain
{
public:
    Domain(std::shared_ptr<formalism::planning::Repository> repository, View<Index<formalism::planning::Domain>, formalism::planning::Repository> domain);

    const std::shared_ptr<formalism::planning::Repository>& get_repository() const noexcept;
    View<Index<formalism::planning::Domain>, formalism::planning::Repository> get_domain() const noexcept;

private:
    std::shared_ptr<formalism::planning::Repository> m_repository;
    View<Index<formalism::planning::Domain>, formalism::planning::Repository> m_domain;
};

}

#endif
