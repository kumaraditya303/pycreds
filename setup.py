# -*- coding: utf-8 -*-
import platform
from pathlib import Path

from setuptools import Extension, setup

ext_modules = []

if "Windows" == platform.system():
    ext_modules.append(
        Extension(
            "pycreds._pycreds",
            libraries=["advapi32"],
            sources=["./pycreds/pycreds_win.cpp"],
            include_dirs=["./third_party/pybind11/include"],
            language="c++",
            extra_compile_args=["/MT"],
        )
    )
elif "Linux" == platform.system():
    ext_modules.append(
        Extension(
            "pycreds._pycreds",
            libraries=["glib-2.0", "secret-1"],
            sources=["./pycreds/pycreds_posix.cpp"],
            include_dirs=[
                "/usr/include/libsecret-1",
                "/usr/lib64/glib-2.0/include",
                "/usr/lib/x86_64-linux-gnu/glib-2.0/include",
                "/usr/include/glib-2.0",
                "./third_party/pybind11/include",
            ],
            language="c++",
        )
    )
elif "Darwin" == platform.system():
    ext_modules.append(
        Extension(
            "pycreds._pycreds",
            sources=["./pycreds/pycreds_darwin.cpp"],
            include_dirs=["./third_party/pybind11/include"],
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
        "cli": ["click==7.1.2"],
    },
    entry_points={"console_scripts": ["pycreds=pycreds.cli:main"]},
    setup_requires=["setuptools_scm", "wheel"],
)
