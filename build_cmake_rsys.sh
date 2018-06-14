#!/bin/bash

#rm -rf cmake-build
#cmake ../. -DCMAKE_BUILD_TYPE=Debug -DCMAKE_INSTALL_PREFIX=/opt/rsys/platform -DREMOTING=1 -DPOCO_ENABLE_PDF=1 -DPOCO_ENABLE_SEVENZIP=1 -DPOCO_UNBUNDLED=1
#make -j`nproc`
#make install

rm -rf cmake-build
cmake -H. -Bcmake-build -DCMAKE_BUILD_TYPE=Debug -DCMAKE_INSTALL_PREFIX=/opt/rsys/platform -DREMOTING=1 -DPOCO_ENABLE_PDF=1 -DPOCO_ENABLE_SEVENZIP=1 -DPOCO_UNBUNDLED=1
cmake --build cmake-build --target all -- -j`nproc`
cmake --build cmake-build --target install

rm -rf cmake-build
cmake -H. -Bcmake-build -DCMAKE_BUILD_TYPE=RelWithDebInfo -DCMAKE_INSTALL_PREFIX=/opt/rsys/platform -DREMOTING=1 -DPOCO_ENABLE_PDF=1 -DPOCO_ENABLE_SEVENZIP=1 -DPOCO_UNBUNDLED=1
cmake --build cmake-build --target all -- -j`nproc`
cmake --build cmake-build --target install

