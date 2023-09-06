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
project(console_callback)

include_directories(${CMAKE_SOURCE_DIR}/commons)

file(
        GLOB_RECURSE
        source_files
        ${CMAKE_SOURCE_DIR}/console_tests/console_callback/src/*.cpp
		
)

add_executable(console_callback ${source_files})

install(TARGETS console_callback CONFIGURATIONS Debug RUNTIME DESTINATION ${CMAKE_SOURCE_DIR}/apps/Debug)
install(TARGETS console_callback CONFIGURATIONS Release RUNTIME DESTINATION ${CMAKE_SOURCE_DIR}/apps/Release)
install(TARGETS console_callback CONFIGURATIONS RelWithDebInfo RUNTIME DESTINATION ${CMAKE_SOURCE_DIR}/apps/RelWithDebInfo)

