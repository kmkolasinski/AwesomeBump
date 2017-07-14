#!/bin/bash

# additinal modules:
# ------------------
#  https://github.com/dbzhang800/KDUpdater.git
#  https://github.com/velnias75/QGitHubReleaseAPI

# Add your QT path here by setting QT_PATH variable
# QT_PATH=/YOUR_PATH_HERE/Qt/5.X/gcc_64/bin/

locations="\
    /Developer/Qt/5.9 \
    ~/Qt/5.9 \
    /opt/Qt5.7.0/5.7/gcc_64/bin \
"
for p in $locations; do
	if [ -e "$p" ]; then
        QT_PATH=$p
	fi
done

if [ ! -e "$QT_PATH" ]; then
    echo "Qt not found at any known locations. Exit."
fi

echo "Qt at path: $QT_PATH"

wget="wget"
tool="gcc_64"
peg="bin-linux"
exe=""

if [[ "$OSTYPE" == "darwin"* ]]; then
	wget="curl -L -o master.zip"
	tool="clang_64"
    peg="bin-osx"
	exe=".app"
fi

export PATH=$QT_PATH:$PATH

if [ ! -e "$QT_PATH" ]; then
	echo " ---------------------------------"
	echo "      Error: Wrong Qt path."
	echo " ---------------------------------"
	echo " Qt not found at '$QT_PATH'."
	echo " Please set the QT_PATH variable in the ./unixBuildScript.sh"
	echo ""
	exit 1
fi

if [ "$(ls -A Sources/utils/QtnProperty)" ]; then
    echo "*** QtnProperty module is already initialized. No action is performed."
else
    echo "*** Initializing QtnProperty module"
    # Copy QtnProperty directly from the repository
    pushd Sources/utils/QtnProperty
    $wget https://github.com/kmkolasinski/QtnProperty/archive/master.zip
    unzip master.zip
    rm master.zip 
    mv QtnProperty-master/* .
    rm -r QtnProperty-master    
    popd
fi

if Sources/utils/QtnProperty/$peg/QtnPEG > /dev/null ; then
    qmake \
        && make \
        && echo "*** Copying binary from `cat workdir/current` ..." \
        && cp -vr workdir/`cat workdir/current`/bin/AwesomeBump$exe ./Bin
else
	echo " --------------------------------------"
    echo "      Error: QtnPEG failed to run."
	echo " --------------------------------------"
    echo "Try to rebuild the QtnPEG binary from Sources/utils/QtnProperty directory:"
    echo "$QT_PATH/qmake Property.pro"
fi
