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

#ifndef TYR_PLANNING_PROGRAMS_AXIOM_HPP_
#define TYR_PLANNING_PROGRAMS_AXIOM_HPP_

#include "tyr/formalism/views.hpp"
#include "tyr/planning/declarations.hpp"

namespace tyr::planning
{

class AxiomEvaluatorProgram
{
public:
    // Mapping from program fluent predicate to task derived predicate
    using PredicateToPredicateMapping =
        UnorderedMap<View<Index<formalism::Predicate<formalism::FluentTag>>, formalism::Repository>,
                     View<Index<formalism::Predicate<formalism::DerivedTag>>, formalism::OverlayRepository<formalism::Repository>>>;

    explicit AxiomEvaluatorProgram(const LiftedTask& task);

    View<Index<formalism::Program>, formalism::Repository> get_program() const;
    const formalism::RepositoryPtr& get_repository() const;

private:
    PredicateToPredicateMapping prediate_to_predicate;

    formalism::RepositoryPtr m_repository;
    View<Index<formalism::Program>, formalism::Repository> m_program;
};

}

#endif