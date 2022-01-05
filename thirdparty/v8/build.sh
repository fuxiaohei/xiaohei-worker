#!/usr/bin/env bash

pwd=$(pwd)
version=$(cat version)

echo "deps/v8: get depot_tools"	
git clone --depth=1 https://chromium.googlesource.com/chromium/tools/depot_tools.git
export PATH=$(pwd)/depot_tools:$PATH
gclient

echo "deps/v8: fetch v8"
fetch v8

echo "deps/v8: gclient sync"
cd v8
git checkout $version
gclient sync

mkdir -p out.gn/release
cat <<EOF > out.gn/release/args.gn
is_component_build = false
v8_monolithic = true
use_custom_libcxx = false
v8_enable_i18n_support = true
v8_use_external_startup_data = false
symbol_level = 0
is_debug = false
treat_warnings_as_errors = false
EOF

echo "deps/v8: build v8"
gn gen out.gn/release
ninja -C out.gn/release v8_monolith

echo "deps/v8: copy libs"
cd $pwd
cp -R v8/include ./include
cp v8/out.gn/release/obj/v8_monolith.a ./lib/libv8_monolith.a
