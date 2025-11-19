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

#ifndef TYR_FORMALISM_TERM_PROXY_HPP_
#define TYR_FORMALISM_TERM_PROXY_HPP_

#include "tyr/common/variant.hpp"
#include "tyr/formalism/repository.hpp"
#include "tyr/formalism/term.hpp"

namespace tyr::formalism
{
class TermProxy
{
private:
    const Repository* m_repo;
    Term m_term;

public:
    TermProxy(const Repository& repo, Term term) : m_repo(&repo), m_term(term) {}

    const auto& index_variant() const noexcept { return m_term.value; }
    const auto& context() const noexcept { return *m_repo; }

    template<typename F>
    decltype(auto) visit(F&& f) const
    {
        return std::visit(
            [&](auto&& index) -> decltype(auto)
            {
                using Index = std::decay_t<decltype(index)>;

                if constexpr (HasProxyType<Index>)
                {
                    using Proxy = typename Index::ProxyType;
                    return std::forward<F>(f)(Proxy(context(), index));
                }
                else
                {
                    return std::forward<F>(f)(index);
                }
            },
            index_variant());
    }
};
}

#endif
