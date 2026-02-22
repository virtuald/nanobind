import gc

import pytest
import test_multiple_inheritance_ext as t


def _collect():
    gc.collect()


def test_cpp_declared_mi_issubclass():
    assert issubclass(t.MIType, t.Base1)
    assert issubclass(t.MIType, t.Base2)


def test_cpp_declared_mi_isinstance():
    obj = t.MIType()
    assert isinstance(obj, t.Base1)
    assert isinstance(obj, t.Base2)


def test_cpp_declared_mi_method_dispatch():
    obj = t.MIType()
    assert obj.base1() == "base1"
    assert obj.base2() == "base2"
    assert obj.cpp() == "mi"


def test_pass_as_base2_pointer_adjustment():
    obj = t.MIType(123)
    assert obj.value == 123
    assert t.pass_as_base2_value(obj) == 123
    assert t.pass_as_base2_method(obj) == "base2"


def test_mi_unaligned_base_identity():
    c = t.MIType(11)
    d = t.MIDerived(22)

    assert t.as_base1_mitype(c) is c
    assert t.as_base2_mitype(c) is c
    assert t.as_base1_miderived(d) is d
    assert t.as_base2_miderived(d) is d


def test_mi_base_return_dynamic_type():
    c1 = t.new_mitype_as_base1()
    assert type(c1) is t.MIType
    assert c1.value == 101

    c2 = t.new_mitype_as_base2()
    assert type(c2) is t.MIType
    assert c2.value == 102

    d1 = t.new_miderived_as_base1()
    assert type(d1) is t.MIDerived
    assert d1.value == 201

    d2 = t.new_miderived_as_base2()
    assert type(d2) is t.MIDerived
    assert d2.value == 202


def test_mi_unaligned_base():
    c = t.I801C()
    d = t.I801D()

    assert t.i801b1_c(c) is c
    assert t.i801b2_c(c) is c
    assert t.i801b1_d(d) is d
    assert t.i801b2_d(d) is d


def test_mi_base_return():
    c1 = t.i801c_b1()
    assert type(c1) is t.I801C
    assert c1.a == 1
    assert c1.b == 2

    d1 = t.i801d_b1()
    assert type(d1) is t.I801D
    assert d1.a == 1
    assert d1.b == 2

    c2 = t.i801c_b2()
    assert type(c2) is t.I801C
    assert c2.a == 1
    assert c2.b == 2

    d2 = t.i801d_b2()
    assert type(d2) is t.I801D
    assert d2.a == 1
    assert d2.b == 2

    e1 = t.i801e_c()
    assert type(e1) is t.I801C
    assert e1.a == 1
    assert e1.b == 2

    e2 = t.i801e_b2()
    assert type(e2) is t.I801B2
    assert e2.b == 2


def test_mi_lifetime_raw_take_ownership():
    t.mi_tracked_reset()

    obj1 = t.new_tracked_raw_base1()
    assert type(obj1) is t.MITrackedDerived
    assert t.mi_tracked_as_base1_marker(obj1) == 2011
    assert t.mi_tracked_as_base2_value(obj1) == 11
    assert t.mi_tracked_live() == 1
    assert t.mi_tracked_destruct() == 0

    obj2 = t.new_tracked_raw_base2()
    assert type(obj2) is t.MITrackedDerived
    assert t.mi_tracked_as_base1_marker(obj2) == 2012
    assert t.mi_tracked_as_base2_value(obj2) == 12
    assert t.mi_tracked_live() == 2
    assert t.mi_tracked_destruct() == 0

    del obj1, obj2
    _collect()
    assert t.mi_tracked_live() == 0
    assert t.mi_tracked_destruct() == 2


def test_mi_lifetime_unique_ptr_consume_once():
    t.mi_tracked_reset()

    obj1 = t.new_tracked_unique_base1()
    assert type(obj1) is t.MITrackedDerived
    assert t.mi_tracked_as_base1_marker(obj1) == 2021
    assert t.mi_tracked_as_base2_value(obj1) == 21
    assert t.mi_tracked_live() == 1

    assert t.consume_tracked_unique_base1(obj1) == 2021
    assert t.mi_tracked_live() == 0
    assert t.mi_tracked_destruct() == 1

    with pytest.warns(RuntimeWarning, match="relinquished instance"):
        with pytest.raises(TypeError):
            t.consume_tracked_unique_base1(obj1)

    obj2 = t.new_tracked_unique_base2()
    assert type(obj2) is t.MITrackedDerived
    assert t.mi_tracked_as_base1_marker(obj2) == 2022
    assert t.mi_tracked_as_base2_value(obj2) == 22
    assert t.consume_tracked_unique_base2(obj2) == 22
    assert t.mi_tracked_live() == 0
    assert t.mi_tracked_destruct() == 2


def test_mi_lifetime_shared_ptr_paths():
    t.mi_tracked_reset()

    obj1 = t.new_tracked_shared_base1()
    assert type(obj1) is t.MITrackedDerived
    assert t.mi_tracked_as_base1_marker(obj1) == 2031
    assert t.mi_tracked_as_base2_value(obj1) == 31
    assert t.consume_tracked_shared_base1(obj1) == 2031
    assert t.consume_tracked_shared_base2(obj1) == 31
    assert t.mi_tracked_live() == 1
    assert t.mi_tracked_destruct() == 0

    obj2 = t.new_tracked_shared_base2()
    assert type(obj2) is t.MITrackedDerived
    assert t.mi_tracked_as_base1_marker(obj2) == 2032
    assert t.mi_tracked_as_base2_value(obj2) == 32
    assert t.consume_tracked_shared_base1(obj2) == 2032
    assert t.consume_tracked_shared_base2(obj2) == 32
    assert t.mi_tracked_live() == 2
    assert t.mi_tracked_destruct() == 0

    del obj1, obj2
    _collect()
    assert t.mi_tracked_live() == 0
    assert t.mi_tracked_destruct() == 2
