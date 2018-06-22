rem POCO_STATIC=1 - for static build
rem POCO_UNBUNDLED - for no built-in version of libs
rem CMAKE_INSTALL_PREFIX=path - for install path

mkdir cmake-build
cd cmake-build
del CMakeCache.txt

cmake .. -G"Visual Studio 15 2017 Win64" -DCMAKE_INSTALL_PREFIX="/opt/rsys/platform" -DPOCO_ENABLE_SQL_POSTGRESQL=1 -DPOCO_ENABLE_SQL_MYSQL=1 -DPOCO_ENABLE_SEVENZIP=1 -DREMOTING=1 -DPOCO_DISABLE_INTERNAL_OPENSSL=1 -DPOCO_ENABLE_NETSSL_WIN=0 -DPOCO_ENABLE_TESTS=OFF -DPOCO_ENABLE_SAMPLES=OFF
cmake --build . --config Debug --target install
cmake --build . --config RelWithDebInfo --target install
cd ..
