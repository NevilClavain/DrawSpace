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
project(planets_luaext)

include_directories(${lua_include_dir})
link_directories(${lua_lib_dir})

include_directories(${physfs_include_dir})
link_directories(${physfs_lib_dir})

link_directories(${jsonjsmn_lib_dir})

include_directories(${assimp_include_dir})
link_directories(${assimp_lib_dir})

include_directories(${bullet_include_dir})
link_directories(${bullet_lib_dir})

include_directories(${CMAKE_SOURCE_DIR}/src)
include_directories(${CMAKE_SOURCE_DIR}/CORE_logger/src)
include_directories(${CMAKE_SOURCE_DIR}/lua_core/src)

file(
        GLOB_RECURSE
        source_files
		${CMAKE_SOURCE_DIR}/lua_extensions/planets_luaext/src/*.h
        ${CMAKE_SOURCE_DIR}/lua_extensions/planets_luaext/src/*.cpp
		
)

add_library(planets_luaext SHARED ${source_files})
target_link_libraries(planets_luaext drawspace_core CORE_logger lua physfs jsmn assimp-vc140-mt BulletDynamics_vs2017 BulletCollision_vs2017 LinearMath_vs2017 lua_core)
install(TARGETS planets_luaext CONFIGURATIONS Debug RUNTIME DESTINATION ${CMAKE_SOURCE_DIR}/apps/Debug)
install(TARGETS planets_luaext CONFIGURATIONS Release RUNTIME DESTINATION ${CMAKE_SOURCE_DIR}/apps/Release)
install(TARGETS planets_luaext CONFIGURATIONS RelWithDebInfo RUNTIME DESTINATION ${CMAKE_SOURCE_DIR}/apps/RelWithDebInfo)
