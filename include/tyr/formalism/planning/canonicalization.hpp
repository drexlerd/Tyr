

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

#ifndef TYR_FORMALISM_PLANNING_CANONICALIZATION_HPP_
#define TYR_FORMALISM_PLANNING_CANONICALIZATION_HPP_

#include "tyr/common/canonicalization.hpp"
#include "tyr/common/comparators.hpp"
#include "tyr/formalism/canonicalization.hpp"
#include "tyr/formalism/planning/datas.hpp"

#include <algorithm>

namespace tyr::formalism::planning
{

/**
 * Datalog
 */

template<OpKind Op, typename T>
bool is_canonical(const Data<UnaryOperator<Op, T>>& data)
{
    return true;
}

template<OpKind Op, typename T>
bool is_canonical(const Data<BinaryOperator<Op, T>>& data)
{
    return true;
}

template<typename T>
bool is_canonical(const Data<BinaryOperator<OpAdd, T>>& data)
{
    return data.lhs <= data.rhs;
}

template<typename T>
bool is_canonical(const Data<BinaryOperator<OpMul, T>>& data)
{
    return data.lhs <= data.rhs;
}

template<OpKind Op, typename T>
bool is_canonical(const Data<MultiOperator<Op, T>>& data)
{
    return true;
}

template<typename T>
bool is_canonical(const Data<MultiOperator<OpAdd, T>>& data)
{
    return is_canonical(data.args);
}

template<typename T>
bool is_canonical(const Data<MultiOperator<OpMul, T>>& data)
{
    return is_canonical(data.args);
}

template<typename T>
bool is_canonical(const Data<BooleanOperator<T>>& data)
{
    return true;
}

template<typename T>
bool is_canonical(const Data<ArithmeticOperator<T>>& data)
{
    return true;
}

template<FactKind T>
bool is_canonical(const Data<Atom<T>>& data)
{
    return true;
}

template<FactKind T>
bool is_canonical(const Data<Literal<T>>& data)
{
    return true;
}

template<FactKind T>
bool is_canonical(const Data<GroundAtom<T>>& data)
{
    return true;
}

template<FactKind T>
bool is_canonical(const Data<GroundLiteral<T>>& data)
{
    return true;
}

template<FactKind T>
bool is_canonical(const Data<FunctionTerm<T>>& data)
{
    return true;
}

inline bool is_canonical(const Data<FunctionExpression>& data) { return true; }

template<FactKind T>
bool is_canonical(const Data<GroundFunctionTerm<T>>& data)
{
    return true;
}

inline bool is_canonical(const Data<GroundFunctionExpression>& data) { return true; }

template<FactKind T>
bool is_canonical(const Data<GroundFunctionTermValue<T>>& data)
{
    return true;
}

template<NumericEffectOpKind Op, FactKind T>
bool is_canonical(const Data<NumericEffect<Op, T>>& data)
{
    return true;
}

template<NumericEffectOpKind Op, FactKind T>
bool is_canonical(const Data<GroundNumericEffect<Op, T>>& data)
{
    return true;
}

inline bool is_canonical(const Data<ConditionalEffect>& data) { return true; }

inline bool is_canonical(const Data<GroundConditionalEffect>& data) { return true; }

inline bool is_canonical(const Data<ConjunctiveEffect>& data) { return is_canonical(data.literals) && is_canonical(data.numeric_effects); }

inline bool is_canonical(const Data<GroundConjunctiveEffect>& data) { return is_canonical(data.facts) && is_canonical(data.numeric_effects); }

inline bool is_canonical(const Data<Action>& data) { return true; }

inline bool is_canonical(const Data<GroundAction>& data) { return true; }

inline bool is_canonical(const Data<Axiom>& data) { return true; }

inline bool is_canonical(const Data<GroundAxiom>& data) { return true; }

inline bool is_canonical(const Data<Metric>& data) { return true; }

inline bool is_canonical(const Data<Task>& data)
{
    return is_canonical(data.derived_predicates) && is_canonical(data.objects) && is_canonical(data.static_atoms) && is_canonical(data.fluent_atoms)
           && is_canonical(data.static_fterm_values) && is_canonical(data.fluent_fterm_values) && is_canonical(data.axioms);
}

inline bool is_canonical(const Data<Domain>& data)
{
    return is_canonical(data.static_predicates) && is_canonical(data.fluent_predicates) && is_canonical(data.derived_predicates)
           && is_canonical(data.static_functions) && is_canonical(data.fluent_functions) && is_canonical(data.constants) && is_canonical(data.actions)
           && is_canonical(data.axioms);
}

template<FactKind T>
bool is_canonical(const Data<FDRVariable<T>>& data)
{
    return true;
}

template<FactKind T>
bool is_canonical(const Data<FDRFact<T>>& data)
{
    return true;
}

inline bool is_canonical(const Data<FDRConjunctiveCondition>& data)
{
    return is_canonical(data.static_literals) && is_canonical(data.fluent_literals) && is_canonical(data.derived_literals)
           && is_canonical(data.numeric_constraints);
}

inline bool is_canonical(const Data<GroundFDRConjunctiveCondition>& data)
{
    return is_canonical(data.static_literals) && is_canonical(data.fluent_facts) && is_canonical(data.derived_literals)
           && is_canonical(data.numeric_constraints);
}

inline bool is_canonical(const Data<FDRTask>& data)
{
    return is_canonical(data.derived_predicates) && is_canonical(data.objects) && is_canonical(data.static_atoms) && is_canonical(data.fluent_atoms)
           && is_canonical(data.derived_atoms) && is_canonical(data.static_fterm_values) && is_canonical(data.fluent_fterm_values) && is_canonical(data.axioms)
           && is_canonical(data.fluent_variables) && is_canonical(data.fluent_facts) && is_canonical(data.ground_actions) && is_canonical(data.ground_axioms);
}

/**
 * Datalog
 */

template<OpKind Op, typename T>
void canonicalize(Data<UnaryOperator<Op, T>>& data)
{
    // Trivially canonical
}

template<OpKind Op, typename T>
void canonicalize(Data<BinaryOperator<Op, T>>& data)
{
    // Canonicalization for commutative operator in spezializations
}

template<typename T>
void canonicalize(Data<BinaryOperator<OpAdd, T>>& data)
{
    if (data.lhs > data.rhs)
        std::swap(data.lhs, data.rhs);
}

template<typename T>
void canonicalize(Data<BinaryOperator<OpMul, T>>& data)
{
    if (data.lhs > data.rhs)
        std::swap(data.lhs, data.rhs);
}

template<OpKind Op, typename T>
void canonicalize(Data<MultiOperator<Op, T>>& data)
{
    // Canonicalization for commutative operator in spezializations
}

template<typename T>
void canonicalize(Data<MultiOperator<OpAdd, T>>& data)
{
    canonicalize(data.args);
}

template<typename T>
void canonicalize(Data<MultiOperator<OpMul, T>>& data)
{
    canonicalize(data.args);
}

template<typename T>
void canonicalize(Data<BooleanOperator<T>>& data)
{
    // Trivially canonical
}

template<typename T>
void canonicalize(Data<ArithmeticOperator<T>>& data)
{
    // Trivially canonical
}

template<FactKind T>
void canonicalize(Data<Atom<T>>& data)
{
    // Trivially canonical
}

template<FactKind T>
void canonicalize(Data<Literal<T>>& data)
{
    // Trivially canonical
}

template<FactKind T>
void canonicalize(Data<GroundAtom<T>>& data)
{
    // Trivially canonical
}

template<FactKind T>
void canonicalize(Data<GroundLiteral<T>>& data)
{
    // Trivially canonical
}

template<FactKind T>
void canonicalize(Data<FunctionTerm<T>>& data)
{
    // Trivially canonical
}

inline void canonicalize(Data<FunctionExpression>& data)
{
    // Trivially canonical
}

template<FactKind T>
void canonicalize(Data<GroundFunctionTerm<T>>& data)
{
    // Trivially canonical
}

inline void canonicalize(Data<GroundFunctionExpression>& data)
{
    // Trivially canonical
}

template<FactKind T>
void canonicalize(Data<GroundFunctionTermValue<T>>& data)
{
    // Trivially canonical
}

template<NumericEffectOpKind Op, FactKind T>
void canonicalize(Data<NumericEffect<Op, T>>& data)
{
}

template<NumericEffectOpKind Op, FactKind T>
void canonicalize(Data<GroundNumericEffect<Op, T>>& data)
{
}

/**
 * Planning
 */

inline void canonicalize(Data<ConditionalEffect>& data) {}

inline void canonicalize(Data<GroundConditionalEffect>& data) {}

inline void canonicalize(Data<ConjunctiveEffect>& data)
{
    canonicalize(data.literals);
    canonicalize(data.numeric_effects);
}

inline void canonicalize(Data<GroundConjunctiveEffect>& data)
{
    canonicalize(data.facts);
    canonicalize(data.numeric_effects);
}

inline void canonicalize(Data<Action>& data) { canonicalize(data.effects); }

inline void canonicalize(Data<GroundAction>& data) { canonicalize(data.effects); }

inline void canonicalize(Data<Axiom>& data) {}

inline void canonicalize(Data<GroundAxiom>& data) {}

inline void canonicalize(Data<Metric>& data) {}

inline void canonicalize(Data<Task>& data)
{
    canonicalize(data.derived_predicates);
    canonicalize(data.objects);
    canonicalize(data.static_atoms);
    canonicalize(data.fluent_atoms);
    canonicalize(data.static_fterm_values);
    canonicalize(data.fluent_fterm_values);
    canonicalize(data.axioms);
}

inline void canonicalize(Data<Domain>& data)
{
    canonicalize(data.static_predicates);
    canonicalize(data.fluent_predicates);
    canonicalize(data.derived_predicates);
    canonicalize(data.static_functions);
    canonicalize(data.fluent_functions);
    canonicalize(data.constants);
    canonicalize(data.actions);
    canonicalize(data.axioms);
}

template<FactKind T>
void canonicalize(Data<FDRVariable<T>>& data)
{
    // Trivially canonical
}

template<FactKind T>
void canonicalize(Data<FDRFact<T>>& data)
{
    // Trivially canonical
}

inline void canonicalize(Data<FDRConjunctiveCondition>& data)
{
    canonicalize(data.static_literals);
    canonicalize(data.fluent_literals);
    canonicalize(data.derived_literals);
    canonicalize(data.numeric_constraints);
}

inline void canonicalize(Data<GroundFDRConjunctiveCondition>& data)
{
    canonicalize(data.static_literals);
    canonicalize(data.fluent_facts);
    canonicalize(data.derived_literals);
    canonicalize(data.numeric_constraints);
}

inline void canonicalize(Data<FDRTask>& data)
{
    canonicalize(data.derived_predicates);
    canonicalize(data.objects);
    canonicalize(data.static_atoms);
    canonicalize(data.fluent_atoms);
    canonicalize(data.derived_atoms);
    canonicalize(data.static_fterm_values);
    canonicalize(data.fluent_fterm_values);
    canonicalize(data.axioms);
    canonicalize(data.fluent_variables);
    canonicalize(data.fluent_facts);
    canonicalize(data.ground_actions);
    canonicalize(data.ground_axioms);
}

}

#endif
