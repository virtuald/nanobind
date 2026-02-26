#include <nanobind/nanobind.h>
#include <nanobind/multiple_inheritance.h>

namespace nb = nanobind;

struct MIBase {
    virtual ~MIBase() = default;
    int value = 123;
};

struct MIDerived : MIBase { };
struct MIUnrelated { };

static void *identity_cast(void *ptr) noexcept { return ptr; }

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

    m.def("shim_construct_and_call", []() {
        MIBase value;
        return value.value;
    });
}
