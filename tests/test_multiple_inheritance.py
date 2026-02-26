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
