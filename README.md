

![Image](https://github.com/kmkolasinski/AwesomeBump/blob/master/Sources/resources/githubimage3.jpg)

**Official AB website is down** :( But in future I will move all the information here on GitHub server.

Info
====

Since I have a lot of work now (from June to September) I have no time for AB programming, so there will be a long break before next commits and upgrades. During this time you (AB users) could help other users to solve thier problems reported in the Issues tab.


AwesomeBump 4.0 Beta
====================

AwesomeBump is a free and open source program written using Qt library designed to generate normal, height, specular or ambient occlusion, metallic, roughness textures from a single image. Additional features like material textures or grunge maps are available. Since the image processing is done in 99% on GPU the program runs very fast and all the parameters can be changed in real time. AB was made to be a new alternative to known gimp plugin called Insane Bump or commercial tool: Crazy Bump.

Feel free to contact me if you find any bugs and problems: <awesomebump.help@gmail.com>

If you want to fix or improve something, please fork my repository and send me the pull-request with changes.

AB is using tinyobjloader for loading the OBJ files: https://github.com/syoyo/tinyobjloader

Cube maps textures were taken from Humus page [link](http://www.humus.name/index.php?page=Textures)

Project Webpage: http://awesomebump.besaba.com/

### Download binaries from [here](https://github.com/kmkolasinski/AwesomeBump/releases) (.zip or .tar.gz files).



Installation
============

Since version 3.0
-----------------

Steps are almost the same as for earlier versions.
See the [pdf ](https://github.com/kmkolasinski/AwesomeBump/releases/download/BuildingAB/BuildingInstruction.pdf) for instructions.



For versions older than 3.0
---------------------------

1. Video tutorial: https://www.youtube.com/watch?v=R-WgHserrS0
2. In order to build AwesomeBump you will need to have qt-sdk installed: http://www.qt.io/download/
3. Download the sourcecode of AB from github and unzip it. Run Qt then open a new project using the AwesomeBump.pro file (located in AB source code folder.).
4. Setup the build path and run path (in project build settings panel) and compile the project (remember that your graphics card has to support at least openGL 4.0). Compilation step will take some time. Run the program. In case of problems see the `log.txt` file.
5. I hope you will like it :)

Command-line instructions
-------------------------

1. Open terminal and unzip or untar downloaded source code.

2. Enter to created folder

3. Open linuxBuildScript.sh file and set the propper Qt5 path. Save and close.

4. Run script with command: sh linuxBuildScript.sh

5. If everything gone well AB should start after compilation process. See Bin/ folder. There should be an AwesomeBump file now.

### OpenGL 3.30 support

You can now build AB to run all openGL instructions with 3.30 compatibility (note that tessellation will not work with 3.30). See [PDF ](https://github.com/kmkolasinski/AwesomeBump/releases/download/BuildingAB/BuildingInstruction.pdf) file and "Step 8" for more datails. Basically you just have to add `CONFIG+=release_gl330` command in the qmake settings in order to build 3.30-supported version of AwesomeBump.

License
=======

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
