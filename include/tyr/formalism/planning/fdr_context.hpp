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
#include "tyr/formalism/planning/declarations.hpp"
#include "tyr/formalism/planning/fdr_fact_view.hpp"
#include "tyr/formalism/planning/fdr_value.hpp"
#include "tyr/formalism/planning/fdr_variable_data.hpp"
#include "tyr/formalism/planning/fdr_variable_index.hpp"
#include "tyr/formalism/planning/fdr_variable_view.hpp"
#include "tyr/formalism/planning/ground_atom_view.hpp"
#include "tyr/formalism/planning/ground_literal_view.hpp"

#include <concepts>
#include <vector>

namespace tyr::formalism::planning
{

class BinaryFDRContext
{
public:
    explicit BinaryFDRContext(Repository& context);

    Data<FDRFact<FluentTag>> get_fact(Index<GroundAtom<FluentTag>> atom);

    Data<FDRFact<FluentTag>> get_fact(Index<GroundLiteral<FluentTag>> literal);

    View<IndexList<FDRVariable<FluentTag>>, Repository> get_variables() const;

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
    GeneralFDRContext(const std::vector<std::vector<View<Index<GroundAtom<FluentTag>>, Repository>>>& mutexes, Repository& context);

    Data<FDRFact<FluentTag>> get_fact(Index<GroundAtom<FluentTag>> atom) const;

    Data<FDRFact<FluentTag>> get_fact(Index<GroundLiteral<FluentTag>> literal) const;

    View<IndexList<FDRVariable<FluentTag>>, Repository> get_variables() const;

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
