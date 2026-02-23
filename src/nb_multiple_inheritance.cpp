/*
    src/nb_multiple_inheritance.cpp: libnanobind functionality for MI pointer casts

    Copyright (c) 2026 Wenzel Jakob

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE file.
*/

#include "nb_internals.h"

#include <deque>
#include <vector>

NAMESPACE_BEGIN(NB_NAMESPACE)
NAMESPACE_BEGIN(detail)

using nb_type_cast_fn = void *(*)(void *);

struct nb_type_cast_edge {
    const std::type_info *dst;
    nb_type_cast_fn cast;
};

using nb_type_cast_graph = tsl::robin_map<const std::type_info *,
                                          std::vector<nb_type_cast_edge>,
                                          std_typeinfo_hash, std_typeinfo_eq>;

static nb_type_cast_graph &nb_type_cast_graph_ref() {
    static nb_type_cast_graph graph;
    return graph;
}

NB_CORE void nb_type_register_cast(const std::type_info *src,
                                   const std::type_info *dst,
                                   void *(*cast)(void *)) noexcept {
    check(src && dst && cast,
          "nanobind::detail::nb_type_register_cast(): invalid input!");

    nb_type_cast_graph &graph = nb_type_cast_graph_ref();
    lock_internals guard(internals);

    auto [it, inserted] = graph.try_emplace(src);
    std::vector<nb_type_cast_edge> &edges = it.value();

    (void) inserted;
    for (nb_type_cast_edge &edge : edges) {
        if (edge.dst == dst || *edge.dst == *dst) {
            edge.cast = cast;
            return;
        }
    }

    edges.push_back(nb_type_cast_edge{ dst, cast });
}

bool nb_type_apply_cast(const std::type_info *src,
                        const std::type_info *dst,
                        void *value,
                        void **out) noexcept {
    check(src && dst && out,
          "nanobind::detail::nb_type_apply_cast(): invalid input!");

    if (src == dst || *src == *dst) {
        *out = value;
        return true;
    }

    nb_type_cast_graph &graph = nb_type_cast_graph_ref();

    using pred_map = tsl::robin_map<const std::type_info *, nb_type_cast_edge,
                                    std_typeinfo_hash, std_typeinfo_eq>;

    std::vector<nb_type_cast_fn> path;

    {
        lock_internals guard(internals);

        auto it_src = graph.find(src);
        if (it_src == graph.end())
            return false;

        pred_map pred;
        std::deque<const std::type_info *> queue;
        queue.push_back(src);
        pred.emplace(src, nb_type_cast_edge{ nullptr, nullptr });

        const std::type_info *found = nullptr;

        while (!queue.empty() && !found) {
            const std::type_info *cur = queue.front();
            queue.pop_front();

            auto it = graph.find(cur);
            if (it == graph.end())
                continue;

            for (const nb_type_cast_edge &edge : it->second) {
                if (!pred.try_emplace(edge.dst,
                                      nb_type_cast_edge{ cur, edge.cast }).second)
                    continue;

                if (edge.dst == dst || *edge.dst == *dst) {
                    found = edge.dst;
                    break;
                }

                queue.push_back(edge.dst);
            }
        }

        if (!found)
            return false;

        const std::type_info *cur = found;
        while (!(cur == src || *cur == *src)) {
            auto it = pred.find(cur);
            check(it != pred.end() && it->second.dst && it->second.cast,
                  "nanobind::detail::nb_type_apply_cast(): corrupted cast graph state!");
            path.push_back(it->second.cast);
            cur = it->second.dst;
        }
    }

    void *result = value;
    for (auto it = path.rbegin(); it != path.rend(); ++it) {
        result = (*it)(result);
        if (!result)
            return false;
    }

    *out = result;
    return true;
}

NAMESPACE_END(detail)
NAMESPACE_END(NB_NAMESPACE)
