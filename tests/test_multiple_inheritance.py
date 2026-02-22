import pytest

import test_multiple_inheritance_ext as t


def test_scaffold_import_and_smoke():
    assert t.scaffold_present() is True


@pytest.mark.xfail(strict=False, reason="MI C++ behavior not implemented yet (task 01 scaffold only)")
def test_cpp_declared_mi():
    raise AssertionError("pending MI implementation")


@pytest.mark.xfail(strict=False, reason="MI base offset behavior not implemented yet (task 01 scaffold only)")
def test_offset_base_identity():
    raise AssertionError("pending MI implementation")


@pytest.mark.xfail(strict=False, reason="MI dynamic base-return behavior not implemented yet (task 01 scaffold only)")
def test_base_pointer_return_dynamic_type():
    raise AssertionError("pending MI implementation")
