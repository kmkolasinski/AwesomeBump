![Image](https://github.com/kmkolasinski/AwesomeBump/blob/Release/Sources/resources/promo/githubimage5.jpg)


AwesomeBump 5.1 (2019)
====================

AwesomeBump is a free and open source program written using Qt library designed to generate normal, height, specular or ambient occlusion, metallic, roughness textures from a single image. Additional features like material textures or grunge maps are available. Since the image processing is done in 99% on GPU the program runs very fast and all the parameters can be changed in real time. AB was made to be a new alternative to known gimp plugin called Insane Bump or commercial tool: Crazy Bump.

**AwesomeBump** is totaly written in Qt thus you don’t need to install any aditionall libraries. Just download and install Qt SDK, download the project from the repository, build and run. It will work (or should) on any platform supported by Qt.

#### What can AwesomeBump do?

* convert from normal map to height map,
* convert from height map (bump map) to normal map,
* extract the bump from the arbitrary image,
* calculate ambient occlusion and specularity of image.
* perspective tranformation of the image,
* creating seamless texture (simple linear filter, random mode, or mirror filter),
* generate roughness and metallic textures (different types of surface analysis are available),
* real time tessellation is available,
* saving images to following formats: PNG, JPG,BMP,TGA
* edit one texture which contains different materials
* add some grunge to your map with grunge texture.
* mix two bumpmaps togheter with normal map mixer.
* and many others, see our videos on YouTube

#### Contributors:
I would like to thanks those people for thier big effort to make this software more awesome!

* [ppiecuch](https://github.com/ppiecuch)
* [hevedy](https://github.com/Hevedy)
* [robert42](https://github.com/Robert42)
* [CodePhase](https://github.com/CodePhase)
* [Calinou](https://github.com/Calinou)
* and many others !!!

#### Need help???
Feel free to contact me if you find any bugs and problems: <awesomebump.help@gmail.com>

If you want to fix or improve something, please fork my repository and send me the pull-request with changes.

Binary packages
============
Binary packages for selected system can be found at [link](https://github.com/kmkolasinski/AwesomeBump/releases). Download zip or tar file, extract and run binary file.

* [ABv5.1 Ubuntu 18LTE](https://github.com/kmkolasinski/AwesomeBump/releases/tag/Linuxv5.1.1) (Qt 5.9)
* [ABv5.0 Ubuntu 14LTE](https://github.com/kmkolasinski/AwesomeBump/releases/tag/Linuxv5.0)

* [ABv5.1 Windows 7/8/10](https://github.com/kmkolasinski/AwesomeBump/releases/tag/Winx32v5.1)
* [ABv5.0 Windows 7](https://github.com/kmkolasinski/AwesomeBump/releases/tag/Winx32v5.0)


Building from source
============
* Since version 5.0 you can use UNIX build script sh **unixBuildScript.sh** (see description below) for building AB on UNIX systems. 
 

Since version 3.0
-----------------

The best place to start with compiling/installing would be to read the appropriate sections in the [wiki ](https://github.com/kmkolasinski/AwesomeBump/wiki). (note that the current version of cmake script does not work :()

For those using QtCreator, the steps are almost the same as for earlier versions.
See the [pdf ](https://github.com/kmkolasinski/AwesomeBump/releases/download/BuildingAB/BuildingInstruction.pdf) for instructions.

For versions older than 3.0
---------------------------

1. Video tutorial: https://www.youtube.com/watch?v=R-WgHserrS0
2. In order to build AwesomeBump you will need to have qt-sdk installed: http://www.qt.io/download/
3. Download the sourcecode of AB from github and unzip it. Run Qt then open a new project using the AwesomeBump.pro file (located in AB source code folder.).
4. Setup the build path and run path (in project build settings panel) and compile the project (remember that your graphics card has to support at least openGL 4.0). Compilation step will take some time. Run the program. In case of problems see the `log.txt` file.

Unix (bash) automated script
-------------------------

1. Open terminal and unzip or untar downloaded source code.
2. Enter to created folder
3. Open **unixBuildScript.sh** file and set the propper Qt5 path. Save and close.
4. Run script with command: `sh unixBuildScript.sh`
5. If everything gone well AB should start after compilation process. See **Bin/** folder. There should be an AwesomeBump file now.
6. Run AwesomeBump with `./RunAwesomeBump.sh` script. 

Windows (Qt 5.4.2 + Mingw x86) steps (thanks to Andrey Kuznetsov)
-------------------------

* Download sources with git
 1. `git clone https://github.com/kmkolasinski/AwesomeBump`
 2. `git submodule init`
 3. `git submodule update`

* Build QtnProperty (see below)
 1. Download win_flex and win_bison (see ![QtProperty](https://github.com/kmkolasinski/QtnProperty/tree/af948d54ad25755609cdcaf6f15cb58302ee8b91) project instructions) and paste it, e.g., here: `D:\win_flex`
 2. Comment `QMAKE_CXXFLAGS += /wd4065 in PEG.pro`
 3. In Bison.pri check win_bison command. For me this worked
 ```
 win32:bison.commands = D:\win_flex\win_bison -d -o ${QMAKE_FILE_PATH}/${QMAKE_FILE_BASE}.parser.cpp ${QMAKE_FILE_IN}
 ```
 4. In Flex.pri check win_flex command. For me this worked
 ```
 win32:flex.commands = D:\win_flex\win_flex --wincompat -o ${QMAKE_FILE_PATH}/${QMAKE_FILE_BASE}.lexer.cpp ${QMAKE_FILE_IN}
 ```
* OpenGL 3.30 support

  `For OpenGL 3.3 use #define USE_OPENGL_330 in CommonObjects.h (this can be defined in QtCreator see pdf instruction)`
  
* Build and Run using QtCreator.

### OpenGL 3.30 support

You can now build AB to run all openGL instructions with 3.30 compatibility (note that tessellation will not work with 3.30). See [PDF ](https://github.com/kmkolasinski/AwesomeBump/releases/download/BuildingAB/BuildingInstruction.pdf) file and "Step 8" for more datails. Basically you just have to add `CONFIG+=gl330` command in the qmake settings in order to build 3.30-supported version of AwesomeBump.

License
=======

 * AB is using tinyobjloader for loading the OBJ files: https://github.com/syoyo/tinyobjloader
 * Cube maps textures were taken from Humus page [link](http://www.humus.name/index.php?page=Textures)
 * Most of the GUI controls are done with  [QtnProperty](https://github.com/lexxmark/QtnProperty) framework.


This program is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with this program. If not, see <http://www.gnu.org/licenses/>.
