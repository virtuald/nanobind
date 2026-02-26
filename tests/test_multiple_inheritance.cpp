#include <nanobind/nanobind.h>
#include <nanobind/multiple_inheritance.h>

namespace nb = nanobind;

struct MIBase {
    virtual ~MIBase() = default;
    int value = 123;
};

struct MIDerived : MIBase { };
struct MIUnrelated { };

struct MIRoot {
    virtual ~MIRoot() = default;
};

struct MIB : MIRoot { };
struct MIC : MIRoot { };
struct MID : MIB, MIC { };

static void *identity_cast(void *ptr) noexcept { return ptr; }

static MID *mid_singleton() {
    static MID v;
    return &v;
}

NB_MODULE(test_multiple_inheritance_ext, m) {
    m.def("registry_miss_before_registration", []() {
        auto fn = nb::detail::nb_type_find_cast(&typeid(MIDerived), &typeid(MIBase));
        return fn == nullptr;
    });

    m.def("registry_register_and_lookup_hit", []() {
        nb::detail::nb_type_register_cast(&typeid(MIDerived), &typeid(MIBase), identity_cast);
        auto fn = nb::detail::nb_type_find_cast(&typeid(MIDerived), &typeid(MIBase));
        return fn == identity_cast;
    });

    m.def("registry_register_with_shim_helper", []() {
        nb::mi::register_cast<MIDerived, MIBase>();
        auto fn = nb::detail::nb_type_find_cast(&typeid(MIDerived), &typeid(MIBase));
        MIDerived value;
        return fn && fn(&value) == static_cast<MIBase *>(&value);
    });

    m.def("registry_unrelated_pair_is_miss", []() {
        auto fn = nb::detail::nb_type_find_cast(&typeid(MIDerived), &typeid(MIUnrelated));
        return fn == nullptr;
    });

    nb::mi::class_<MIBase>(m, "MIBase")
        .def(nb::init<>())
        .def("value", [](const MIBase &self) { return self.value; });

    nb::mi::class_<MIRoot>(m, "MIRoot").def(nb::init<>());
    nb::mi::class_<MIB, MIRoot>(m, "MIB").def(nb::init<>());
    nb::mi::class_<MIC, MIRoot>(m, "MIC").def(nb::init<>());
    nb::mi::class_<MID, nb::mi::bases<MIB, MIC>>(m, "MID").def(nb::init<>());

    m.def("mid_base_addresses", [](MID &d) {
        return nb::make_tuple(
            (uintptr_t) static_cast<MIB *>(&d),
            (uintptr_t) static_cast<MIC *>(&d),
            (uintptr_t) static_cast<MIRoot *>(static_cast<MIB *>(&d)),
            (uintptr_t) static_cast<MIRoot *>(static_cast<MIC *>(&d)));
    });

    m.def("expect_b_ref", [](MIB &b) { return (uintptr_t) &b; });
    m.def("expect_c_ptr", [](MIC *c) { return (uintptr_t) c; });
    m.def("expect_root_from_b_ref", [](MIB &b) {
        return (uintptr_t) static_cast<MIRoot *>(&b);
    });
    m.def("expect_root_from_c_ptr", [](MIC *c) {
        return (uintptr_t) static_cast<MIRoot *>(c);
    });

    m.def("make_mid", []() -> MID * { return mid_singleton(); }, nb::rv_policy::reference);
    m.def("as_b_view", []() -> MIB * { return static_cast<MIB *>(mid_singleton()); },
          nb::rv_policy::reference);
    m.def("as_c_view", []() -> MIC * { return static_cast<MIC *>(mid_singleton()); },
          nb::rv_policy::reference);

    m.def("shim_construct_and_call", []() {
        MIBase value;
        return value.value;
    });
}
