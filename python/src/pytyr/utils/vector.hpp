/*
    nanobind/stl/vector.h: type caster for std::vector<...>

    Copyright (c) 2022 Wenzel Jakob

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE file.
*/

#pragma once

#include <nanobind/nanobind.h>
#include <type_traits>
#include <tyr/common/vector.hpp>

NAMESPACE_BEGIN(NB_NAMESPACE)
NAMESPACE_BEGIN(detail)

// Adapted from nanobind/stl/detail/nb_list.h
template<typename Type, template<typename> typename Ptr, bool IndexPointers, typename TemplateSizeType, class Allocator, typename C>
struct type_caster<::tyr::View<::cista::basic_vector<Type, Ptr, IndexPointers, TemplateSizeType, Allocator>, C>>
{
    using ViewT = ::tyr::View<::cista::basic_vector<Type, Ptr, IndexPointers, TemplateSizeType, Allocator>, C>;

    using Entry = std::conditional_t<::tyr::ViewConcept<Type, C>, ::tyr::View<Type, C>, Type>;
    using Caster = make_caster<Entry>;

    NB_TYPE_CASTER(ViewT, io_name(NB_TYPING_SEQUENCE, NB_TYPING_LIST) + const_name("[") + Caster::Name + const_name("]"))

    // No Python -> C++ conversion (cannot build a View without backing storage + context)
    bool from_python(handle, uint8_t, cleanup_list*) noexcept { return false; }

    // Taken from nanobind/stl/detail/nb_list.h
    template<typename T>
    static handle from_cpp(T&& src, rv_policy policy, cleanup_list* cleanup)
    {
        object ret = steal(PyList_New(src.size()));

        if (ret.is_valid())
        {
            Py_ssize_t index = 0;

            for (auto&& value : src)
            {
                handle h = Caster::from_cpp(forward_like_<T>(value), policy, cleanup);

                if (!h.is_valid())
                {
                    ret.reset();
                    break;
                }

                NB_LIST_SET_ITEM(ret.ptr(), index++, h.ptr());
            }
        }

        return ret.release();
    }
};

NAMESPACE_END(detail)
NAMESPACE_END(NB_NAMESPACE)
