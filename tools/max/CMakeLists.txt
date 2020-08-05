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
project(max)

set(tool_name max)

file(
        GLOB_RECURSE
        source_files
		${CMAKE_SOURCE_DIR}/tools/max/max_assets/scripts_bank/*.lua
)

add_custom_target(${tool_name} SOURCES ${source_files})

install(DIRECTORY ${CMAKE_SOURCE_DIR}/tools/max/max_assets DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>)

# shaders needed by max tool
install(FILES ${CMAKE_SOURCE_DIR}/$<CONFIGURATION>/texture_ps.cso OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/max_assets/shaders_bank/hlsl_4_0)
install(FILES ${CMAKE_SOURCE_DIR}/$<CONFIGURATION>/texture_vs.cso OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/max_assets/shaders_bank/hlsl_4_0)

# skin needed by max tool#CEGUI stuff : gui skin
install(DIRECTORY ${CMAKE_SOURCE_DIR}/gui_skins/maxskin DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>)


# deploy continent model in max tools assets directory
install(FILES ${CMAKE_SOURCE_DIR}/samples/lua/lua_models/continent/scripts_bank/continent_model.lua OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/max_assets/scripts_bank)
install(FILES ${CMAKE_SOURCE_DIR}/samples/lua/lua_models/continent/meshes_bank/continent.ac OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/max_assets/meshes_bank)
install(FILES ${CMAKE_SOURCE_DIR}/samples/lua/lua_models/continent/textures_bank/DefaultMaterial_albedo.jpg OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/max_assets/textures_bank)
install(FILES ${CMAKE_SOURCE_DIR}/$<CONFIGURATION>/lit_ps.cso OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/max_assets/shaders_bank/hlsl_4_0)
install(FILES ${CMAKE_SOURCE_DIR}/$<CONFIGURATION>/lit_vs.cso OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/max_assets/shaders_bank/hlsl_4_0)


# model raptor scene
install(FILES ${CMAKE_SOURCE_DIR}/samples/lua/lua_scenes/model_raptor_scene.lua OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/max_assets/scripts_bank)

# deploy raptor model in max tools assets directory
install(FILES ${CMAKE_SOURCE_DIR}/samples/lua/lua_models/raptor/scripts_bank/raptor_model.lua OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/max_assets/scripts_bank)
install(FILES ${CMAKE_SOURCE_DIR}/samples/lua/lua_models/raptor/meshes_bank/raptor.fbx OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/max_assets/meshes_bank)
install(FILES ${CMAKE_SOURCE_DIR}/samples/lua/lua_models/raptor/textures_bank/raptorDif2.png OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/max_assets/textures_bank)
install(FILES ${CMAKE_SOURCE_DIR}/samples/lua/lua_models/raptor/textures_bank/raptorBump.png OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/max_assets/textures_bank)
install(FILES ${CMAKE_SOURCE_DIR}/samples/lua/lua_models/raptor/textures_bank/raptorSpec.png OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/max_assets/textures_bank)
install(FILES ${CMAKE_SOURCE_DIR}/$<CONFIGURATION>/lit_ps.cso OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/max_assets/shaders_bank/hlsl_4_0)
install(FILES ${CMAKE_SOURCE_DIR}/$<CONFIGURATION>/lit_vs.cso OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/max_assets/shaders_bank/hlsl_4_0)

# model trex scene
install(FILES ${CMAKE_SOURCE_DIR}/samples/lua/lua_scenes/model_trex_scene.lua OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/max_assets/scripts_bank)

# deploy trex model in max tools assets directory
install(FILES ${CMAKE_SOURCE_DIR}/samples/lua/lua_models/trex/scripts_bank/trex_model.lua OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/max_assets/scripts_bank)
install(FILES ${CMAKE_SOURCE_DIR}/samples/lua/lua_models/trex/meshes_bank/trex.fbx OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/max_assets/meshes_bank)
install(FILES ${CMAKE_SOURCE_DIR}/samples/lua/lua_models/trex/textures_bank/trex_difuse.png OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/max_assets/textures_bank)
install(FILES ${CMAKE_SOURCE_DIR}/$<CONFIGURATION>/lit_ps.cso OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/max_assets/shaders_bank/hlsl_4_0)
install(FILES ${CMAKE_SOURCE_DIR}/$<CONFIGURATION>/lit_vs.cso OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/max_assets/shaders_bank/hlsl_4_0)

# dinos scene
install(FILES ${CMAKE_SOURCE_DIR}/samples/lua/lua_scenes/dinos_scene.lua OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/max_assets/scripts_bank)

# deploy skydome model in max tools assets directory
install(FILES ${CMAKE_SOURCE_DIR}/samples/lua/lua_models/skydome/meshes_bank/skydome.ac OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/max_assets/meshes_bank)
install(FILES ${CMAKE_SOURCE_DIR}/samples/lua/lua_models/skydome/scripts_bank/skydome_model.lua OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/max_assets/scripts_bank)
install(FILES ${CMAKE_SOURCE_DIR}/$<CONFIGURATION>/skydome_ps.cso OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/max_assets/shaders_bank/hlsl_4_0)
install(FILES ${CMAKE_SOURCE_DIR}/$<CONFIGURATION>/skydome_vs.cso OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/max_assets/shaders_bank/hlsl_4_0)


