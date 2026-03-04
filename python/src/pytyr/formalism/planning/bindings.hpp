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

#ifndef TYR_PYTHON_FORMALISM_PLANNING_BINDINGS_HPP
#define TYR_PYTHON_FORMALISM_PLANNING_BINDINGS_HPP

#include "../../init_declarations.hpp"

namespace tyr::formalism::planning
{
template<FactKind T>
void bind_predicate(nb::module_& m, const std::string& name)
{
    using V = View<Index<Predicate<T>>, Repository>;

    nb::class_<V>(m, name.c_str())
        .def("__str__", [](const V& self) { return to_string(self); })
        .def("__repr__", [](const V& self) { return to_string(self); })
        .def("__eq__", [](const V& self, const V& other) { return EqualTo<V> {}(self, other); })
        .def("__hash__", [](const V& self) { return Hash<V> {}(self); })
        .def("get_name", &V::get_name)
        .def("get_arity", &V::get_arity);
}

template<FactKind T>
void bind_atom(nb::module_& m, const std::string& name)
{
    using V = View<Index<Atom<T>>, Repository>;

    nb::class_<V>(m, name.c_str())  //
        .def("__str__", [](const V& self) { return to_string(self); })
        .def("__repr__", [](const V& self) { return to_string(self); })
        .def("__eq__", [](const V& self, const V& other) { return EqualTo<V> {}(self, other); })
        .def("__hash__", [](const V& self) { return Hash<V> {}(self); })
        .def("get_predicate", &V::get_predicate)
        .def("get_terms", &V::get_terms);
}

template<FactKind T>
void bind_ground_atom(nb::module_& m, const std::string& name)
{
    nb::class_<View<Index<GroundAtom<T>>, Repository>>(m, name.c_str())  //
        .def("__str__", [](View<Index<GroundAtom<T>>, Repository> self) { return to_string(self); });
}

template<FactKind T>
void bind_literal(nb::module_& m, const std::string& name)
{
    nb::class_<View<Index<Literal<T>>, Repository>>(m, name.c_str())  //
        .def("__str__", [](View<Index<Literal<T>>, Repository> self) { return to_string(self); });
}

template<FactKind T>
void bind_ground_literal(nb::module_& m, const std::string& name)
{
    nb::class_<View<Index<GroundLiteral<T>>, Repository>>(m, name.c_str())  //
        .def("__str__", [](View<Index<GroundLiteral<T>>, Repository> self) { return to_string(self); });
}

template<FactKind T>
void bind_fdr_variable(nb::module_& m, const std::string& name)
{
    nb::class_<View<Index<FDRVariable<T>>, Repository>>(m, name.c_str())  //
        .def("__str__", [](View<Index<FDRVariable<T>>, Repository> self) { return to_string(self); });
}

template<FactKind T>
void bind_fdr_fact(nb::module_& m, const std::string& name)
{
    nb::class_<View<Data<FDRFact<T>>, Repository>>(m, name.c_str())  //
        .def("__str__", [](View<Data<FDRFact<T>>, Repository> self) { return to_string(self); });
}

template<FactKind T>
void bind_function(nb::module_& m, const std::string& name)
{
    nb::class_<View<Index<Function<T>>, Repository>>(m, name.c_str())
        .def("__str__", [](View<Index<Function<T>>, Repository> self) { return to_string(self); })
        .def("get_name", &View<Index<Function<T>>, Repository>::get_name);
}

template<FactKind T>
void bind_function_term(nb::module_& m, const std::string& name)
{
    nb::class_<View<Index<FunctionTerm<T>>, Repository>>(m, name.c_str())  //
        .def("__str__", [](View<Index<FunctionTerm<T>>, Repository> self) { return to_string(self); });
}

template<FactKind T>
void bind_ground_function_term(nb::module_& m, const std::string& name)
{
    nb::class_<View<Index<GroundFunctionTerm<T>>, Repository>>(m, name.c_str())  //
        .def("__str__", [](View<Index<GroundFunctionTerm<T>>, Repository> self) { return to_string(self); });
}

template<FactKind T>
void bind_ground_function_term_value(nb::module_& m, const std::string& name)
{
    nb::class_<View<Index<GroundFunctionTermValue<T>>, Repository>>(m, name.c_str())  //
        .def("__str__", [](View<Index<GroundFunctionTermValue<T>>, Repository> self) { return to_string(self); });
}

template<NumericEffectOpKind Op, FactKind T>
void bind_numeric_effect(nb::module_& m, const std::string& name)
{
    nb::class_<View<Index<NumericEffect<Op, T>>, Repository>>(m, name.c_str())  //
        .def("__str__", [](View<Index<NumericEffect<Op, T>>, Repository> self) { return to_string(self); });
}

template<NumericEffectOpKind Op, FactKind T>
void bind_ground_numeric_effect(nb::module_& m, const std::string& name)
{
    nb::class_<View<Index<GroundNumericEffect<Op, T>>, Repository>>(m, name.c_str())  //
        .def("__str__", [](View<Index<GroundNumericEffect<Op, T>>, Repository> self) { return to_string(self); });
}

template<FactKind T>
void bind_numeric_effect_operator(nb::module_& m, const std::string& name)
{
    nb::class_<View<Data<NumericEffectOperator<T>>, Repository>>(m, name.c_str())  //
        .def("__str__", [](View<Data<NumericEffectOperator<T>>, Repository> self) { return to_string(self); });
}

template<FactKind T>
void bind_ground_numeric_effect_operator(nb::module_& m, const std::string& name)
{
    nb::class_<View<Data<GroundNumericEffectOperator<T>>, Repository>>(m, name.c_str())  //
        .def("__str__", [](View<Data<GroundNumericEffectOperator<T>>, Repository> self) { return to_string(self); });
}

template<OpKind Op, typename T>
void bind_unary_operator(nb::module_& m, const std::string& name)
{
    nb::class_<View<Index<UnaryOperator<Op, T>>, Repository>>(m, name.c_str())  //
        .def("__str__", [](View<Index<UnaryOperator<Op, T>>, Repository> self) { return to_string(self); });
}

template<OpKind Op, typename T>
void bind_binary_operator(nb::module_& m, const std::string& name)
{
    nb::class_<View<Index<BinaryOperator<Op, T>>, Repository>>(m, name.c_str())  //
        .def("__str__", [](View<Index<BinaryOperator<Op, T>>, Repository> self) { return to_string(self); });
}

template<OpKind Op, typename T>
void bind_multi_operator(nb::module_& m, const std::string& name)
{
    nb::class_<View<Index<MultiOperator<Op, T>>, Repository>>(m, name.c_str())  //
        .def("__str__", [](View<Index<MultiOperator<Op, T>>, Repository> self) { return to_string(self); });
}

template<typename T>
void bind_arithmethic_operator(nb::module_& m, const std::string& name)
{
    nb::class_<View<Data<ArithmeticOperator<T>>, Repository>>(m, name.c_str())  //
        .def("__str__", [](View<Data<ArithmeticOperator<T>>, Repository> self) { return to_string(self); });
}

template<typename T>
void bind_boolean_operator(nb::module_& m, const std::string& name)
{
    nb::class_<View<Data<BooleanOperator<T>>, Repository>>(m, name.c_str())  //
        .def("__str__", [](View<Data<BooleanOperator<T>>, Repository> self) { return to_string(self); });
}
}

#endif
