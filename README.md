# PyCreds - Python Library to keep credentials safe by storing in platform keystores.

![CI](https://github.com/kumaraditya303/pycreds/workflows/CI/badge.svg) ![](https://img.shields.io/pypi/implementation/pycreds?style=flat-square) ![](https://img.shields.io/pypi/pyversions/pycreds?logo=python&style=flat-square) ![](https://img.shields.io/pypi/v/pycreds?style=flat-square) [![Downloads](https://pepy.tech/badge/pycreds)](https://pepy.tech/project/pycreds)

PyCreds is a Python Library written in C++ to keep credentials safe by storing in platform keystores. On MacOS the passwords are managed by the Keychain, on Linux they are managed by the Secret Service API/libsecret, and on Windows they are managed by Credential Vault.

---

# Features 🚀

- **Cross Platform** - PyCreds is a cross platform solution for storing credentials, Windows, Linux and MacOS are fully supported.
- **Security** - Uses platform specific credential vault for storing credentials.
- **API** - Provides high level operations such as `get_password`, `set_password`, `delete_password`, `find_password` and `find_credentials`.
- **Command Line Interface** - Ships with a Cli based on [click](https://github.com/pallets/click) for Cli usage.

---

# Installation ✔

Install with pip:

```bash
$ pip install pycreds
# Or Install with cli
$ pip install pycreds[cli]
```

---

# Docs

- `get_password(service, account)`

  Get the stored password for `service` and `account`.

  `service` - The string service name.
  `account` - The string account name.

  Returns password as `string` if found else raises `ValueError`.

- `set_password(service, account, password)`

  Save the `password` for `service` and `account`.

  `service` - The string service name.
  `account` - The string account name.
  `password` - The string password.

  Returns `True` if successful else raises `ValueError`.

- `delete_password(service, account)`

  Delete the `password` for `service` and `account`.

  `service` - The string service name.
  `account` - The string account name.

  Returns `True` if successful else raises `ValueError`.

- `find_password(service)`

  Finds `password` for `service`.

  `service` - The string service name.

  Returns password as `string` if found else returns `None`.

- `find_credentials(service)`

  Finds credentials for `service`.

  `service` - The string service name.

  Returns a list of credential dict in format `{"account": "foo", "password": "bar"}`.

---

# Cli Docs

```txt
Usage: pycreds [OPTIONS] COMMAND [ARGS]...

  PyCreds Command Line Interface.

Options:
  --help  Show this message and exit.

Commands:
  delete  Delete Password.
  get     Get Password.
  set     Set Password.
```

---
