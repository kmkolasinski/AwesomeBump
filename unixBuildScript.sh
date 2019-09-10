#!/bin/bash

# Add your QT path here by setting MY_QT_PATH variable
# MY_QT_PATH=/YOUR_PATH_HERE/Qt/5.X/gcc_64/bin/
MY_QT_PATH=/opt/Qt5.9.0/5.9/gcc_64/bin/
BUILD_WITH_OPENGL_330_SUPPORT=$1

MAKE_NUM_THREADS='-j 8'
wget="wget"
tool="gcc_64"
exe=""
APP_SUFFIX=""
QMAKE_CONFIG=""

if [[ $BUILD_WITH_OPENGL_330_SUPPORT == "gl330" ]]; then
    QMAKE_CONFIG="CONFIG+=gl330"
    APP_SUFFIX="GL330"
fi

if [[ "$OSTYPE" == "darwin"* ]]; then
	wget="curl -L -o master.zip"
	tool="clang_64"
	exe=".app"
fi

export PATH=$MY_QT_PATH:$PATH

if [ ! -e "$MY_QT_PATH" ]; then
	echo " ---------------------------------"
	echo "      Error: Wrong Qt path."
	echo " ---------------------------------"
	echo " Qt not found at '$MY_QT_PATH'."	
	echo " Please set the MY_QT_PATH variable in the ./unixBuildScript.sh"
	echo ""
	exit 1
fi

if [ "$(ls -A Sources/utils/QtnProperty)" ]; then
    echo "QtnProperty module is already initialized. No action is performed."
else
    echo "Initializing QtnProperty module"
    # Copy QtnProperty directly from the repository
    cd Sources/utils/QtnProperty
    $wget https://github.com/kmkolasinski/QtnProperty/archive/master.zip
    unzip master.zip
    rm master.zip 
    mv QtnProperty-master/* .
    rm -r QtnProperty-master    
    cd ../../../
fi

rm .qmake.stash
rm Makefile
rm Sources/Makefile

${MY_QT_PATH}/qmake ./AwesomeBump.pro ${QMAKE_CONFIG} \
    && make clean && make $MAKE_NUM_THREADS \
	&& echo "*** Copying binary from `cat workdir/current` ..." \
	&& cp -vr workdir/`cat workdir/current`/bin/AwesomeBump$exe ./Bin/AwesomeBump$APP_SUFFIX$exe


