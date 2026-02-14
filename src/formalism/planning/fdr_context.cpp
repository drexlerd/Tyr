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
#include "tyr/formalism/planning/canonicalization.hpp"
#include "tyr/formalism/planning/declarations.hpp"
#include "tyr/formalism/planning/fdr_value.hpp"
#include "tyr/formalism/planning/fdr_variable_data.hpp"
#include "tyr/formalism/planning/fdr_variable_index.hpp"
#include "tyr/formalism/planning/fdr_variable_view.hpp"
#include "tyr/formalism/planning/ground_atom_view.hpp"
#include "tyr/formalism/planning/ground_literal_view.hpp"
#include "tyr/formalism/planning/repository.hpp"

#include <concepts>
#include <vector>

namespace tyr::formalism::planning
{

BinaryFDRContext::BinaryFDRContext(Repository& context) : m_context(context), m_variables(), m_mapping() {}

Data<FDRFact<FluentTag>> BinaryFDRContext::get_fact(Index<GroundAtom<FluentTag>> atom)
{
    if (auto it = m_mapping.find(atom); it != m_mapping.end())
        return it->second;

    m_builder.clear();
    m_builder.domain_size = 2;
    m_builder.atoms.push_back(atom);
    canonicalize(m_builder);
    const auto var_index = m_context.get_or_create(m_builder, m_buffer).first;

    m_variables.push_back(var_index);
    const auto fact = Data<FDRFact<FluentTag>>(var_index, FDRValue { 1 });
    m_mapping.emplace(atom, fact);

    return fact;
}

Data<FDRFact<FluentTag>> BinaryFDRContext::get_fact(Index<GroundLiteral<FluentTag>> literal)
{
    auto literal_view = make_view(literal, m_context);
    auto pos_fact = this->get_fact(literal_view.get_atom().get_index());

    if (literal_view.get_polarity())
        return pos_fact;

    return Data<FDRFact<FluentTag>>(make_view(pos_fact, m_context).get_variable().get_index(), FDRValue { 0 });
}

View<IndexList<FDRVariable<FluentTag>>, Repository> BinaryFDRContext::get_variables() const { return make_view(m_variables, m_context); }

GeneralFDRContext::GeneralFDRContext(const std::vector<std::vector<View<Index<GroundAtom<FluentTag>>, Repository>>>& mutexes, Repository& context) :
    m_context(context),
    m_variables(),
    m_mapping()
{
    auto buffer = buffer::Buffer();
    auto variable = Data<FDRVariable<FluentTag>>();

    for (const auto& group : mutexes)
    {
        variable.clear();
        variable.domain_size = group.size() + 1;
        for (const auto& atom : group)
            variable.atoms.push_back(atom.get_index());
        canonicalize(variable);
        const auto new_variable = context.get_or_create(variable, buffer).first;
        m_variables.push_back(new_variable);
        for (uint_t i = 0; i < group.size(); ++i)
            m_mapping.emplace(group[i].get_index(), Data<FDRFact<FluentTag>>(new_variable, FDRValue { i + 1 }));
    }
}

Data<FDRFact<FluentTag>> GeneralFDRContext::get_fact(Index<GroundAtom<FluentTag>> atom) const { return m_mapping.at(atom); }

Data<FDRFact<FluentTag>> GeneralFDRContext::get_fact(Index<GroundLiteral<FluentTag>> literal) const
{
    auto literal_view = make_view(literal, m_context);
    auto fact = this->get_fact(literal_view.get_atom().get_index());

    if (literal_view.get_polarity())
        return fact;

    fact.value = FDRValue { 0 };
    return fact;
}

View<IndexList<FDRVariable<FluentTag>>, Repository> GeneralFDRContext::get_variables() const { return make_view(m_variables, m_context); }

}
