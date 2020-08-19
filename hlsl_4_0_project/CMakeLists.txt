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
project(hlsl_4_0_shaders)

file(
        GLOB_RECURSE
        vs_source_files
		${CMAKE_SOURCE_DIR}/hlsl_4_0_project/src/*_vs.hlsl
)

file(
        GLOB_RECURSE
        ps_source_files
		${CMAKE_SOURCE_DIR}/hlsl_4_0_project/src/*_ps.hlsl
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

add_custom_target(shadersc SOURCES ${vs_source_files} ${ps_source_files})


