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

#include "tyr/grounder/rule_scheduler.hpp"

#include "tyr/formalism/formatter.hpp"

namespace tyr::grounder
{

RuleSchedulerStratum::RuleSchedulerStratum(const analysis::RuleStratum& rules,
                                           const analysis::ListenerStratum& listeners,
                                           const formalism::Repository& context) :
    m_rules(rules),
    m_listeners(listeners),
    m_context(context),
    m_active_set(),
    m_active()
{
}

void RuleSchedulerStratum::activate_all()
{
    m_active_set.clear();
    for (const auto rule : m_rules)
        m_active_set.insert(rule);
}

void RuleSchedulerStratum::clear() noexcept { m_active_set.clear(); }

void RuleSchedulerStratum::on_generate(Index<formalism::Predicate<formalism::FluentTag>> predicate)
{
    if (const auto it = m_listeners.find(predicate); it != m_listeners.end())
        for (const auto rule : it->second)
            m_active_set.insert(rule);
}

View<IndexList<formalism::Rule>, formalism::Repository> RuleSchedulerStratum::active_rules()
{
    m_active.clear();
    for (const auto rule : m_active_set)
        m_active.push_back(rule);

    m_active_set.clear();

    return make_view(m_active, m_context);
}

RuleSchedulerStrata
create_rule_scheduler_strata(const analysis::RuleStrata& rules, const analysis::ListenerStrata& listeners, const formalism::Repository& context)
{
    assert(rules.data.size() == listeners.data.size());

    auto result = RuleSchedulerStrata {};
    for (uint_t i = 0; i < rules.data.size(); ++i)
        result.data.emplace_back(rules.data[i], listeners.data[i], context);

    return result;
}

}
