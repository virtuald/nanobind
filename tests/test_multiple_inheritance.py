import pytest
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
