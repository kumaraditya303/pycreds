# -*- coding: utf-8 -*-
from click.testing import CliRunner

from pycreds.cli import main


def test_cli_set_password(runner: CliRunner):
    result = runner.invoke(main, ["set", "foo", "bar", "secret"])
    assert result.exit_code == 0
    assert "Password saved." in result.output


def test_cli_get_password(runner: CliRunner):
    result = runner.invoke(main, ["get", "foo", "bar"])
    assert result.exit_code == 0
    assert "Password not found." in result.output
    result = runner.invoke(main, ["set", "foo", "bar", "secret"])
    assert result.exit_code == 0
    assert "Password saved." in result.output
    result = runner.invoke(main, ["get", "foo", "bar"])
    assert result.exit_code == 0
    assert "secret" in result.output


def test_cli_delete_password(runner: CliRunner):
    result = runner.invoke(main, ["delete", "foo", "bar"])
    assert result.exit_code == 0
    assert "Password not found." in result.output
    result = runner.invoke(main, ["set", "foo", "bar", "secret"])
    assert result.exit_code == 0
    assert "Password saved." in result.output
    result = runner.invoke(main, ["delete", "foo", "bar"])
    assert result.exit_code == 0
    assert "Password deleted." in result.output
