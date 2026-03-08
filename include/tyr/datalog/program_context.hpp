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

#ifndef TYR_DATALOG_PROGRAM_CONTEXT_HPP_
#define TYR_DATALOG_PROGRAM_CONTEXT_HPP_

#include "tyr/analysis/domains.hpp"
#include "tyr/analysis/listeners.hpp"
#include "tyr/analysis/stratification.hpp"
#include "tyr/formalism/datalog/program_index.hpp"
#include "tyr/formalism/datalog/repository.hpp"

namespace tyr::datalog
{
/// @brief `ProgramContext` stores the program and all derived necessities together.
class ProgramContext
{
public:
    ProgramContext(View<Index<formalism::datalog::Program>, formalism::datalog::Repository> program,
                   formalism::datalog::RepositoryPtr repository,
                   analysis::ProgramVariableDomains domains,
                   analysis::RuleStrata strata,
                   analysis::ListenerStrata listeners) :
        program(program),
        repository(std::move(repository)),
        domains(std::move(domains)),
        strata(std::move(strata)),
        listeners(std::move(listeners))
    {
    }
    auto get_program() const noexcept { return program; }
    auto& get_repository() noexcept { return *repository; }
    const auto& get_repository() const noexcept { return *repository; }
    const auto& get_domains() const noexcept { return domains; }
    const auto& get_strata() const noexcept { return strata; }
    const auto& get_listeners() const noexcept { return listeners; }

private:
    View<Index<formalism::datalog::Program>, formalism::datalog::Repository> program;
    formalism::datalog::RepositoryPtr repository;
    analysis::ProgramVariableDomains domains;
    analysis::RuleStrata strata;
    analysis::ListenerStrata listeners;
};
}
#endif
