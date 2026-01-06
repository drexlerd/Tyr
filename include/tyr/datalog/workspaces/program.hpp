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

#ifndef TYR_DATALOG_WORKSPACES_PROGRAM_HPP_
#define TYR_DATALOG_WORKSPACES_PROGRAM_HPP_

#include "tyr/common/equal_to.hpp"
#include "tyr/common/hash.hpp"
#include "tyr/datalog/program_context.hpp"
#include "tyr/datalog/rule_scheduler.hpp"
#include "tyr/datalog/workspaces/d2p.hpp"
#include "tyr/datalog/workspaces/facts.hpp"
#include "tyr/datalog/workspaces/p2d.hpp"
#include "tyr/datalog/workspaces/rule.hpp"
#include "tyr/datalog/workspaces/rule_delta.hpp"
#include "tyr/datalog/workspaces/worker.hpp"
#include "tyr/formalism/datalog/builder.hpp"
#include "tyr/formalism/planning/builder.hpp"

#include <chrono>
#include <oneapi/tbb/enumerable_thread_specific.h>
#include <vector>

namespace tyr::datalog
{
class CostBuckets
{
public:
    using Atom = Index<formalism::datalog::GroundAtom<formalism::FluentTag>>;
    using Bucket = UnorderedSet<Atom>;
    using Cost = uint_t;

    CostBuckets() : m_buckets(1), m_current(0), m_total_size(0) {}

    void clear() noexcept
    {
        for (auto& b : m_buckets)
            b.clear();
        m_total_size = 0;
        m_current = 0;
    }

    [[nodiscard]] Cost current_cost() const noexcept { return m_current; }

    [[nodiscard]] bool empty() const noexcept { return m_total_size == 0; }

    void resize_to_fit(Cost c)
    {
        if (c >= m_buckets.size())
            m_buckets.resize(static_cast<size_t>(c) + 1);
    }

    bool insert(Cost c, Atom a)
    {
        resize_to_fit(c);
        const auto [it, inserted] = m_buckets[c].insert(a);
        if (inserted)
            ++m_total_size;
        return inserted;
    }

    bool erase(Cost c, Atom a)
    {
        if (c >= m_buckets.size())
            return false;
        const auto erased = m_buckets[c].erase(a) > 0;
        if (erased)
            --m_total_size;
        return erased;
    }

    void update(const CostUpdate& update, Atom a)
    {
        if (update.old_cost.has_value())
            erase(*update.old_cost, a);
        insert(update.new_cost, a);
    }

    void clear_current()
    {
        if (m_current >= m_buckets.size())
            return;
        m_total_size -= m_buckets[m_current].size();
        m_buckets[m_current].clear();
    }

    bool advance_to_next_nonempty()
    {
        while (m_current < m_buckets.size() && m_buckets[m_current].empty())
            ++m_current;
        return m_current < m_buckets.size();
    }

    const Bucket& get_current_bucket() const
    {
        static const Bucket kEmpty {};
        if (m_current >= m_buckets.size())
            return kEmpty;
        return m_buckets[m_current];
    }

private:
    std::vector<Bucket> m_buckets;
    uint_t m_current = 0;
    size_t m_total_size = 0;
};

struct ProgramWorkspace
{
    formalism::datalog::Repository& repository;
    FactsWorkspace facts;

    std::vector<RuleWorkspace> rules;
    std::vector<RuleDeltaWorkspace> rule_deltas;

    D2PWorkspace d2p;
    P2DWorkspace p2d;

    oneapi::tbb::enumerable_thread_specific<WorkerWorkspace> worker;

    formalism::planning::Builder planning_builder;
    formalism::datalog::Builder datalog_builder;

    RuleSchedulerStrata schedulers;

    CostBuckets cost_buckets;

    struct Statistics
    {
        std::chrono::nanoseconds ground_seq_total_time { 0 };
        std::chrono::nanoseconds merge_seq_total_time { 0 };

        size_t num_merges_inserted { 0 };
        size_t num_merges_discarded { 0 };
    } statistics;

    explicit ProgramWorkspace(ProgramContext& context, const ConstProgramWorkspace& cws);

    void clear() noexcept;
};

struct ConstProgramWorkspace
{
    ConstFactsWorkspace facts;

    std::vector<ConstRuleWorkspace> rules;

    explicit ConstProgramWorkspace(ProgramContext& context);
};
}

#endif
