rd /s /q build
mkdir build
cd build
git clone https://github.com/Microsoft/vcpkg
cd vcpkg
powershell -exec bypass scripts\bootstrap.ps1
vcpkg install fltk boost openssl libjpeg-turbo zlib
cd ..
CMake  .. "-DCMAKE_TOOLCHAIN_FILE=build\vcpkg\scripts\buildsystems\vcpkg.cmake"
cd ..
   if [[ "${TRAVIS_OS_NAME}" == "linux" ]]; then
      CMAKE_URL="https://cmake.org/files/v3.7/cmake-3.7.2-Linux-x86_64.tar.gz"
      mkdir cmake && travis_retry wget --no-check-certificate --quiet -O - ${CMAKE_URL} | tar --strip-components=1 -xz -C cmake
      export PATH=${DEPS_DIR}/cmake/bin:${PATH}
    else
      brew upgrade cmake || brew install cmake
    fi
  - cmake --version
 