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

#include "tyr/common/equal_to.hpp"
#include "tyr/common/hash.hpp"
#include "tyr/datalog/program_context.hpp"
#include "tyr/datalog/workspaces/program.hpp"
#include "tyr/formalism/datalog/declarations.hpp"
#include "tyr/formalism/datalog/repository.hpp"
#include "tyr/formalism/datalog/views.hpp"
#include "tyr/formalism/planning/declarations.hpp"
#include "tyr/formalism/planning/repository.hpp"
#include "tyr/formalism/planning/views.hpp"
#include "tyr/planning/declarations.hpp"

namespace tyr::planning
{

class AxiomEvaluatorProgram
{
public:
    // Mapping from program fluent predicate to task derived predicate
    using PredicateToPredicateMapping = UnorderedMap<Index<formalism::Predicate<formalism::FluentTag>>, Index<formalism::Predicate<formalism::DerivedTag>>>;

    explicit AxiomEvaluatorProgram(View<Index<formalism::planning::Task>, formalism::planning::Repository> task);

    const PredicateToPredicateMapping& get_predicate_to_predicate_mapping() const noexcept;
    datalog::ProgramContext& get_program_context() noexcept;
    const datalog::ProgramContext& get_program_context() const noexcept;
    const datalog::ConstProgramWorkspace& get_const_program_workspace() const noexcept;

private:
    PredicateToPredicateMapping m_predicate_to_predicate;

    datalog::ProgramContext m_program_context;

    datalog::ConstProgramWorkspace m_program_workspace;
};

}

#endif