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
project(luasamples_gui)

set(sample_name guiluademo)

file(
        GLOB_RECURSE
        source_files
		${CMAKE_SOURCE_DIR}/samples/lua/gui/guiluademo_assets/scripts_bank/*.lua
)

add_custom_target(${sample_name} SOURCES ${source_files})

install(DIRECTORY ${CMAKE_SOURCE_DIR}/samples/lua/gui/guiluademo_assets DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>)

#CEGUI stuff : gui skin
# install(DIRECTORY ${CMAKE_SOURCE_DIR}/gui_skins/xfskin2 DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>)
install(DIRECTORY ${CMAKE_SOURCE_DIR}/gui_skins/xfskin2/fonts DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/guiskins)
install(DIRECTORY ${CMAKE_SOURCE_DIR}/gui_skins/xfskin2/imagesets DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/guiskins)
install(DIRECTORY ${CMAKE_SOURCE_DIR}/gui_skins/xfskin2/layouts DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/guiskins)
install(DIRECTORY ${CMAKE_SOURCE_DIR}/gui_skins/xfskin2/looknfeel DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/guiskins)
install(DIRECTORY ${CMAKE_SOURCE_DIR}/gui_skins/xfskin2/schemes DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/guiskins)
install(DIRECTORY ${CMAKE_SOURCE_DIR}/gui_skins/xfskin2/xml_schemas DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/guiskins)
