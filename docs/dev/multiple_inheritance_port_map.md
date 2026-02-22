# Multiple inheritance (MI) port map

This document tracks pybind11 MI-related tests and the nanobind port plan.

## Source files (pybind11)

Exact upstream files under `/work/nb/pybind11/tests/`:

- `test_multiple_inheritance.cpp`
- `test_multiple_inheritance.py`
- `test_python_multiple_inheritance.cpp`
- `test_python_multiple_inheritance.py`
- `test_class_sh_disowning_mi.cpp`
- `test_class_sh_disowning_mi.py`
- `test_class_sh_mi_thunks.cpp`
- `test_class_sh_mi_thunks.py`

## Port now (v1 scope)

From `/work/nb/pybind11/tests/test_multiple_inheritance.py`:

- `test_multiple_inheritance_cpp`
  - nanobind target: `tests/test_multiple_inheritance.py::test_cpp_declared_mi`
- `test_mi_unaligned_base`
  - nanobind target: `tests/test_multiple_inheritance.py::test_offset_base_identity`
- `test_mi_base_return`
  - nanobind target: `tests/test_multiple_inheritance.py::test_base_pointer_return_dynamic_type`

## Deferred (explicit)

### Python-defined MI across extension bases (out of v1 scope)

Deferred tests:

- `test_multiple_inheritance_mix1`
- `test_multiple_inheritance_mix2`
- `test_multiple_inheritance_python`
- `test_multiple_inheritance_python_many_bases`

Reason: Python-defined MI across multiple nanobind extension bases is out-of-scope for v1.

### Virtual inheritance / deep diamond behavior (out of v1 scope)

Deferred tests:

- `test_multiple_inheritance_virtbase`
- `test_diamond_inheritance`
- `test_pr3635_diamond_*`
- `test_python_inherit_from_mi`

Reason: virtual inheritance and deep-diamond semantics are deferred in v1.

### Python-side MI layout semantics (out of v1 scope)

Deferred tests from `/work/nb/pybind11/tests/test_python_multiple_inheritance.py`:

- `test_PC`
- `test_PPCC`

Reason: depends on Python-side MI layout semantics deferred in v1.

### pybind11 smart_holder/classh-specific tests (out of v1 scope)

Deferred suites:

- `/work/nb/pybind11/tests/test_class_sh_disowning_mi.py::*`
- `/work/nb/pybind11/tests/test_class_sh_mi_thunks.py::*`

Reason: pybind11 `smart_holder`/`classh` specific; nanobind v1 MI focuses on default holder paths.
