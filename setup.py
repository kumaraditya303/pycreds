# -*- coding: utf-8 -*-
import sys
from pathlib import Path

from pybind11 import get_include
from setuptools import Extension, setup

ext_modules = []

if sys.platform == "win32":
    ext_modules.append(
        Extension(
            "pycreds._pycreds",
            libraries=["advapi32"],
            sources=["./pycreds/pycreds.cpp"],
            include_dirs=[get_include()],
            language="c++",
        )
    )
elif sys.platform == "linux":
    ext_modules.append(
        Extension(
            "pycreds._pycreds",
            libraries=["glib-2.0", "secret-1"],
            sources=["./pycreds/pycreds.cpp"],
            include_dirs=[
                "/usr/include/libsecret-1",
                "/usr/include/glib-2.0",
                "/usr/lib/glib-2.0/include",
                "/usr/lib/x86_64-linux-gnu/glib-2.0/include",
                "/usr/lib64/glib-2.0/include",
                get_include(),
            ],
            language="c++",
        )
    )
elif sys.platform == "darwin":
    ext_modules.append(
        Extension(
            "pycreds._pycreds",
            sources=["./pycreds/pycreds.cpp"],
            include_dirs=[get_include()],
            language="c++",
            extra_compile_args=[
                "-std=c++14",
                "-stdlib=libc++",
            ],
            extra_link_args=["-framework", "Appkit"],
        )
    )

setup(
    name="pycreds",
    author="Kumar Aditya",
    author_email="",
    url="https://github.com/kumaraditya303/pycreds",
    description="Python Library to keep credentials safe by storing in platform keystores.",
    long_description=Path("README.md").read_text(),
    long_description_content_type="text/markdown",
    license="BSD License",
    packages=["pycreds"],
    include_package_data=True,
    zip_safe=False,
    keywords=["credentials", "vault"],
    ext_modules=ext_modules,
    classifiers=[
        "Programming Language :: C++",
        "Programming Language :: Python",
        "Programming Language :: Python :: 3",
        "Programming Language :: Python :: 3.7",
        "Programming Language :: Python :: 3.8",
        "Programming Language :: Python :: 3.9",
        "Programming Language :: Python :: 3 :: Only",
        "Programming Language :: Python :: Implementation :: CPython",
        "Development Status :: 5 - Production/Stable",
        "Operating System :: OS Independent",
        "Topic :: Security",
        "License :: OSI Approved :: BSD License",
    ],
    use_scm_version=True,
    python_requires=">=3.7",
    extras_require={
        "cli": ["click==8.0.4"],
    },
    entry_points={"console_scripts": ["pycreds=pycreds.cli:main"]},
)
