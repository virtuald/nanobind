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
