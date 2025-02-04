# -*-LIC_BEGIN-*-
#                                                                          
# MaGE rendering framework
# Emmanuel Chaumont Copyright (c) 2023
#                                                                          
# This file is part of MaGE.                                          
#                                                                          
#    MaGE is free software: you can redistribute it and/or modify     
#    it under the terms of the GNU General Public License as published by  
#    the Free Software Foundation, either version 3 of the License, or     
#    (at your option) any later version.                                   
#                                                                          
#    MaGE is distributed in the hope that it will be useful,          
#    but WITHOUT ANY WARRANTY; without even the implied warranty of        
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         
#    GNU General Public License for more details.                          
#                                                                          
#    You should have received a copy of the GNU General Public License     
#    along with MaGE.  If not, see <http://www.gnu.org/licenses/>.    
#
# -*-LIC_END-*-

cmake_minimum_required(VERSION 3.0)
project(SYSTEM_animations)

include_directories(${CMAKE_SOURCE_DIR}/commons)

include_directories(${CMAKE_SOURCE_DIR}/CORE_ecs/src)
include_directories(${CMAKE_SOURCE_DIR}/CORE_time/src)
include_directories(${CMAKE_SOURCE_DIR}/CORE_maths/src)
include_directories(${CMAKE_SOURCE_DIR}/CORE_buffer/src)
include_directories(${CMAKE_SOURCE_DIR}/CORE_services/src)

include_directories(${CMAKE_SOURCE_DIR}/RENDERING_control/src)
include_directories(${CMAKE_SOURCE_DIR}/SYSTEM_resource/src)

include_directories(${st_tree_include_dir})


file(
        GLOB_RECURSE
        source_files
		${CMAKE_SOURCE_DIR}/SYSTEM_animations/src/*.h
        ${CMAKE_SOURCE_DIR}/SYSTEM_animations/src/*.cpp		
)

add_definitions( -D_FROMCMAKE )

add_library(SYSTEM_animations ${source_files})

