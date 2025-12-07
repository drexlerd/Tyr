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

#include "tyr/planning/formatter.hpp"

#include "tyr/formalism/overlay_repository.hpp"
#include "tyr/formalism/repository.hpp"
#include "tyr/planning/domain.hpp"
#include "tyr/planning/ground_task.hpp"
#include "tyr/planning/lifted_task.hpp"

#include <fmt/core.h>
#include <fmt/ostream.h>
#include <fmt/ranges.h>
#include <ostream>

namespace tyr
{
std::ostream& print(std::ostream& os, const planning::Domain& el)
{
    fmt::print(os, "{}", to_string(el.get_domain()));
    return os;
}

std::ostream& print(std::ostream& os, const planning::LiftedTask& el)
{
    fmt::print(os, "{}", to_string(el.get_task()));
    return os;
}

std::ostream& print(std::ostream& os, const planning::GroundTask& el)
{
    fmt::print(os, "{}", to_string(el.get_task()));
    return os;
}

namespace planning
{
std::ostream& operator<<(std::ostream& os, const Domain& el) { return tyr::print(os, el); }

std::ostream& operator<<(std::ostream& os, const LiftedTask& el) { return tyr::print(os, el); }

std::ostream& operator<<(std::ostream& os, const GroundTask& el) { return tyr::print(os, el); }
}
}
