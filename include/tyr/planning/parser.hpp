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

#ifndef TYR_PLANNING_PARSER_HPP_
#define TYR_PLANNING_PARSER_HPP_

#include "tyr/formalism/declarations.hpp"
#include "tyr/planning/declarations.hpp"

#include <loki/loki.hpp>

namespace tyr::planning
{
class Parser
{
public:
    explicit Parser(const fs::path& domain_filepath, const loki::ParserOptions& options = loki::ParserOptions());

    LiftedTaskPtr parse_task(const fs::path& problem_filepath, const loki::ParserOptions& options = loki::ParserOptions());

    DomainPtr get_domain() const;

private:
    loki::Parser m_loki_parser;
    loki::DomainTranslationResult m_loki_domain_translation_result;

    formalism::RepositoryPtr m_domain_repository;
    DomainPtr m_domain;
};
}

#endif
