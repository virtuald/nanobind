#include <nanobind/nanobind.h>
#include <nanobind/multiple_inheritance.h>

namespace nb = nanobind;

struct Base1 {
    const char *base1() const { return "base1"; }
};

struct Base2 : Base1 {
    const char *base2() const { return "base2"; }
};

struct MIType : Base2 {
    const char *cpp() const { return "mi"; }
};

NB_MODULE(test_multiple_inheritance_ext, m) {
    nb::class_<Base1>(m, "Base1")
        .def(nb::init<>())
        .def("base1", &Base1::base1);

    nb::class_<Base2, Base1>(m, "Base2")
        .def(nb::init<>())
        .def("base2", &Base2::base2);

    nb::mi::class_<MIType, Base2, Base1>(m, "MIType")
        .def(nb::init<>())
        .def("cpp", &MIType::cpp);
}
