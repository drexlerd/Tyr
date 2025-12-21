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

#ifndef TYR_GROUNDER_RULE_SCHEDULER_HPP_
#define TYR_GROUNDER_RULE_SCHEDULER_HPP_

#include "tyr/formalism/predicate_view.hpp"
#include "tyr/formalism/repository.hpp"
#include "tyr/formalism/rule_view.hpp"
//
#include "tyr/analysis/listeners.hpp"
#include "tyr/analysis/stratification.hpp"

namespace tyr::grounder
{

class RuleSchedulerStratum
{
public:
    RuleSchedulerStratum(const analysis::RuleStratum& rules, const analysis::ListenerStratum& listeners, const formalism::Repository& context);

    void clear() noexcept;

    void activate_all();

    void on_generate(Index<formalism::Predicate<formalism::FluentTag>> predicate);

    View<IndexList<formalism::Rule>, formalism::Repository> active_rules();

private:
    const analysis::RuleStratum& m_rules;
    const analysis::ListenerStratum& m_listeners;
    const formalism::Repository& m_context;

    UnorderedSet<Index<formalism::Rule>> m_active_set;  ///< build active set
    IndexList<formalism::Rule> m_active;                ///< final active set
};

struct RuleSchedulerStrata
{
    std::vector<RuleSchedulerStratum> data;
};

extern RuleSchedulerStrata
create_rule_scheduler_strata(const analysis::RuleStrata& rules, const analysis::ListenerStrata& listeners, const formalism::Repository& context);

}

#endif