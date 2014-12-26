AwesomeBump  1.0
===========

AwesomeBump is a free and open source program written using Qt library designed to generate normal, height, specular or ambient occlusion textures from a single image. Since the image processing is done in 99% on GPU the program runs very fast and all the parameters can be changed in real time. AB was made to be a new alternative to known gimp plugin called Insane Bump or commercial tool: Crazy Bump.  

Feel free to contact me if you find any bugs and problems: support@awesomebump.besaba.com

If you want to fix or improve something please fork my repo and send me the pull-request with changes. 


INSTALLATION:
============
0. Video tutorial: https://www.youtube.com/watch?v=R-WgHserrS0
1. In order to build AwesomeBump you will need to have qt-sdk installed: http://www.qt.io/download/ 
2. After installation open  new project using the AwesomeBump.pro file.
3. Setup the build path and run path (in project build settings panel) and compile the project (remember that your graphics card has to support at least openGL 4.0)
4. I hope you will like it :)


Project Webpage: http://awesomebump.besaba.com/

New in version 1.0:
============
1) log.txt file is created during each run. This file contains
   information about possible errors and program outputs. In case some
   problems (eq. GL version incompatibility) see this file.
   
2) New camera.cpp class introduced in order to improve 3D image
   manipulation. Right button can be used to pan 3D plane. Left to rotate
   camera around paned point.
   
3) Now 2D image can be dragged with mouse right button  and zoomed with
   mouse wheel.
   
4) New perspective tool funcionality added. You can stretch each corner
   of 2D image in order to correct and align image. Reset transform button
   restore settings.
   
5)  Added support for TGA files (import and export).



