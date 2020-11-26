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
project(gameroom)

include_directories(${lua_include_dir})
link_directories(${lua_lib_dir})

include_directories(${physfs_include_dir})
link_directories(${physfs_lib_dir})

include_directories(${bullet_include_dir})
link_directories(${bullet_lib_dir})

include_directories(${jsonjsmn_include_dir})
link_directories(${jsonjsmn_lib_dir})

include_directories(${assimp_include_dir})
link_directories(${assimp_lib_dir})

include_directories(${CMAKE_SOURCE_DIR}/src)

file(
        GLOB_RECURSE
        source_files
		${CMAKE_SOURCE_DIR}/modules/gameroom/src/*.h
        ${CMAKE_SOURCE_DIR}/modules/gameroom/src/*.cpp
		
)

add_library(gameroom SHARED ${source_files})
target_link_libraries(gameroom lua physfs jsmn assimp-vc140-mt BulletDynamics_vs2017 BulletCollision_vs2017 LinearMath_vs2017 drawspace_core)
install(TARGETS gameroom CONFIGURATIONS Debug RUNTIME DESTINATION ${CMAKE_SOURCE_DIR}/apps/Debug)
install(TARGETS gameroom CONFIGURATIONS Release RUNTIME DESTINATION ${CMAKE_SOURCE_DIR}/apps/Release)

install(FILES ${CMAKE_SOURCE_DIR}/hlsl_4_0_project/src/mat_input_constants.hlsl OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/console_data/shaders_bank/hlsl_4_0)
install(FILES ${CMAKE_SOURCE_DIR}/hlsl_4_0_project/src/color_ps.hlsl OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/console_data/shaders_bank/hlsl_4_0)
install(FILES ${CMAKE_SOURCE_DIR}/hlsl_4_0_project/src/color_vs.hlsl OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/console_data/shaders_bank/hlsl_4_0)


install(DIRECTORY ${CMAKE_SOURCE_DIR}/modules/gameroom/lua_commons DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>)
install(FILES ${CMAKE_SOURCE_DIR}/config_deployment/gameroom/user.lua OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/lua_commons)

