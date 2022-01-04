#!/usr/bin/env bash

pwd=$(pwd)

openssl_name=openssl-1.1.1m
openssl_tar=${openssl_name}.tar.gz
openssl_tar_url=https://www.openssl.org/source/${openssl_tar}
openssl_target=${pwd}/${openssl_name}/target

# clone source code
echo "deps/openssl: ${openssl_tar} start download"
curl --output ${openssl_tar} ${openssl_tar_url}
echo "deps/openssl: ${openssl_tar} download ok"
tar -xzvf ${openssl_tar}

# build source
cd ${openssl_name}
./config --prefix=${openssl_target}
make -j4
make install

cd ../
rm -rf include/ lib/
cp -R ${openssl_target}/include ./include
mkdir -p ./lib
cp -R  ${openssl_target}/lib/libcrypto.a ./lib/libcrypto.a
cp -R  ${openssl_target}/lib/libssl.a ./lib/libssl.a


echo "deps/openssl: libcrypto and libssl are built"

# clean codes
rm -rf ${openssl_name}
rm -rf ${openssl_tar}