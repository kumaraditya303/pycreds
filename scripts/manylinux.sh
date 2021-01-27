#!/usr/bin/env bash

mkdir -p /tmp/temp-wheels

find /tmp/temp-wheels/ -type f -delete

yum install libsecret-devel -y

for PYBIN in /opt/python/cp3[789]*/bin; do
    "${PYBIN}/pip" install -U pip build --no-cache-dir
    (cd /io/ && "${PYBIN}/python" -m build -o /tmp/temp-wheels)
done
"$PYBIN/pip" install auditwheel

mkdir -p /io/dist/

for whl in /tmp/temp-wheels/*.whl; do
    auditwheel repair "$whl" -w /io/dist/
done
