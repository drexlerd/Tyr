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

#ifndef TYR_FORMALISM_PLANNING_FORMATTER_HPP_
#define TYR_FORMALISM_PLANNING_FORMATTER_HPP_

#include "tyr/common/formatter.hpp"
#include "tyr/common/iostream.hpp"
#include "tyr/formalism/formatter.hpp"
#include "tyr/formalism/planning/declarations.hpp"
#include "tyr/formalism/planning/planning_domain.hpp"
#include "tyr/formalism/planning/planning_fdr_task.hpp"
#include "tyr/formalism/planning/planning_task.hpp"
#include "tyr/formalism/planning/repository.hpp"

#include <fmt/core.h>
#include <fmt/ostream.h>
#include <fmt/ranges.h>
#include <ostream>

namespace tyr
{
/**
 * Forward declarations
 */
namespace formalism::planning
{
template<OpKind Op, typename T>
std::ostream& operator<<(std::ostream& os, const Data<UnaryOperator<Op, T>>& el);

template<OpKind Op, typename T>
std::ostream& operator<<(std::ostream& os, const UnaryOperatorView<Op, T>& el);

template<OpKind Op, typename T>
std::ostream& operator<<(std::ostream& os, const Data<BinaryOperator<Op, T>>& el);

template<OpKind Op, typename T>
std::ostream& operator<<(std::ostream& os, const BinaryOperatorView<Op, T>& el);

template<OpKind Op, typename T>
std::ostream& operator<<(std::ostream& os, const Data<MultiOperator<Op, T>>& el);

template<OpKind Op, typename T>
std::ostream& operator<<(std::ostream& os, const MultiOperatorView<Op, T>& el);

template<typename T>
std::ostream& operator<<(std::ostream& os, const Data<ArithmeticOperator<T>>& el);

template<typename T>
std::ostream& operator<<(std::ostream& os, const ArithmeticOperatorView<T>& el);

template<typename T>
std::ostream& operator<<(std::ostream& os, const Data<BooleanOperator<T>>& el);

template<typename T>
std::ostream& operator<<(std::ostream& os, const BooleanOperatorView<T>& el);

template<FactKind T>
std::ostream& operator<<(std::ostream& os, const Data<Atom<T>>& el);

template<FactKind T>
std::ostream& operator<<(std::ostream& os, const AtomView<T>& el);

template<FactKind T>
std::ostream& operator<<(std::ostream& os, const Data<Literal<T>>& el);

template<FactKind T>
std::ostream& operator<<(std::ostream& os, const LiteralView<T>& el);

template<FactKind T>
std::ostream& operator<<(std::ostream& os, const Data<GroundAtom<T>>& el);

template<FactKind T>
std::ostream& operator<<(std::ostream& os, const GroundAtomView<T>& el);

template<FactKind T>
std::ostream& operator<<(std::ostream& os, const Data<GroundLiteral<T>>& el);

template<FactKind T>
std::ostream& operator<<(std::ostream& os, const GroundLiteralView<T>& el);

template<FactKind T>
std::ostream& operator<<(std::ostream& os, const Data<FunctionTerm<T>>& el);

template<FactKind T>
std::ostream& operator<<(std::ostream& os, const FunctionTermView<T>& el);

template<FactKind T>
std::ostream& operator<<(std::ostream& os, const Data<GroundFunctionTerm<T>>& el);

template<FactKind T>
std::ostream& operator<<(std::ostream& os, const GroundFunctionTermView<T>& el);

template<FactKind T>
std::ostream& operator<<(std::ostream& os, const Data<GroundFunctionTermValue<T>>& el);

template<FactKind T>
std::ostream& operator<<(std::ostream& os, const GroundFunctionTermValueView<T>& el);

std::ostream& operator<<(std::ostream& os, const Data<FunctionExpression>& el);

std::ostream& operator<<(std::ostream& os, const FunctionExpressionView& el);

std::ostream& operator<<(std::ostream& os, const Data<GroundFunctionExpression>& el);

std::ostream& operator<<(std::ostream& os, const GroundFunctionExpressionView& el);

std::ostream& operator<<(std::ostream& os, const OpAssign& el);

std::ostream& operator<<(std::ostream& os, const OpIncrease& el);

std::ostream& operator<<(std::ostream& os, const OpDecrease& el);

std::ostream& operator<<(std::ostream& os, const OpScaleUp& el);

std::ostream& operator<<(std::ostream& os, const OpScaleDown& el);

std::ostream& operator<<(std::ostream& os, const Minimize& el);

std::ostream& operator<<(std::ostream& os, const Maximize& el);

template<NumericEffectOpKind Op, FactKind T>
std::ostream& operator<<(std::ostream& os, const Data<NumericEffect<Op, T>>& el);

template<NumericEffectOpKind Op, FactKind T>
std::ostream& operator<<(std::ostream& os, const NumericEffectView<Op, T>& el);

template<NumericEffectOpKind Op, FactKind T>
std::ostream& operator<<(std::ostream& os, const Data<GroundNumericEffect<Op, T>>& el);

template<NumericEffectOpKind Op, FactKind T>
std::ostream& operator<<(std::ostream& os, const GroundNumericEffectView<Op, T>& el);

template<FactKind T>
std::ostream& operator<<(std::ostream& os, const Data<NumericEffectOperator<T>>& el);

template<FactKind T>
std::ostream& operator<<(std::ostream& os, const NumericEffectOperatorView<T>& el);

template<FactKind T>
std::ostream& operator<<(std::ostream& os, const Data<GroundNumericEffectOperator<T>>& el);

template<FactKind T>
std::ostream& operator<<(std::ostream& os, const GroundNumericEffectOperatorView<T>& el);

std::ostream& operator<<(std::ostream& os, const Data<ConjunctiveCondition>& el);

std::ostream& operator<<(std::ostream& os, const ConjunctiveConditionView& el);

std::ostream& operator<<(std::ostream& os, const Data<GroundConjunctiveCondition>& el);

std::ostream& operator<<(std::ostream& os, const GroundConjunctiveConditionView& el);

std::ostream& operator<<(std::ostream& os, const Data<ConditionalEffect>& el);

std::ostream& operator<<(std::ostream& os, const ConditionalEffectView& el);

std::ostream& operator<<(std::ostream& os, const Data<GroundConditionalEffect>& el);

std::ostream& operator<<(std::ostream& os, const GroundConditionalEffectView& el);

std::ostream& operator<<(std::ostream& os, const Data<ConjunctiveEffect>& el);

std::ostream& operator<<(std::ostream& os, const ConjunctiveEffectView& el);

std::ostream& operator<<(std::ostream& os, const Data<GroundConjunctiveEffect>& el);

std::ostream& operator<<(std::ostream& os, const GroundConjunctiveEffectView& el);

std::ostream& operator<<(std::ostream& os, const Data<Action>& el);

std::ostream& operator<<(std::ostream& os, const ActionView& el);

std::ostream& operator<<(std::ostream& os, const Data<GroundAction>& el);

std::ostream& operator<<(std::ostream& os, const GroundActionView& el);

std::ostream& operator<<(std::ostream& os, const ActionBindingView& el);

std::ostream& operator<<(std::ostream& os, const AxiomBindingView& el);

struct PlanFormatting
{
};

std::ostream& operator<<(std::ostream& os, const std::pair<GroundActionView, PlanFormatting>& el);

std::ostream& operator<<(std::ostream& os, const Data<Axiom>& el);

std::ostream& operator<<(std::ostream& os, const AxiomView& el);

std::ostream& operator<<(std::ostream& os, const Data<GroundAxiom>& el);

std::ostream& operator<<(std::ostream& os, const GroundAxiomView& el);

std::ostream& operator<<(std::ostream& os, const Data<Metric>& el);

std::ostream& operator<<(std::ostream& os, const MetricView& el);

std::ostream& operator<<(std::ostream& os, const Data<Task>& el);

std::ostream& operator<<(std::ostream& os, const TaskView& el);

std::ostream& operator<<(std::ostream& os, const Data<Domain>& el);

std::ostream& operator<<(std::ostream& os, const DomainView& el);

template<FactKind T>
std::ostream& operator<<(std::ostream& os, const Data<FDRVariable<T>>& el);

template<FactKind T>
std::ostream& operator<<(std::ostream& os, const FDRVariableView<T>& el);

std::ostream& operator<<(std::ostream& os, const FDRValue& el);

template<FactKind T>
std::ostream& operator<<(std::ostream& os, const Data<FDRFact<T>>& el);

template<FactKind T>
std::ostream& operator<<(std::ostream& os, const FDRFactView<T>& el);

std::ostream& operator<<(std::ostream& os, const Data<GroundConjunctiveCondition>& el);

std::ostream& operator<<(std::ostream& os, const GroundConjunctiveConditionView& el);

std::ostream& operator<<(std::ostream& os, const Data<FDRTask>& el);

std::ostream& operator<<(std::ostream& os, const FDRTaskView& el);
}  // end namespace formalism

/**
 * Definitions
 */

template<formalism::OpKind Op, typename T>
std::ostream& print(std::ostream& os, const Data<formalism::planning::UnaryOperator<Op, T>>& el)
{
    fmt::print(os, "({} {})", to_string(Op {}), to_string(el.arg));
    return os;
}

template<formalism::OpKind Op, typename T>
std::ostream& print(std::ostream& os, const formalism::planning::UnaryOperatorView<Op, T>& el)
{
    fmt::print(os, "({} {})", to_string(Op {}), to_string(el.get_arg()));
    return os;
}

template<formalism::OpKind Op, typename T>
std::ostream& print(std::ostream& os, const Data<formalism::planning::BinaryOperator<Op, T>>& el)
{
    fmt::print(os, "({} {} {})", to_string(Op {}), to_string(el.lhs), to_string(el.rhs));
    return os;
}

template<formalism::OpKind Op, typename T>
std::ostream& print(std::ostream& os, const formalism::planning::BinaryOperatorView<Op, T>& el)
{
    fmt::print(os, "({} {} {})", to_string(Op {}), to_string(el.get_lhs()), to_string(el.get_rhs()));
    return os;
}

template<formalism::OpKind Op, typename T>
std::ostream& print(std::ostream& os, const Data<formalism::planning::MultiOperator<Op, T>>& el)
{
    fmt::print(os, "({} {})", to_string(Op {}), fmt::format("{}", fmt::join(to_strings(el.args), " ")));
    return os;
}

template<formalism::OpKind Op, typename T>
std::ostream& print(std::ostream& os, const formalism::planning::MultiOperatorView<Op, T>& el)
{
    fmt::print(os, "({} {})", to_string(Op {}), fmt::format("{}", fmt::join(to_strings(el.get_args()), " ")));
    return os;
}

template<typename T>
std::ostream& print(std::ostream& os, const Data<formalism::planning::ArithmeticOperator<T>>& el)
{
    fmt::print(os, "{}", to_string(el.value));
    return os;
}

template<typename T>
std::ostream& print(std::ostream& os, const formalism::planning::ArithmeticOperatorView<T>& el)
{
    fmt::print(os, "{}", to_string(el.get_variant()));
    return os;
}

template<typename T>
std::ostream& print(std::ostream& os, const Data<formalism::planning::BooleanOperator<T>>& el)
{
    fmt::print(os, "{}", to_string(el.value));
    return os;
}

template<typename T>
std::ostream& print(std::ostream& os, const formalism::planning::BooleanOperatorView<T>& el)
{
    fmt::print(os, "{}", to_string(el.get_variant()));
    return os;
}

template<formalism::FactKind T>
std::ostream& print(std::ostream& os, const Data<formalism::planning::Atom<T>>& el)
{
    fmt::print(os, "({} {})", to_string(el.predicate), fmt::format("{}", fmt::join(to_strings(el.terms), " ")));
    return os;
}

template<formalism::FactKind T>
std::ostream& print(std::ostream& os, const formalism::planning::AtomView<T>& el)
{
    fmt::print(os, "({} {})", to_string(el.get_predicate().get_name()), fmt::format("{}", fmt::join(to_strings(el.get_terms()), " ")));
    return os;
}

template<formalism::FactKind T>
std::ostream& print(std::ostream& os, const Data<formalism::planning::Literal<T>>& el)
{
    if (el.polarity)
        print(os, to_string(el.atom));
    else
        fmt::print(os, "(not {})", to_string(el.atom));
    return os;
}

template<formalism::FactKind T>
std::ostream& print(std::ostream& os, const formalism::planning::LiteralView<T>& el)
{
    if (el.get_polarity())
        print(os, to_string(el.get_atom()));
    else
        fmt::print(os, "(not {})", to_string(el.get_atom()));
    return os;
}

template<formalism::FactKind T>
std::ostream& print(std::ostream& os, const Data<formalism::planning::GroundAtom<T>>& el)
{
    fmt::print(os, "({})", to_string(el.binding));
    return os;
}

template<formalism::FactKind T>
std::ostream& print(std::ostream& os, const formalism::planning::GroundAtomView<T>& el)
{
    fmt::print(os, "({} {})", to_string(el.get_predicate().get_name()), fmt::format("{}", fmt::join(to_strings(el.get_row().get_objects()), " ")));
    return os;
}

template<formalism::FactKind T>
std::ostream& print(std::ostream& os, const Data<formalism::planning::GroundLiteral<T>>& el)
{
    if (el.polarity)
        print(os, to_string(el.atom));
    else
        fmt::print(os, "(not {})", to_string(el.atom));
    return os;
}

template<formalism::FactKind T>
std::ostream& print(std::ostream& os, const formalism::planning::GroundLiteralView<T>& el)
{
    if (el.get_polarity())
        print(os, to_string(el.get_atom()));
    else
        fmt::print(os, "(not {})", to_string(el.get_atom()));
    return os;
}

template<formalism::FactKind T>
std::ostream& print(std::ostream& os, const Data<formalism::planning::FunctionTerm<T>>& el)
{
    fmt::print(os, "({} {})", to_string(el.function), fmt::format("{}", fmt::join(to_strings(el.terms), " ")));
    return os;
}

template<formalism::FactKind T>
std::ostream& print(std::ostream& os, const formalism::planning::FunctionTermView<T>& el)
{
    fmt::print(os, "({} {})", to_string(el.get_function().get_name()), fmt::format("{}", fmt::join(to_strings(el.get_terms()), " ")));
    return os;
}

template<formalism::FactKind T>
std::ostream& print(std::ostream& os, const Data<formalism::planning::GroundFunctionTerm<T>>& el)
{
    fmt::print(os, "({})", to_string(el.binding));
    return os;
}

template<formalism::FactKind T>
std::ostream& print(std::ostream& os, const formalism::planning::GroundFunctionTermView<T>& el)
{
    fmt::print(os, "({} {})", to_string(el.get_function().get_name()), fmt::format("{}", fmt::join(to_strings(el.get_row().get_objects()), " ")));
    return os;
}

template<formalism::FactKind T>
std::ostream& print(std::ostream& os, const Data<formalism::planning::GroundFunctionTermValue<T>>& el)
{
    fmt::print(os, "(= {} {})", to_string(el.fterm), to_string(el.value));
    return os;
}

template<formalism::FactKind T>
std::ostream& print(std::ostream& os, const formalism::planning::GroundFunctionTermValueView<T>& el)
{
    fmt::print(os, "(= {} {})", to_string(el.get_fterm()), to_string(el.get_value()));
    return os;
}

std::ostream& print(std::ostream& os, const Data<formalism::planning::FunctionExpression>& el);

std::ostream& print(std::ostream& os, const formalism::planning::FunctionExpressionView& el);

std::ostream& print(std::ostream& os, const Data<formalism::planning::GroundFunctionExpression>& el);

std::ostream& print(std::ostream& os, const formalism::planning::GroundFunctionExpressionView& el);

std::ostream& print(std::ostream& os, const formalism::planning::OpAssign& el);

std::ostream& print(std::ostream& os, const formalism::planning::OpIncrease& el);

std::ostream& print(std::ostream& os, const formalism::planning::OpDecrease& el);

std::ostream& print(std::ostream& os, const formalism::planning::OpScaleUp& el);

std::ostream& print(std::ostream& os, const formalism::planning::OpScaleDown& el);

std::ostream& print(std::ostream& os, const formalism::planning::Minimize& el);

std::ostream& print(std::ostream& os, const formalism::planning::Maximize& el);

template<formalism::planning::NumericEffectOpKind Op, formalism::FactKind T>
std::ostream& print(std::ostream& os, const Data<formalism::planning::NumericEffect<Op, T>>& el)
{
    fmt::print(os, "({} {} {})", to_string(Op {}), to_string(el.fterm), to_string(el.fexpr));
    return os;
}

template<formalism::planning::NumericEffectOpKind Op, formalism::FactKind T>
std::ostream& print(std::ostream& os, const formalism::planning::NumericEffectView<Op, T>& el)
{
    fmt::print(os, "({} {} {})", to_string(Op {}), to_string(el.get_fterm()), to_string(el.get_fexpr()));
    return os;
}

template<formalism::planning::NumericEffectOpKind Op, formalism::FactKind T>
std::ostream& print(std::ostream& os, const Data<formalism::planning::GroundNumericEffect<Op, T>>& el)
{
    fmt::print(os, "({} {} {})", to_string(Op {}), to_string(el.fterm), to_string(el.fexpr));
    return os;
}

template<formalism::planning::NumericEffectOpKind Op, formalism::FactKind T>
std::ostream& print(std::ostream& os, const formalism::planning::GroundNumericEffectView<Op, T>& el)
{
    fmt::print(os, "({} {} {})", to_string(Op {}), to_string(el.get_fterm()), to_string(el.get_fexpr()));
    return os;
}

template<formalism::FactKind T>
std::ostream& print(std::ostream& os, const Data<formalism::planning::NumericEffectOperator<T>>& el)
{
    return tyr::print(os, el.value);
}

template<formalism::FactKind T>
std::ostream& print(std::ostream& os, const formalism::planning::NumericEffectOperatorView<T>& el)
{
    return tyr::print(os, el.get_variant());
}

template<formalism::FactKind T>
std::ostream& print(std::ostream& os, const Data<formalism::planning::GroundNumericEffectOperator<T>>& el)
{
    return tyr::print(os, el.value);
}

template<formalism::FactKind T>
std::ostream& print(std::ostream& os, const formalism::planning::GroundNumericEffectOperatorView<T>& el)
{
    return tyr::print(os, el.get_variant());
}

std::ostream& print(std::ostream& os, const Data<formalism::planning::ConditionalEffect>& el);

std::ostream& print(std::ostream& os, const formalism::planning::ConditionalEffectView& el);

std::ostream& print(std::ostream& os, const Data<formalism::planning::GroundConditionalEffect>& el);

std::ostream& print(std::ostream& os, const formalism::planning::GroundConditionalEffectView& el);

std::ostream& print(std::ostream& os, const Data<formalism::planning::ConjunctiveEffect>& el);

std::ostream& print(std::ostream& os, const formalism::planning::ConjunctiveEffectView& el);

std::ostream& print(std::ostream& os, const Data<formalism::planning::GroundConjunctiveEffect>& el);

std::ostream& print(std::ostream& os, const formalism::planning::GroundConjunctiveEffectView& el);

std::ostream& print(std::ostream& os, const Data<formalism::planning::Action>& el);

std::ostream& print(std::ostream& os, const formalism::planning::ActionView& el);

std::ostream& print(std::ostream& os, const Data<formalism::planning::GroundAction>& el);

std::ostream& print(std::ostream& os, const formalism::planning::GroundActionView& el);

std::ostream& print(std::ostream& os, const std::pair<formalism::planning::GroundActionView, formalism::planning::PlanFormatting>& el);

std::ostream& print(std::ostream& os, const Data<formalism::planning::Axiom>& el);

std::ostream& print(std::ostream& os, const formalism::planning::AxiomView& el);

std::ostream& print(std::ostream& os, const Data<formalism::planning::GroundAxiom>& el);

std::ostream& print(std::ostream& os, const formalism::planning::GroundAxiomView& el);

std::ostream& print(std::ostream& os, const formalism::planning::ActionBindingView& el);

std::ostream& print(std::ostream& os, const formalism::planning::AxiomBindingView& el);

std::ostream& print(std::ostream& os, const Data<formalism::planning::Metric>& el);

std::ostream& print(std::ostream& os, const formalism::planning::MetricView& el);

std::ostream& print(std::ostream& os, const Data<formalism::planning::Task>& el);

std::ostream& print(std::ostream& os, const formalism::planning::TaskView& el);

std::ostream& print(std::ostream& os, const Data<formalism::planning::Domain>& el);

std::ostream& print(std::ostream& os, const formalism::planning::DomainView& el);

template<formalism::FactKind T>
std::ostream& print(std::ostream& os, const Data<formalism::planning::FDRVariable<T>>& el)
{
    os << "FDRVariable(\n";
    {
        IndentScope scope(os);

        os << print_indent << "index = " << el.index << "\n";

        os << print_indent << "atoms = " << el.atoms << "\n";
    }
    os << print_indent << ")";

    return os;
}

template<formalism::FactKind T>
std::ostream& print(std::ostream& os, const formalism::planning::FDRVariableView<T>& el)
{
    os << "FDRVariable(\n";
    {
        IndentScope scope(os);

        os << print_indent << "index = " << el.get_index() << "\n";

        os << print_indent << "atoms = " << el.get_atoms() << "\n";
    }
    os << print_indent << ")";

    return os;
}

std::ostream& print(std::ostream& os, const formalism::planning::FDRValue& el);

template<formalism::FactKind T>
std::ostream& print(std::ostream& os, const Data<formalism::planning::FDRFact<T>>& el)
{
    fmt::print(os, "<{},{}>", to_string(el.variable), to_string(el.value));
    return os;
}

template<formalism::FactKind T>
std::ostream& print(std::ostream& os, const formalism::planning::FDRFactView<T>& el)
{
    if (el.get_value() == formalism::planning::FDRValue::none())
        fmt::print(os,
                   "<{},{}>: (none-of {})",
                   to_string(el.get_variable().get_index()),
                   to_string(el.get_value()),
                   fmt::join(to_strings(el.get_variable().get_atoms()), " "));
    else
        fmt::print(os,
                   "<{},{}>: {}",
                   to_string(el.get_variable().get_index()),
                   to_string(el.get_value()),
                   to_string(el.get_variable().get_atoms()[uint_t(el.get_value()) - 1]));
    return os;
}

std::ostream& print(std::ostream& os, const Data<formalism::planning::ConjunctiveCondition>& el);

std::ostream& print(std::ostream& os, const formalism::planning::ConjunctiveConditionView& el);

std::ostream& print(std::ostream& os, const Data<formalism::planning::GroundConjunctiveCondition>& el);

std::ostream& print(std::ostream& os, const formalism::planning::GroundConjunctiveConditionView& el);

std::ostream& print(std::ostream& os, const Data<formalism::planning::FDRTask>& el);

std::ostream& print(std::ostream& os, const formalism::planning::FDRTaskView& el);

std::ostream& print(std::ostream& os, const formalism::planning::PlanningDomain& el);

std::ostream& print(std::ostream& os, const formalism::planning::PlanningTask& el);

std::ostream& print(std::ostream& os, const formalism::planning::PlanningFDRTask& el);

namespace formalism::planning
{
template<OpKind Op, typename T>
std::ostream& operator<<(std::ostream& os, const Data<UnaryOperator<Op, T>>& el)
{
    return tyr::print(os, el);
}

template<OpKind Op, typename T>
std::ostream& operator<<(std::ostream& os, const UnaryOperatorView<Op, T>& el)
{
    return tyr::print(os, el);
}

template<OpKind Op, typename T>
std::ostream& operator<<(std::ostream& os, const Data<BinaryOperator<Op, T>>& el)
{
    return tyr::print(os, el);
}

template<OpKind Op, typename T>
std::ostream& operator<<(std::ostream& os, const BinaryOperatorView<Op, T>& el)
{
    return tyr::print(os, el);
}

template<OpKind Op, typename T>
std::ostream& operator<<(std::ostream& os, const Data<MultiOperator<Op, T>>& el)
{
    return tyr::print(os, el);
}

template<OpKind Op, typename T>
std::ostream& operator<<(std::ostream& os, const MultiOperatorView<Op, T>& el)
{
    return tyr::print(os, el);
}

template<typename T>
std::ostream& operator<<(std::ostream& os, const Data<ArithmeticOperator<T>>& el)
{
    return tyr::print(os, el);
}

template<typename T>
std::ostream& operator<<(std::ostream& os, const ArithmeticOperatorView<T>& el)
{
    return tyr::print(os, el);
}

template<typename T>
std::ostream& operator<<(std::ostream& os, const Data<BooleanOperator<T>>& el)
{
    return tyr::print(os, el);
}

template<typename T>
std::ostream& operator<<(std::ostream& os, const BooleanOperatorView<T>& el)
{
    return tyr::print(os, el);
}

template<FactKind T>
std::ostream& operator<<(std::ostream& os, const Data<Atom<T>>& el)
{
    return tyr::print(os, el);
}

template<FactKind T>
std::ostream& operator<<(std::ostream& os, const AtomView<T>& el)
{
    return tyr::print(os, el);
}

template<FactKind T>
std::ostream& operator<<(std::ostream& os, const Data<Literal<T>>& el)
{
    return tyr::print(os, el);
}

template<FactKind T>
std::ostream& operator<<(std::ostream& os, const LiteralView<T>& el)
{
    return tyr::print(os, el);
}

template<FactKind T>
std::ostream& operator<<(std::ostream& os, const Data<GroundAtom<T>>& el)
{
    return tyr::print(os, el);
}

template<FactKind T>
std::ostream& operator<<(std::ostream& os, const GroundAtomView<T>& el)
{
    return tyr::print(os, el);
}

template<FactKind T>
std::ostream& operator<<(std::ostream& os, const Data<GroundLiteral<T>>& el)
{
    return tyr::print(os, el);
}

template<FactKind T>
std::ostream& operator<<(std::ostream& os, const GroundLiteralView<T>& el)
{
    return tyr::print(os, el);
}

template<FactKind T>
std::ostream& operator<<(std::ostream& os, const Data<FunctionTerm<T>>& el)
{
    return tyr::print(os, el);
}

template<FactKind T>
std::ostream& operator<<(std::ostream& os, const FunctionTermView<T>& el)
{
    return tyr::print(os, el);
}

template<FactKind T>
std::ostream& operator<<(std::ostream& os, const Data<GroundFunctionTerm<T>>& el)
{
    return tyr::print(os, el);
}

template<FactKind T>
std::ostream& operator<<(std::ostream& os, const GroundFunctionTermView<T>& el)
{
    return tyr::print(os, el);
}

template<FactKind T>
std::ostream& operator<<(std::ostream& os, const Data<GroundFunctionTermValue<T>>& el)
{
    return tyr::print(os, el);
}

template<FactKind T>
std::ostream& operator<<(std::ostream& os, const GroundFunctionTermValueView<T>& el)
{
    return tyr::print(os, el);
}

std::ostream& operator<<(std::ostream& os, const Data<FunctionExpression>& el);

std::ostream& operator<<(std::ostream& os, const FunctionExpressionView& el);

std::ostream& operator<<(std::ostream& os, const Data<GroundFunctionExpression>& el);

std::ostream& operator<<(std::ostream& os, const GroundFunctionExpressionView& el);

std::ostream& operator<<(std::ostream& os, const OpAssign& el);

std::ostream& operator<<(std::ostream& os, const OpIncrease& el);

std::ostream& operator<<(std::ostream& os, const OpDecrease& el);

std::ostream& operator<<(std::ostream& os, const OpScaleUp& el);

std::ostream& operator<<(std::ostream& os, const OpScaleDown& el);

std::ostream& operator<<(std::ostream& os, const Minimize& el);

std::ostream& operator<<(std::ostream& os, const Maximize& el);

template<NumericEffectOpKind Op, FactKind T>
std::ostream& operator<<(std::ostream& os, const Data<NumericEffect<Op, T>>& el)
{
    return tyr::print(os, el);
}

template<NumericEffectOpKind Op, FactKind T>
std::ostream& operator<<(std::ostream& os, const NumericEffectView<Op, T>& el)
{
    return tyr::print(os, el);
}

template<NumericEffectOpKind Op, FactKind T>
std::ostream& operator<<(std::ostream& os, const Data<GroundNumericEffect<Op, T>>& el)
{
    return tyr::print(os, el);
}

template<NumericEffectOpKind Op, FactKind T>
std::ostream& operator<<(std::ostream& os, const GroundNumericEffectView<Op, T>& el)
{
    return tyr::print(os, el);
}

template<FactKind T>
std::ostream& operator<<(std::ostream& os, const Data<NumericEffectOperator<T>>& el)
{
    return tyr::print(os, el);
}

template<FactKind T>
std::ostream& operator<<(std::ostream& os, const NumericEffectOperatorView<T>& el)
{
    return tyr::print(os, el);
}

template<FactKind T>
std::ostream& operator<<(std::ostream& os, const Data<GroundNumericEffectOperator<T>>& el)
{
    return tyr::print(os, el);
}

template<FactKind T>
std::ostream& operator<<(std::ostream& os, const GroundNumericEffectOperatorView<T>& el)
{
    return tyr::print(os, el);
}

std::ostream& operator<<(std::ostream& os, const Data<ConditionalEffect>& el);

std::ostream& operator<<(std::ostream& os, const ConditionalEffectView& el);

std::ostream& operator<<(std::ostream& os, const Data<GroundConditionalEffect>& el);

std::ostream& operator<<(std::ostream& os, const GroundConditionalEffectView& el);

std::ostream& operator<<(std::ostream& os, const Data<ConjunctiveEffect>& el);

std::ostream& operator<<(std::ostream& os, const ConjunctiveEffectView& el);

std::ostream& operator<<(std::ostream& os, const Data<GroundConjunctiveEffect>& el);

std::ostream& operator<<(std::ostream& os, const GroundConjunctiveEffectView& el);

std::ostream& operator<<(std::ostream& os, const Data<Action>& el);

std::ostream& operator<<(std::ostream& os, const ActionView& el);

std::ostream& operator<<(std::ostream& os, const Data<GroundAction>& el);

std::ostream& operator<<(std::ostream& os, const GroundActionView& el);

std::ostream& operator<<(std::ostream& os, const std::pair<GroundActionView, PlanFormatting>& el);

std::ostream& operator<<(std::ostream& os, const Data<Axiom>& el);

std::ostream& operator<<(std::ostream& os, const AxiomView& el);

std::ostream& operator<<(std::ostream& os, const Data<GroundAxiom>& el);

std::ostream& operator<<(std::ostream& os, const GroundAxiomView& el);

std::ostream& operator<<(std::ostream& os, const ActionBindingView& el);

std::ostream& operator<<(std::ostream& os, const AxiomBindingView& el);

std::ostream& operator<<(std::ostream& os, const Data<Metric>& el);

std::ostream& operator<<(std::ostream& os, const MetricView& el);

std::ostream& operator<<(std::ostream& os, const Data<Task>& el);

std::ostream& operator<<(std::ostream& os, const TaskView& el);

std::ostream& operator<<(std::ostream& os, const Data<Domain>& el);

std::ostream& operator<<(std::ostream& os, const DomainView& el);

template<FactKind T>
std::ostream& operator<<(std::ostream& os, const Data<FDRVariable<T>>& el)
{
    return tyr::print(os, el);
}

template<FactKind T>
std::ostream& operator<<(std::ostream& os, const FDRVariableView<T>& el)
{
    return tyr::print(os, el);
}

std::ostream& operator<<(std::ostream& os, const FDRValue& el);

template<FactKind T>
std::ostream& operator<<(std::ostream& os, const Data<FDRFact<T>>& el)
{
    return tyr::print(os, el);
}

template<FactKind T>
std::ostream& operator<<(std::ostream& os, const FDRFactView<T>& el)
{
    return tyr::print(os, el);
}

std::ostream& operator<<(std::ostream& os, const Data<ConjunctiveCondition>& el);

std::ostream& operator<<(std::ostream& os, const ConjunctiveConditionView& el);

std::ostream& operator<<(std::ostream& os, const Data<GroundConjunctiveCondition>& el);

std::ostream& operator<<(std::ostream& os, const GroundConjunctiveConditionView& el);

std::ostream& operator<<(std::ostream& os, const Data<FDRTask>& el);

std::ostream& operator<<(std::ostream& os, const FDRTaskView& el);

std::ostream& operator<<(std::ostream& os, const PlanningDomain& el);

std::ostream& operator<<(std::ostream& os, const PlanningTask& el);

std::ostream& operator<<(std::ostream& os, const PlanningFDRTask& el);

}
}
#endif
