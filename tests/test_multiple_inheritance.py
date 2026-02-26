import pytest
from common import collect
import test_multiple_inheritance_ext as t


def test_registry_smoke_miss_before_registration():
    assert t.registry_miss_before_registration()


def test_registry_smoke_register_and_hit():
    assert t.registry_register_and_lookup_hit()


def test_registry_smoke_unrelated_pair_remains_miss():
    assert t.registry_unrelated_pair_is_miss()


def test_registry_smoke_register_via_shim_helper():
    assert t.registry_register_with_shim_helper()


def test_smoke_shim_does_not_change_non_mi_behavior():
    obj = t.MIBase()
    assert obj.value() == 123
    assert t.shim_construct_and_call() == 123


def test_python_mi_isinstance_and_issubclass():
    d = t.MID()
    assert isinstance(d, t.MIB)
    assert isinstance(d, t.MIC)
    assert issubclass(t.MID, t.MIB)
    assert issubclass(t.MID, t.MIC)


def test_python_mi_mro_contains_all_bases():
    mro = t.MID.__mro__
    assert mro[0] is t.MID
    assert t.MIB in mro
    assert t.MIC in mro
    assert t.MIRoot in mro


def test_mi_argument_upcasts_with_pointer_adjustment():
    d = t.MID()
    b_addr, c_addr, root_b_addr, root_c_addr = t.mid_base_addresses(d)

    assert t.expect_b_ref(d) == b_addr
    assert t.expect_c_ptr(d) == c_addr
    assert t.expect_root_from_b_ref(d) == root_b_addr
    assert t.expect_root_from_c_ptr(d) == root_c_addr


def test_mi_wrong_base_rejected():
    b = t.MIB()
    c = t.MIC()

    with pytest.raises(TypeError):
        t.expect_c_ptr(b)

    with pytest.raises(TypeError):
        t.expect_b_ref(c)


def test_mi_identity_stable_across_base_views():
    d = t.make_mid()
    b = t.as_b_view()
    c = t.as_c_view()

    assert id(d) == id(b)
    assert id(d) == id(c)


def test_take_a_b_c_d_matrix():
    a = t.A()
    b = t.B()
    c = t.C()
    d = t.D()

    assert t.take_a(a).name() == "A"
    with pytest.raises(TypeError):
        t.take_b(a)
    with pytest.raises(TypeError):
        t.take_c(a)
    with pytest.raises(TypeError):
        t.take_d(a)

    assert t.take_a(b).name() == "A"
    assert t.take_b(b).name() == "B"
    with pytest.raises(TypeError):
        t.take_c(b)
    with pytest.raises(TypeError):
        t.take_d(b)

    assert t.take_a(c).name() == "A"
    with pytest.raises(TypeError):
        t.take_b(c)
    assert t.take_c(c).name() == "C"
    with pytest.raises(TypeError):
        t.take_d(c)

    assert t.take_a(d).name() == "A"
    assert t.take_b(d).name() == "B"
    assert t.take_c(d).name() == "C"
    assert t.take_d(d).name() == "D"


def test_shared_ptr_roundtrip():
    d = t.D()
    assert t.take_d_shared_ptr(d).name() == "D"


def test_base_view_downcast():
    d_as_a = t.d_factory()
    dd = t.take_d(d_as_a)
    assert dd.name() == "D"


def test_dispatch_through_each_base_view():
    a = t.A()
    b = t.B()
    c = t.C()
    d = t.D()

    assert a.name() == "A"
    assert b.name() == "B"
    assert c.name() == "C"
    assert d.name() == "D"

    assert t.take_a(d).name() == "A"
    assert t.take_b(d).name() == "B"
    assert t.take_c(d).name() == "C"
    assert t.take_d(d).name() == "D"


@pytest.fixture
def mi_lifetime_clean():
    collect()
    t.mi_lifetime_reset()
    yield
    collect()
    t.mi_lifetime_reset()


def test_mi_lifetime_python_owned_destructor_once(mi_lifetime_clean):
    value = t.MITrackedLeaf()
    assert t.mi_lifetime_stats() == (1, 0, 1)

    del value
    collect()

    assert t.mi_lifetime_stats() == (1, 1, 0)


def test_mi_lifetime_shared_ownership_destructor_once(mi_lifetime_clean):
    value = t.mi_lifetime_make_shared()
    left = t.mi_lifetime_as_left(value)
    right = t.mi_lifetime_as_right(value)

    assert id(value) == id(left)
    assert id(value) == id(right)
    assert t.mi_lifetime_stats() == (1, 0, 1)

    del left, right, value
    collect()

    assert t.mi_lifetime_stats() == (1, 1, 0)


def test_mi_lifetime_regression_multi_base_views_release_once(mi_lifetime_clean):
    left = t.mi_lifetime_singleton_left()
    right = t.mi_lifetime_singleton_right()

    assert id(left) == id(right)
    assert t.mi_lifetime_stats() == (1, 0, 1)

    t.mi_lifetime_release_singleton()
    del left, right
    collect()

    assert t.mi_lifetime_stats() == (1, 1, 0)


@pytest.mark.xfail(reason="Virtual inheritance in MI shim is out-of-scope in v1")
def test_virtual_inheritance_out_of_scope_v1():
    # Mirrors Boost.Python polymorphism2.py::test_pure_virtual coverage note:
    # Q : virtual P interaction is intentionally deferred in the first MI slice.
    p = t.P()
    with pytest.raises(RuntimeError):
        p.f()
