#!/bin/bash

set -e

source /usr/local/share/liri-travis/functions

# Install artifacts
travis_start "artifacts"
msg "Install artifacts..."
/usr/local/bin/liri-download-artifacts $TRAVIS_BRANCH cmakeshared-artifacts.tar.gz
travis_end "artifacts"

# Configure
travis_start "configure"
msg "Setup CMake..."
mkdir build
cd build
if [ "$CXX" == "clang++" ]; then
    clazy="-DLIRI_ENABLE_CLAZY:BOOL=ON"
fi
cmake .. $clazy \
    -DCMAKE_INSTALL_PREFIX=/usr \
    -DINSTALL_LIBDIR=/usr/lib64 \
    -DINSTALL_QMLDIR=/usr/lib64/qt5/qml \
    -DINSTALL_PLUGINSDIR=/usr/lib64/qt5/plugins
travis_end "configure"

# Build
travis_start "build"
msg "Build..."
make -j $(nproc)
travis_end "build"

# Install
travis_start "install"
msg "Install..."
make install
travis_end "install"

# Test
travis_start "test"
msg "Test..."
dbus-run-session -- \
    xvfb-run -a -s "-screen 0 800x600x24" \
    ctest -V
travis_end "test"

# Package
travis_start "package"
msg "Package..."
mkdir -p artifacts
tar czf artifacts/libliri-artifacts.tar.gz -T install_manifest.txt
travis_end "package"
