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
#include "tyr/formalism/canonicalization.hpp"
#include "tyr/formalism/declarations.hpp"
#include "tyr/formalism/ground_atom_view.hpp"
#include "tyr/formalism/ground_literal_view.hpp"
#include "tyr/formalism/planning/fdr_value.hpp"
#include "tyr/formalism/planning/fdr_variable_data.hpp"
#include "tyr/formalism/planning/fdr_variable_index.hpp"
#include "tyr/formalism/planning/fdr_variable_view.hpp"

#include <concepts>
#include <vector>

namespace tyr::formalism
{

template<typename Derived>
class FDRContextMixin
{
private:
    /// @brief Helper to cast to Derived.
    constexpr const auto& self() const { return static_cast<const Derived&>(*this); }
    constexpr auto& self() { return static_cast<Derived&>(*this); }

public:
    Data<FDRFact<FluentTag>> get_fact(Index<GroundAtom<FluentTag>> atom) { return self().get_fact_impl(atom); }

    Data<FDRFact<FluentTag>> get_fact(Index<GroundLiteral<FluentTag>> literal) { return self().get_fact_impl(literal); }
};

template<Context C>
class BinaryFDRContext : public FDRContextMixin<BinaryFDRContext<C>>
{
public:
    explicit BinaryFDRContext(C& context) : m_context(context), m_variables(), m_mapping() {}

    Data<FDRFact<FluentTag>> get_fact_impl(Index<GroundAtom<FluentTag>> atom)
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

    Data<FDRFact<FluentTag>> get_fact_impl(Index<GroundLiteral<FluentTag>> literal)
    {
        auto literal_view = make_view(literal, m_context);
        auto pos_fact = this->get_fact(literal_view.get_atom().get_index());

        if (literal_view.get_polarity())
            return pos_fact;

        return Data<FDRFact<FluentTag>>(make_view(pos_fact, m_context).get_variable().get_index(), FDRValue { 0 });
    }

    View<IndexList<FDRVariable<FluentTag>>, C> get_variables() const { return make_view(m_variables, m_context); }

private:
    C& m_context;
    Data<FDRVariable<FluentTag>> m_builder;
    buffer::Buffer m_buffer;
    IndexList<FDRVariable<FluentTag>> m_variables;
    UnorderedMap<Index<GroundAtom<FluentTag>>, Data<FDRFact<FluentTag>>> m_mapping;
};

template<Context C>
class GeneralFDRContext : public FDRContextMixin<GeneralFDRContext<C>>
{
public:
    // Create mapping based on mutexes.
    GeneralFDRContext(const std::vector<std::vector<View<Index<GroundAtom<FluentTag>>, C>>>& mutexes, C& context) :
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

    Data<FDRFact<FluentTag>> get_fact_impl(Index<GroundAtom<FluentTag>> atom) const { return m_mapping.at(atom); }

    Data<FDRFact<FluentTag>> get_fact_impl(Index<GroundLiteral<FluentTag>> literal) const
    {
        auto literal_view = make_view(literal, m_context);
        auto pos_fact = this->get_fact(literal_view.get_atom());

        if (literal_view.get_polarity())
            return pos_fact;

        const auto var_index = pos_fact.get_variable().get_index();
        return Data<FDRFact<FluentTag>>(var_index, FDRValue { 0 });
    }

    View<IndexList<FDRVariable<FluentTag>>, C> get_variables() const { return make_view(m_variables, m_context); }

private:
    C& m_context;
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
