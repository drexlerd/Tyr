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

#ifndef TYR_FORMALISM_FORMATTER_HPP_
#define TYR_FORMALISM_FORMATTER_HPP_

#include "tyr/common/formatter.hpp"
#include "tyr/common/iostream.hpp"
#include "tyr/formalism/datas.hpp"
#include "tyr/formalism/views.hpp"

#include <fmt/core.h>
#include <fmt/ostream.h>
#include <fmt/ranges.h>
#include <ostream>

namespace tyr
{
/**
 * Forward declarations
 */
namespace formalism
{
inline std::ostream& operator<<(std::ostream& os, const ParameterIndex& el);

inline std::ostream& operator<<(std::ostream& os, OpEq el);

inline std::ostream& operator<<(std::ostream& os, OpNe el);

inline std::ostream& operator<<(std::ostream& os, OpLe el);

inline std::ostream& operator<<(std::ostream& os, OpLt el);

inline std::ostream& operator<<(std::ostream& os, OpGe el);

inline std::ostream& operator<<(std::ostream& os, OpGt el);

inline std::ostream& operator<<(std::ostream& os, OpAdd el);

inline std::ostream& operator<<(std::ostream& os, OpSub el);

inline std::ostream& operator<<(std::ostream& os, OpMul el);

inline std::ostream& operator<<(std::ostream& os, OpDiv el);

inline std::ostream& operator<<(std::ostream& os, const Data<Variable>& el);

template<Context C>
inline std::ostream& operator<<(std::ostream& os, const View<Index<Variable>, C>& el);

inline std::ostream& operator<<(std::ostream& os, const Data<Object>& el);

inline std::ostream& operator<<(std::ostream& os, const Data<formalism::Variable>& el);

template<formalism::Context C>
inline std::ostream& operator<<(std::ostream& os, const View<Index<formalism::Variable>, C>& el);

template<Context C>
inline std::ostream& operator<<(std::ostream& os, const View<Index<Object>, C>& el);

inline std::ostream& operator<<(std::ostream& os, const Data<Binding>& el);

template<formalism::Context C>
inline std::ostream& operator<<(std::ostream& os, const View<Index<Binding>, C>& el);

inline std::ostream& operator<<(std::ostream& os, const Data<Term>& el);

template<Context C>
inline std::ostream& operator<<(std::ostream& os, const View<Data<Term>, C>& el);

template<FactKind T>
inline std::ostream& operator<<(std::ostream& os, const Data<Predicate<T>>& el);

template<FactKind T, Context C>
inline std::ostream& operator<<(std::ostream& os, const View<Index<Predicate<T>>, C>& el);

template<FactKind T>
inline std::ostream& operator<<(std::ostream& os, const Data<Function<T>>& el);

template<FactKind T, Context C>
inline std::ostream& operator<<(std::ostream& os, const View<Index<Function<T>>, C>& el);

}  // end namespace formalism

/**
 * Definitions
 */

inline std::ostream& print(std::ostream& os, const formalism::ParameterIndex& el)
{
    fmt::print(os, "V{}", uint_t(el));
    return os;
}

inline std::ostream& print(std::ostream& os, formalism::OpEq el)
{
    fmt::print(os, "=");
    return os;
}

inline std::ostream& print(std::ostream& os, formalism::OpNe el)
{
    fmt::print(os, "!=");
    return os;
}

inline std::ostream& print(std::ostream& os, formalism::OpLe el)
{
    fmt::print(os, "<=");
    return os;
}

inline std::ostream& print(std::ostream& os, formalism::OpLt el)
{
    fmt::print(os, "<");
    return os;
}

inline std::ostream& print(std::ostream& os, formalism::OpGe el)
{
    fmt::print(os, ">=");
    return os;
}

inline std::ostream& print(std::ostream& os, formalism::OpGt el)
{
    fmt::print(os, ">");
    return os;
}

inline std::ostream& print(std::ostream& os, formalism::OpAdd el)
{
    fmt::print(os, "+");
    return os;
}

inline std::ostream& print(std::ostream& os, formalism::OpSub el)
{
    fmt::print(os, "-");
    return os;
}

inline std::ostream& print(std::ostream& os, formalism::OpMul el)
{
    fmt::print(os, "*");
    return os;
}

inline std::ostream& print(std::ostream& os, formalism::OpDiv el)
{
    fmt::print(os, "/");
    return os;
}

inline std::ostream& print(std::ostream& os, const Data<formalism::Variable>& el)
{
    fmt::print(os, "{}", to_string(el.name));
    return os;
}

template<formalism::Context C>
inline std::ostream& print(std::ostream& os, const View<Index<formalism::Variable>, C>& el)
{
    fmt::print(os, "{}", to_string(el.get_name()));
    return os;
}

inline std::ostream& print(std::ostream& os, const Data<formalism::Object>& el)
{
    fmt::print(os, "{}", to_string(el.name));
    return os;
}

template<formalism::Context C>
inline std::ostream& print(std::ostream& os, const View<Index<formalism::Object>, C>& el)
{
    fmt::print(os, "{}", to_string(el.get_name()));
    return os;
}

inline std::ostream& print(std::ostream& os, const Data<formalism::Binding>& el)
{
    fmt::print(os, "{}", fmt::join(to_strings(el.objects), " "));
    return os;
}

template<formalism::Context C>
inline std::ostream& print(std::ostream& os, const View<Index<formalism::Binding>, C>& el)
{
    fmt::print(os, "{}", fmt::join(to_strings(el.get_objects()), " "));
    return os;
}

inline std::ostream& print(std::ostream& os, const Data<formalism::Term>& el)
{
    fmt::print(os, "{}", to_string(el.value));
    return os;
}

template<formalism::Context C>
inline std::ostream& print(std::ostream& os, const View<Data<formalism::Term>, C>& el)
{
    fmt::print(os, "{}", to_string(el.get_variant()));
    return os;
}

template<formalism::FactKind T>
inline std::ostream& print(std::ostream& os, const Data<formalism::Predicate<T>>& el)
{
    fmt::print(os, "{}/{}", to_string(el.name), to_string(el.arity));
    return os;
}

template<formalism::FactKind T, formalism::Context C>
inline std::ostream& print(std::ostream& os, const View<Index<formalism::Predicate<T>>, C>& el)
{
    fmt::print(os, "{}/{}", to_string(el.get_name()), to_string(el.get_arity()));
    return os;
}

template<formalism::FactKind T>
inline std::ostream& print(std::ostream& os, const Data<formalism::Function<T>>& el)
{
    fmt::print(os, "{}/{}", to_string(el.name), to_string(el.arity));
    return os;
}

template<formalism::FactKind T, formalism::Context C>
inline std::ostream& print(std::ostream& os, const View<Index<formalism::Function<T>>, C>& el)
{
    fmt::print(os, "{}/{}", to_string(el.get_name()), to_string(el.get_arity()));
    return os;
}

namespace formalism
{
inline std::ostream& operator<<(std::ostream& os, const ParameterIndex& el) { return tyr::print(os, el); }

inline std::ostream& operator<<(std::ostream& os, OpEq el) { return tyr::print(os, el); }

inline std::ostream& operator<<(std::ostream& os, OpNe el) { return tyr::print(os, el); }

inline std::ostream& operator<<(std::ostream& os, OpLe el) { return tyr::print(os, el); }

inline std::ostream& operator<<(std::ostream& os, OpLt el) { return tyr::print(os, el); }

inline std::ostream& operator<<(std::ostream& os, OpGe el) { return tyr::print(os, el); }

inline std::ostream& operator<<(std::ostream& os, OpGt el) { return tyr::print(os, el); }

inline std::ostream& operator<<(std::ostream& os, OpAdd el) { return tyr::print(os, el); }

inline std::ostream& operator<<(std::ostream& os, OpSub el) { return tyr::print(os, el); }

inline std::ostream& operator<<(std::ostream& os, OpMul el) { return tyr::print(os, el); }

inline std::ostream& operator<<(std::ostream& os, OpDiv el) { return tyr::print(os, el); }

inline std::ostream& operator<<(std::ostream& os, const Data<Variable>& el) { return tyr::print(os, el); }

template<Context C>
inline std::ostream& operator<<(std::ostream& os, const View<Index<Variable>, C>& el)
{
    return tyr::print(os, el);
}

inline std::ostream& operator<<(std::ostream& os, const Data<Object>& el) { return tyr::print(os, el); }

template<Context C>
inline std::ostream& operator<<(std::ostream& os, const View<Index<Object>, C>& el)
{
    return tyr::print(os, el);
}

inline std::ostream& operator<<(std::ostream& os, const Data<Binding>& el) { return tyr::print(os, el); }

template<formalism::Context C>
inline std::ostream& operator<<(std::ostream& os, const View<Index<Binding>, C>& el)
{
    return tyr::print(os, el);
}

inline std::ostream& operator<<(std::ostream& os, const Data<Term>& el) { return tyr::print(os, el); }

template<Context C>
inline std::ostream& operator<<(std::ostream& os, const View<Data<Term>, C>& el)
{
    return tyr::print(os, el);
}

template<FactKind T>
inline std::ostream& operator<<(std::ostream& os, const Data<Predicate<T>>& el)
{
    return tyr::print(os, el);
}

template<FactKind T, Context C>
inline std::ostream& operator<<(std::ostream& os, const View<Index<Predicate<T>>, C>& el)
{
    return tyr::print(os, el);
}

template<FactKind T>
inline std::ostream& operator<<(std::ostream& os, const Data<Function<T>>& el)
{
    return tyr::print(os, el);
}

template<FactKind T, Context C>
inline std::ostream& operator<<(std::ostream& os, const View<Index<Function<T>>, C>& el)
{
    return tyr::print(os, el);
}

}
}
#endif
