/*
    nanobind/multiple_inheritance.h: Optional multiple inheritance scaffolding

    Copyright (c) 2026 Wenzel Jakob

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE file.
*/

#pragma once

#include <nanobind/nanobind.h>

NAMESPACE_BEGIN(NB_NAMESPACE)
namespace mi {

/**
 * Placeholder wrapper for future multiple-inheritance registration metadata.
 *
 * This is intentionally inert in task 01. It only provides a stable opt-in
 * include surface for subsequent incremental MI work.
 */
template <typename... Bases> struct bases { };

// TODO(v1-mi): register extra base relationships for classes opting into MI.
// TODO(v1-mi): register MI-aware cast edges for base/derived pointer traversal.

} // namespace mi
NAMESPACE_END(NB_NAMESPACE)
