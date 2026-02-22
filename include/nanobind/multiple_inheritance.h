/*
    nanobind/multiple_inheritance.h: Optional multiple inheritance scaffolding

    Copyright (c) 2026 Wenzel Jakob

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE file.
*/

#pragma once

#include <nanobind/nanobind.h>

#include <type_traits>
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

template <typename Derived, typename Base>
NB_INLINE void *mi_cast_derived_to_base(void *ptr) {
    return (void *) static_cast<Base *>((Derived *) ptr);
}

template <typename Derived, typename Base>
NB_INLINE void *mi_cast_base_to_derived(void *ptr) {
    return (void *) dynamic_cast<Derived *>((Base *) ptr);
}

template <typename Derived, typename Base>
NB_INLINE void mi_register_cast_pair() {
    nb_type_register_cast(&typeid(Derived), &typeid(Base),
                          &mi_cast_derived_to_base<Derived, Base>);
    nb_type_register_cast(&typeid(Base), &typeid(Derived),
                          &mi_cast_base_to_derived<Derived, Base>);
}

template <typename Derived, typename... Bases>
NB_INLINE void mi_register_cast_pairs() {
    (mi_register_cast_pair<Derived, Bases>(), ...);
}

} // namespace detail

namespace mi {

/// Opt-in MI API (v1): include <nanobind/multiple_inheritance.h> and use
/// nb::mi::class_. The default nb::class_ path is unchanged.
template <typename... Bases> struct bases { };

template <typename T, typename B1, typename... Bn>
class class_ : public nanobind::class_<T, B1> {
    using Base = nanobind::class_<T, B1>;

public:
    template <typename... Extra>
    NB_INLINE class_(handle scope, const char *name, const Extra &... extra)
        : Base(scope, name, detail::mi_extra_bases<Bn...>{}, extra...) {
#if !defined(__cpp_rtti) && !defined(__GXX_RTTI) && !defined(_CPPRTTI)
        static_assert(false,
                      "nanobind::mi::class_ requires RTTI (dynamic_cast/typeid).");
#endif
        static_assert(std::is_polymorphic_v<T>,
                      "nanobind::mi::class_: T must be polymorphic in v1 "
                      "(e.g., have a virtual destructor).");
        static_assert((std::is_polymorphic_v<B1> && ... && std::is_polymorphic_v<Bn>),
                      "nanobind::mi::class_: all MI bases must be polymorphic in v1 "
                      "(required for Base*->Derived* dynamic_cast).");
        detail::mi_register_cast_pairs<T, B1, Bn...>();
    }
};

} // namespace mi

NAMESPACE_END(NB_NAMESPACE)
