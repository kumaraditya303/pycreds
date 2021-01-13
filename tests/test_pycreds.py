# -*- coding: utf-8 -*-
import pytest

from pycreds import (
    delete_password,
    find_credentials,
    find_password,
    get_password,
    set_password,
)


@pytest.fixture(autouse=True)
def clear_vault():
    while True:
        try:
            delete_password("foo", "bar")
        except ValueError:
            break


def test_set_password():
    expected_password = "secret"
    status = set_password("foo", "bar", expected_password)
    assert status
    password = get_password("foo", "bar")
    assert password == expected_password


def test_get_password():
    with pytest.raises(ValueError):
        get_password("foo", "bar")
    expected_password = "secret"
    status = set_password("foo", "bar", expected_password)
    assert status
    password = get_password("foo", "bar")
    assert password == expected_password


def test_delete_password():
    with pytest.raises(ValueError):
        delete_password("foo", "bar")
    expected_password = "secret"
    status = set_password("foo", "bar", expected_password)
    assert status
    status = delete_password("foo", "bar")
    assert status
    with pytest.raises(ValueError):
        get_password("foo", "bar")


def test_find_password():
    password = find_password("not found")
    assert password is None
    expected_password = "secret"
    status = set_password("foo", "bar", expected_password)
    assert status
    password = find_password("foo")
    assert password == "secret"


def test_find_credentials():
    credentials = find_credentials("not found")
    assert credentials == []
    expected_credentials = {"account": "bar", "password": "secret"}
    set_password("foo", "bar", "secret")
    credentials = find_credentials("foo")
    assert expected_credentials in credentials
