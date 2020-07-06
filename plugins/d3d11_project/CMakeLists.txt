cmake_minimum_required(VERSION 3.0)
project(d3d11)

include_directories(${dx11sdk_include_dir})
link_directories(${dx11sdk_lib_dir})
include_directories(${fw1fontwrapper_include_dir})
link_directories(${fw1fontwrapper_lib_dir})
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
		${CMAKE_SOURCE_DIR}/Plugins/d3d11/src/*.h
        ${CMAKE_SOURCE_DIR}/Plugins/d3d11/src/*.cpp
		
)

add_library(drawspaced3d11 SHARED ${source_files})
target_link_libraries(drawspaced3d11 d3d11 d3dx11 dxgi FW1FontWrapper CEGUIBase-0 CEGUIDirect3D11Renderer-0 jsmn_r physfs ceguiwrapper drawspace_core)