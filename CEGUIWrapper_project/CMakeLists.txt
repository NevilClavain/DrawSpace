# -*-LIC_BEGIN-*-
#                                                                          
# DrawSpace Rendering engine                                               
# Emmanuel Chaumont Copyright (c) 2013-2023                     
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
project(cegui_wrapper)

include_directories(${cegui_include_dir})
include_directories(${jsonjsmn_include_dir})
include_directories(${CMAKE_SOURCE_DIR}/src)
include_directories(${CMAKE_SOURCE_DIR}/CORE_logger/src)

file(
        GLOB_RECURSE
        source_files
		${CMAKE_SOURCE_DIR}/CEGUIWrapper/*.h
        ${CMAKE_SOURCE_DIR}/CEGUIWrapper/*.cpp
		
)
add_library(ceguiwrapper ${source_files})

install(FILES ${cegui_lib_dir}/Debug/CEGUIBase-0_d.dll DESTINATION ${CMAKE_SOURCE_DIR}/apps/Debug)
install(FILES ${cegui_lib_dir}/Release/CEGUIBase-0.dll DESTINATION ${CMAKE_SOURCE_DIR}/apps/Release)
install(FILES ${cegui_lib_dir}/RelWithDebInfo/CEGUIBase-0.dll DESTINATION ${CMAKE_SOURCE_DIR}/apps/RelWithDebInfo)

install(FILES ${cegui_lib_dir}/Debug/CEGUICommonDialogs-0_d.dll DESTINATION ${CMAKE_SOURCE_DIR}/apps/Debug)
install(FILES ${cegui_lib_dir}/Release/CEGUICommonDialogs-0.dll DESTINATION ${CMAKE_SOURCE_DIR}/apps/Release)
install(FILES ${cegui_lib_dir}/RelWithDebInfo/CEGUICommonDialogs-0.dll DESTINATION ${CMAKE_SOURCE_DIR}/apps/RelWithDebInfo)

install(FILES ${cegui_lib_dir}/Debug/CEGUICoreWindowRendererSet_d.dll DESTINATION ${CMAKE_SOURCE_DIR}/apps/Debug)
install(FILES ${cegui_lib_dir}/Release/CEGUICoreWindowRendererSet.dll DESTINATION ${CMAKE_SOURCE_DIR}/apps/Release)
install(FILES ${cegui_lib_dir}/RelWithDebInfo/CEGUICoreWindowRendererSet.dll DESTINATION ${CMAKE_SOURCE_DIR}/apps/RelWithDebInfo)

install(FILES ${cegui_lib_dir}/Debug/CEGUIDirect3D9Renderer-0_d.dll DESTINATION ${CMAKE_SOURCE_DIR}/apps/Debug)
install(FILES ${cegui_lib_dir}/Release/CEGUIDirect3D9Renderer-0.dll DESTINATION ${CMAKE_SOURCE_DIR}/apps/Release)
install(FILES ${cegui_lib_dir}/RelWithDebInfo/CEGUIDirect3D9Renderer-0.dll DESTINATION ${CMAKE_SOURCE_DIR}/apps/RelWithDebInfo)

install(FILES ${cegui_lib_dir}/Debug/CEGUIDirect3D11Renderer-0_d.dll DESTINATION ${CMAKE_SOURCE_DIR}/apps/Debug)
install(FILES ${cegui_lib_dir}/Release/CEGUIDirect3D11Renderer-0.dll DESTINATION ${CMAKE_SOURCE_DIR}/apps/Release)
install(FILES ${cegui_lib_dir}/RelWithDebInfo/CEGUIDirect3D11Renderer-0.dll DESTINATION ${CMAKE_SOURCE_DIR}/apps/RelWithDebInfo)

install(FILES ${cegui_lib_dir}/Debug/CEGUIFreeImageImageCodec_d.dll DESTINATION ${CMAKE_SOURCE_DIR}/apps/Debug)
install(FILES ${cegui_lib_dir}/Release/CEGUIFreeImageImageCodec.dll DESTINATION ${CMAKE_SOURCE_DIR}/apps/Release)
install(FILES ${cegui_lib_dir}/RelWithDebInfo/CEGUIFreeImageImageCodec.dll DESTINATION ${CMAKE_SOURCE_DIR}/apps/RelWithDebInfo)

install(FILES ${cegui_lib_dir}/Debug/CEGUIXercesParser_d.dll DESTINATION ${CMAKE_SOURCE_DIR}/apps/Debug)
install(FILES ${cegui_lib_dir}/Release/CEGUIXercesParser.dll DESTINATION ${CMAKE_SOURCE_DIR}/apps/Release)
install(FILES ${cegui_lib_dir}/RelWithDebInfo/CEGUIXercesParser.dll DESTINATION ${CMAKE_SOURCE_DIR}/apps/RelWithDebInfo)

install(FILES ${cegui_lib_dir}/Debug/FreeImage_d.dll DESTINATION ${CMAKE_SOURCE_DIR}/apps/Debug)
install(FILES ${cegui_lib_dir}/Release/FreeImage.dll DESTINATION ${CMAKE_SOURCE_DIR}/apps/Release)
install(FILES ${cegui_lib_dir}/RelWithDebInfo/FreeImage.dll DESTINATION ${CMAKE_SOURCE_DIR}/apps/RelWithDebInfo)

install(FILES ${cegui_lib_dir}/Debug/freetype_d.dll DESTINATION ${CMAKE_SOURCE_DIR}/apps/Debug)
install(FILES ${cegui_lib_dir}/Release/freetype.dll DESTINATION ${CMAKE_SOURCE_DIR}/apps/Release)
install(FILES ${cegui_lib_dir}/RelWithDebInfo/freetype.dll DESTINATION ${CMAKE_SOURCE_DIR}/apps/RelWithDebInfo)

install(FILES ${cegui_lib_dir}/Debug/xerces-c_3_d.dll DESTINATION ${CMAKE_SOURCE_DIR}/apps/Debug)
install(FILES ${cegui_lib_dir}/Release/xerces-c_3.dll DESTINATION ${CMAKE_SOURCE_DIR}/apps/Release)
install(FILES ${cegui_lib_dir}/RelWithDebInfo/xerces-c_3.dll DESTINATION ${CMAKE_SOURCE_DIR}/apps/RelWithDebInfo)

