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
#include "datas.hpp"
#include "indices.hpp"
#include "views.hpp"

namespace tyr::formalism::planning
{
namespace
{
template<typename T>
auto bind_get_or_create_canonical()
{
    return [](Repository& self, Data<T>& builder)
    {
        canonicalize(builder);
        return self.template get_or_create<T>(builder);
    };
}
}

/**
 * bind_module_definitions
 */

void bind_module_definitions(nb::module_& m)
{
    {
        nb::class_<loki::ParserOptions>(m, "ParserOptions")
            .def(nb::init<>())
            .def_rw("strict", &loki::ParserOptions::strict, "Enable strict mode")
            .def_rw("verbose", &loki::ParserOptions::verbose, "Enable verbose output");
    }

    {
        nb::class_<Parser>(m, "Parser")
            .def(nb::init<const fs::path&, const loki::ParserOptions&>(), "domain_filepath"_a, "parser_options"_a)
            .def(nb::init<const std::string&, const fs::path&, const loki::ParserOptions&>(), "domain_description"_a, "domain_filepath"_a, "parser_options"_a)
            .def("parse_task", nb::overload_cast<const fs::path&, const loki::ParserOptions&>(&Parser::parse_task), "task_filepath"_a, "parser_options"_a)
            .def("parse_task",
                 nb::overload_cast<const std::string&, const fs::path&, const loki::ParserOptions&>(&Parser::parse_task),
                 "task_description"_a,
                 "task_filepath"_a,
                 "parser_options"_a)
            .def("get_domain", &Parser::get_domain);
    }

    nb::class_<Minimize>(m, "Minimize")
        .def(nb::init<>())
        .def("__str__", [](const Minimize& self) { return to_string(self); })
        .def("__repr__", [](const Minimize& self) { return to_string(self); });

    nb::class_<Maximize>(m, "Maximize")
        .def(nb::init<>())
        .def("__str__", [](const Maximize& self) { return to_string(self); })
        .def("__repr__", [](const Maximize& self) { return to_string(self); });

    /**
     * Common
     */

    bind_fixed_uint<FDRValue>(m, "FDRValue");
    bind_fixed_uint<ParameterIndex>(m, "ParameterIndex");

    /**
     * Index
     */

    bind_indices(m);

    /**
     * Data
     */

    bind_datas(m);

    /**
     * Views
     */

    bind_views(m);

    /**
     * RepositoryFactory
     */

    nb::class_<RepositoryFactory>(m, "RepositoryFactory")  //
        .def(nb::new_([]() { return std::make_shared<RepositoryFactory>(); }))
        .def("create_repository", &RepositoryFactory::create_shared, "parent_repository"_a = nullptr);

    /**
     * Repository
     */

    {
        auto cls = nb::class_<Repository>(m, "Repository")  //
                       .def(
                           "create",
                           [](Repository& self, const Data<Term>& builder) { return make_view(builder, self); },
                           "builder"_a)
                       .def(
                           "create",
                           [](Repository& self, const Data<FunctionExpression>& builder) { return make_view(builder, self); },
                           "builder"_a)
                       .def(
                           "create",
                           [](Repository& self, const Data<GroundFunctionExpression>& builder) { return make_view(builder, self); },
                           "builder"_a)
                       .def(
                           "create",
                           [](Repository& self, const Data<BooleanOperator<Data<FunctionExpression>>>& builder) { return make_view(builder, self); },
                           "builder"_a)
                       .def(
                           "create",
                           [](Repository& self, const Data<BooleanOperator<Data<GroundFunctionExpression>>>& builder) { return make_view(builder, self); },
                           "builder"_a)
                       .def(
                           "create",
                           [](Repository& self, const Data<ArithmeticOperator<Data<FunctionExpression>>>& builder) { return make_view(builder, self); },
                           "builder"_a)
                       .def(
                           "create",
                           [](Repository& self, const Data<ArithmeticOperator<Data<GroundFunctionExpression>>>& builder) { return make_view(builder, self); },
                           "builder"_a)
                       .def(
                           "create",
                           [](Repository& self, const Data<NumericEffectOperator<FluentTag>>& builder) { return make_view(builder, self); },
                           "builder"_a)
                       .def(
                           "create",
                           [](Repository& self, const Data<NumericEffectOperator<AuxiliaryTag>>& builder) { return make_view(builder, self); },
                           "builder"_a)
                       .def(
                           "create",
                           [](Repository& self, const Data<GroundNumericEffectOperator<FluentTag>>& builder) { return make_view(builder, self); },
                           "builder"_a)
                       .def(
                           "create",
                           [](Repository& self, const Data<GroundNumericEffectOperator<AuxiliaryTag>>& builder) { return make_view(builder, self); },
                           "builder"_a)

                       .def("get_or_create", bind_get_or_create_canonical<Object>(), "builder"_a)
                       .def("get_or_create", bind_get_or_create_canonical<Variable>(), "builder"_a)
                       .def("get_or_create",
                            nb::overload_cast<const Data<RelationBinding<Predicate<StaticTag>>>&>(&Repository::get_or_create<Predicate<StaticTag>>),
                            "builder"_a)
                       .def("get_or_create",
                            nb::overload_cast<const Data<RelationBinding<Predicate<FluentTag>>>&>(&Repository::get_or_create<Predicate<FluentTag>>),
                            "builder"_a)
                       .def("get_or_create",
                            nb::overload_cast<const Data<RelationBinding<Predicate<DerivedTag>>>&>(&Repository::get_or_create<Predicate<DerivedTag>>),
                            "builder"_a)
                       .def("get_or_create",
                            nb::overload_cast<const Data<RelationBinding<Function<StaticTag>>>&>(&Repository::get_or_create<Function<StaticTag>>),
                            "builder"_a)
                       .def("get_or_create",
                            nb::overload_cast<const Data<RelationBinding<Function<FluentTag>>>&>(&Repository::get_or_create<Function<FluentTag>>),
                            "builder"_a)
                       .def("get_or_create",
                            nb::overload_cast<const Data<RelationBinding<Function<AuxiliaryTag>>>&>(&Repository::get_or_create<Function<AuxiliaryTag>>),
                            "builder"_a)
                       .def("get_or_create", nb::overload_cast<const Data<RelationBinding<Action>>&>(&Repository::get_or_create<Action>), "builder"_a)
                       .def("get_or_create", nb::overload_cast<const Data<RelationBinding<Axiom>>&>(&Repository::get_or_create<Axiom>), "builder"_a)
                       .def("get_or_create", bind_get_or_create_canonical<Predicate<StaticTag>>(), "builder"_a)
                       .def("get_or_create", bind_get_or_create_canonical<Predicate<FluentTag>>(), "builder"_a)
                       .def("get_or_create", bind_get_or_create_canonical<Predicate<DerivedTag>>(), "builder"_a)
                       .def("get_or_create", bind_get_or_create_canonical<Atom<StaticTag>>(), "builder"_a)
                       .def("get_or_create", bind_get_or_create_canonical<Atom<FluentTag>>(), "builder"_a)
                       .def("get_or_create", bind_get_or_create_canonical<Atom<DerivedTag>>(), "builder"_a)
                       .def("get_or_create", bind_get_or_create_canonical<GroundAtom<StaticTag>>(), "builder"_a)
                       .def("get_or_create", bind_get_or_create_canonical<GroundAtom<FluentTag>>(), "builder"_a)
                       .def("get_or_create", bind_get_or_create_canonical<GroundAtom<DerivedTag>>(), "builder"_a)
                       .def("get_or_create", bind_get_or_create_canonical<Literal<StaticTag>>(), "builder"_a)
                       .def("get_or_create", bind_get_or_create_canonical<Literal<FluentTag>>(), "builder"_a)
                       .def("get_or_create", bind_get_or_create_canonical<Literal<DerivedTag>>(), "builder"_a)
                       .def("get_or_create", bind_get_or_create_canonical<GroundLiteral<StaticTag>>(), "builder"_a)
                       .def("get_or_create", bind_get_or_create_canonical<GroundLiteral<FluentTag>>(), "builder"_a)
                       .def("get_or_create", bind_get_or_create_canonical<GroundLiteral<DerivedTag>>(), "builder"_a)
                       .def("get_or_create", bind_get_or_create_canonical<FDRVariable<FluentTag>>(), "builder"_a)
                       .def("get_or_create", bind_get_or_create_canonical<Function<StaticTag>>(), "builder"_a)
                       .def("get_or_create", bind_get_or_create_canonical<Function<FluentTag>>(), "builder"_a)
                       .def("get_or_create", bind_get_or_create_canonical<Function<AuxiliaryTag>>(), "builder"_a)
                       .def("get_or_create", bind_get_or_create_canonical<FunctionTerm<StaticTag>>(), "builder"_a)
                       .def("get_or_create", bind_get_or_create_canonical<FunctionTerm<FluentTag>>(), "builder"_a)
                       .def("get_or_create", bind_get_or_create_canonical<FunctionTerm<AuxiliaryTag>>(), "builder"_a)

                       .def("get_or_create", bind_get_or_create_canonical<GroundFunctionTerm<StaticTag>>(), "builder"_a)
                       .def("get_or_create", bind_get_or_create_canonical<GroundFunctionTerm<FluentTag>>(), "builder"_a)
                       .def("get_or_create", bind_get_or_create_canonical<GroundFunctionTerm<AuxiliaryTag>>(), "builder"_a)
                       .def("get_or_create", bind_get_or_create_canonical<GroundFunctionTermValue<StaticTag>>(), "builder"_a)
                       .def("get_or_create", bind_get_or_create_canonical<GroundFunctionTermValue<FluentTag>>(), "builder"_a)
                       .def("get_or_create", bind_get_or_create_canonical<GroundFunctionTermValue<AuxiliaryTag>>(), "builder"_a)

                       .def("get_or_create", bind_get_or_create_canonical<UnaryOperator<OpSub, Data<FunctionExpression>>>(), "builder"_a)
                       .def("get_or_create", bind_get_or_create_canonical<BinaryOperator<OpAdd, Data<FunctionExpression>>>(), "builder"_a)
                       .def("get_or_create", bind_get_or_create_canonical<BinaryOperator<OpSub, Data<FunctionExpression>>>(), "builder"_a)
                       .def("get_or_create", bind_get_or_create_canonical<BinaryOperator<OpMul, Data<FunctionExpression>>>(), "builder"_a)
                       .def("get_or_create", bind_get_or_create_canonical<BinaryOperator<OpDiv, Data<FunctionExpression>>>(), "builder"_a)
                       .def("get_or_create", bind_get_or_create_canonical<BinaryOperator<OpEq, Data<FunctionExpression>>>(), "builder"_a)
                       .def("get_or_create", bind_get_or_create_canonical<BinaryOperator<OpNe, Data<FunctionExpression>>>(), "builder"_a)
                       .def("get_or_create", bind_get_or_create_canonical<BinaryOperator<OpGe, Data<FunctionExpression>>>(), "builder"_a)
                       .def("get_or_create", bind_get_or_create_canonical<BinaryOperator<OpGt, Data<FunctionExpression>>>(), "builder"_a)
                       .def("get_or_create", bind_get_or_create_canonical<BinaryOperator<OpLe, Data<FunctionExpression>>>(), "builder"_a)
                       .def("get_or_create", bind_get_or_create_canonical<BinaryOperator<OpLt, Data<FunctionExpression>>>(), "builder"_a)
                       .def("get_or_create", bind_get_or_create_canonical<MultiOperator<OpAdd, Data<FunctionExpression>>>(), "builder"_a)
                       .def("get_or_create", bind_get_or_create_canonical<MultiOperator<OpMul, Data<FunctionExpression>>>(), "builder"_a)

                       .def("get_or_create", bind_get_or_create_canonical<NumericEffect<OpAssign, FluentTag>>(), "builder"_a)
                       .def("get_or_create", bind_get_or_create_canonical<NumericEffect<OpIncrease, FluentTag>>(), "builder"_a)
                       .def("get_or_create", bind_get_or_create_canonical<NumericEffect<OpDecrease, FluentTag>>(), "builder"_a)
                       .def("get_or_create", bind_get_or_create_canonical<NumericEffect<OpScaleUp, FluentTag>>(), "builder"_a)
                       .def("get_or_create", bind_get_or_create_canonical<NumericEffect<OpScaleDown, FluentTag>>(), "builder"_a)
                       .def("get_or_create", bind_get_or_create_canonical<NumericEffect<OpIncrease, AuxiliaryTag>>(), "builder"_a)

                       .def("get_or_create", bind_get_or_create_canonical<ConjunctiveCondition>(), "builder"_a)
                       .def("get_or_create", bind_get_or_create_canonical<ConjunctiveEffect>(), "builder"_a)
                       .def("get_or_create", bind_get_or_create_canonical<ConditionalEffect>(), "builder"_a)
                       .def("get_or_create", bind_get_or_create_canonical<Action>(), "builder"_a)
                       .def("get_or_create", bind_get_or_create_canonical<Axiom>(), "builder"_a)

                       .def("get_or_create", bind_get_or_create_canonical<UnaryOperator<OpSub, Data<GroundFunctionExpression>>>(), "builder"_a)
                       .def("get_or_create", bind_get_or_create_canonical<BinaryOperator<OpAdd, Data<GroundFunctionExpression>>>(), "builder"_a)
                       .def("get_or_create", bind_get_or_create_canonical<BinaryOperator<OpSub, Data<GroundFunctionExpression>>>(), "builder"_a)
                       .def("get_or_create", bind_get_or_create_canonical<BinaryOperator<OpMul, Data<GroundFunctionExpression>>>(), "builder"_a)
                       .def("get_or_create", bind_get_or_create_canonical<BinaryOperator<OpDiv, Data<GroundFunctionExpression>>>(), "builder"_a)
                       .def("get_or_create", bind_get_or_create_canonical<BinaryOperator<OpEq, Data<GroundFunctionExpression>>>(), "builder"_a)
                       .def("get_or_create", bind_get_or_create_canonical<BinaryOperator<OpNe, Data<GroundFunctionExpression>>>(), "builder"_a)
                       .def("get_or_create", bind_get_or_create_canonical<BinaryOperator<OpGe, Data<GroundFunctionExpression>>>(), "builder"_a)
                       .def("get_or_create", bind_get_or_create_canonical<BinaryOperator<OpGt, Data<GroundFunctionExpression>>>(), "builder"_a)
                       .def("get_or_create", bind_get_or_create_canonical<BinaryOperator<OpLe, Data<GroundFunctionExpression>>>(), "builder"_a)
                       .def("get_or_create", bind_get_or_create_canonical<BinaryOperator<OpLt, Data<GroundFunctionExpression>>>(), "builder"_a)
                       .def("get_or_create", bind_get_or_create_canonical<MultiOperator<OpAdd, Data<GroundFunctionExpression>>>(), "builder"_a)
                       .def("get_or_create", bind_get_or_create_canonical<MultiOperator<OpMul, Data<GroundFunctionExpression>>>(), "builder"_a)

                       .def("get_or_create", bind_get_or_create_canonical<GroundNumericEffect<OpAssign, FluentTag>>(), "builder"_a)
                       .def("get_or_create", bind_get_or_create_canonical<GroundNumericEffect<OpIncrease, FluentTag>>(), "builder"_a)
                       .def("get_or_create", bind_get_or_create_canonical<GroundNumericEffect<OpDecrease, FluentTag>>(), "builder"_a)
                       .def("get_or_create", bind_get_or_create_canonical<GroundNumericEffect<OpScaleUp, FluentTag>>(), "builder"_a)
                       .def("get_or_create", bind_get_or_create_canonical<GroundNumericEffect<OpScaleDown, FluentTag>>(), "builder"_a)
                       .def("get_or_create", bind_get_or_create_canonical<GroundNumericEffect<OpIncrease, AuxiliaryTag>>(), "builder"_a)

                       .def("get_or_create", bind_get_or_create_canonical<GroundConjunctiveCondition>(), "builder"_a)
                       .def("get_or_create", bind_get_or_create_canonical<GroundConjunctiveEffect>(), "builder"_a)
                       .def("get_or_create", bind_get_or_create_canonical<GroundConditionalEffect>(), "builder"_a)
                       .def("get_or_create", bind_get_or_create_canonical<GroundAction>(), "builder"_a)
                       .def("get_or_create", bind_get_or_create_canonical<GroundAxiom>(), "builder"_a)

                       .def("get_or_create", bind_get_or_create_canonical<Metric>(), "builder"_a)
                       .def("get_or_create", bind_get_or_create_canonical<Domain>(), "builder"_a)
                       .def("get_or_create", bind_get_or_create_canonical<Task>(), "builder"_a)
                       .def("get_or_create", bind_get_or_create_canonical<FDRTask>(), "builder"_a);
    }

    /**
     * FDRContext
     */

    nb::class_<FDRContext>(m, "FDRContext")  //
        .def(nb::new_([](RepositoryPtr repository) { return std::make_shared<FDRContext>(std::move(repository)); }), "repository"_a)
        .def(nb::new_([](const std::vector<std::vector<GroundAtomView<FluentTag>>>& ground_mutex_groups, RepositoryPtr repository)
                      { return std::make_shared<FDRContext>(ground_mutex_groups, std::move(repository)); }),
             "ground_mutex_groups"_a,
             "repository"_a)
        .def("get_fact", nb::overload_cast<GroundAtomView<FluentTag>>(&FDRContext::get_fact_view), "atom"_a)
        .def("get_fact", nb::overload_cast<GroundLiteralView<FluentTag>>(&FDRContext::get_fact_view), "literal"_a);

    /**
     * PlanningDomain
     */

    {
        nb::class_<PlanningDomain>(m, "PlanningDomain")  //
            .def(nb::init<DomainView, RepositoryPtr, RepositoryFactoryPtr>(), "domain"_a, "repository"_a, "repository_factory"_a)
            .def("get_domain", &PlanningDomain::get_domain)
            .def("get_repository", &PlanningDomain::get_repository)
            .def("get_repository_factory", &PlanningDomain::get_repository_factory);
    }

    {
        nb::class_<PlanningTask>(m, "PlanningTask")  //
            .def(nb::new_([](TaskView task, FDRContextPtr fdr_context, RepositoryPtr repository, PlanningDomain planning_domain)
                          { return PlanningTask(task, std::move(fdr_context), std::move(repository), std::move(planning_domain)); }),
                 "task"_a,
                 "fdr_context"_a,
                 "repository"_a,
                 "planning_domain"_a)
            .def("get_task", &PlanningTask::get_task)
            .def("get_repository", &PlanningTask::get_repository)
            .def("get_fdr_context", &PlanningTask::get_fdr_context, nb::rv_policy::reference_internal)
            .def("get_domain", &PlanningTask::get_domain);
    }

    {
        nb::class_<PlanningFDRTask>(m, "PlanningFDRTask")  //
            .def("get_task", &PlanningFDRTask::get_task)
            .def("get_repository", &PlanningFDRTask::get_repository)
            .def("get_fdr_context", &PlanningFDRTask::get_fdr_context, nb::rv_policy::reference_internal)
            .def("get_domain", &PlanningFDRTask::get_domain);
    }
}

}