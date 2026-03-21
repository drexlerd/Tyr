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

/**
 * Data/Index/View
 */

template<typename V>
nb::class_<V>& add_print(nb::class_<V>& cls)
{
    return cls  //
        .def("__str__", [](const V& self) { return to_string(self); })
        .def("__repr__", [](const V& self) { return to_string(self); });
}

template<typename V>
nb::class_<V>& add_hash(nb::class_<V>& cls)
{
    return cls  //
        .def("__eq__", [](const V& self, const V& other) { return EqualTo<V> {}(self, other); })
        .def("__hash__", [](const V& self) { return Hash<V> {}(self); });
}

/**
 * Data
 */

void bind_object_builder(nb::module_& m, const std::string& name)
{
    using V = Data<Object>;

    nb::class_<V>(m, name.c_str())  //
        .def(nb::init<>())
        .def_rw("name", &V::name);
}

void bind_binding_builder(nb::module_& m, const std::string& name)
{
    using V = Data<Binding>;

    auto cls = nb::class_<V>(m, name.c_str())  //
                   .def(nb::init<>())
                   .def_rw("objects", &V::objects);
    add_print(cls);
    add_hash(cls);
}

void bind_variable_builder(nb::module_& m, const std::string& name)
{
    using V = Data<Variable>;

    auto cls = nb::class_<V>(m, name.c_str())  //
                   .def(nb::init<>())
                   .def_rw("name", &V::name);
    add_print(cls);
    add_hash(cls);
}

template<FactKind T>
void bind_predicate_builder(nb::module_& m, const std::string& name)
{
    using V = Data<Predicate<T>>;

    auto cls = nb::class_<V>(m, name.c_str())  //
                   .def(nb::init<>())
                   .def_rw("name", &V::name)
                   .def_rw("arity", &V::arity);
    add_print(cls);
    add_hash(cls);
}

template<FactKind T>
void bind_ground_atom_builder(nb::module_& m, const std::string& name)
{
    using V = Data<GroundAtom<T>>;

    auto cls = nb::class_<V>(m, name.c_str())  //
                   .def(nb::init<>())
                   .def_rw("predicate", &V::predicate)
                   .def_rw("row", &V::row);
    add_print(cls);
    add_hash(cls);
}

/**
 * Index
 */

template<typename Tag>
void bind_relation_binding_index(nb::module_& m, const std::string& name)
{
    using V = RelationBindingIndex<Tag>;

    auto cls = nb::class_<V>(m, name.c_str())  //
                   .def_ro("relation_index", &V::relation)
                   .def_ro("row_index", &V::row);
    add_print(cls);
    add_hash(cls);
}

/**
 * Views
 */

void bind_object(nb::module_& m, const std::string& name)
{
    using V = ObjectView;

    auto cls = nb::class_<V>(m, name.c_str())  //
                   .def("get_index", &V::get_index)
                   .def("get_name", &V::get_name);
    add_print(cls);
    add_hash(cls);
}

void bind_variable(nb::module_& m, const std::string& name)
{
    using V = VariableView;

    auto cls = nb::class_<V>(m, name.c_str())  //
                   .def("get_index", &V::get_index)
                   .def("get_name", &V::get_name);
    add_print(cls);
    add_hash(cls);
}

void bind_term(nb::module_& m, const std::string& name)
{
    using V = TermView;

    auto cls = nb::class_<V>(m, name.c_str())  //
                   .def("get_variant", &V::get_variant);
    add_print(cls);
    add_hash(cls);
}

template<typename T>
void bind_relation_binding(nb::module_& m, const std::string& name)
{
    using V = View<RelationBindingIndex<T>, Repository>;

    auto cls = nb::class_<V>(m, name.c_str())  //
                   .def("get_index", &V::get_index)
                   .def("get_objects", &V::get_objects);
    add_print(cls);
    add_hash(cls);
}

template<FactKind T>
void bind_predicate(nb::module_& m, const std::string& name)
{
    using V = PredicateView<T>;

    auto cls = nb::class_<V>(m, name.c_str())  //
                   .def("get_index", &V::get_index)
                   .def("get_name", &V::get_name)
                   .def("get_arity", &V::get_arity);
    add_print(cls);
    add_hash(cls);
}

template<FactKind T>
void bind_atom(nb::module_& m, const std::string& name)
{
    using V = AtomView<T>;

    auto cls = nb::class_<V>(m, name.c_str())  //
                   .def("get_index", &V::get_index)
                   .def("get_predicate", &V::get_predicate)
                   .def("get_terms", &V::get_terms);
    add_print(cls);
    add_hash(cls);
}

template<FactKind T>
void bind_ground_atom(nb::module_& m, const std::string& name)
{
    using V = GroundAtomView<T>;

    auto cls = nb::class_<V>(m, name.c_str())  //
                   .def("get_index", &V::get_index)
                   .def("get_predicate", &V::get_predicate)
                   .def("get_objects", [](const V& self) { return self.get_row().get_objects(); });
    add_print(cls);
    add_hash(cls);
}

template<FactKind T>
void bind_literal(nb::module_& m, const std::string& name)
{
    using V = LiteralView<T>;

    auto cls = nb::class_<V>(m, name.c_str())  //
                   .def("get_index", &V::get_index)
                   .def("get_atom", &V::get_atom)
                   .def("get_polarity", &V::get_polarity);
    add_print(cls);
    add_hash(cls);
}

template<FactKind T>
void bind_ground_literal(nb::module_& m, const std::string& name)
{
    using V = GroundLiteralView<T>;

    auto cls = nb::class_<V>(m, name.c_str())  //
                   .def("get_index", &V::get_index)
                   .def("get_atom", &V::get_atom)
                   .def("get_polarity", &V::get_polarity);
    add_print(cls);
    add_hash(cls);
}

template<FactKind T>
void bind_fdr_variable(nb::module_& m, const std::string& name)
{
    using V = FDRVariableView<T>;

    auto cls = nb::class_<V>(m, name.c_str())  //
                   .def("get_index", &V::get_index)
                   .def("get_domain_size", &V::get_domain_size)
                   .def("get_atoms", &V::get_atoms);
    add_print(cls);
    add_hash(cls);
}

template<FactKind T>
void bind_fdr_fact(nb::module_& m, const std::string& name)
{
    using V = FDRFactView<T>;

    auto cls = nb::class_<V>(m, name.c_str())  //
                   .def("get_value", &V::get_value)
                   .def("get_atom", &V::get_atom);
    add_print(cls);
    add_hash(cls);
}

template<FactKind T>
void bind_function(nb::module_& m, const std::string& name)
{
    using V = FunctionView<T>;

    auto cls = nb::class_<V>(m, name.c_str())  //
                   .def("get_index", &V::get_index)
                   .def("get_name", &V::get_name)
                   .def("get_arity", &V::get_arity);
    add_print(cls);
    add_hash(cls);
}

template<FactKind T>
void bind_function_term(nb::module_& m, const std::string& name)
{
    using V = FunctionTermView<T>;

    auto cls = nb::class_<V>(m, name.c_str())  //
                   .def("get_index", &V::get_index)
                   .def("get_function", &V::get_function)
                   .def("get_terms", &V::get_terms);
    add_print(cls);
    add_hash(cls);
}

template<FactKind T>
void bind_ground_function_term(nb::module_& m, const std::string& name)
{
    using V = GroundFunctionTermView<T>;

    auto cls = nb::class_<V>(m, name.c_str())  //
                   .def("get_index", &V::get_index)
                   .def("get_function", &V::get_function)
                   .def("get_objects", [](const V& self) { return self.get_row().get_objects(); });
    add_print(cls);
    add_hash(cls);
}

template<FactKind T>
void bind_ground_function_term_value(nb::module_& m, const std::string& name)
{
    using V = GroundFunctionTermValueView<T>;

    auto cls = nb::class_<V>(m, name.c_str())  //
                   .def("get_index", &V::get_index)
                   .def("get_fterm", &V::get_fterm)
                   .def("get_value", &V::get_value);
    add_print(cls);
    add_hash(cls);
}

template<NumericEffectOpKind Op, FactKind T>
void bind_numeric_effect(nb::module_& m, const std::string& name)
{
    using V = NumericEffectView<Op, T>;

    auto cls = nb::class_<V>(m, name.c_str())  //
                   .def("get_index", &V::get_index)
                   .def("get_fterm", &V::get_fterm)
                   .def("get_fexpr", &V::get_fexpr);
    add_print(cls);
    add_hash(cls);
}

template<NumericEffectOpKind Op, FactKind T>
void bind_ground_numeric_effect(nb::module_& m, const std::string& name)
{
    using V = GroundNumericEffectView<Op, T>;

    auto cls = nb::class_<V>(m, name.c_str())  //
                   .def("get_index", &V::get_index)
                   .def("get_fterm", &V::get_fterm)
                   .def("get_fexpr", &V::get_fexpr);
    add_print(cls);
    add_hash(cls);
}

template<FactKind T>
void bind_numeric_effect_operator(nb::module_& m, const std::string& name)
{
    using V = NumericEffectOperatorView<T>;

    auto cls = nb::class_<V>(m, name.c_str())  //
                   .def("get_variant", &V::get_variant);
    add_print(cls);
    add_hash(cls);
}

template<FactKind T>
void bind_ground_numeric_effect_operator(nb::module_& m, const std::string& name)
{
    using V = GroundNumericEffectOperatorView<T>;

    auto cls = nb::class_<V>(m, name.c_str())  //
                   .def("get_variant", &V::get_variant);
    add_print(cls);
    add_hash(cls);
}

void bind_function_expression(nb::module_& m, const std::string& name)
{
    using V = FunctionExpressionView;

    auto cls = nb::class_<V>(m, name.c_str())  //
                   .def("get_variant", &V::get_variant);
    add_print(cls);
    add_hash(cls);
}

void bind_conjunctive_condition(nb::module_& m, const std::string& name)
{
    using V = ConjunctiveConditionView;

    auto cls = nb::class_<V>(m, name.c_str())  //
                   .def("get_index", &V::get_index)
                   .def("get_variables", &V::get_variables)
                   .def("get_arity", &V::get_arity)
                   .def("get_static_literals", &V::get_literals<StaticTag>)
                   .def("get_fluent_literals", &V::get_literals<FluentTag>)
                   .def("get_derived_literals", &V::get_literals<DerivedTag>)
                   .def("get_numeric_constraints", &V::get_numeric_constraints);
    add_print(cls);
    add_hash(cls);
}

void bind_conjunctive_effect(nb::module_& m, const std::string& name)
{
    using V = ConjunctiveEffectView;

    auto cls = nb::class_<V>(m, name.c_str())  //
                   .def("get_index", &V::get_index)
                   .def("get_literals", &V::get_literals)
                   .def("get_numeric_effects", &V::get_numeric_effects)
                   .def("get_auxiliary_numeric_effect", &V::get_auxiliary_numeric_effect);
    add_print(cls);
    add_hash(cls);
}

void bind_conditional_effect(nb::module_& m, const std::string& name)
{
    using V = ConditionalEffectView;

    auto cls = nb::class_<V>(m, name.c_str())  //
                   .def("get_index", &V::get_index)
                   .def("get_variables", &V::get_variables)
                   .def("get_arity", &V::get_arity)
                   .def("get_condition", &V::get_condition)
                   .def("get_effect", &V::get_effect);
    add_print(cls);
    add_hash(cls);
}

void bind_action(nb::module_& m, const std::string& name)
{
    using V = ActionView;

    auto cls = nb::class_<V>(m, name.c_str())  //
                   .def("get_index", &V::get_index)
                   .def("get_name", &V::get_name)
                   .def("get_original_arity", &V::get_original_arity)
                   .def("get_arity", &V::get_arity)
                   .def("get_variables", &V::get_variables)
                   .def("get_condition", &V::get_condition)
                   .def("get_effects", &V::get_effects);
    add_print(cls);
    add_hash(cls);
}

void bind_axiom(nb::module_& m, const std::string& name)
{
    using V = AxiomView;

    auto cls = nb::class_<V>(m, name.c_str())  //
                   .def("get_index", &V::get_index)
                   .def("get_arity", &V::get_arity)
                   .def("get_variables", &V::get_variables)
                   .def("get_body", &V::get_body)
                   .def("get_head", &V::get_head);
    add_print(cls);
    add_hash(cls);
}

void bind_ground_function_expression(nb::module_& m, const std::string& name)
{
    using V = GroundFunctionExpressionView;

    auto cls = nb::class_<V>(m, name.c_str())  //
                   .def("get_variant", &V::get_variant);
    add_print(cls);
    add_hash(cls);
}

void bind_ground_conjunctive_condition(nb::module_& m, const std::string& name)
{
    using V = GroundConjunctiveConditionView;

    auto cls = nb::class_<V>(m, name.c_str())  //
                   .def("get_index", &V::get_index)
                   .def("get_static_facts", &V::get_facts<StaticTag>)
                   .def("get_fluent_facts", &V::get_facts<FluentTag>)
                   .def("get_derived_facts", &V::get_facts<DerivedTag>)
                   .def("get_numeric_constraints", &V::get_numeric_constraints);
    add_print(cls);
    add_hash(cls);
}

void bind_ground_conjunctive_effect(nb::module_& m, const std::string& name)
{
    using V = GroundConjunctiveEffectView;

    auto cls = nb::class_<V>(m, name.c_str())  //
                   .def("get_index", &V::get_index)
                   .def("get_facts", &V::get_facts)
                   .def("get_numeric_effects", &V::get_numeric_effects)
                   .def("get_auxiliary_numeric_effect", &V::get_auxiliary_numeric_effect);
    add_print(cls);
    add_hash(cls);
}

void bind_ground_conditional_effect(nb::module_& m, const std::string& name)
{
    using V = GroundConditionalEffectView;

    auto cls = nb::class_<V>(m, name.c_str())  //
                   .def("get_index", &V::get_index)
                   .def("get_condition", &V::get_condition)
                   .def("get_effect", &V::get_effect);
    add_print(cls);
    add_hash(cls);
}

void bind_ground_action(nb::module_& m, const std::string& name)
{
    using V = GroundActionView;

    auto cls = nb::class_<V>(m, name.c_str())  //
                   .def("get_index", &V::get_index)
                   .def("get_action", &V::get_action)
                   .def("get_objects", [](const V& self) { return self.get_row().get_objects(); })
                   .def("get_condition", &V::get_condition)
                   .def("get_effects", &V::get_effects);
    add_print(cls);
    add_hash(cls);
}

void bind_ground_axiom(nb::module_& m, const std::string& name)
{
    using V = GroundAxiomView;

    auto cls = nb::class_<V>(m, name.c_str())  //
                   .def("get_index", &V::get_index)
                   .def("get_axiom", &V::get_axiom)
                   .def("get_objects", [](const V& self) { return self.get_row().get_objects(); })
                   .def("get_body", &V::get_body)
                   .def("get_head", &V::get_head);
    add_print(cls);
    add_hash(cls);
}

void bind_metric(nb::module_& m, const std::string& name)
{
    using V = MetricView;

    auto cls = nb::class_<V>(m, name.c_str())  //
                   .def("get_index", &V::get_index)
                   .def("get_objective", &V::get_objective)
                   .def("get_fexpr", &V::get_fexpr);
    add_print(cls);
    add_hash(cls);
}

void bind_domain(nb::module_& m, const std::string& name)
{
    using V = DomainView;

    auto cls = nb::class_<V>(m, name.c_str())  //
                   .def("get_index", &V::get_index)
                   .def("get_name", &V::get_name)
                   .def("get_static_predicates", &V::get_predicates<StaticTag>)
                   .def("get_fluent_predicates", &V::get_predicates<FluentTag>)
                   .def("get_derived_predicates", &V::get_predicates<DerivedTag>)
                   .def("get_static_functions", &V::get_functions<StaticTag>)
                   .def("get_fluent_functions", &V::get_functions<FluentTag>)
                   .def("get_auxiliary_function", &V::get_auxiliary_function)
                   .def("get_constants", &V::get_constants)
                   .def("get_actions", &V::get_actions)
                   .def("get_axioms", &V::get_axioms);
    add_print(cls);
    add_hash(cls);
}

void bind_lifted_task(nb::module_& m, const std::string& name)
{
    using V = TaskView;

    auto cls = nb::class_<V>(m, name.c_str())  //
                   .def("get_index", &V::get_index)
                   .def("get_name", &V::get_name)
                   .def("get_domain", &V::get_domain)
                   .def("get_derived_predicates", &V::get_derived_predicates)
                   .def("get_objects", &V::get_objects)
                   .def("get_static_atoms", &V::get_atoms<StaticTag>)
                   .def("get_fluent_atoms", &V::get_atoms<FluentTag>)
                   .def("get_static_fterm_values", &V::get_fterm_values<StaticTag>)
                   .def("get_fluent_fterm_values", &V::get_fterm_values<FluentTag>)
                   .def("get_auxiliary_fterm_value", &V::get_auxiliary_fterm_value)
                   .def("get_goal", &V::get_goal)
                   .def("get_metric", &V::get_metric)
                   .def("get_axioms", &V::get_axioms);
    add_print(cls);
    add_hash(cls);
}

void bind_ground_task(nb::module_& m, const std::string& name)
{
    using V = FDRTaskView;

    auto cls = nb::class_<V>(m, name.c_str())  //
                   .def("get_index", &V::get_index)
                   .def("get_name", &V::get_name)
                   .def("get_domain", &V::get_domain)
                   .def("get_derived_predicates", &V::get_derived_predicates)
                   .def("get_objects", &V::get_objects)
                   .def("get_static_atoms", &V::get_atoms<StaticTag>)
                   .def("get_fluent_atoms", &V::get_atoms<FluentTag>)
                   .def("get_derived_atoms", &V::get_atoms<DerivedTag>)
                   .def("get_static_fterm", &V::get_fterms<StaticTag>)
                   .def("get_fluent_fterm", &V::get_fterms<FluentTag>)
                   .def("get_auxiliary_fterm", &V::get_auxiliary_fterm)
                   .def("get_static_fterm_values", &V::get_fterm_values<StaticTag>)
                   .def("get_fluent_fterm_values", &V::get_fterm_values<FluentTag>)
                   .def("get_auxiliary_fterm_value", &V::get_auxiliary_fterm_value)
                   .def("get_goal", &V::get_goal)
                   .def("get_metric", &V::get_metric)
                   .def("get_axioms", &V::get_axioms)
                   .def("get_fluent_variables", &V::get_fluent_variables)
                   .def("get_fluent_facts", &V::get_fluent_facts)
                   .def("get_ground_actions", &V::get_ground_actions)
                   .def("get_ground_axioms", &V::get_ground_axioms);
    add_print(cls);
    add_hash(cls);
}

template<OpKind Op, typename T>
void bind_unary_operator(nb::module_& m, const std::string& name)
{
    using V = UnaryOperatorView<Op, T>;

    auto cls = nb::class_<V>(m, name.c_str())  //
                   .def("get_index", &V::get_index)
                   .def("get_arg", &V::get_arg);
    add_print(cls);
    add_hash(cls);
}

template<OpKind Op, typename T>
void bind_binary_operator(nb::module_& m, const std::string& name)
{
    using V = BinaryOperatorView<Op, T>;

    auto cls = nb::class_<V>(m, name.c_str())  //
                   .def("get_index", &V::get_index)
                   .def("get_lhs", &V::get_lhs)
                   .def("get_rhs", &V::get_rhs);
    add_print(cls);
    add_hash(cls);
}

template<OpKind Op, typename T>
void bind_multi_operator(nb::module_& m, const std::string& name)
{
    using V = MultiOperatorView<Op, T>;

    auto cls = nb::class_<V>(m, name.c_str())  //
                   .def("get_index", &V::get_index)
                   .def("get_args", &V::get_args);
    add_print(cls);
    add_hash(cls);
}

template<typename T>
void bind_arithmethic_operator(nb::module_& m, const std::string& name)
{
    using V = ArithmeticOperatorView<T>;

    auto cls = nb::class_<V>(m, name.c_str())  //
                   .def("get_variant", &V::get_variant);
    add_print(cls);
    add_hash(cls);
}

template<typename T>
void bind_boolean_operator(nb::module_& m, const std::string& name)
{
    using V = BooleanOperatorView<T>;

    auto cls = nb::class_<V>(m, name.c_str())  //
                   .def("get_variant", &V::get_variant);
    add_print(cls);
    add_hash(cls);
}
}

#endif
