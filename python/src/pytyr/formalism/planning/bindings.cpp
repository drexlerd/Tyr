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

    nb::class_<Repository>(m, "Repository")  //
        .def("get_or_create", nb::overload_cast<Data<Object>&>(&Repository::get_or_create<Object>), "builder"_a)
        .def("get_or_create", nb::overload_cast<Data<Variable>&>(&Repository::get_or_create<Variable>), "builder"_a)
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
        .def("get_or_create", nb::overload_cast<Data<Predicate<StaticTag>>&>(&Repository::get_or_create<Predicate<StaticTag>>), "builder"_a)
        .def("get_or_create", nb::overload_cast<Data<Predicate<FluentTag>>&>(&Repository::get_or_create<Predicate<FluentTag>>), "builder"_a)
        .def("get_or_create", nb::overload_cast<Data<Predicate<DerivedTag>>&>(&Repository::get_or_create<Predicate<DerivedTag>>), "builder"_a)
        .def("get_or_create", nb::overload_cast<Data<Atom<StaticTag>>&>(&Repository::get_or_create<Atom<StaticTag>>), "builder"_a)
        .def("get_or_create", nb::overload_cast<Data<Atom<FluentTag>>&>(&Repository::get_or_create<Atom<FluentTag>>), "builder"_a)
        .def("get_or_create", nb::overload_cast<Data<Atom<DerivedTag>>&>(&Repository::get_or_create<Atom<DerivedTag>>), "builder"_a)
        .def("get_or_create", nb::overload_cast<Data<GroundAtom<StaticTag>>&>(&Repository::get_or_create<GroundAtom<StaticTag>>), "builder"_a)
        .def("get_or_create", nb::overload_cast<Data<GroundAtom<FluentTag>>&>(&Repository::get_or_create<GroundAtom<FluentTag>>), "builder"_a)
        .def("get_or_create", nb::overload_cast<Data<GroundAtom<DerivedTag>>&>(&Repository::get_or_create<GroundAtom<DerivedTag>>), "builder"_a)
        .def("get_or_create", nb::overload_cast<Data<Literal<StaticTag>>&>(&Repository::get_or_create<Literal<StaticTag>>), "builder"_a)
        .def("get_or_create", nb::overload_cast<Data<Literal<FluentTag>>&>(&Repository::get_or_create<Literal<FluentTag>>), "builder"_a)
        .def("get_or_create", nb::overload_cast<Data<Literal<DerivedTag>>&>(&Repository::get_or_create<Literal<DerivedTag>>), "builder"_a)
        .def("get_or_create", nb::overload_cast<Data<GroundLiteral<StaticTag>>&>(&Repository::get_or_create<GroundLiteral<StaticTag>>), "builder"_a)
        .def("get_or_create", nb::overload_cast<Data<GroundLiteral<FluentTag>>&>(&Repository::get_or_create<GroundLiteral<FluentTag>>), "builder"_a)
        .def("get_or_create", nb::overload_cast<Data<GroundLiteral<DerivedTag>>&>(&Repository::get_or_create<GroundLiteral<DerivedTag>>), "builder"_a)
        .def("get_or_create", nb::overload_cast<Data<FDRVariable<FluentTag>>&>(&Repository::get_or_create<FDRVariable<FluentTag>>), "builder"_a)
        .def("get_or_create", nb::overload_cast<Data<Function<StaticTag>>&>(&Repository::get_or_create<Function<StaticTag>>), "builder"_a)
        .def("get_or_create", nb::overload_cast<Data<Function<FluentTag>>&>(&Repository::get_or_create<Function<FluentTag>>), "builder"_a)
        .def("get_or_create", nb::overload_cast<Data<Function<AuxiliaryTag>>&>(&Repository::get_or_create<Function<AuxiliaryTag>>), "builder"_a)
        .def("get_or_create", nb::overload_cast<Data<FunctionTerm<StaticTag>>&>(&Repository::get_or_create<FunctionTerm<StaticTag>>), "builder"_a)
        .def("get_or_create", nb::overload_cast<Data<FunctionTerm<FluentTag>>&>(&Repository::get_or_create<FunctionTerm<FluentTag>>), "builder"_a)
        .def("get_or_create", nb::overload_cast<Data<FunctionTerm<AuxiliaryTag>>&>(&Repository::get_or_create<FunctionTerm<AuxiliaryTag>>), "builder"_a)
        .def("get_or_create", nb::overload_cast<Data<GroundFunctionTerm<StaticTag>>&>(&Repository::get_or_create<GroundFunctionTerm<StaticTag>>), "builder"_a)
        .def("get_or_create", nb::overload_cast<Data<GroundFunctionTerm<FluentTag>>&>(&Repository::get_or_create<GroundFunctionTerm<FluentTag>>), "builder"_a)
        .def("get_or_create",
             nb::overload_cast<Data<GroundFunctionTerm<AuxiliaryTag>>&>(&Repository::get_or_create<GroundFunctionTerm<AuxiliaryTag>>),
             "builder"_a)
        .def("get_or_create",
             nb::overload_cast<Data<GroundFunctionTermValue<StaticTag>>&>(&Repository::get_or_create<GroundFunctionTermValue<StaticTag>>),
             "builder"_a)
        .def("get_or_create",
             nb::overload_cast<Data<GroundFunctionTermValue<FluentTag>>&>(&Repository::get_or_create<GroundFunctionTermValue<FluentTag>>),
             "builder"_a)
        .def("get_or_create",
             nb::overload_cast<Data<GroundFunctionTermValue<AuxiliaryTag>>&>(&Repository::get_or_create<GroundFunctionTermValue<AuxiliaryTag>>),
             "builder"_a)

        .def("get_or_create",
             nb::overload_cast<Data<UnaryOperator<OpSub, Data<FunctionExpression>>>&>(
                 &Repository::get_or_create<UnaryOperator<OpSub, Data<FunctionExpression>>>),
             "builder"_a)
        .def("get_or_create",
             nb::overload_cast<Data<BinaryOperator<OpAdd, Data<FunctionExpression>>>&>(
                 &Repository::get_or_create<BinaryOperator<OpAdd, Data<FunctionExpression>>>),
             "builder"_a)
        .def("get_or_create",
             nb::overload_cast<Data<BinaryOperator<OpSub, Data<FunctionExpression>>>&>(
                 &Repository::get_or_create<BinaryOperator<OpSub, Data<FunctionExpression>>>),
             "builder"_a)
        .def("get_or_create",
             nb::overload_cast<Data<BinaryOperator<OpMul, Data<FunctionExpression>>>&>(
                 &Repository::get_or_create<BinaryOperator<OpMul, Data<FunctionExpression>>>),
             "builder"_a)
        .def("get_or_create",
             nb::overload_cast<Data<BinaryOperator<OpDiv, Data<FunctionExpression>>>&>(
                 &Repository::get_or_create<BinaryOperator<OpDiv, Data<FunctionExpression>>>),
             "builder"_a)
        .def("get_or_create",
             nb::overload_cast<Data<BinaryOperator<OpEq, Data<FunctionExpression>>>&>(
                 &Repository::get_or_create<BinaryOperator<OpEq, Data<FunctionExpression>>>),
             "builder"_a)
        .def("get_or_create",
             nb::overload_cast<Data<BinaryOperator<OpNe, Data<FunctionExpression>>>&>(
                 &Repository::get_or_create<BinaryOperator<OpNe, Data<FunctionExpression>>>),
             "builder"_a)
        .def("get_or_create",
             nb::overload_cast<Data<BinaryOperator<OpGe, Data<FunctionExpression>>>&>(
                 &Repository::get_or_create<BinaryOperator<OpGe, Data<FunctionExpression>>>),
             "builder"_a)
        .def("get_or_create",
             nb::overload_cast<Data<BinaryOperator<OpGt, Data<FunctionExpression>>>&>(
                 &Repository::get_or_create<BinaryOperator<OpGt, Data<FunctionExpression>>>),
             "builder"_a)
        .def("get_or_create",
             nb::overload_cast<Data<BinaryOperator<OpLe, Data<FunctionExpression>>>&>(
                 &Repository::get_or_create<BinaryOperator<OpLe, Data<FunctionExpression>>>),
             "builder"_a)
        .def("get_or_create",
             nb::overload_cast<Data<BinaryOperator<OpLt, Data<FunctionExpression>>>&>(
                 &Repository::get_or_create<BinaryOperator<OpLt, Data<FunctionExpression>>>),
             "builder"_a)
        .def("get_or_create",
             nb::overload_cast<Data<MultiOperator<OpAdd, Data<FunctionExpression>>>&>(
                 &Repository::get_or_create<MultiOperator<OpAdd, Data<FunctionExpression>>>),
             "builder"_a)
        .def("get_or_create",
             nb::overload_cast<Data<MultiOperator<OpMul, Data<FunctionExpression>>>&>(
                 &Repository::get_or_create<MultiOperator<OpMul, Data<FunctionExpression>>>),
             "builder"_a)
        .def("get_or_create", nb::overload_cast<Data<ConjunctiveCondition>&>(&Repository::get_or_create<ConjunctiveCondition>), "builder"_a)
        .def("get_or_create",
             nb::overload_cast<Data<NumericEffect<OpAssign, FluentTag>>&>(&Repository::get_or_create<NumericEffect<OpAssign, FluentTag>>),
             "builder"_a)
        .def("get_or_create",
             nb::overload_cast<Data<NumericEffect<OpIncrease, FluentTag>>&>(&Repository::get_or_create<NumericEffect<OpIncrease, FluentTag>>),
             "builder"_a)
        .def("get_or_create",
             nb::overload_cast<Data<NumericEffect<OpDecrease, FluentTag>>&>(&Repository::get_or_create<NumericEffect<OpDecrease, FluentTag>>),
             "builder"_a)
        .def("get_or_create",
             nb::overload_cast<Data<NumericEffect<OpScaleUp, FluentTag>>&>(&Repository::get_or_create<NumericEffect<OpScaleUp, FluentTag>>),
             "builder"_a)
        .def("get_or_create",
             nb::overload_cast<Data<NumericEffect<OpScaleDown, FluentTag>>&>(&Repository::get_or_create<NumericEffect<OpScaleDown, FluentTag>>),
             "builder"_a)
        .def("get_or_create",
             nb::overload_cast<Data<NumericEffect<OpIncrease, AuxiliaryTag>>&>(&Repository::get_or_create<NumericEffect<OpIncrease, AuxiliaryTag>>),
             "builder"_a)
        .def("get_or_create", nb::overload_cast<Data<ConjunctiveEffect>&>(&Repository::get_or_create<ConjunctiveEffect>), "builder"_a)
        .def("get_or_create", nb::overload_cast<Data<ConditionalEffect>&>(&Repository::get_or_create<ConditionalEffect>), "builder"_a)
        .def("get_or_create", nb::overload_cast<Data<Action>&>(&Repository::get_or_create<Action>), "builder"_a)
        .def("get_or_create", nb::overload_cast<Data<Axiom>&>(&Repository::get_or_create<Axiom>), "builder"_a)

        .def("get_or_create",
             nb::overload_cast<Data<UnaryOperator<OpSub, Data<GroundFunctionExpression>>>&>(
                 &Repository::get_or_create<UnaryOperator<OpSub, Data<GroundFunctionExpression>>>),
             "builder"_a)
        .def("get_or_create",
             nb::overload_cast<Data<BinaryOperator<OpAdd, Data<GroundFunctionExpression>>>&>(
                 &Repository::get_or_create<BinaryOperator<OpAdd, Data<GroundFunctionExpression>>>),
             "builder"_a)
        .def("get_or_create",
             nb::overload_cast<Data<BinaryOperator<OpSub, Data<GroundFunctionExpression>>>&>(
                 &Repository::get_or_create<BinaryOperator<OpSub, Data<GroundFunctionExpression>>>),
             "builder"_a)
        .def("get_or_create",
             nb::overload_cast<Data<BinaryOperator<OpMul, Data<GroundFunctionExpression>>>&>(
                 &Repository::get_or_create<BinaryOperator<OpMul, Data<GroundFunctionExpression>>>),
             "builder"_a)
        .def("get_or_create",
             nb::overload_cast<Data<BinaryOperator<OpDiv, Data<GroundFunctionExpression>>>&>(
                 &Repository::get_or_create<BinaryOperator<OpDiv, Data<GroundFunctionExpression>>>),
             "builder"_a)
        .def("get_or_create",
             nb::overload_cast<Data<BinaryOperator<OpEq, Data<GroundFunctionExpression>>>&>(
                 &Repository::get_or_create<BinaryOperator<OpEq, Data<GroundFunctionExpression>>>),
             "builder"_a)
        .def("get_or_create",
             nb::overload_cast<Data<BinaryOperator<OpNe, Data<GroundFunctionExpression>>>&>(
                 &Repository::get_or_create<BinaryOperator<OpNe, Data<GroundFunctionExpression>>>),
             "builder"_a)
        .def("get_or_create",
             nb::overload_cast<Data<BinaryOperator<OpGe, Data<GroundFunctionExpression>>>&>(
                 &Repository::get_or_create<BinaryOperator<OpGe, Data<GroundFunctionExpression>>>),
             "builder"_a)
        .def("get_or_create",
             nb::overload_cast<Data<BinaryOperator<OpGt, Data<GroundFunctionExpression>>>&>(
                 &Repository::get_or_create<BinaryOperator<OpGt, Data<GroundFunctionExpression>>>),
             "builder"_a)
        .def("get_or_create",
             nb::overload_cast<Data<BinaryOperator<OpLe, Data<GroundFunctionExpression>>>&>(
                 &Repository::get_or_create<BinaryOperator<OpLe, Data<GroundFunctionExpression>>>),
             "builder"_a)
        .def("get_or_create",
             nb::overload_cast<Data<BinaryOperator<OpLt, Data<GroundFunctionExpression>>>&>(
                 &Repository::get_or_create<BinaryOperator<OpLt, Data<GroundFunctionExpression>>>),
             "builder"_a)
        .def("get_or_create",
             nb::overload_cast<Data<MultiOperator<OpAdd, Data<GroundFunctionExpression>>>&>(
                 &Repository::get_or_create<MultiOperator<OpAdd, Data<GroundFunctionExpression>>>),
             "builder"_a)
        .def("get_or_create",
             nb::overload_cast<Data<MultiOperator<OpMul, Data<GroundFunctionExpression>>>&>(
                 &Repository::get_or_create<MultiOperator<OpMul, Data<GroundFunctionExpression>>>),
             "builder"_a)
        .def("get_or_create", nb::overload_cast<Data<GroundConjunctiveCondition>&>(&Repository::get_or_create<GroundConjunctiveCondition>), "builder"_a)
        .def("get_or_create",
             nb::overload_cast<Data<GroundNumericEffect<OpAssign, FluentTag>>&>(&Repository::get_or_create<GroundNumericEffect<OpAssign, FluentTag>>),
             "builder"_a)
        .def("get_or_create",
             nb::overload_cast<Data<GroundNumericEffect<OpIncrease, FluentTag>>&>(&Repository::get_or_create<GroundNumericEffect<OpIncrease, FluentTag>>),
             "builder"_a)
        .def("get_or_create",
             nb::overload_cast<Data<GroundNumericEffect<OpDecrease, FluentTag>>&>(&Repository::get_or_create<GroundNumericEffect<OpDecrease, FluentTag>>),
             "builder"_a)
        .def("get_or_create",
             nb::overload_cast<Data<GroundNumericEffect<OpScaleUp, FluentTag>>&>(&Repository::get_or_create<GroundNumericEffect<OpScaleUp, FluentTag>>),
             "builder"_a)
        .def("get_or_create",
             nb::overload_cast<Data<GroundNumericEffect<OpScaleDown, FluentTag>>&>(&Repository::get_or_create<GroundNumericEffect<OpScaleDown, FluentTag>>),
             "builder"_a)
        .def("get_or_create",
             nb::overload_cast<Data<GroundNumericEffect<OpIncrease, AuxiliaryTag>>&>(&Repository::get_or_create<GroundNumericEffect<OpIncrease, AuxiliaryTag>>),
             "builder"_a)
        .def("get_or_create", nb::overload_cast<Data<GroundConjunctiveEffect>&>(&Repository::get_or_create<GroundConjunctiveEffect>), "builder"_a)
        .def("get_or_create", nb::overload_cast<Data<GroundConditionalEffect>&>(&Repository::get_or_create<GroundConditionalEffect>), "builder"_a)
        .def("get_or_create", nb::overload_cast<Data<GroundAction>&>(&Repository::get_or_create<GroundAction>), "builder"_a)
        .def("get_or_create", nb::overload_cast<Data<GroundAxiom>&>(&Repository::get_or_create<GroundAxiom>), "builder"_a)

        .def("get_or_create", nb::overload_cast<Data<Metric>&>(&Repository::get_or_create<Metric>), "builder"_a)
        .def("get_or_create", nb::overload_cast<Data<Domain>&>(&Repository::get_or_create<Domain>), "builder"_a)
        .def("get_or_create", nb::overload_cast<Data<Task>&>(&Repository::get_or_create<Task>), "builder"_a)
        .def("get_or_create", nb::overload_cast<Data<FDRTask>&>(&Repository::get_or_create<FDRTask>), "builder"_a);

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