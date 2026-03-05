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

#ifndef TYR_FORMALISM_FDR_CONTEXT_HPP_
#define TYR_FORMALISM_FDR_CONTEXT_HPP_

#include "tyr/buffer/declarations.hpp"
#include "tyr/common/equal_to.hpp"
#include "tyr/common/hash.hpp"
#include "tyr/common/types.hpp"
#include "tyr/formalism/planning/canonicalization.hpp"
#include "tyr/formalism/planning/declarations.hpp"
#include "tyr/formalism/planning/fdr_fact_view.hpp"
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

class BinaryFDRContext
{
public:
    explicit BinaryFDRContext(Repository& context) : m_context(context), m_variables(), m_mapping() {}

    Data<FDRFact<FluentTag>> get_fact(Index<GroundAtom<FluentTag>> atom)
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

    Data<FDRFact<FluentTag>> get_fact(View<Index<GroundAtom<FluentTag>>, Repository> atom) { return get_fact(atom.get_index()); }

    View<Data<FDRFact<FluentTag>>, Repository> get_fact_view(View<Index<GroundAtom<FluentTag>>, Repository> atom)
    {
        return make_view(get_fact(atom), m_context);
    }

    Data<FDRFact<FluentTag>> get_fact(Index<GroundLiteral<FluentTag>> literal) { return get_fact(make_view(literal, m_context)); }

    Data<FDRFact<FluentTag>> get_fact(View<Index<GroundLiteral<FluentTag>>, Repository> literal)
    {
        auto pos_fact = this->get_fact(literal.get_atom().get_index());

        if (literal.get_polarity())
            return pos_fact;

        return Data<FDRFact<FluentTag>>(make_view(pos_fact, m_context).get_variable().get_index(), FDRValue { 0 });
    }

    View<Data<FDRFact<FluentTag>>, Repository> get_fact_view(View<Index<GroundLiteral<FluentTag>>, Repository> literal)
    {
        return make_view(get_fact(literal), m_context);
    }

    View<IndexList<FDRVariable<FluentTag>>, Repository> get_variables() const { return make_view(m_variables, m_context); }

private:
    Repository& m_context;
    Data<FDRVariable<FluentTag>> m_builder;
    buffer::Buffer m_buffer;
    IndexList<FDRVariable<FluentTag>> m_variables;
    UnorderedMap<Index<GroundAtom<FluentTag>>, Data<FDRFact<FluentTag>>> m_mapping;
};

class GeneralFDRContext
{
public:
    // Create mapping based on mutexes.
    GeneralFDRContext(const std::vector<std::vector<View<Index<GroundAtom<FluentTag>>, Repository>>>& mutexes, Repository& context) :
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

    Data<FDRFact<FluentTag>> get_fact(Index<GroundAtom<FluentTag>> atom) const { return m_mapping.at(atom); }

    Data<FDRFact<FluentTag>> get_fact(View<Index<GroundAtom<FluentTag>>, Repository> atom) const { return get_fact(atom.get_index()); }

    View<Data<FDRFact<FluentTag>>, Repository> get_fact_view(View<Index<GroundAtom<FluentTag>>, Repository> atom)
    {
        return make_view(get_fact(atom), m_context);
    }

    Data<FDRFact<FluentTag>> get_fact(Index<GroundLiteral<FluentTag>> literal) const { return get_fact(make_view(literal, m_context)); }

    Data<FDRFact<FluentTag>> get_fact(View<Index<GroundLiteral<FluentTag>>, Repository> literal) const
    {
        auto fact = this->get_fact(literal.get_atom().get_index());

        if (literal.get_polarity())
            return fact;

        fact.value = FDRValue { 0 };
        return fact;
    }

    View<Data<FDRFact<FluentTag>>, Repository> get_fact_view(View<Index<GroundLiteral<FluentTag>>, Repository> literal)
    {
        return make_view(get_fact(literal), m_context);
    }

    View<IndexList<FDRVariable<FluentTag>>, Repository> get_variables() const { return make_view(m_variables, m_context); }

private:
    Repository& m_context;
    IndexList<FDRVariable<FluentTag>> m_variables;
    UnorderedMap<Index<GroundAtom<FluentTag>>, Data<FDRFact<FluentTag>>> m_mapping;
};

template<typename T>
concept FDRContext = requires(T& a, Index<GroundAtom<FluentTag>> atom, Index<GroundLiteral<FluentTag>> literal) {
    { a.get_fact(atom) };
    { a.get_fact(literal) };
};

}

#endif
