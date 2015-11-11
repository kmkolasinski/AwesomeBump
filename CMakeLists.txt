cmake_minimum_required(VERSION 2.6)
project(awesomebump)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

# This is the default install directory prefix for external resources
# from the source's Bin/ directory
# Only used when doing "Release" builds
set(RESOURCE_DEFAULT lib64/awesomebump)

# Setting the now REQUIRED base directory for resource files in Bin/
# if a custom path was not provided by the user at compile time
# This path is referenced in code when loading these resources
if(NOT RESOURCE_BASE)
  if(CMAKE_BUILD_TYPE MATCHES "Release")
    # Default path for "Release" builds
    # AB must be installed via 'make install' for the program to work
    set(RESOURCE_BASE ${CMAKE_INSTALL_PREFIX}/${RESOURCE_DEFAULT})
  else()
    # "Debug" and other builds will install resources to the same
    # dir as the executable if no path is provided at compile time
    # Install is optional; you can just copy Bin/* to the binary's dir
    # or move the binary to Bin/
    # Installing via 'make install' will copy the resource files to
    # the binary's directory for you to make things easier though.
    set(RESOURCE_BASE ".")
    install(DIRECTORY Bin/ DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
  endif()
endif()

# Making sure the required dependencies are present on the build system
# This list probably needs to be expanded
find_package(Qt5Core REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5OpenGL REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(Qt5DBus REQUIRED)
find_package(OpenGL REQUIRED)

# Including support for OpenGL 3.3.0
# Use the option -Drelease_gl330=1 to trigger it
if(release_gl330)
  add_definitions(-DUSE_OPENGL_330)
endif()

# Including source files and setting flags for the build
add_definitions(${Qt5Widgets_DEFINITIONS} -DRESOURCE_BASE="${RESOURCE_BASE}/")
qt5_wrap_ui(UI_HEADERS Sources/allaboutdialog.ui Sources/dialogheightcalculator.ui Sources/dialoglogger.ui
    Sources/dialogshortcuts.ui Sources/formbasemapconversionlevels.ui Sources/formimageprop.ui
    Sources/formmaterialindicesmanager.ui Sources/formsettingscontainer.ui Sources/formsettingsfield.ui
    Sources/mainwindow.ui )
qt5_add_resources(UI_RESOURCES Sources/content.qrc)

set(CMAKE_CXX_FLAGS "${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS}")
set(AwesomeBump_SRCS
    Sources/utils/Mesh.cpp Sources/utils/qglbuffers.cpp
    Sources/utils/tinyobj/tiny_obj_loader.cc Sources/CommonObjects.cpp
    Sources/allaboutdialog.cpp Sources/camera.cpp Sources/dialogheightcalculator.cpp
    Sources/camera.cpp Sources/dialogheightcalculator.cpp Sources/camera.cpp
    Sources/dialogheightcalculator.cpp Sources/camera.cpp
    Sources/dialogheightcalculator.cpp Sources/dialoglogger.cpp Sources/dialogshortcuts.cpp
    Sources/dialoglogger.cpp Sources/dialogshortcuts.cpp Sources/formimagebase.cpp
    Sources/formbasemapconversionlevels.cpp Sources/formimagebase.cpp
    Sources/formbasemapconversionlevels.cpp Sources/formimageprop.cpp
    Sources/formmaterialindicesmanager.cpp Sources/formsettingscontainer.cpp
    Sources/formsettingsfield.cpp Sources/glimageeditor.cpp Sources/glwidget.cpp
    Sources/glwidgetbase.cpp Sources/mainwindow.cpp Sources/main.cpp) 

# Configure the linker and finalize binary compilation
add_executable(awesomebump ${AwesomeBump_SRCS} ${UI_HEADERS} ${UI_RESOURCES} Sources/resources/icon.icns)
target_link_libraries(awesomebump Qt5::Core Qt5::DBus Qt5::Gui Qt5::Widgets Qt5::OpenGL
    GL)

# Create an install target for "Release" builds using custom or default binary and
# resource file paths
if(${CMAKE_BUILD_TYPE} MATCHES "Release")
  install(TARGETS awesomebump RUNTIME DESTINATION bin)
  install(DIRECTORY Bin/ DESTINATION ${RESOURCE_BASE})
endif()

configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/cmake_uninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
    IMMEDIATE @ONLY)

add_custom_target(uninstall
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)