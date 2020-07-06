cmake_minimum_required(VERSION 3.0)
project(d3d9)

include_directories(${dx9sdk_include_dir})
link_directories(${dx9sdk_lib_dir})
include_directories(${cegui_include_dir})
link_directories(${cegui_lib_dir})
include_directories(${jsonjsmn_include_dir})
link_directories(${jsonjsmn_lib_dir})

link_directories(${physfs_lib_dir})


include_directories(${CMAKE_SOURCE_DIR}/src)
include_directories(${CMAKE_SOURCE_DIR}/CEGUIWrapper)

file(
        GLOB_RECURSE
        source_files
		${CMAKE_SOURCE_DIR}/Plugins/d3d9/src/*.h
        ${CMAKE_SOURCE_DIR}/Plugins/d3d9/src/*.cpp
		
)

add_library(drawspaced3d9 SHARED ${source_files})
target_link_libraries(drawspaced3d9 d3d9 d3dx9 CEGUIBase-0 CEGUIDirect3D9Renderer-0 jsmn_r physfs ceguiwrapper drawspace_core)