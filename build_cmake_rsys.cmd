rem POCO_STATIC=1 - for static build
rem POCO_UNBUNDLED - for no built-in version of libs
rem CMAKE_INSTALL_PREFIX=path - for install path


mkdir cmake-build


cd cmake-build


cmake .. -DCMAKE_BUILD_TYPE=Debug -G"NMake Makefiles JOM" -DCMAKE_INSTALL_PREFIX=/opt/rsys/platform -DREMOTING=1 -DPOCO_DISABLE_INTERNAL_OPENSSL=1 -DPOCO_ENABLE_NETSSL_WIN=0
jom /j12

jom install


del CMakeCache.txt



cmake .. -DCMAKE_BUILD_TYPE=Release -G"NMake Makefiles JOM" -DCMAKE_INSTALL_PREFIX="/opt/rsys/platform" -DREMOTING=1 -DPOCO_DISABLE_INTERNAL_OPENSSL=1 -DPOCO_ENABLE_NETSSL_WIN=0

jom /j12

jom install



cd ..
