#!/bin/bash
# Build the package for distribution on a manylinux docker
# You still have to do the repair and the upload after this

if [ -z ${PYBIN+"test"} ]
    then echo "PYBIN is not set"
    exit
fi

$PYBIN/pip install cmake cython
mkdir -p cpp_build
cd cpp_build
$PYBIN/cmake ../../../ -DCMAKE_BUILD_TYPE=Release
make -j
cd ..
$PYBIN/cython --cplus umo.pyx
$PYBIN/python setup.py sdist bdist_wheel


