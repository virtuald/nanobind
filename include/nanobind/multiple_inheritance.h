/*
    nanobind/multiple_inheritance.h: Optional multiple inheritance scaffolding

    Copyright (c) 2026 Wenzel Jakob

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE file.
*/

#pragma once

#include <nanobind/nanobind.h>

#include <typeinfo>

NAMESPACE_BEGIN(NB_NAMESPACE)

namespace detail {

template <typename... Bases> struct mi_extra_bases { };

template <typename... Bases>
NB_INLINE void type_extra_apply(type_init_data &t, mi_extra_bases<Bases...>) {
    if constexpr (sizeof...(Bases) > 0) {
        static const std::type_info *extra_bases[] = { &typeid(Bases)... };
        t.flags |= (uint32_t) type_init_flags::has_extra_bases;
        t.extra_bases = extra_bases;
        t.extra_bases_count = sizeof...(Bases);
    }
}

} // namespace detail

namespace mi {

template <typename... Bases> struct bases { };

template <typename T, typename B1, typename... Bn>
class class_ : public nanobind::class_<T, B1> {
    using Base = nanobind::class_<T, B1>;

public:
    template <typename... Extra>
    NB_INLINE class_(handle scope, const char *name, const Extra &... extra)
        : Base(scope, name, detail::mi_extra_bases<Bn...>{}, extra...) { }
};

} // namespace mi

NAMESPACE_END(NB_NAMESPACE)
