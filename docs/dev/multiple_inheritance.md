# Multiple inheritance (MI) in nanobind (v1)

This feature is **opt-in**.

## Public opt-in API

Use exactly one additional public header:

```cpp
#include <nanobind/multiple_inheritance.h>
```

Then bind MI classes via `nb::mi::class_`:

```cpp
nb::mi::class_<Derived, Base2, Base1>(m, "Derived");
```

`nb::class_` remains the default path for non-MI bindings and is unchanged.

## Supported in v1

- C++-declared **public multiple inheritance** bindings.
- Base/derived cross-casting for registered edges using RTTI-backed casts.
- Returning non-primary base pointers where dynamic type recovery is available.

## Hard limits in v1

- **Public MI only**.
- **Polymorphic types required** on downcast edges:
  - the bound derived type must be polymorphic;
  - all declared MI bases must be polymorphic.
- **No virtual inheritance** support in v1.
- **No Python-defined MI across multiple nanobind extension bases** in v1.
- **No smart-holder-specific MI semantics** in v1 (pybind11 `smart_holder` parity is out of scope).

## Notes

- `nb::mi::class_` requires RTTI (`typeid`/`dynamic_cast`).
- Deferred scope is tracked in `docs/dev/multiple_inheritance_port_map.md`.
