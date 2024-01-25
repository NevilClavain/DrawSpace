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
project(hlsl40_shaders)

file(
        GLOB_RECURSE
        vs_source_files
		${CMAKE_SOURCE_DIR}/hlsl40_shaders/resources/*_vs.hlsl
)

file(
        GLOB_RECURSE
        ps_source_files
		${CMAKE_SOURCE_DIR}/hlsl40_shaders/resources/*_ps.hlsl
)

file(
        GLOB_RECURSE
        json_files
		${CMAKE_SOURCE_DIR}/hlsl40_shaders/resources/*.json
)

set_property(SOURCE ${ps_source_files} PROPERTY VS_SHADER_TYPE Pixel)
set_property(SOURCE ${ps_source_files} PROPERTY VS_SHADER_MODEL 4.0)
set_property(SOURCE ${ps_source_files} PROPERTY VS_SHADER_DISABLE_OPTIMIZATIONS False)
set_property(SOURCE ${ps_source_files} PROPERTY VS_SHADER_ENABLE_DEBUG False)
set_property(SOURCE ${ps_source_files} PROPERTY VS_SHADER_ENTRYPOINT ps_main)

set_property(SOURCE ${vs_source_files} PROPERTY VS_SHADER_TYPE Vertex)
set_property(SOURCE ${vs_source_files} PROPERTY VS_SHADER_MODEL 4.0)
set_property(SOURCE ${vs_source_files} PROPERTY VS_SHADER_DISABLE_OPTIMIZATIONS False)
set_property(SOURCE ${vs_source_files} PROPERTY VS_SHADER_ENABLE_DEBUG False)
set_property(SOURCE ${vs_source_files} PROPERTY VS_SHADER_ENTRYPOINT vs_main)

add_custom_target(hlsl40_shaders SOURCES ${vs_source_files} ${ps_source_files})
add_custom_target(hlsl40_shaders_metadata SOURCES ${json_files})

install(DIRECTORY ${CMAKE_SOURCE_DIR}/hlsl40_shaders/resources OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/shaders)




