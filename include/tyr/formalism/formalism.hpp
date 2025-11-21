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

#ifndef TYR_FORMALISM_FORMALISM_HPP_
#define TYR_FORMALISM_FORMALISM_HPP_

// Include specialization headers first
#include "tyr/formalism/data_traits.hpp"
#include "tyr/formalism/formatter.hpp"
#include "tyr/formalism/index_traits.hpp"
#include "tyr/formalism/proxy_traits.hpp"
//
#include "tyr/formalism/arithmetic_operator.hpp"
#include "tyr/formalism/arithmetic_operator_proxy.hpp"
#include "tyr/formalism/atom.hpp"
#include "tyr/formalism/atom_index.hpp"
#include "tyr/formalism/atom_proxy.hpp"
#include "tyr/formalism/binary_operator.hpp"
#include "tyr/formalism/binary_operator_index.hpp"
#include "tyr/formalism/binary_operator_proxy.hpp"
#include "tyr/formalism/boolean_operator.hpp"
#include "tyr/formalism/boolean_operator_proxy.hpp"
#include "tyr/formalism/declarations.hpp"
#include "tyr/formalism/double.hpp"
#include "tyr/formalism/function.hpp"
#include "tyr/formalism/function_expression.hpp"
#include "tyr/formalism/function_expression_proxy.hpp"
#include "tyr/formalism/function_index.hpp"
#include "tyr/formalism/function_proxy.hpp"
#include "tyr/formalism/function_term.hpp"
#include "tyr/formalism/function_term_index.hpp"
#include "tyr/formalism/function_term_proxy.hpp"
#include "tyr/formalism/ground_atom.hpp"
#include "tyr/formalism/ground_atom_index.hpp"
#include "tyr/formalism/ground_atom_proxy.hpp"
#include "tyr/formalism/ground_function_expression.hpp"
#include "tyr/formalism/ground_function_expression_proxy.hpp"
#include "tyr/formalism/ground_function_term.hpp"
#include "tyr/formalism/ground_function_term_index.hpp"
#include "tyr/formalism/ground_function_term_proxy.hpp"
#include "tyr/formalism/ground_function_term_value.hpp"
#include "tyr/formalism/ground_function_term_value_index.hpp"
#include "tyr/formalism/ground_function_term_value_proxy.hpp"
#include "tyr/formalism/ground_literal.hpp"
#include "tyr/formalism/ground_literal_index.hpp"
#include "tyr/formalism/ground_literal_proxy.hpp"
#include "tyr/formalism/ground_rule.hpp"
#include "tyr/formalism/ground_rule_index.hpp"
#include "tyr/formalism/ground_rule_proxy.hpp"
#include "tyr/formalism/literal.hpp"
#include "tyr/formalism/literal_index.hpp"
#include "tyr/formalism/literal_proxy.hpp"
#include "tyr/formalism/multi_operator.hpp"
#include "tyr/formalism/multi_operator_index.hpp"
#include "tyr/formalism/multi_operator_proxy.hpp"
#include "tyr/formalism/object.hpp"
#include "tyr/formalism/object_index.hpp"
#include "tyr/formalism/object_proxy.hpp"
#include "tyr/formalism/parameter_index.hpp"
#include "tyr/formalism/predicate.hpp"
#include "tyr/formalism/predicate_index.hpp"
#include "tyr/formalism/predicate_proxy.hpp"
#include "tyr/formalism/program.hpp"
#include "tyr/formalism/program_index.hpp"
#include "tyr/formalism/program_proxy.hpp"
#include "tyr/formalism/repository.hpp"
#include "tyr/formalism/rule.hpp"
#include "tyr/formalism/rule_index.hpp"
#include "tyr/formalism/rule_proxy.hpp"
#include "tyr/formalism/term.hpp"
#include "tyr/formalism/term_proxy.hpp"
#include "tyr/formalism/unary_operator.hpp"
#include "tyr/formalism/unary_operator_index.hpp"
#include "tyr/formalism/unary_operator_proxy.hpp"
#include "tyr/formalism/variable.hpp"
#include "tyr/formalism/variable_index.hpp"
#include "tyr/formalism/variable_proxy.hpp"

#endif
