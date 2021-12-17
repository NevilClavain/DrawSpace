# -*-LIC_BEGIN-*-
#                                                                          
# DrawSpace Rendering engine                                               
# Emmanuel Chaumont Copyright (c) 2013-2021                     
#                                                                          
# This file is part of DrawSpace.                                          
#                                                                          
#    DrawSpace is free software: you can redistribute it and/or modify     
#    it under the terms of the GNU General Public License as published by  
#    the Free Software Foundation, either version 3 of the License, or     
#    (at your option) any later version.                                   
#                                                                          
#    DrawSpace is distributed in the hope that it will be useful,          
#    but WITHOUT ANY WARRANTY; without even the implied warranty of        
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         
#    GNU General Public License for more details.                          
#                                                                          
#    You should have received a copy of the GNU General Public License     
#    along with DrawSpace.  If not, see <http://www.gnu.org/licenses/>.    
#
# -*-LIC_END-*-

cmake_minimum_required(VERSION 3.0)
project(ds_engine)

# disable ZERO_CHECK project generation
set(CMAKE_SUPPRESS_REGENERATION true)

# 3rd-parts localisations
set(physfs_include_dir ${CMAKE_SOURCE_DIR}/thirdparts/physfs-2.0.3/sdk/include)
set(physfs_lib_dir ${CMAKE_SOURCE_DIR}/thirdparts/physfs-2.0.3/sdk/lib/x86)

set(lua_include_dir ${CMAKE_SOURCE_DIR}/thirdparts/lua-1.5/sdk/include)
set(lua_lib_dir ${CMAKE_SOURCE_DIR}/thirdparts/lua-1.5/sdk/lib/x86)

set(jsonjsmn_include_dir ${CMAKE_SOURCE_DIR}/thirdparts/JSON-jsmn/sdk/include)
set(jsonjsmn_lib_dir ${CMAKE_SOURCE_DIR}/thirdparts/JSON-jsmn/sdk/lib/x86)

set(fw1fontwrapper_include_dir ${CMAKE_SOURCE_DIR}/thirdparts/FW1FontWrapper/sdk/include)
set(fw1fontwrapper_lib_dir ${CMAKE_SOURCE_DIR}/thirdparts/FW1FontWrapper/sdk/lib/x86)

set(dx11sdk_include_dir ${CMAKE_SOURCE_DIR}/thirdparts/dx11sdk/sdk/Include)
set(dx11sdk_lib_dir ${CMAKE_SOURCE_DIR}/thirdparts/dx11sdk/sdk/Lib/x86)

set(dx9sdk_include_dir ${CMAKE_SOURCE_DIR}/thirdparts/dx9sdk/sdk/Include)
set(dx9sdk_lib_dir ${CMAKE_SOURCE_DIR}/thirdparts/dx9sdk/sdk/lib/x86)

set(cegui_include_dir ${CMAKE_SOURCE_DIR}/thirdparts/CEGUI-0.8.7/sdk/include/cegui-0)
set(cegui_lib_dir ${CMAKE_SOURCE_DIR}/thirdparts/CEGUI-0.8.7/sdk/lib/x86)

set(bullet_include_dir ${CMAKE_SOURCE_DIR}/thirdparts/bullet-2.87/sdk/include)
set(bullet_lib_dir ${CMAKE_SOURCE_DIR}/thirdparts/bullet-2.87/sdk/lib/x86)

set(assimp_include_dir ${CMAKE_SOURCE_DIR}/thirdparts/Assimp-4.1.0/sdk/include)
set(assimp_lib_dir ${CMAKE_SOURCE_DIR}/thirdparts/Assimp-4.1.0/sdk/lib/x86)

# core, plugins and runtimes
add_subdirectory(drawspacecore_project)
add_subdirectory(CEGUIWrapper_project)
add_subdirectory(Plugins/d3d11_project)
# add_subdirectory(Plugins/d3d9_project)
add_subdirectory(rt_project)
add_subdirectory(vm_project)

# modules
add_subdirectory(modules/gameroom_project)
add_subdirectory(modules/skyboxmod_project)
add_subdirectory(modules/planetsmod_project)

# lua extensions
add_subdirectory(lua_extensions/impostors_luaext)

# demos
add_subdirectory(samples/cpp/impostors)
add_subdirectory(samples/cpp/water)
add_subdirectory(samples/lua/texture)
add_subdirectory(samples/lua/water)
add_subdirectory(samples/lua/gui)
add_subdirectory(samples/lua/planet)
add_subdirectory(samples/lua/nebulae)
add_subdirectory(samples/lua/dummyspace)

#tools
add_subdirectory(tools/max)

# shaders
add_subdirectory(hlsl_4_0_project)



