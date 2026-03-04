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

#include "bindings.hpp"

#include "../../common/bindings.hpp"

namespace tyr::formalism::planning
{

void bind_module_definitions(nb::module_& m)
{
    nb::class_<View<Index<Object>, Repository>>(m, "Object")  //
        .def("__str__", [](View<Index<Object>, Repository> self) { return to_string(self); })
        .def("get_name", &View<Index<Object>, Repository>::get_name);

    nb::class_<View<Index<Variable>, Repository>>(m, "Variable")  //
        .def("__str__", [](View<Index<Variable>, Repository> self) { return to_string(self); })
        .def("get_name", &View<Index<Variable>, Repository>::get_name);

    bind_fixed_uint<ParameterIndex>(m, "ParameterIndex");

    nb::class_<View<Data<Term>, Repository>>(m, "Term")  //
        .def("__str__", [](View<Data<Term>, Repository> self) { return to_string(self); })
        .def("get_variant", &View<Data<Term>, Repository>::get_variant);

    bind_predicate<StaticTag>(m, "StaticPredicate");
    bind_predicate<FluentTag>(m, "FluentPredicate");
    bind_predicate<DerivedTag>(m, "DerivedPredicate");

    bind_atom<StaticTag>(m, "StaticAtom");
    bind_atom<FluentTag>(m, "FluentAtom");
    bind_atom<DerivedTag>(m, "DerivedAtom");

    bind_ground_atom<StaticTag>(m, "StaticGroundAtom");
    bind_ground_atom<FluentTag>(m, "FluentGroundAtom");
    bind_ground_atom<DerivedTag>(m, "DerivedGroundAtom");

    bind_literal<StaticTag>(m, "StaticLiteral");
    bind_literal<FluentTag>(m, "FluentLiteral");
    bind_literal<DerivedTag>(m, "DerivedLiteral");

    bind_ground_literal<StaticTag>(m, "StaticGroundLiteral");
    bind_ground_literal<FluentTag>(m, "FluentGroundLiteral");
    bind_ground_literal<DerivedTag>(m, "DerivedGroundLiteral");

    bind_function<StaticTag>(m, "StaticFunction");
    bind_function<FluentTag>(m, "FluentFunction");
    bind_function<AuxiliaryTag>(m, "AuxiliaryFunction");

    nb::class_<View<Index<ConjunctiveCondition>, Repository>>(m, "ConjunctiveCondition");

    nb::class_<View<Index<ConjunctiveEffect>, Repository>>(m, "ConjunctiveEffect");

    nb::class_<View<Index<ConditionalEffect>, Repository>>(m, "ConditionalEffect");

    nb::class_<View<Index<Action>, Repository>>(m, "Action")  //
        .def("__str__", [](View<Index<Action>, Repository> self) { return to_string(self); })
        .def("get_name", &View<Index<Action>, Repository>::get_name)
        .def("get_original_arity", &View<Index<Action>, Repository>::get_original_arity)
        .def("get_arity", &View<Index<Action>, Repository>::get_arity)
        .def("get_variables", &View<Index<Action>, Repository>::get_variables)
        .def("get_condition", &View<Index<Action>, Repository>::get_condition)
        .def("get_effects", &View<Index<Action>, Repository>::get_effects);

    nb::class_<View<Index<Axiom>, Repository>>(m, "Axiom");

    nb::class_<View<Index<GroundConjunctiveCondition>, Repository>>(m, "GroundConjunctiveCondition");

    nb::class_<View<Index<GroundConjunctiveEffect>, Repository>>(m, "GroundConjunctiveEffect");

    nb::class_<View<Index<GroundConditionalEffect>, Repository>>(m, "GroundConditionalEffect");

    nb::class_<View<Index<GroundAction>, Repository>>(m, "GroundAction")  //
        .def("__str__", [](View<Index<GroundAction>, Repository> self) { return to_string(self); })
        .def("get_action", &View<Index<GroundAction>, Repository>::get_action)
        .def("get_binding", &View<Index<GroundAction>, Repository>::get_binding)
        .def("get_condition", &View<Index<GroundAction>, Repository>::get_condition)
        .def("get_effects", &View<Index<GroundAction>, Repository>::get_effects);

    nb::class_<View<Index<GroundAxiom>, Repository>>(m, "GroundAxiom");
}

}