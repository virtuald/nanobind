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

template <typename Src, typename Dst, typename = void>
struct is_static_ptr_castable : std::false_type { };

template <typename Src, typename Dst>
struct is_static_ptr_castable<
    Src, Dst,
    std::void_t<decltype(static_cast<Dst *>((Src *) nullptr))>> : std::true_type { };

template <typename Src, typename Dst>
NB_INLINE void register_cast() {
    using SrcT = std::remove_cv_t<std::remove_reference_t<Src>>;
    using DstT = std::remove_cv_t<std::remove_reference_t<Dst>>;

    detail::nb_type_register_cast(
        &typeid(SrcT), &typeid(DstT),
        [](void *ptr) noexcept -> void * {
            SrcT *src = static_cast<SrcT *>(ptr);

            if constexpr (is_static_ptr_castable<SrcT, DstT>::value) {
                return static_cast<DstT *>(src);
            } else if constexpr (std::is_polymorphic_v<SrcT>) {
                return dynamic_cast<DstT *>(src);
            } else {
                return nullptr;
            }
        });
}

template <typename Derived, typename Base>
NB_INLINE void register_edge_casts() {
    register_cast<Derived, Base>();
    register_cast<Base, Derived>();
}

namespace mi_detail {

template <typename T, typename... Ts>
constexpr size_t base_count_v = (0 + ... + (std::is_base_of_v<Ts, T> ? 1u : 0u));

template <typename T, typename... Ts>
constexpr size_t alias_count_v = (0 + ... + (std::is_base_of_v<T, Ts> ? 1u : 0u));

template <typename T, typename Alias>
struct class_with_optional_alias {
    using type = ::nanobind::class_<T, Alias>;
};

template <typename T>
struct class_with_optional_alias<T, T> {
    using type = ::nanobind::class_<T>;
};

template <typename T, bool Multi, typename... Ts>
struct class_selector;

template <typename T, typename... Ts>
struct class_selector<T, false, Ts...> {
    using type = ::nanobind::class_<T, Ts...>;
};

template <typename T, typename... Ts>
struct class_selector<T, true, Ts...> {
    using Alias = typename ::nanobind::detail::extract<
        T, ::nanobind::detail::is_alias, Ts...>::type;

    using type = typename class_with_optional_alias<T, Alias>::type;
};

} // namespace mi_detail

template <typename T, typename... Ts>
class class_
    : public mi_detail::class_selector<
          T,
          (mi_detail::base_count_v<T, Ts...> >= 2),
          Ts...>::type {
    static constexpr size_t BaseCount = mi_detail::base_count_v<T, Ts...>;
    static constexpr size_t AliasCount = mi_detail::alias_count_v<T, Ts...>;
    static constexpr bool HasMultipleBases = BaseCount >= 2;

    using NbClass = typename mi_detail::class_selector<T, HasMultipleBases, Ts...>::type;
    using DirectBase = typename NbClass::Base;

    static_assert(
        AliasCount <= 1,
        "nanobind::mi::class_<> accepts at most one alias (trampoline) type.");

    static_assert(
        BaseCount + AliasCount == sizeof...(Ts),
        "nanobind::mi::class_<> was invoked with extra arguments that could not be handled");

    static NB_INLINE tuple make_bases_tuple() {
        static_assert(HasMultipleBases,
                      "nanobind::mi::class_::make_bases_tuple() should only be used for MI bindings.");

        tuple result = steal<tuple>(PyTuple_New((Py_ssize_t) BaseCount));
        if (!result.is_valid())
            detail::raise_python_error();

        size_t index = 0;
        auto append = [&](handle h) {
            if (!h.is_valid())
                detail::raise("nanobind::mi::class_: attempted to reference an unbound base type!");
            Py_INCREF(h.ptr());
            NB_TUPLE_SET_ITEM(result.ptr(), (Py_ssize_t) index++, h.ptr());
        };

        ((std::is_base_of_v<Ts, T> ? append(type<Ts>()) : (void) 0), ...);

        return result;
    }

    template <typename U>
    static NB_INLINE void register_edge_cast_if_base() {
        if constexpr (std::is_base_of_v<U, T>)
            register_edge_casts<T, U>();
    }

public:
    template <typename... Extra, bool B = HasMultipleBases,
              std::enable_if_t<!B, int> = 0>
    NB_INLINE class_(handle scope, const char *name, const Extra &... extra)
        : NbClass(scope, name, extra...) {
        if constexpr (!std::is_same_v<DirectBase, T>)
            register_edge_casts<T, DirectBase>();
    }

    template <typename... Extra, bool B = HasMultipleBases,
              std::enable_if_t<B, int> = 0>
    NB_INLINE class_(handle scope, const char *name, const Extra &... extra)
        : NbClass(scope, name,
                  detail::type_bases_py((object) make_bases_tuple()),
                  extra...) {
        (register_edge_cast_if_base<Ts>(), ...);
    }
};

NAMESPACE_END(mi)
NAMESPACE_END(NB_NAMESPACE)
