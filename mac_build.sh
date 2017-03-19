rm -rf build
mkdir build
cd build
xcode-select --install
/usr/bin/ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)" </dev/null
brew install fltk boost openssl libjpeg-turbo
cmake -G Xcode -DOPENSSL_ROOT_DIR=/usr/local/opt/openssl/ ..
cd ..