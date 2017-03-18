git clone https://github.com/Microsoft/vcpkg
cd vcpkg
powershell -exec bypass scripts\bootstrap.ps1
vcpkg install fltk boost openssl libjpeg-turbo zlib
cd ..
mkdir build
cd build
CMake  .. "-DCMAKE_TOOLCHAIN_FILE=vcpkg\scripts\buildsystems\vcpkg.cmake"
cd ..