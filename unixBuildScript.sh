#!/bin/bash

tool="gcc_64"
exe=""

if [[ "$OSTYPE" == "darwin"* ]]; then
  tool="clang_64"
  exe=".app"
fi

# Add your QT path here
MY_QT_PATH=~/Qt/5.5/$tool/bin/
export PATH=$MY_QT_PATH:$PATH

if [ ! -e "$MY_QT_PATH" ]; then
	echo "Qt not found at $MY_QT_PATH"
	exit 1
fi

qmake \
	&& make \
	&& echo "*** Copying binary from `cat workdir/current` ..." \
	&& cp -vr workdir/`cat workdir/current`/Bin/AwesomeBump$exe ./Bin
