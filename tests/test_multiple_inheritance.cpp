#include <nanobind/nanobind.h>
#include <nanobind/multiple_inheritance.h>
#include <nanobind/stl/shared_ptr.h>
#include <nanobind/trampoline.h>

#include <memory>

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

struct P {
    virtual ~P() = default;
    virtual const char *f() = 0;
    const char *g() { return "P::g()"; }
};

struct PyP : P {
    NB_TRAMPOLINE(P, 1);

    const char *f() override {
        NB_OVERRIDE_PURE(f);
    }
};

struct Q : virtual P {
    const char *f() override { return "Q::f()"; }
};

struct MIA {
    MIA() : x(0) { }
    virtual ~MIA() = default;
    const char *name() { return "A"; }
    int x;
};

struct MIBoost : MIA {
    MIBoost() : x(1) { }
    const char *name() { return "B"; }
    int x;
};

struct MICBoost : MIA {
    MICBoost() : x(2) { }
    virtual ~MICBoost() = default;
    const char *name() { return "C"; }
    int x;
};

struct MIDBoost : MIBoost, MICBoost {
    MIDBoost() : x(3) { }
    const char *name() { return "D"; }
    int x;
};

struct MITrackedRoot {
    virtual ~MITrackedRoot() = default;
};

struct MITrackedLeft : MITrackedRoot {
    virtual ~MITrackedLeft() = default;
};

struct MITrackedRight : MITrackedRoot {
    virtual ~MITrackedRight() = default;
};

struct MITrackedLeaf : MITrackedLeft, MITrackedRight {
    MITrackedLeaf() {
        constructed++;
        alive++;
    }

    ~MITrackedLeaf() override {
        destroyed++;
        alive--;
    }

    static int constructed;
    static int destroyed;
    static int alive;
};

int MITrackedLeaf::constructed = 0;
int MITrackedLeaf::destroyed = 0;
int MITrackedLeaf::alive = 0;

static std::shared_ptr<MITrackedLeaf> tracked_singleton;

MIA take_a(const MIA &a) { return a; }
MIBoost take_b(MIBoost &b) { return b; }
MICBoost take_c(MICBoost *c) { return *c; }
MIDBoost take_d(MIDBoost *const &d) { return *d; }
MIDBoost take_d_shared_ptr(std::shared_ptr<MIDBoost> d) { return *d; }
std::shared_ptr<MIA> d_factory() { return std::shared_ptr<MIBoost>(new MIDBoost); }

static void *identity_cast(void *ptr) noexcept { return ptr; }

static MID *mid_singleton() {
    static MID v;
    return &v;
}

static std::shared_ptr<MITrackedLeaf> tracked_make_shared() {
    return std::make_shared<MITrackedLeaf>();
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

    // Boost.Python polymorphism2.cpp virtual inheritance fixture
    nb::mi::class_<P, PyP>(m, "P")
        .def(nb::init<>())
        .def("f", &P::f)
        .def("g", &P::g);

    nb::mi::class_<Q, nb::mi::bases<P>>(m, "Q")
        .def(nb::init<>())
        .def("f", &Q::f)
        .def("g", &P::g);

    // Boost.Python m1.cpp-style multiple inheritance fixture
    nb::mi::class_<MIA>(m, "A")
        .def(nb::init<>())
        .def("name", &MIA::name);

    nb::mi::class_<MIBoost, MIA>(m, "B")
        .def(nb::init<>())
        .def("name", &MIBoost::name);

    nb::mi::class_<MICBoost, MIA>(m, "C")
        .def(nb::init<>())
        .def("name", &MICBoost::name);

    nb::mi::class_<MIDBoost, nb::mi::bases<MIBoost, MICBoost>>(m, "D")
        .def(nb::init<>())
        .def("name", &MIDBoost::name);

    m.def("take_a", &take_a);
    m.def("take_b", &take_b);
    m.def("take_c", &take_c);
    m.def("take_d", &take_d);
    m.def("take_d_shared_ptr", &take_d_shared_ptr);
    m.def("d_factory", &d_factory);

    m.def("mi_lifetime_reset", []() {
        MITrackedLeaf::constructed = 0;
        MITrackedLeaf::destroyed = 0;
        MITrackedLeaf::alive = 0;
        tracked_singleton.reset();
    });

    m.def("mi_lifetime_stats", []() {
        return nb::make_tuple(MITrackedLeaf::constructed,
                              MITrackedLeaf::destroyed,
                              MITrackedLeaf::alive);
    });

    nb::mi::class_<MITrackedRoot>(m, "MITrackedRoot");
    nb::mi::class_<MITrackedLeft, MITrackedRoot>(m, "MITrackedLeft");
    nb::mi::class_<MITrackedRight, MITrackedRoot>(m, "MITrackedRight");
    nb::mi::class_<MITrackedLeaf, nb::mi::bases<MITrackedLeft, MITrackedRight>>(m,
                                                                                 "MITrackedLeaf")
        .def(nb::init<>());

    m.def("mi_lifetime_make_shared", &tracked_make_shared);
    m.def("mi_lifetime_as_left", [](std::shared_ptr<MITrackedLeaf> value) {
        return std::static_pointer_cast<MITrackedLeft>(value);
    });
    m.def("mi_lifetime_as_right", [](std::shared_ptr<MITrackedLeaf> value) {
        return std::static_pointer_cast<MITrackedRight>(value);
    });
    m.def("mi_lifetime_singleton_left", []() {
        if (!tracked_singleton)
            tracked_singleton = std::make_shared<MITrackedLeaf>();
        return std::static_pointer_cast<MITrackedLeft>(tracked_singleton);
    });
    m.def("mi_lifetime_singleton_right", []() {
        if (!tracked_singleton)
            tracked_singleton = std::make_shared<MITrackedLeaf>();
        return std::static_pointer_cast<MITrackedRight>(tracked_singleton);
    });
    m.def("mi_lifetime_release_singleton", []() { tracked_singleton.reset(); });

    m.def("shim_construct_and_call", []() {
        MIBase value;
        return value.value;
    });
}
