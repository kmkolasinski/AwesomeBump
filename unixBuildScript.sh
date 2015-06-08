#!/bin/bash

tool="gcc_64"

if [[ "$OSTYPE" == "darwin"* ]];
  tool="clang_64"
fi

# Add your QT path here
MY_QT_PATH=your_similar_qt_path/Qt/5.4/$tool/bin/

export PATH=$MY_QT_PATH:$PATH
(cd Sources && qmake && make)
cp AwesomeBump ./Bin/
(cd ./Bin && ./AwesomeBump)

