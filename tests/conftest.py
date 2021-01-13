# -*- coding: utf-8 -*-
import pytest
from click.testing import CliRunner

from pycreds import delete_password


@pytest.fixture(autouse=True, scope="function")
def clear_vault():
    while True:
        try:
            delete_password("foo", "bar")
        except ValueError:
            break


@pytest.fixture()
def runner():
    yield CliRunner()
