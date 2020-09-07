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
project(luasamples_texture)

set(sample_name waterluademo)

file(
        GLOB_RECURSE
        source_files
		${CMAKE_SOURCE_DIR}/samples/lua/water/waterluademo_assets/scripts_bank/*.lua
)

add_custom_target(${sample_name} SOURCES ${source_files})

install(DIRECTORY ${CMAKE_SOURCE_DIR}/samples/lua/water/waterluademo_assets DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>)

#CEGUI stuff : gui skin
install(DIRECTORY ${CMAKE_SOURCE_DIR}/gui_skins/testskin DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>)

# deploy land model
install(DIRECTORY ${CMAKE_SOURCE_DIR}/samples/lua/lua_models/land/scripts_bank OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/waterluademo_assets)
install(DIRECTORY ${CMAKE_SOURCE_DIR}/samples/lua/lua_models/land/textures_bank OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/waterluademo_assets)
install(DIRECTORY ${CMAKE_SOURCE_DIR}/samples/lua/lua_models/land/meshes_bank OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/waterluademo_assets)
install(FILES ${CMAKE_SOURCE_DIR}/hlsl_4_0_project/src/lit_ps.hlsl OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/waterluademo_assets/shaders_bank/hlsl_4_0)
install(FILES ${CMAKE_SOURCE_DIR}/hlsl_4_0_project/src/lit_vs.hlsl OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/waterluademo_assets/shaders_bank/hlsl_4_0)

# deploy waterquad model
install(DIRECTORY ${CMAKE_SOURCE_DIR}/samples/lua/lua_models/waterquad/scripts_bank OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/waterluademo_assets)
install(DIRECTORY ${CMAKE_SOURCE_DIR}/samples/lua/lua_models/waterquad/textures_bank OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/waterluademo_assets)
install(DIRECTORY ${CMAKE_SOURCE_DIR}/samples/lua/lua_models/waterquad/meshes_bank OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/waterluademo_assets)
install(FILES ${CMAKE_SOURCE_DIR}/hlsl_4_0_project/src/color_ps.hlsl OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/waterluademo_assets/shaders_bank/hlsl_4_0)
install(FILES ${CMAKE_SOURCE_DIR}/hlsl_4_0_project/src/color_vs.hlsl OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/waterluademo_assets/shaders_bank/hlsl_4_0)


# deploy skydome model in max tools assets directory
install(DIRECTORY ${CMAKE_SOURCE_DIR}/samples/lua/lua_models/skydome/scripts_bank OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/waterluademo_assets)
install(DIRECTORY ${CMAKE_SOURCE_DIR}/samples/lua/lua_models/skydome/textures_bank OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/waterluademo_assets)
install(DIRECTORY ${CMAKE_SOURCE_DIR}/samples/lua/lua_models/skydome/meshes_bank OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/waterluademo_assets)
install(FILES ${CMAKE_SOURCE_DIR}/hlsl_4_0_project/src/skydome_ps.hlsl OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/waterluademo_assets/shaders_bank/hlsl_4_0)
install(FILES ${CMAKE_SOURCE_DIR}/hlsl_4_0_project/src/skydome_vs.hlsl OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/waterluademo_assets/shaders_bank/hlsl_4_0)


# deploy metalcube model
install(DIRECTORY ${CMAKE_SOURCE_DIR}/samples/lua/lua_models/metalcube/scripts_bank OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/waterluademo_assets)
install(DIRECTORY ${CMAKE_SOURCE_DIR}/samples/lua/lua_models/metalcube/textures_bank OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/waterluademo_assets)
install(DIRECTORY ${CMAKE_SOURCE_DIR}/samples/lua/lua_models/metalcube/meshes_bank OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/waterluademo_assets)
install(FILES ${CMAKE_SOURCE_DIR}/hlsl_4_0_project/src/lit_ps.hlsl OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/waterluademo_assets/shaders_bank/hlsl_4_0)
install(FILES ${CMAKE_SOURCE_DIR}/hlsl_4_0_project/src/lit_vs.hlsl OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/waterluademo_assets/shaders_bank/hlsl_4_0)
install(FILES ${CMAKE_SOURCE_DIR}/hlsl_4_0_project/src/color_ps.hlsl OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/waterluademo_assets/shaders_bank/hlsl_4_0)
install(FILES ${CMAKE_SOURCE_DIR}/hlsl_4_0_project/src/color_vs.hlsl OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/waterluademo_assets/shaders_bank/hlsl_4_0)

# deploy spherebump model
install(DIRECTORY ${CMAKE_SOURCE_DIR}/samples/lua/lua_models/spherebump/scripts_bank OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/waterluademo_assets)
install(DIRECTORY ${CMAKE_SOURCE_DIR}/samples/lua/lua_models/spherebump/textures_bank OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/waterluademo_assets)
install(DIRECTORY ${CMAKE_SOURCE_DIR}/samples/lua/lua_models/spherebump/meshes_bank OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/waterluademo_assets)
install(FILES ${CMAKE_SOURCE_DIR}/hlsl_4_0_project/src/lit_ps.hlsl OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/waterluademo_assets/shaders_bank/hlsl_4_0)
install(FILES ${CMAKE_SOURCE_DIR}/hlsl_4_0_project/src/lit_vs.hlsl OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/waterluademo_assets/shaders_bank/hlsl_4_0)
install(FILES ${CMAKE_SOURCE_DIR}/hlsl_4_0_project/src/color_ps.hlsl OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/waterluademo_assets/shaders_bank/hlsl_4_0)
install(FILES ${CMAKE_SOURCE_DIR}/hlsl_4_0_project/src/color_vs.hlsl OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/waterluademo_assets/shaders_bank/hlsl_4_0)



# required shaders for this demo
install(FILES ${CMAKE_SOURCE_DIR}/hlsl_4_0_project/src/mat_input_constants.hlsl OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/waterluademo_assets/shaders_bank/hlsl_4_0)
install(FILES ${CMAKE_SOURCE_DIR}/hlsl_4_0_project/src/generic_rendering.hlsl OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/waterluademo_assets/shaders_bank/hlsl_4_0)
install(FILES ${CMAKE_SOURCE_DIR}/hlsl_4_0_project/src/spherelod_commons.hlsl OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/waterluademo_assets/shaders_bank/hlsl_4_0)
install(FILES ${CMAKE_SOURCE_DIR}/hlsl_4_0_project/src/atmospheric_scattering.hlsl OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/waterluademo_assets/shaders_bank/hlsl_4_0)
install(FILES ${CMAKE_SOURCE_DIR}/hlsl_4_0_project/src/fbm.hlsl OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/waterluademo_assets/shaders_bank/hlsl_4_0)


install(FILES ${CMAKE_SOURCE_DIR}/hlsl_4_0_project/src/texture_ps.hlsl OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/waterluademo_assets/shaders_bank/hlsl_4_0)
install(FILES ${CMAKE_SOURCE_DIR}/hlsl_4_0_project/src/texture_vs.hlsl OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/waterluademo_assets/shaders_bank/hlsl_4_0)

install(FILES ${CMAKE_SOURCE_DIR}/hlsl_4_0_project/src/texture_mirror_ps.hlsl OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/waterluademo_assets/shaders_bank/hlsl_4_0)
install(FILES ${CMAKE_SOURCE_DIR}/hlsl_4_0_project/src/texture_mirror_vs.hlsl OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/waterluademo_assets/shaders_bank/hlsl_4_0)

install(FILES ${CMAKE_SOURCE_DIR}/hlsl_4_0_project/src/color_ps.hlsl OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/waterluademo_assets/shaders_bank/hlsl_4_0)
install(FILES ${CMAKE_SOURCE_DIR}/hlsl_4_0_project/src/color_vs.hlsl OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/waterluademo_assets/shaders_bank/hlsl_4_0)

install(FILES ${CMAKE_SOURCE_DIR}/hlsl_4_0_project/src/water_bump_ps.hlsl OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/waterluademo_assets/shaders_bank/hlsl_4_0)
install(FILES ${CMAKE_SOURCE_DIR}/hlsl_4_0_project/src/water_bump_vs.hlsl OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/waterluademo_assets/shaders_bank/hlsl_4_0)

install(FILES ${CMAKE_SOURCE_DIR}/hlsl_4_0_project/src/water_waves_ps.hlsl OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/waterluademo_assets/shaders_bank/hlsl_4_0)
install(FILES ${CMAKE_SOURCE_DIR}/hlsl_4_0_project/src/water_waves_vs.hlsl OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/waterluademo_assets/shaders_bank/hlsl_4_0)

install(FILES ${CMAKE_SOURCE_DIR}/hlsl_4_0_project/src/water_mask_ps.hlsl OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/waterluademo_assets/shaders_bank/hlsl_4_0)
install(FILES ${CMAKE_SOURCE_DIR}/hlsl_4_0_project/src/water_mask_vs.hlsl OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/waterluademo_assets/shaders_bank/hlsl_4_0)

