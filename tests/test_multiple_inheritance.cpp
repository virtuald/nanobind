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
    virtual ~MIType() = default;
    const char *cpp() const { return "mi"; }
};

struct MIDerived : MIType {
    explicit MIDerived(int value = 0) : MIType(value) { }
    virtual ~MIDerived() = default;
    const char *cpp_derived() const { return "mi_derived"; }
};

// pybind11 Issue #801-compatible fixtures (adapted)
struct I801B1 {
    int a = 1;
    I801B1() = default;
    I801B1(const I801B1 &) = default;
    virtual ~I801B1() = default;
    int get_a() const { return a; }
};

struct I801B2 : I801B1 {
    int b = 2;
    I801B2() = default;
    I801B2(const I801B2 &) = default;
    virtual ~I801B2() = default;
    int get_b() const { return b; }
};

struct I801Pad {
    virtual ~I801Pad() = default;
    int pad = 77;
};

struct I801C : I801Pad, I801B2 {
    virtual ~I801C() = default;
};

struct I801D : I801C {
    virtual ~I801D() = default;
};

struct I801E : I801D {
    virtual ~I801E() = default;
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

    nb::mi::class_<MIDerived, MIType>(m, "MIDerived")
        .def(nb::init<int>(), nb::arg("value") = 0)
        .def("cpp_derived", &MIDerived::cpp_derived);

    m.def("pass_as_base2_value", [](Base2 *b) { return b->get_value(); });
    m.def("pass_as_base2_method", [](Base2 *b) { return b->base2(); });

    m.def("as_base1_mitype", [](MIType *p) { return static_cast<Base1 *>(p); });
    m.def("as_base2_mitype", [](MIType *p) { return static_cast<Base2 *>(p); });
    m.def("as_base1_miderived", [](MIDerived *p) { return static_cast<Base1 *>(p); });
    m.def("as_base2_miderived", [](MIDerived *p) { return static_cast<Base2 *>(p); });

    m.def("new_mitype_as_base1", []() -> Base1 * { return new MIType(101); });
    m.def("new_mitype_as_base2", []() -> Base2 * { return new MIType(102); });
    m.def("new_miderived_as_base1", []() -> Base1 * { return new MIDerived(201); });
    m.def("new_miderived_as_base2", []() -> Base2 * { return new MIDerived(202); });

    nb::class_<I801B1>(m, "I801B1")
        .def(nb::init<>())
        .def_prop_ro("a", &I801B1::get_a);
    nb::class_<I801B2, I801B1>(m, "I801B2")
        .def(nb::init<>())
        .def_prop_ro("b", &I801B2::get_b);
    nb::mi::class_<I801C, I801B2, I801B1>(m, "I801C")
        .def(nb::init<>());
    nb::mi::class_<I801D, I801C>(m, "I801D")
        .def(nb::init<>());

    m.def("i801b1_c", [](I801C *c) { return static_cast<I801B1 *>(c); });
    m.def("i801b2_c", [](I801C *c) { return static_cast<I801B2 *>(c); });
    m.def("i801b1_d", [](I801D *d) { return static_cast<I801B1 *>(d); });
    m.def("i801b2_d", [](I801D *d) { return static_cast<I801B2 *>(d); });

    m.def("i801c_b1", []() -> I801B1 * { return new I801C(); });
    m.def("i801c_b2", []() -> I801B2 * { return new I801C(); });
    m.def("i801d_b1", []() -> I801B1 * { return new I801D(); });
    m.def("i801d_b2", []() -> I801B2 * { return new I801D(); });

    // Unregistered-derived scenario from pybind11 #801.
    // Use static storage + reference policy to avoid ownership ambiguity with raw pointers.
    m.def("i801e_c",
          []() -> I801C * {
              static I801E e;
              return static_cast<I801C *>(&e);
          },
          nb::rv_policy::reference);
    m.def("i801e_b2",
          []() -> I801B2 * {
              static I801E e;
              return static_cast<I801B2 *>(&e);
          },
          nb::rv_policy::reference);
}
