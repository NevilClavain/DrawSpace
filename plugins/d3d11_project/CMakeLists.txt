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
target_link_libraries(drawspaced3d11 d3d11 d3dx11 dxgi FW1FontWrapper CEGUIBase-0 CEGUIDirect3D11Renderer-0 jsmn physfs ceguiwrapper drawspace_core)
install(TARGETS drawspaced3d11 CONFIGURATIONS Debug RUNTIME DESTINATION ${CMAKE_SOURCE_DIR}/apps/Debug)
install(TARGETS drawspaced3d11 CONFIGURATIONS Release RUNTIME DESTINATION ${CMAKE_SOURCE_DIR}/apps/Release)
install(FILES ${fw1fontwrapper_lib_dir}/$<CONFIGURATION>/FW1FontWrapper.dll DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>)