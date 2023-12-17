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
project(console_ecs)

include_directories(${CMAKE_SOURCE_DIR}/commons)
include_directories(${CMAKE_SOURCE_DIR}/CORE_allocator/src)
include_directories(${CMAKE_SOURCE_DIR}/CORE_ecs/src)

include_directories(${st_tree_include_dir})

file(
        GLOB_RECURSE
        source_files
        ${CMAKE_SOURCE_DIR}/console_tests/console_ecs/src/*.cpp
		
)

add_executable(console_ecs ${source_files})
target_link_libraries(console_ecs CORE_ecs CORE_logger CORE_allocator CORE_file CORE_logger CORE_json CORE_services)


install(TARGETS console_ecs CONFIGURATIONS Debug RUNTIME DESTINATION ${CMAKE_SOURCE_DIR}/apps/Debug)
install(TARGETS console_ecs CONFIGURATIONS Release RUNTIME DESTINATION ${CMAKE_SOURCE_DIR}/apps/Release)
install(TARGETS console_ecs CONFIGURATIONS RelWithDebInfo RUNTIME DESTINATION ${CMAKE_SOURCE_DIR}/apps/RelWithDebInfo)

