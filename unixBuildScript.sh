#!/bin/bash

# Add your QT path here by setting MY_QT_PATH variable
# MY_QT_PATH=/YOUR_PATH_HERE/Qt/5.X/gcc_64/bin
# MY_QT_PATH=/opt/Qt5.9.0/5.9/gcc_64/bin
if [ "x$MY_QT_PATH" = "x" ]; then
    MY_QT_PATH=/usr/bin
fi
if [ "x$MY_QMAKE" = "x" ]; then
    MY_QMAKE=qmake-qt5
fi
_DO_INSTALL=false
for var in "$@"
do
    # Each preceding x in this file is to prevent script injection.
    if [ "x$var" = "xinstall" ]; then
        _DO_INSTALL=true
    fi
done
me="`basename $0`"
if [ ! -f "$MY_QT_PATH/$MY_QMAKE" ]; then
    >&2 cat <<END

Error: $MY_QT_PATH/$MY_QMAKE doesn't exist. Change MY_QT_PATH to the
directory containing qmake-qt5 and if you don't want to use qmake-qt5
change MY_QMAKE to the path relative to MY_QT_PATH, then
run:

    export MY_QT_PATH
    export MY_QMAKE
    ./$me

...or if you don't have qmake-qt5,
END
    if [ -f "`command -v dnf`" ]; then
        >&2 cat <<END
first install the following rpm packages:
    sudo dnf install -y qt5-qtbase-devel qt5-qtscript-devel
END
    else
        >&2 cat <<END
try installing the following deb packages:
    qtbase5-dev qt5-qtscript-dev
END
    fi
    exit 1
fi

if [ ! -d "Sources" ]; then
    echo "Error: Sources doesn't exist in `pwd`."
    echo " You must run this from the AwesomeBump repo."
    exit 1
fi
Icon="`pwd`/Sources/resources/icons/icon.png"  # Diffuse map
Exec="`pwd`/Bin/AwesomeBump"
Path="`pwd`/Bin"
SC_NAME=AwesomeBump.desktop
SC_TMP="$Path/$SC_NAME"
# ^ use SC_NAME since that is also used to detect the destination.


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

${MY_QT_PATH}/$MY_QMAKE ./AwesomeBump.pro ${QMAKE_CONFIG} \
    && make clean && make $MAKE_NUM_THREADS \
	&& echo "*** Copying binary from `cat workdir/current` ..." \
	&& cp -vr workdir/`cat workdir/current`/bin/AwesomeBump$exe ./Bin/AwesomeBump$APP_SUFFIX$exe
code=$?
if [ $code -ne 0 ]; then
    echo "Error: $MY_QMAKE returned code $code."
    exit $code
fi

SC_PATH=~/Desktop/$SC_NAME
if [ -f "$Exec" ]; then
    echo "[$me] * Writing $SC_PATH (from $SC_TMP) for $Exec..."
    cat >$SC_TMP <<END
[Desktop Entry]
Name=AwesomeBump (git)
Exec=$Exec
Path=$Path
Icon=$Icon
Terminal=false
Type=Application
GenericName=AwesomeBump
Comment=AwesomeBump
Keywords=3d;cg;texturing;bump;
Categories=Graphics;3DGraphics;
END
if [ ! -f "$SC_TMP" ]; then
    echo "Error: writing $SC_TMP failed."
    exit 1
fi
chmod -x $SC_TMP

if [ "x$_DO_INSTALL" = "xtrue" ]; then
    if [ ! -f "`command -v xdg-desktop-icon`" ]; then
        _DO_INSTALL=false
        echo "Error: The xdg-desktop-icon command was not found, so $SC_TMP was not installed."
    fi
fi

if [ "x$_DO_INSTALL" = "xtrue" ]; then
    xdg-desktop-icon install $SC_TMP --novendor
    if [ $? -ne 0 ]; then
        echo "Installing $SC_PATH from $SC_TMP failed."
    else
        echo "The desktop shortcut to $Exec was installed."
    fi
    # chmod +x $SC_PATH
    # ^ +x should already be done on $SC_TMP, but do it on $SC_PATH just to make sure.
else
    cat <<END
To use directly from $Path, install the run-in-place icon:
    xdg-desktop-icon install $SC_TMP --novendor
    chmod +x $SC_PATH
    # or run:
    # ./$me install
END
fi

cat <<END
To uninstall the icon later, run:
    xdg-desktop-icon uninstall $SC_PATH

END
else
    echo "Error: $MY_QMAKE didn't produce '$Exec'."
fi
