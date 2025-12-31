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

#include "tyr/datalog/rule_scheduler.hpp"

#include "tyr/common/config.hpp"  // for uint_t
#include "tyr/datalog/rule_scheduler.hpp"
#include "tyr/formalism/datalog/formatter.hpp"
#include "tyr/formalism/datalog/views.hpp"  // for View

#include <assert.h>       // for assert
#include <gtl/phmap.hpp>  // for operator!=, flat_hash_set
#include <utility>        // for pair

using namespace tyr::formalism;
using namespace tyr::formalism::datalog;

namespace tyr::datalog
{

RuleSchedulerStratum::RuleSchedulerStratum(const analysis::RuleStratum& rules, const analysis::ListenerStratum& listeners, const Repository& context) :
    m_rules(rules),
    m_listeners(listeners),
    m_context(context),
    m_active_predicates(),
    m_active_set(),
    m_active()
{
    for (const auto rule : rules)
    {
        const auto predicate = uint_t(make_view(rule, context).get_head().get_predicate().get_index());
        if (predicate >= m_active_predicates.size())
            m_active_predicates.resize(predicate + 1, false);
    }
}

void RuleSchedulerStratum::activate_all()
{
    m_active.clear();
    for (const auto rule : m_rules)
        m_active.push_back(rule);
}

void RuleSchedulerStratum::on_start_iteration() noexcept { m_active_predicates.reset(); }

void RuleSchedulerStratum::on_generate(Index<Predicate<FluentTag>> predicate)
{
    assert(uint_t(predicate) < m_active_predicates.size());

    m_active_predicates.set(uint_t(predicate));
}

void RuleSchedulerStratum::on_finish_iteration()
{
    m_active_set.clear();
    for (auto i = m_active_predicates.find_first(); i != boost::dynamic_bitset<>::npos; i = m_active_predicates.find_next(i))
        if (const auto it = m_listeners.find(Index<Predicate<FluentTag>>(i)); it != m_listeners.end())
            for (const auto rule : it->second)
                m_active_set.insert(rule);

    m_active.clear();
    for (const auto rule : m_active_set)
        m_active.push_back(rule);
}

View<IndexList<Rule>, Repository> RuleSchedulerStratum::get_active_rules() { return make_view(m_active, m_context); }

RuleSchedulerStrata create_rule_scheduler_strata(const analysis::RuleStrata& rules, const analysis::ListenerStrata& listeners, const Repository& context)
{
    assert(rules.data.size() == listeners.data.size());

    auto result = RuleSchedulerStrata {};
    for (uint_t i = 0; i < rules.data.size(); ++i)
        result.data.emplace_back(rules.data[i], listeners.data[i], context);

    return result;
}

}
