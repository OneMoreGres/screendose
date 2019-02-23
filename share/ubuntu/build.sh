#!/bin/bash

set -e

SELF_PATH="${0%/*}"
ROOT="$SELF_PATH/../.."

# setup
if [ ! -z "$QT_BIN" ]; then export PATH="$QT_BIN:$PATH";
elif [ -d "/opt/qt/default/bin" ]; then export PATH="/opt/qt/default/bin:$PATH";
elif [ -d "/opt/qt/bin" ]; then export PATH="/opt/qt/bin:$PATH";
elif [ -d "/usr/local/opt/qt/bin" ]; then export PATH="/usr/local/opt/qt/bin:$PATH";
fi

export VERSION=`cat "$ROOT/version"`
echo `g++ --version`
echo `qmake --version`


# build
lrelease "$ROOT/src/screendose.pro"
qmake "$ROOT/"
make -j`nproc`


# pack
strip screendose
make INSTALL_ROOT=appdir install
linuxdeployqt --appimage-extract
unset QTDIR
unset QT_PLUGIN_PATH
unset LD_LIBRARY_PATH
#./squashfs-root/AppRun ./appdir/usr/share/applications/*.desktop -bundle-non-qt-libs
./squashfs-root/AppRun ./appdir/usr/share/applications/*.desktop -appimage
mv ScreenDose-*.AppImage screendose-$VERSION-x64.appimage

ls -l `pwd`/screendose-$VERSION-x64.appimage

echo "Finished"
