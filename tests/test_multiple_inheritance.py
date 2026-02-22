import test_multiple_inheritance_ext as t


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
