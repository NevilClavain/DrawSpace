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
project(rt)

include_directories(${CMAKE_SOURCE_DIR}/commons)
include_directories(${CMAKE_SOURCE_DIR}/CORE_app/src)
include_directories(${CMAKE_SOURCE_DIR}/CORE_json/src)
include_directories(${CMAKE_SOURCE_DIR}/CORE_module/src)
include_directories(${CMAKE_SOURCE_DIR}/CORE_buffer/src)
include_directories(${jsonjsmn_include_dir})

link_directories(${jsonjsmn_lib_dir})



file(
        GLOB_RECURSE
        source_files
		${CMAKE_SOURCE_DIR}/rt/src/*.h
        ${CMAKE_SOURCE_DIR}/rt/src/*.cpp
		
)

add_executable(rt WIN32 ${source_files})

target_link_libraries(rt CORE_module CORE_filesystem CORE_app CORE_allocator CORE_ecs CORE_json CORE_logger CORE_file CORE_services jsmn)


install(TARGETS rt CONFIGURATIONS Debug RUNTIME DESTINATION ${CMAKE_SOURCE_DIR}/apps/Debug)
install(TARGETS rt CONFIGURATIONS Release RUNTIME DESTINATION ${CMAKE_SOURCE_DIR}/apps/Release)
install(TARGETS rt CONFIGURATIONS RelWithDebInfo RUNTIME DESTINATION ${CMAKE_SOURCE_DIR}/apps/RelWithDebInfo)

install(DIRECTORY ${CMAKE_SOURCE_DIR}/config_deployment/rt_config DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>)

install(FILES ${assimp_lib_dir}/$<CONFIGURATION>/assimp-vc140-mt.dll DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>)




