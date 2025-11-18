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

#ifndef TYR_FORMALISM_REPOSITORY_HPP_
#define TYR_FORMALISM_REPOSITORY_HPP_

#include "tyr/cista/indexed_hash_set.hpp"
#include "tyr/formalism/atom.hpp"
#include "tyr/formalism/declarations.hpp"
#include "tyr/formalism/double.hpp"
#include "tyr/formalism/ground_atom.hpp"
#include "tyr/formalism/ground_literal.hpp"
#include "tyr/formalism/ground_rule.hpp"
#include "tyr/formalism/literal.hpp"
#include "tyr/formalism/program.hpp"
#include "tyr/formalism/relation.hpp"
#include "tyr/formalism/rule.hpp"
#include "tyr/formalism/signed.hpp"
#include "tyr/formalism/symbol.hpp"
#include "tyr/formalism/term.hpp"
#include "tyr/formalism/unsigned.hpp"
#include "tyr/formalism/variable.hpp"

#include <boost/hana.hpp>

namespace tyr::formalism
{
class Repository
{
private:
    template<typename T>
    using MappedType = boost::hana::pair<boost::hana::type<T>, cista::IndexedHashSet<T>>;

    template<typename T>
    using MappedListType = boost::hana::pair<boost::hana::type<T>, cista::IndexedHashSetList<T>>;

    using HanaRepository = boost::hana::map<MappedListType<AtomImpl<StaticTag>>,
                                            MappedListType<AtomImpl<StaticTag>>,
                                            MappedListType<LiteralImpl<StaticTag>>,
                                            MappedListType<LiteralImpl<StaticTag>>,
                                            MappedListType<GroundAtomImpl<StaticTag>>,
                                            MappedListType<GroundAtomImpl<StaticTag>>,
                                            MappedListType<GroundLiteralImpl<StaticTag>>,
                                            MappedListType<GroundLiteralImpl<StaticTag>>,
                                            MappedListType<RelationImpl<StaticTag>>,
                                            MappedListType<RelationImpl<FluentTag>>,
                                            MappedType<VariableImpl>,
                                            MappedType<SymbolImpl>,
                                            MappedType<RuleImpl>,
                                            MappedType<GroundRuleImpl>,
                                            MappedType<ProgramImpl>>;

    HanaRepository m_repository;
};
}

#endif
