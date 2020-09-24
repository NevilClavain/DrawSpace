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
project(luasamples_planet)

set(sample_name planetluademo)

file(
        GLOB_RECURSE
        source_files
		${CMAKE_SOURCE_DIR}/samples/lua/planet/planetluademo_assets/scripts_bank/*.lua
)

add_custom_target(${sample_name} SOURCES ${source_files})

install(DIRECTORY ${CMAKE_SOURCE_DIR}/samples/lua/planet/planetluademo_assets DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>)

#CEGUI stuff : gui skin
install(DIRECTORY ${CMAKE_SOURCE_DIR}/gui_skins/xfskin2 DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>)

# required shaders for this demo
install(FILES ${CMAKE_SOURCE_DIR}/hlsl_4_0_project/src/texture_ps.hlsl OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/planetluademo_assets/shaders_bank/hlsl_4_0)
install(FILES ${CMAKE_SOURCE_DIR}/hlsl_4_0_project/src/texture_vs.hlsl OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/planetluademo_assets/shaders_bank/hlsl_4_0)
install(FILES ${CMAKE_SOURCE_DIR}/hlsl_4_0_project/src/mat_input_constants.hlsl OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/planetluademo_assets/shaders_bank/hlsl_4_0)



# required lua models for this demo

# install spacebox model
install(DIRECTORY ${CMAKE_SOURCE_DIR}/samples/lua/lua_models/spacebox/scripts_bank OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/planetluademo_assets)
install(DIRECTORY ${CMAKE_SOURCE_DIR}/samples/lua/lua_models/spacebox/textures_bank OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/planetluademo_assets)
install(FILES ${CMAKE_SOURCE_DIR}/hlsl_4_0_project/src/texture_ps.hlsl OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/planetluademo_assets/shaders_bank/hlsl_4_0)
install(FILES ${CMAKE_SOURCE_DIR}/hlsl_4_0_project/src/texture_vs.hlsl OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/planetluademo_assets/shaders_bank/hlsl_4_0)
install(FILES ${CMAKE_SOURCE_DIR}/hlsl_4_0_project/src/mat_input_constants.hlsl OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/planetluademo_assets/shaders_bank/hlsl_4_0)


# deploy spherebump model
install(DIRECTORY ${CMAKE_SOURCE_DIR}/samples/lua/lua_models/spherebump/scripts_bank OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/planetluademo_assets)
install(DIRECTORY ${CMAKE_SOURCE_DIR}/samples/lua/lua_models/spherebump/textures_bank OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/planetluademo_assets)
install(DIRECTORY ${CMAKE_SOURCE_DIR}/samples/lua/lua_models/spherebump/meshes_bank OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/planetluademo_assets)
install(FILES ${CMAKE_SOURCE_DIR}/hlsl_4_0_project/src/lit_ps.hlsl OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/planetluademo_assets/shaders_bank/hlsl_4_0)
install(FILES ${CMAKE_SOURCE_DIR}/hlsl_4_0_project/src/lit_vs.hlsl OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/planetluademo_assets/shaders_bank/hlsl_4_0)
install(FILES ${CMAKE_SOURCE_DIR}/hlsl_4_0_project/src/color_ps.hlsl OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/planetluademo_assets/shaders_bank/hlsl_4_0)
install(FILES ${CMAKE_SOURCE_DIR}/hlsl_4_0_project/src/color_vs.hlsl OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/planetluademo_assets/shaders_bank/hlsl_4_0)

# deploy bellerophon model
install(DIRECTORY ${CMAKE_SOURCE_DIR}/samples/lua/lua_models/bellerophon/scripts_bank OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/planetluademo_assets)
install(DIRECTORY ${CMAKE_SOURCE_DIR}/samples/lua/lua_models/bellerophon/textures_bank OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/planetluademo_assets)
install(DIRECTORY ${CMAKE_SOURCE_DIR}/samples/lua/lua_models/bellerophon/meshes_bank OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/planetluademo_assets)
install(FILES ${CMAKE_SOURCE_DIR}/hlsl_4_0_project/src/lit_ps.hlsl OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/planetluademo_assets/shaders_bank/hlsl_4_0)
install(FILES ${CMAKE_SOURCE_DIR}/hlsl_4_0_project/src/lit_vs.hlsl OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/planetluademo_assets/shaders_bank/hlsl_4_0)

# deploy planet model
install(DIRECTORY ${CMAKE_SOURCE_DIR}/samples/lua/lua_models/planet/scripts_bank OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/planetluademo_assets)
install(DIRECTORY ${CMAKE_SOURCE_DIR}/samples/lua/lua_models/planet/textures_bank OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/planetluademo_assets)

# install(FILES ${CMAKE_SOURCE_DIR}/hlsl_4_0_project/src/planet_surface_ps.hlsl OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/planetluademo_assets/shaders_bank/hlsl_4_0)
# install(FILES ${CMAKE_SOURCE_DIR}/hlsl_4_0_project/src/planet_surface_vs.hlsl OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/planetluademo_assets/shaders_bank/hlsl_4_0)
# install(FILES ${CMAKE_SOURCE_DIR}/hlsl_4_0_project/src/planet_atmosphere_ps.hlsl OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/planetluademo_assets/shaders_bank/hlsl_4_0)
# install(FILES ${CMAKE_SOURCE_DIR}/hlsl_4_0_project/src/planet_atmosphere_vs.hlsl OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/planetluademo_assets/shaders_bank/hlsl_4_0)
# install(FILES ${CMAKE_SOURCE_DIR}/hlsl_4_0_project/src/planet_ht_ps.hlsl OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/planetluademo_assets/shaders_bank/hlsl_4_0)
# install(FILES ${CMAKE_SOURCE_DIR}/hlsl_4_0_project/src/planet_ht_vs.hlsl OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/planetluademo_assets/shaders_bank/hlsl_4_0)


install(FILES ${CMAKE_SOURCE_DIR}/$<CONFIGURATION>/planet_surface_ps.cso OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/planetluademo_assets/shaders_bank/hlsl_4_0)
install(FILES ${CMAKE_SOURCE_DIR}/$<CONFIGURATION>/planet_surface_vs.cso OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/planetluademo_assets/shaders_bank/hlsl_4_0)

install(FILES ${CMAKE_SOURCE_DIR}/$<CONFIGURATION>/planet_atmosphere_ps.cso OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/planetluademo_assets/shaders_bank/hlsl_4_0)
install(FILES ${CMAKE_SOURCE_DIR}/$<CONFIGURATION>/planet_atmosphere_vs.cso OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/planetluademo_assets/shaders_bank/hlsl_4_0)

install(FILES ${CMAKE_SOURCE_DIR}/$<CONFIGURATION>/planet_ht_ps.cso OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/planetluademo_assets/shaders_bank/hlsl_4_0)
install(FILES ${CMAKE_SOURCE_DIR}/$<CONFIGURATION>/planet_ht_vs.cso OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/planetluademo_assets/shaders_bank/hlsl_4_0)


