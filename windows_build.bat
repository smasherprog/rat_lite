rd /s /q build
mkdir build
cd build
git clone https://github.com/Microsoft/vcpkg
cd vcpkg
powershell -exec bypass scripts\bootstrap.ps1
vcpkg install fltk boost openssl libjpeg-turbo zlib libuv
cd ..
CMake  .. "-DCMAKE_TOOLCHAIN_FILE=build\vcpkg\scripts\buildsystems\vcpkg.cmake"
cd ..
