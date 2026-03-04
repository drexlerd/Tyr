/*
    nanobind/stl/string.h: type caster for std::string

    Copyright (c) 2022 Wenzel Jakob

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE file.
*/

#pragma once

#include <cista/containers/string.h>
#include <nanobind/nanobind.h>

NAMESPACE_BEGIN(NB_NAMESPACE)
NAMESPACE_BEGIN(detail)

// Taken From nanobind/stl/string.h
template<typename Ptr>
struct type_caster<cista::basic_string<Ptr>>
{
    using String = cista::basic_string<Ptr>;
    using CharT = typename String::CharT;

    // Minimal: only support cista strings over char
    static_assert(std::is_same_v<std::remove_cv_t<CharT>, char>, "nanobind caster: only CharT=char is supported");

    NB_TYPE_CASTER(String, const_name("str"))

    bool from_python(handle src, uint8_t, cleanup_list*) noexcept
    {
        Py_ssize_t size = 0;
        const char* str = PyUnicode_AsUTF8AndSize(src.ptr(), &size);
        if (!str)
        {
            PyErr_Clear();
            return false;
        }
        value = String(std::string_view(str, (size_t) size));  // owning copy
        return true;
    }

    static handle from_cpp(const String& s, rv_policy, cleanup_list*) noexcept { return PyUnicode_FromStringAndSize(s.data(), (Py_ssize_t) s.size()); }
};

NAMESPACE_END(detail)
NAMESPACE_END(NB_NAMESPACE)
