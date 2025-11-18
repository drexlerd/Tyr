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

#ifndef TYR_TYR_HPP_
#define TYR_TYR_HPP_

// Cista
#include "tyr/cista/byte_buffer_segmented.hpp"
#include "tyr/cista/declarations.hpp"
#include "tyr/cista/indexed_hash_set.hpp"

// Common
#include "tyr/common/bits.hpp"
#include "tyr/common/config.hpp"
#include "tyr/common/declarations.hpp"
#include "tyr/common/equal_to.hpp"
#include "tyr/common/hash.hpp"
#include "tyr/common/observer_ptr.hpp"
#include "tyr/common/segmented_vector.hpp"

// Formalism
#include "tyr/formalism/atom.hpp"
#include "tyr/formalism/atom_index.hpp"
#include "tyr/formalism/atom_proxy.hpp"
#include "tyr/formalism/declarations.hpp"
#include "tyr/formalism/function.hpp"
#include "tyr/formalism/function_index.hpp"
#include "tyr/formalism/function_proxy.hpp"
#include "tyr/formalism/function_term.hpp"
#include "tyr/formalism/function_term_index.hpp"
#include "tyr/formalism/function_term_proxy.hpp"
#include "tyr/formalism/ground_atom.hpp"
#include "tyr/formalism/ground_atom_index.hpp"
#include "tyr/formalism/ground_atom_proxy.hpp"
#include "tyr/formalism/ground_literal.hpp"
#include "tyr/formalism/ground_literal_index.hpp"
#include "tyr/formalism/ground_literal_proxy.hpp"
#include "tyr/formalism/ground_rule.hpp"
#include "tyr/formalism/ground_rule_index.hpp"
#include "tyr/formalism/ground_rule_proxy.hpp"
#include "tyr/formalism/literal.hpp"
#include "tyr/formalism/literal_index.hpp"
#include "tyr/formalism/literal_proxy.hpp"
#include "tyr/formalism/object.hpp"
#include "tyr/formalism/object_index.hpp"
#include "tyr/formalism/object_proxy.hpp"
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
#include "tyr/formalism/variable.hpp"
#include "tyr/formalism/variable_index.hpp"
#include "tyr/formalism/variable_proxy.hpp"

#endif
