# -*-LIC_BEGIN-*-
#                                                                          
# renderMe grafx framework
# Emmanuel Chaumont Copyright (c) 2023
#                                                                          
# This file is part of renderMe.                                          
#                                                                          
#    renderMe is free software: you can redistribute it and/or modify     
#    it under the terms of the GNU General Public License as published by  
#    the Free Software Foundation, either version 3 of the License, or     
#    (at your option) any later version.                                   
#                                                                          
#    renderMe is distributed in the hope that it will be useful,          
#    but WITHOUT ANY WARRANTY; without even the implied warranty of        
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         
#    GNU General Public License for more details.                          
#                                                                          
#    You should have received a copy of the GNU General Public License     
#    along with renderMe.  If not, see <http://www.gnu.org/licenses/>.    
#
# -*-LIC_END-*-

cmake_minimum_required(VERSION 3.0)
project(renderme)

set(CMAKE_CXX_STANDARD 17)

# disable ZERO_CHECK project generation
set(CMAKE_SUPPRESS_REGENERATION true)

add_definitions(-D_SILENCE_ALL_CXX17_DEPRECATION_WARNINGS)

# 3rd-parts localisations

set(lua_include_dir ${CMAKE_SOURCE_DIR}/../thirdparts/lua-1.5/sdk/include)
set(lua_lib_dir ${CMAKE_SOURCE_DIR}/../thirdparts/lua-1.5/sdk/lib/x86)

set(jsonjsmn_include_dir ${CMAKE_SOURCE_DIR}/../thirdparts/JSON-jsmn/sdk/include)
set(jsonjsmn_lib_dir ${CMAKE_SOURCE_DIR}/../thirdparts/JSON-jsmn/sdk/lib/x86)

set(fw1fontwrapper_include_dir ${CMAKE_SOURCE_DIR}/../thirdparts/FW1FontWrapper/sdk/include)
set(fw1fontwrapper_lib_dir ${CMAKE_SOURCE_DIR}/../thirdparts/FW1FontWrapper/sdk/lib/x86)

set(dx11sdk_include_dir ${CMAKE_SOURCE_DIR}/../thirdparts/dx11sdk/sdk/Include)
set(dx11sdk_lib_dir ${CMAKE_SOURCE_DIR}/../thirdparts/dx11sdk/sdk/Lib/x86)

set(cegui_include_dir ${CMAKE_SOURCE_DIR}/../thirdparts/CEGUI-0.8.7/sdk/include/cegui-0)
set(cegui_lib_dir ${CMAKE_SOURCE_DIR}/../thirdparts/CEGUI-0.8.7/sdk/lib/x86)

set(bullet_include_dir ${CMAKE_SOURCE_DIR}/../thirdparts/bullet-2.87/sdk/include)
set(bullet_lib_dir ${CMAKE_SOURCE_DIR}/../thirdparts/bullet-2.87/sdk/lib/x86)

set(assimp_include_dir ${CMAKE_SOURCE_DIR}/../thirdparts/Assimp-4.1.0/sdk/include)
set(assimp_lib_dir ${CMAKE_SOURCE_DIR}/../thirdparts/Assimp-4.1.0/sdk/lib/x86)

set(md5_include_dir ${CMAKE_SOURCE_DIR}/../thirdparts/md5/sdk/include)

set(st_tree_include_dir ${CMAKE_SOURCE_DIR}/../thirdparts/st_tree/sdk/include)


add_subdirectory(CORE_allocator)
add_subdirectory(CORE_logger)
add_subdirectory(CORE_file)
add_subdirectory(CORE_filesystem)
add_subdirectory(CORE_json)



add_subdirectory(console_tests/console_helloworld)
add_subdirectory(console_tests/console_jsonparser)




