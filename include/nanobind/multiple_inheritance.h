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
using class_ = ::nanobind::class_<T, Ts...>;

NAMESPACE_END(mi)
NAMESPACE_END(NB_NAMESPACE)
