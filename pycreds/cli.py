# -*- coding: utf-8 -*-
import textwrap

try:
    import click

except ImportError:  # pragma:no cover
    print(
        textwrap.dedent(
            """
        PyCreds Cli Dependencies not found.

        Make sure you have installed PyCreds with cli.
        $ python -m pip install pycreds[cli]"""
        )
    )
    exit(1)

from pycreds import delete_password, get_password, set_password


@click.group()
def main():
    """PyCreds Command Line Interface."""
    pass


@main.command()
@click.argument("service", required=True, type=click.STRING)
@click.argument("account", required=True, type=click.STRING)
@click.argument("password", required=True, type=click.STRING)
def set(service: str, account: str, password: str):
    """Set Password."""

    set_password(service, account, password)
    click.secho("Password saved.", fg="green", bold=True)


@main.command()
@click.argument("service", required=True, type=click.STRING)
@click.argument("account", required=True, type=click.STRING)
def get(service: str, account: str):
    """Get Password."""
    try:
        click.secho(get_password(service, account), fg="green", bold=True)
    except ValueError:
        click.secho("Password not found.", fg="red")


@main.command()
@click.argument("service", required=True, type=click.STRING)
@click.argument("account", required=True, type=click.STRING)
def delete(service: str, account: str):
    """Delete Password."""
    try:
        delete_password(service, account)
        click.secho("Password deleted.", fg="green", bold=True)
    except ValueError:
        click.secho("Password not found.", fg="red")
