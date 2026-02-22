#include <nanobind/nanobind.h>
#include <nanobind/multiple_inheritance.h>

namespace nb = nanobind;

struct Base1 {
    virtual ~Base1() = default;
    const char *base1() const { return "base1"; }
};

struct Base2 : Base1 {
    explicit Base2(int value = 0) : value(value) { }
    virtual ~Base2() = default;

    const char *base2() const { return "base2"; }
    int get_value() const { return value; }

    int value;
};

struct Padding {
    virtual ~Padding() = default;
    int pad = 17;
};

struct MIType : Padding, Base2 {
    explicit MIType(int value = 0) : Base2(value) { }
    const char *cpp() const { return "mi"; }
};

NB_MODULE(test_multiple_inheritance_ext, m) {
    nb::class_<Base1>(m, "Base1")
        .def(nb::init<>())
        .def("base1", &Base1::base1);

    nb::class_<Base2, Base1>(m, "Base2")
        .def(nb::init<int>(), nb::arg("value") = 0)
        .def("base2", &Base2::base2)
        .def_prop_ro("value", &Base2::get_value);

    nb::mi::class_<MIType, Base2, Base1>(m, "MIType")
        .def(nb::init<int>(), nb::arg("value") = 0)
        .def("cpp", &MIType::cpp);

    m.def("pass_as_base2_value", [](Base2 *b) { return b->get_value(); });
    m.def("pass_as_base2_method", [](Base2 *b) { return b->base2(); });
}
