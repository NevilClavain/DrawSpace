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
project(helpers)

include_directories(${CMAKE_SOURCE_DIR}/commons)
include_directories(${CMAKE_SOURCE_DIR}/CORE_ecs/src)
include_directories(${CMAKE_SOURCE_DIR}/CORE_maths/src)
include_directories(${CMAKE_SOURCE_DIR}/CORE_time/src)
include_directories(${CMAKE_SOURCE_DIR}/CORE_json/src)
include_directories(${CMAKE_SOURCE_DIR}/CORE_logger/src)
include_directories(${CMAKE_SOURCE_DIR}/TRANSFORM_control/src)

include_directories(${jsonjsmn_include_dir})
include_directories(${st_tree_include_dir})

file(
        GLOB_RECURSE
        source_files
		${CMAKE_SOURCE_DIR}/helpers/*.h
        ${CMAKE_SOURCE_DIR}/helpers/*.cpp		
)


add_definitions( -D_FROMCMAKE )

add_library(helpers ${source_files})



