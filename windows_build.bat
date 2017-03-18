CMake  .
git clone https://github.com/Microsoft/vcpkg
cd vcpkg
powershell -exec bypass scripts\bootstrap.ps1
vcpkg install fltk boost openssl libjpeg-turbo zlib