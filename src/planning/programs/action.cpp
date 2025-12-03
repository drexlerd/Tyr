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

#include "tyr/planning/programs/action.hpp"

namespace tyr::planning
{
ApplicableActionProgram::ApplicableActionProgram(View<Index<formalism::Program>, formalism::Repository> program,
                                                 std::shared_ptr<formalism::Repository> repository) :
    repository(std::move(repository)),
    program(program)
{
}

ApplicableActionProgram create_applicable_action_program(const LiftedTask& task)
{
    auto repository = std::make_shared<formalism::Repository>();
    auto builder = formalism::Builder();
    auto program_ptr = builder.get_builder<formalism::Program>();
    auto& program = *program_ptr;

    // TODO: build the program and mapping

    formalism::canonicalize(program);
    const auto program_view = repository->get_or_create(program, builder.get_buffer()).first;
    return ApplicableActionProgram(program_view, std::move(repository));
}
}
