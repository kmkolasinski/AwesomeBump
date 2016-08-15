New in version 5.0  
==================

- New GUI done with QtnProperty
- And minor features.
- Minor Fixes

New in version 4.0  
==================

- Grunge texture added
- Angle correction algorithm added when converting from Base map.
- And some minor fixes.

New in version Pi (3.1-
========================

- Added tessellation grid preview button
- Added normal map mixer tool
- Font size can be channged
- DoF filter in 3D window added
- Mipmap levels in "base map to others" tool
- Materials texture tool added
- Now all images work with: drag and drop to materials, copy from clipboard, copy to clipboard
- Project manager filter tool
- Mesh can be dragged to the 3D widget
- In "bump mapping" roughness texture is taken as glossines texture
- Apply UV transformation button added
- Plus some bug fixes and minor changes.

New in version 3.0 + update 3.0.2 (2015-03-29)
==============================================

- Added error logger
- Remove lightning with new AO cancellation feature added.
- Roughness and metallic textures are now supported (PBR shading in action). Additionally new tools (noise filter and color picker) to manipulate these textures are available.
- Color Hue slider added.
- Input image feature for most of textures are available now.
- Selective blur feature (height texture only) is now available.
- And other important changes...
- Thanks to David for new cool models :)
- Update 3.0.1: Improved seamless alhorithms, improved GUI, mouse loop added
- Update 3.0.2: Added bloom effect

New in version 2.1
==================

- Added support for 3D models (only OBJ format)
- Simple project manager
- And few less important.


New in version 2.0
==================

- Slightly redesigned GUI: Tiling/UV methods, General Settings and 3D
  preview sliders are now separated into different tabs.
- Added short explanation about most important algorithms in AB. See ShortAbout.pdf file.
- New random tiling algorithm was added. Which allow to generate
  infinite number of seamless textures.
- New perspective mode - image can be now stretched along x and y axis,
  which is useful during the perspective manipulation.
- Height calculator tool was added. Now the depth of the Normal texture
  can be controlled based on physical dimensions.  
- Size of the texture can be changes at run-time.
- Gray Scale manipulator - now you can choose what weights are used to
  convert image to Gray scale.
- Colour levelling tool was added. This can be used e.g. to make flat
  surfaces.  
- Now the normal image is by default attached to height texture, which
  means any change of height will affect the normals.


Minor changes:

- Unused button in AO tab was removed.
- Fixed problem with "cannot load image".
- Medium detail algorithm changed - it should give better results.
- Redefined the normal step slider, now it has more intuitive usage.
- You can choose your preferred GUI style.
- add many others small improvements.

New in version 1.0
==================

- log.txt file is created during each run. This file contains
  information about possible errors and program outputs. In case some
problems (eq. GL version incompatibility) see this file.
- New camera.cpp class introduced in order to improve 3D image
  manipulation. Right button can be used to pan 3D plane. Left to rotate
  camera around paned point.
- Now 2D image can be dragged with mouse right button  and zoomed with
  mouse wheel.
- New perspective tool funcionality added. You can stretch each corner
  of 2D image in order to correct and align image. Reset transform button
  restore settings.
- Added support for TGA files (import and export).
