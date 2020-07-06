cmake_minimum_required(VERSION 3.0)
project(cegui_wrapper)

include_directories(${cegui_include_dir})
include_directories(${jsonjsmn_include_dir})
include_directories(${CMAKE_SOURCE_DIR}/src)

file(
        GLOB_RECURSE
        source_files
		${CMAKE_SOURCE_DIR}/CEGUIWrapper/*.h
        ${CMAKE_SOURCE_DIR}/CEGUIWrapper/*.cpp
		
)


add_library(ceguiwrapper ${source_files})