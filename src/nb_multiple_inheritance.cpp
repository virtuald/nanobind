/*
    src/nb_multiple_inheritance.cpp: MI cast registry hooks

    Copyright (c) 2022 Wenzel Jakob

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE file.
*/

#include "nb_internals.h"

NAMESPACE_BEGIN(NB_NAMESPACE)
NAMESPACE_BEGIN(detail)

void nb_type_register_cast(const std::type_info *src,
                           const std::type_info *dst,
                           nb_type_cast_fn fn) {
    lock_internals guard(internals);
    internals->type_c2c[{ src, dst }] = fn;
}

nb_type_cast_fn nb_type_find_cast(const std::type_info *src,
                                  const std::type_info *dst) {
    lock_internals guard(internals);
    nb_cast_map::iterator it = internals->type_c2c.find({ src, dst });
    if (it == internals->type_c2c.end())
        return nullptr;
    return it->second;
}

NAMESPACE_END(detail)
NAMESPACE_END(NB_NAMESPACE)
