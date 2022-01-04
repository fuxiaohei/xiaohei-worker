#!/usr/bin/env bash

# clone source code
rm -rf libhv-src
echo "deps/hv: git clone https://github.com/ithewei/libhv.git"
git clone --depth=1 https://github.com/ithewei/libhv.git libhv-src

cd libhv-src

# cp openssl
cp -R ../../openssl/include .
cp -R ../../openssl/lib .

# build
mkdir build
cd build
cmake -DCMAKE_C_FLAGS="${DCMAKE_C_FLAGS} -fPIC" -DCMAKE_CXX_FLAGS="${CMAKE_CXX_FLAGS} -fPIC" -DWITH_OPENSSL=ON -DBUILD_EXAMPLES=OFF -DBUILD_SHARED=OFF -DBUILD_STATIC=ON ..
cmake --build . -j 4

os=$(uname | tr '[:upper:]' '[:lower:]')
arch=$(uname -m | tr '[:upper:]' '[:lower:]')
os_arch="$os"_"$arch"

# create tar.gz
cd ../../
rm -rf include/ lib/
cp -R libhv-src/build/include ./include
mkdir -p ./lib
cp -R libhv-src/build/lib/libhv_static.a ./lib/libhv_static.a


echo "deps/hv: libhv is built"

# clean codes
rm -rf libhv-src