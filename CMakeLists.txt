# -*-LIC_BEGIN-*-
#                                                                          
# DrawSpace Rendering engine                                               
# Emmanuel Chaumont Copyright (c) 2013-2020                     
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

set(physfs_include_dir ${CMAKE_SOURCE_DIR}/thirdparts/physfs-2.0.3/include)
set(physfs_lib_dir ${CMAKE_SOURCE_DIR}/thirdparts/physfs-2.0.3/vs2017/Release)
set(lua_include_dir ${CMAKE_SOURCE_DIR}/thirdparts/lua-1.5/include)
set(jsonjsmn_include_dir ${CMAKE_SOURCE_DIR}/thirdparts/JSON-jsmn/include)
set(jsonjsmn_lib_dir ${CMAKE_SOURCE_DIR}/thirdparts/JSON-jsmn/lib)
set(fw1fontwrapper_include_dir ${CMAKE_SOURCE_DIR}/thirdparts/FW1FontWrapper/include)
set(fw1fontwrapper_lib_dir ${CMAKE_SOURCE_DIR}/thirdparts/FW1FontWrapper/lib/x86)
set(dx11sdk_include_dir ${CMAKE_SOURCE_DIR}/thirdparts/dx11sdk/Include)
set(dx11sdk_lib_dir ${CMAKE_SOURCE_DIR}/thirdparts/dx11sdk/Lib/x86)
set(dx9sdk_include_dir ${CMAKE_SOURCE_DIR}/thirdparts/dx9sdk/Include)
set(dx9sdk_lib_dir ${CMAKE_SOURCE_DIR}/thirdparts/dx9sdk/Lib)
set(cegui_include_dir ${CMAKE_SOURCE_DIR}/thirdparts/CEGUI-0.8.7/include/cegui-0)
set(cegui_lib_dir ${CMAKE_SOURCE_DIR}/thirdparts/CEGUI-0.8.7/lib)
set(bullet_include_dir ${CMAKE_SOURCE_DIR}/thirdparts/bullet-2.87/src)
set(assimp_include_dir ${CMAKE_SOURCE_DIR}/thirdparts/Assimp-4.1.0/include)

add_subdirectory(drawspacecore_project)
add_subdirectory(CEGUIWrapper_project)
add_subdirectory(Plugins/d3d11_project)
add_subdirectory(Plugins/d3d9_project)



