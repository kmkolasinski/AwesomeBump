#!/bin/bash

tool="gcc_64"
exe=""

if [[ "$OSTYPE" == "darwin"* ]]; then
	tool="clang_64"
	exe=".app"
fi

# Add your QT path here
MY_QT_PATH=/home/mkk/Programs/Qt/5.4/gcc_64/bin/
export PATH=$MY_QT_PATH:$PATH

if [ ! -e "$MY_QT_PATH" ]; then
	echo "Qt not found at $MY_QT_PATH"
	exit 1
fi

if [ "$(ls -A Sources/utils/QtnProperty)" ]; then
    echo "QtnProperty module is already initialized. No action is performed."
else
    echo "Initializing QtnProperty module"
    # Copy QtnProperty directly from the repository
    cd Sources/utils/QtnProperty
    wget https://github.com/kmkolasinski/QtnProperty/archive/master.zip
    unzip master.zip
    rm master.zip 
    mv QtnProperty-master/* .
    rm -r QtnProperty-master    
    cd ../../../
fi

qmake \
	&& make \
	&& echo "*** Copying binary from `cat workdir/current` ..." \
	&& cp -vr workdir/`cat workdir/current`/bin/AwesomeBump$exe ./Bin
