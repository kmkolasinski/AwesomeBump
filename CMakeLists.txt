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
    Sources/dialogshortcuts.ui Sources/formimageprop.ui
    Sources/formmaterialindicesmanager.ui Sources/formsettingscontainer.ui Sources/formsettingsfield.ui
    Sources/mainwindow.ui Sources/dockwidget3dsettings.ui)
qt5_add_resources(UI_RESOURCES Sources/content.qrc)

set(CMAKE_CXX_FLAGS "${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS}")
set(AwesomeBump_SRCS
    Sources/utils/Mesh.cpp Sources/utils/qglbuffers.cpp
    Sources/utils/tinyobj/tiny_obj_loader.cc Sources/CommonObjects.cpp
    Sources/allaboutdialog.cpp Sources/camera.cpp Sources/dialogheightcalculator.cpp
    Sources/camera.cpp Sources/dialogheightcalculator.cpp Sources/camera.cpp
    Sources/dialogheightcalculator.cpp Sources/camera.cpp Sources/gpuinfo.cpp
    Sources/dialogheightcalculator.cpp Sources/dialoglogger.cpp Sources/dialogshortcuts.cpp
    Sources/dialoglogger.cpp Sources/dialogshortcuts.cpp
    Sources/formimagebase.cpp Sources/formimagebase.cpp Sources/formimageprop.cpp
    Sources/formmaterialindicesmanager.cpp Sources/formsettingscontainer.cpp
    Sources/formsettingsfield.cpp Sources/glimageeditor.cpp Sources/glwidget.cpp
    Sources/glwidgetbase.cpp Sources/mainwindow.cpp Sources/main.cpp
    Sources/dockwidget3dsettings.cpp) 
# Check for QtnProperty (required)
if(EXISTS "${CMAKE_SOURCE_DIR}/Sources/utils/QtnProperty/QtnPropertyUnity.cpp")
 set(AwesomeBump_SRCS
 	Sources/utils/QtnProperty/QtnPropertyUnity.cpp)
 # choose QtnPeg binary
 if(APPLE)
	 set(QTNPEG "${CMAKE_SOURCE_DIR}/Sources/utils/QtnProperty/bin-osx/QtnPEG")
 elseif(LINUX)
	 set(QTNPEG "${CMAKE_SOURCE_DIR}/Sources/utils/QtnProperty/bin-linux/QtnPEG")
 elseif(WIN32)
	 set(QTNPEG "${CMAKE_SOURCE_DIR}/Sources/utils/QtnProperty/bin-win/QtnPEG")
 else()
	message(FATAL_ERROR "Cannot find QtnPEG binary for this platform.")
 endif()
 if(NOT EXISTS "${QTNPEG}")
	message(FATAL_ERROR "Binary of QtnPEG not found.")
 endif()
 # process pef file
 function(add_pef output file)
  	# Only process *.pef files
  	get_filename_component(ext ${file} EXT)
  	if(NOT ext STREQUAL ".pef")
  		message(STATUS "$file - not a pef extension")
  	endif()
  	set(header_extension "cpp")
  	get_filename_component(base ${file} NAME_WE)
  	set(base_abs ${CMAKE_CURRENT_BINARY_DIR}/${base})
  	set(outfiles ${base_abs}.cpp ${base_abs}.${header_extension})
  	set(${output} ${${output}} ${outfiles} PARENT_SCOPE)
  	add_custom_command(
		OUTPUT ${outfiles}
		COMMAND "${QTNPEG}" "${CMAKE_CURRENT_SOURCE_DIR}/${file}"
		DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/${file}"
		COMMENT "QtnPEG preprocessing of ${file}"
		VERBATIM
 	)
  	set_source_files_properties(${outfiles} PROPERTIES GENERATED TRUE)
 endfunction()
else()
 message(FATAL_ERROR "Could not find QtnProperty submodule.")
endif()

# Configure the linker and finalize binary compilation
add_executable(awesomebump ${AwesomeBump_SRCS} ${UI_HEADERS} ${UI_RESOURCES} Sources/resources/icons/icon.icns)
target_link_libraries(awesomebump Qt5::Core Qt5::DBus Qt5::Gui Qt5::Widgets Qt5::OpenGL
    GL)

# Create an install target for "Release" builds using custom or default binary and
# resource file paths
if(${CMAKE_BUILD_TYPE} MATCHES "Release")
  install(TARGETS awesomebump RUNTIME DESTINATION bin)
  install(DIRECTORY Bin/ DESTINATION ${RESOURCE_BASE})
endif()
