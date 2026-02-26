/*
    nanobind/multiple_inheritance.h: opt-in MI shim scaffold

    Copyright (c) 2022 Wenzel Jakob

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE file.
*/

#pragma once

#include <nanobind/nanobind.h>

NAMESPACE_BEGIN(NB_NAMESPACE)
NAMESPACE_BEGIN(mi)

template <typename... Ts> struct bases { };

template <typename Src, typename Dst>
NB_INLINE void register_cast() {
    using SrcT = std::remove_cv_t<std::remove_reference_t<Src>>;
    using DstT = std::remove_cv_t<std::remove_reference_t<Dst>>;

    detail::nb_type_register_cast(
        &typeid(SrcT), &typeid(DstT),
        [](void *ptr) noexcept -> void * {
            return static_cast<DstT *>(static_cast<SrcT *>(ptr));
        });
}

template <typename T, typename... Ts>
class class_ : public ::nanobind::class_<T, Ts...> {
    using Base = ::nanobind::class_<T, Ts...>;

public:
    using Base::Base;
};

template <typename T, typename... Bases, typename... Ts>
class class_<T, bases<Bases...>, Ts...> : public ::nanobind::class_<T, Ts...> {
    using Base = ::nanobind::class_<T, Ts...>;

    static NB_INLINE tuple make_bases_tuple() {
        constexpr size_t size = sizeof...(Bases);
        static_assert(size > 0, "nanobind::mi::bases<> requires at least one base type.");

        tuple result = steal<tuple>(PyTuple_New((Py_ssize_t) size));
        if (!result.is_valid())
            detail::raise_python_error();

        size_t index = 0;
        auto append = [&](handle h) {
            if (!h.is_valid())
                detail::raise("nanobind::mi::class_: attempted to reference an unbound base type!");
            Py_INCREF(h.ptr());
            NB_TUPLE_SET_ITEM(result.ptr(), (Py_ssize_t) index++, h.ptr());
        };

        (append(type<Bases>()), ...);
        return result;
    }

public:
    template <typename... Extra>
    NB_INLINE class_(handle scope, const char *name, const Extra &... extra)
        : Base(scope, name,
               detail::type_bases_py((object) make_bases_tuple()),
               extra...) {
        (register_cast<T, Bases>(), ...);
    }
};

NAMESPACE_END(mi)
NAMESPACE_END(NB_NAMESPACE)
