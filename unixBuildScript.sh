#!/bin/bash

tool="gcc_64"
exe=""

if [[ "$OSTYPE" == "darwin"* ]]; then
  tool="clang_64"
  exe=".app"
fi

# Add your QT path here
MY_QT_PATH=~/Qt/5.4/$tool/bin/
export PATH=$MY_QT_PATH:$PATH

( cd Sources && qmake && make ) \
	&& cp -r Sources/Build/Bin/AwesomeBump$exe ./Bin \
	&& open ./Bin/AwesomeBump$exe
