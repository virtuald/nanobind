# Multiple inheritance (MI) port map

This document maps pybind11 MI tests to nanobind coverage for v1.

Status values:

- **ported**: covered by a nanobind test today.
- **deferred**: intentionally out of v1 scope.

| pybind11 file + test name | Behavior asserted | nanobind test name/path | Status | Adaptation notes |
| --- | --- | --- | --- | --- |
| `tests/test_multiple_inheritance.py::test_multiple_inheritance_cpp` | C++-declared MI class exposes both base interfaces and expected methods. | `tests/test_multiple_inheritance.py::test_cpp_declared_mi_method_dispatch` (+ `..._issubclass`, `..._isinstance`) | **ported** | nanobind v1 uses explicit opt-in API `nb::mi::class_` from `<nanobind/multiple_inheritance.h>`. |
| `tests/test_multiple_inheritance.py::test_mi_unaligned_base` | Pointer-adjusted (non-first base) identity and instance tracking correctness. | `tests/test_multiple_inheritance.py::test_mi_unaligned_base` and `...::test_mi_unaligned_base_identity` | **ported** | ConstructorStats-specific assertions are not mirrored; identity and offset behavior are validated directly. |
| `tests/test_multiple_inheritance.py::test_mi_base_return` | Returning base pointers recovers most-derived Python type when registered; unregistered derived falls back to returned base type. | `tests/test_multiple_inheritance.py::test_mi_base_return` and `...::test_mi_base_return_dynamic_type` | **ported** | Same dynamic-type expectations covered, including fallback behavior (`I801E`). |
| `tests/test_multiple_inheritance.py::test_multiple_inheritance_mix1` | Python class + one extension base MI behavior. | N/A | **deferred** | Python-defined MI across multiple extension bases is out of v1 scope. |
| `tests/test_multiple_inheritance.py::test_multiple_inheritance_mix2` | Extension base + Python class MI behavior. | N/A | **deferred** | Same as above. |
| `tests/test_multiple_inheritance.py::test_multiple_inheritance_python` | Rich Python-defined MI graph over extension bases. | N/A | **deferred** | Out of scope in v1. |
| `tests/test_multiple_inheritance.py::test_multiple_inheritance_python_many_bases` | Python-defined MI over many extension bases / holder-layout pressure. | N/A | **deferred** | Out of scope in v1. |
| `tests/test_multiple_inheritance.py::test_multiple_inheritance_virtbase` | Virtual inheritance pointer adjustment via MI hierarchy. | N/A | **deferred** | Virtual inheritance is not supported in v1. |
| `tests/test_multiple_inheritance.py::test_diamond_inheritance` | Diamond inheritance identity consistency across base paths. | N/A | **deferred** | Virtual-base / deep-diamond semantics deferred in v1. |
| `tests/test_multiple_inheritance.py::test_pr3635_diamond_*` | Extended diamond/virtual-base correctness across levels. | N/A | **deferred** | Virtual-base / deep-diamond semantics deferred in v1. |
| `tests/test_multiple_inheritance.py::test_python_inherit_from_mi` | Python subclassing from a C++ MI-derived class. | N/A | **deferred** | Deferred with Python-side MI semantics. |
| `tests/test_python_multiple_inheritance.py::test_python_multiple_inheritance*` (`test_PC`, `test_PPCC`) | Python/C++ MI interaction and method/base state consistency. | N/A | **deferred** | Python-side MI semantics deferred in v1. |
| `tests/test_python_multiple_inheritance.py::test_PC` | Python subclass of bound C++ base type. | N/A | **deferred** | Covered by deferred wildcard above; listed explicitly for clarity. |
| `tests/test_python_multiple_inheritance.py::test_PPCC` | Python MI combining Python subclass and bound C++ derived type. | N/A | **deferred** | Covered by deferred wildcard above; listed explicitly for clarity. |
| `tests/test_class_sh_disowning_mi.py::test_class_sh_disowning_mi*` (suite) | smart_holder disowning behavior across MI graphs. | N/A | **deferred** | pybind11 `smart_holder` / `classh` specific; out of nanobind v1 MI scope. |
| `tests/test_class_sh_mi_thunks.py::test_class_sh_mi_thunks*` (suite) | smart_holder MI thunk behavior across raw/shared/unique paths and virtual-base layouts. | N/A | **deferred** | pybind11 `smart_holder` / `classh` specific; out of nanobind v1 MI scope. |
