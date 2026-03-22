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

#include "tyr/formalism/planning/fdr_context.hpp"

#include "tyr/buffer/declarations.hpp"
#include "tyr/common/equal_to.hpp"
#include "tyr/common/hash.hpp"
#include "tyr/common/types.hpp"
#include "tyr/formalism/planning/builder.hpp"
#include "tyr/formalism/planning/canonicalization.hpp"
#include "tyr/formalism/planning/declarations.hpp"
#include "tyr/formalism/planning/fdr_fact_view.hpp"
#include "tyr/formalism/planning/fdr_value.hpp"
#include "tyr/formalism/planning/fdr_variable_data.hpp"
#include "tyr/formalism/planning/fdr_variable_index.hpp"
#include "tyr/formalism/planning/fdr_variable_view.hpp"
#include "tyr/formalism/planning/ground_atom_view.hpp"
#include "tyr/formalism/planning/ground_literal_view.hpp"
#include "tyr/formalism/planning/merge.hpp"
#include "tyr/formalism/planning/repository.hpp"

#include <concepts>
#include <vector>

namespace tyr::formalism::planning
{
namespace
{
std::pair<FDRVariableView<FluentTag>, bool> merge_p2p(FDRVariableView<FluentTag> element, MergeContext& context)
{
    auto variable_ptr = context.builder.template get_builder<FDRVariable<FluentTag>>();
    auto& variable = *variable_ptr;
    variable.clear();

    for (const auto atom : element.get_atoms())
        variable.atoms.push_back(merge_p2p(atom, context).first.get_index());

    canonicalize(variable);
    return context.destination.get_or_create(variable);
}

Data<FDRFact<FluentTag>> merge_p2p(FDRFactView<FluentTag> element, MergeContext& context)
{
    return Data<FDRFact<FluentTag>>(merge_p2p(element.get_variable(), context).first.get_index(), element.get_value());
}
}

FDRContext::FDRContext(RepositoryPtr context) : m_context(std::move(context)), m_builder(), m_variables(), m_mapping() {}

FDRContext::FDRContext(const std::vector<std::vector<GroundAtomView<FluentTag>>>& mutexes, RepositoryPtr context) :
    m_context(std::move(context)),
    m_builder(),
    m_variables(),
    m_mapping()
{
    auto variable = Data<FDRVariable<FluentTag>>();

    for (const auto& group : mutexes)
    {
        variable.clear();
        for (const auto& atom : group)
            variable.atoms.push_back(atom.get_index());
        canonicalize(variable);
        const auto var_index = m_context->get_or_create(variable).first.get_index();
        m_variables.push_back(var_index);
        for (uint_t i = 0; i < group.size(); ++i)
        {
            [[maybe_unused]] const auto [it, inserted] = m_mapping.emplace(group[i].get_index(), Data<FDRFact<FluentTag>>(var_index, FDRValue { i + 1 }));
            assert(inserted && "Assumes non overlapping mutex groups");
        }
    }
}

FDRContext::FDRContext(const FDRContext& other, Builder& builder, RepositoryPtr context) :
    m_context(std::move(context)),
    m_builder(),
    m_variables(),
    m_mapping()
{
    auto merge_context = MergeContext { builder, *m_context };

    for (const auto variable : make_view(other.m_variables, *other.m_context))
        m_variables.push_back(merge_p2p(variable, merge_context).first.get_index());

    for (const auto [atom_index, fact_data] : other.m_mapping)
        m_mapping.emplace(merge_p2p(make_view(atom_index, *other.m_context), merge_context).first.get_index(),
                          merge_p2p(make_view(fact_data, *other.m_context), merge_context));
}

Data<FDRFact<FluentTag>> FDRContext::get_fact(Index<GroundAtom<FluentTag>> atom)
{
    // Find explicit ground mutex group assignment
    if (auto it = m_mapping.find(atom); it != m_mapping.end())
        return it->second;

    // Construct a new binary FDR variable
    m_builder.clear();
    m_builder.atoms.push_back(atom);
    canonicalize(m_builder);
    const auto var_index = m_context->get_or_create(m_builder).first.get_index();

    m_variables.push_back(var_index);
    const auto fact = Data<FDRFact<FluentTag>>(var_index, FDRValue { 1 });
    m_mapping.emplace(atom, fact);

    return fact;
}

Data<FDRFact<FluentTag>> FDRContext::get_fact(GroundAtomView<FluentTag> atom) { return get_fact(atom.get_index()); }

FDRFactView<FluentTag> FDRContext::get_fact_view(GroundAtomView<FluentTag> atom) { return make_view(get_fact(atom), *m_context); }

Data<FDRFact<FluentTag>> FDRContext::get_fact(Index<GroundLiteral<FluentTag>> literal) { return get_fact(make_view(literal, *m_context)); }

Data<FDRFact<FluentTag>> FDRContext::get_fact(GroundLiteralView<FluentTag> literal)
{
    auto pos_fact = this->get_fact(literal.get_atom().get_index());

    if (literal.get_polarity())
        return pos_fact;

    return Data<FDRFact<FluentTag>>(make_view(pos_fact, *m_context).get_variable().get_index(), FDRValue { 0 });
}

FDRFactView<FluentTag> FDRContext::get_fact_view(GroundLiteralView<FluentTag> literal) { return make_view(get_fact(literal), *m_context); }

FDRVariableListView<FluentTag> FDRContext::get_variables() const { return make_view(m_variables, *m_context); }

}
