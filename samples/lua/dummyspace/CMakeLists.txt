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
project(luasamples_dummyspace)

set(sample_name dummyspaceluademo)

file(
        GLOB_RECURSE
        source_files
		${CMAKE_SOURCE_DIR}/samples/lua/dummyspace/dummyspaceluademo_assets/scripts_bank/*.lua
)

add_custom_target(${sample_name} SOURCES ${source_files})

install(DIRECTORY ${CMAKE_SOURCE_DIR}/samples/lua/dummyspace/dummyspaceluademo_assets DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>)


# required shaders for this demo
install(FILES ${CMAKE_SOURCE_DIR}/hlsl_4_0_project/src/texture_ps.hlsl OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/dummyspaceluademo_assets/shaders_bank/hlsl_4_0)
install(FILES ${CMAKE_SOURCE_DIR}/hlsl_4_0_project/src/texture_vs.hlsl OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/dummyspaceluademo_assets/shaders_bank/hlsl_4_0)


# required lua models for this demo

# install spacebox model
install(DIRECTORY ${CMAKE_SOURCE_DIR}/samples/lua/lua_models/spacebox/scripts_bank OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/dummyspaceluademo_assets)
install(DIRECTORY ${CMAKE_SOURCE_DIR}/samples/lua/lua_models/spacebox/textures_bank OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/dummyspaceluademo_assets)
install(FILES ${CMAKE_SOURCE_DIR}/hlsl_4_0_project/src/texture_ps.hlsl OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/dummyspaceluademo_assets/shaders_bank/hlsl_4_0)
install(FILES ${CMAKE_SOURCE_DIR}/hlsl_4_0_project/src/texture_vs.hlsl OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/dummyspaceluademo_assets/shaders_bank/hlsl_4_0)
install(FILES ${CMAKE_SOURCE_DIR}/hlsl_4_0_project/src/mat_input_constants.hlsl OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/dummyspaceluademo_assets/shaders_bank/hlsl_4_0)

#install ceres planet model
install(DIRECTORY ${CMAKE_SOURCE_DIR}/samples/lua/lua_models/ceresplanet/scripts_bank OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/dummyspaceluademo_assets)
install(DIRECTORY ${CMAKE_SOURCE_DIR}/samples/lua/lua_models/ceresplanet/textures_bank OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/dummyspaceluademo_assets)
install(DIRECTORY ${CMAKE_SOURCE_DIR}/samples/lua/lua_models/ceresplanet/meshes_bank OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/dummyspaceluademo_assets)
install(FILES ${CMAKE_SOURCE_DIR}/hlsl_4_0_project/src/lit_ps.hlsl OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/dummyspaceluademo_assets/shaders_bank/hlsl_4_0)
install(FILES ${CMAKE_SOURCE_DIR}/hlsl_4_0_project/src/lit_vs.hlsl OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/dummyspaceluademo_assets/shaders_bank/hlsl_4_0)

#install asteroid model
install(DIRECTORY ${CMAKE_SOURCE_DIR}/samples/lua/lua_models/asteroid/scripts_bank OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/dummyspaceluademo_assets)
install(DIRECTORY ${CMAKE_SOURCE_DIR}/samples/lua/lua_models/asteroid/textures_bank OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/dummyspaceluademo_assets)
install(DIRECTORY ${CMAKE_SOURCE_DIR}/samples/lua/lua_models/asteroid/meshes_bank OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/dummyspaceluademo_assets)
install(FILES ${CMAKE_SOURCE_DIR}/hlsl_4_0_project/src/lit_ps.hlsl OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/dummyspaceluademo_assets/shaders_bank/hlsl_4_0)
install(FILES ${CMAKE_SOURCE_DIR}/hlsl_4_0_project/src/lit_vs.hlsl OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/dummyspaceluademo_assets/shaders_bank/hlsl_4_0)

#install boulder model
install(DIRECTORY ${CMAKE_SOURCE_DIR}/samples/lua/lua_models/boulder/scripts_bank OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/dummyspaceluademo_assets)
install(DIRECTORY ${CMAKE_SOURCE_DIR}/samples/lua/lua_models/boulder/textures_bank OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/dummyspaceluademo_assets)
install(DIRECTORY ${CMAKE_SOURCE_DIR}/samples/lua/lua_models/boulder/meshes_bank OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/dummyspaceluademo_assets)
install(FILES ${CMAKE_SOURCE_DIR}/hlsl_4_0_project/src/lit_ps.hlsl OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/dummyspaceluademo_assets/shaders_bank/hlsl_4_0)
install(FILES ${CMAKE_SOURCE_DIR}/hlsl_4_0_project/src/lit_vs.hlsl OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/dummyspaceluademo_assets/shaders_bank/hlsl_4_0)

# deploy bellerophon model
install(DIRECTORY ${CMAKE_SOURCE_DIR}/samples/lua/lua_models/bellerophon/scripts_bank OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/dummyspaceluademo_assets)
install(DIRECTORY ${CMAKE_SOURCE_DIR}/samples/lua/lua_models/bellerophon/textures_bank OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/dummyspaceluademo_assets)
install(DIRECTORY ${CMAKE_SOURCE_DIR}/samples/lua/lua_models/bellerophon/meshes_bank OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/dummyspaceluademo_assets)
install(FILES ${CMAKE_SOURCE_DIR}/hlsl_4_0_project/src/lit_ps.hlsl OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/dummyspaceluademo_assets/shaders_bank/hlsl_4_0)
install(FILES ${CMAKE_SOURCE_DIR}/hlsl_4_0_project/src/lit_vs.hlsl OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/dummyspaceluademo_assets/shaders_bank/hlsl_4_0)



