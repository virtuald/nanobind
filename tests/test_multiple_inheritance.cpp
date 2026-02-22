#include <nanobind/nanobind.h>
#include <nanobind/multiple_inheritance.h>

namespace nb = nanobind;

NB_MODULE(test_multiple_inheritance_ext, m) {
    m.def("scaffold_present", []() { return true; });
}
