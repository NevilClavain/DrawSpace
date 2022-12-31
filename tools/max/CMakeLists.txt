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
install(FILES ${CMAKE_SOURCE_DIR}/hlsl_4_0_project/src/texture_ps.hlsl OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/max_assets/shaders_bank/hlsl_4_0)
install(FILES ${CMAKE_SOURCE_DIR}/hlsl_4_0_project/src/texture_vs.hlsl OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/max_assets/shaders_bank/hlsl_4_0)
install(FILES ${CMAKE_SOURCE_DIR}/hlsl_4_0_project/src/mat_input_constants.hlsl OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/max_assets/shaders_bank/hlsl_4_0)


# skin needed by max tool

#CEGUI stuff : gui skin
install(DIRECTORY ${CMAKE_SOURCE_DIR}/gui_skins/testskin/fonts DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/guiskins)
install(DIRECTORY ${CMAKE_SOURCE_DIR}/gui_skins/testskin/imagesets DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/guiskins)
install(DIRECTORY ${CMAKE_SOURCE_DIR}/gui_skins/testskin/layouts DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/guiskins)
install(DIRECTORY ${CMAKE_SOURCE_DIR}/gui_skins/testskin/looknfeel DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/guiskins)
install(DIRECTORY ${CMAKE_SOURCE_DIR}/gui_skins/testskin/schemes DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/guiskins)
install(DIRECTORY ${CMAKE_SOURCE_DIR}/gui_skins/testskin/xml_schemas DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/guiskins)

##################################################
############################ Models installation
##################################################


# install spacebox model
install(DIRECTORY ${CMAKE_SOURCE_DIR}/samples/lua/lua_models/spacebox/scripts_bank OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/max_assets)
install(DIRECTORY ${CMAKE_SOURCE_DIR}/samples/lua/lua_models/spacebox/textures_bank OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/max_assets)
install(FILES ${CMAKE_SOURCE_DIR}/hlsl_4_0_project/src/texture_ps.hlsl OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/max_assets/shaders_bank/hlsl_4_0)
install(FILES ${CMAKE_SOURCE_DIR}/hlsl_4_0_project/src/texture_vs.hlsl OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/max_assets/shaders_bank/hlsl_4_0)
install(FILES ${CMAKE_SOURCE_DIR}/hlsl_4_0_project/src/mat_input_constants.hlsl OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/max_assets/shaders_bank/hlsl_4_0)


# deploy continent model
install(DIRECTORY ${CMAKE_SOURCE_DIR}/samples/lua/lua_models/continent/scripts_bank OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/max_assets)
install(DIRECTORY ${CMAKE_SOURCE_DIR}/samples/lua/lua_models/continent/textures_bank OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/max_assets)
install(DIRECTORY ${CMAKE_SOURCE_DIR}/samples/lua/lua_models/continent/meshes_bank OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/max_assets)
install(FILES ${CMAKE_SOURCE_DIR}/hlsl_4_0_project/src/lit_ps.hlsl OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/max_assets/shaders_bank/hlsl_4_0)
install(FILES ${CMAKE_SOURCE_DIR}/hlsl_4_0_project/src/lit_vs.hlsl OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/max_assets/shaders_bank/hlsl_4_0)
install(FILES ${CMAKE_SOURCE_DIR}/hlsl_4_0_project/src/color_ps.hlsl OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/max_assets/shaders_bank/hlsl_4_0)
install(FILES ${CMAKE_SOURCE_DIR}/hlsl_4_0_project/src/color_vs.hlsl OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/max_assets/shaders_bank/hlsl_4_0)
install(FILES ${CMAKE_SOURCE_DIR}/hlsl_4_0_project/src/mat_input_constants.hlsl OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/max_assets/shaders_bank/hlsl_4_0)
install(FILES ${CMAKE_SOURCE_DIR}/hlsl_4_0_project/src/generic_rendering.hlsl OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/max_assets/shaders_bank/hlsl_4_0)



# deploy boulder model
install(DIRECTORY ${CMAKE_SOURCE_DIR}/samples/lua/lua_models/boulder/scripts_bank OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/max_assets)
install(DIRECTORY ${CMAKE_SOURCE_DIR}/samples/lua/lua_models/boulder/textures_bank OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/max_assets)
install(DIRECTORY ${CMAKE_SOURCE_DIR}/samples/lua/lua_models/boulder/meshes_bank OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/max_assets)
install(FILES ${CMAKE_SOURCE_DIR}/hlsl_4_0_project/src/lit_ps.hlsl OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/max_assets/shaders_bank/hlsl_4_0)
install(FILES ${CMAKE_SOURCE_DIR}/hlsl_4_0_project/src/lit_vs.hlsl OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/max_assets/shaders_bank/hlsl_4_0)
install(FILES ${CMAKE_SOURCE_DIR}/hlsl_4_0_project/src/color_ps.hlsl OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/max_assets/shaders_bank/hlsl_4_0)
install(FILES ${CMAKE_SOURCE_DIR}/hlsl_4_0_project/src/color_vs.hlsl OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/max_assets/shaders_bank/hlsl_4_0)
install(FILES ${CMAKE_SOURCE_DIR}/hlsl_4_0_project/src/mat_input_constants.hlsl OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/max_assets/shaders_bank/hlsl_4_0)
install(FILES ${CMAKE_SOURCE_DIR}/hlsl_4_0_project/src/generic_rendering.hlsl OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/max_assets/shaders_bank/hlsl_4_0)


# deploy spherebump model
install(DIRECTORY ${CMAKE_SOURCE_DIR}/samples/lua/lua_models/spherebump/scripts_bank OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/max_assets)
install(DIRECTORY ${CMAKE_SOURCE_DIR}/samples/lua/lua_models/spherebump/textures_bank OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/max_assets)
install(DIRECTORY ${CMAKE_SOURCE_DIR}/samples/lua/lua_models/spherebump/meshes_bank OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/max_assets)
install(FILES ${CMAKE_SOURCE_DIR}/hlsl_4_0_project/src/lit_ps.hlsl OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/max_assets/shaders_bank/hlsl_4_0)
install(FILES ${CMAKE_SOURCE_DIR}/hlsl_4_0_project/src/lit_vs.hlsl OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/max_assets/shaders_bank/hlsl_4_0)
install(FILES ${CMAKE_SOURCE_DIR}/hlsl_4_0_project/src/color_ps.hlsl OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/max_assets/shaders_bank/hlsl_4_0)
install(FILES ${CMAKE_SOURCE_DIR}/hlsl_4_0_project/src/color_vs.hlsl OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/max_assets/shaders_bank/hlsl_4_0)
install(FILES ${CMAKE_SOURCE_DIR}/hlsl_4_0_project/src/mat_input_constants.hlsl OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/max_assets/shaders_bank/hlsl_4_0)
install(FILES ${CMAKE_SOURCE_DIR}/hlsl_4_0_project/src/generic_rendering.hlsl OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/max_assets/shaders_bank/hlsl_4_0)


# deploy metalcube model
install(DIRECTORY ${CMAKE_SOURCE_DIR}/samples/lua/lua_models/metalcube/scripts_bank OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/max_assets)
install(DIRECTORY ${CMAKE_SOURCE_DIR}/samples/lua/lua_models/metalcube/textures_bank OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/max_assets)
install(DIRECTORY ${CMAKE_SOURCE_DIR}/samples/lua/lua_models/metalcube/meshes_bank OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/max_assets)
install(FILES ${CMAKE_SOURCE_DIR}/hlsl_4_0_project/src/lit_ps.hlsl OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/max_assets/shaders_bank/hlsl_4_0)
install(FILES ${CMAKE_SOURCE_DIR}/hlsl_4_0_project/src/lit_vs.hlsl OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/max_assets/shaders_bank/hlsl_4_0)
install(FILES ${CMAKE_SOURCE_DIR}/hlsl_4_0_project/src/color_ps.hlsl OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/max_assets/shaders_bank/hlsl_4_0)
install(FILES ${CMAKE_SOURCE_DIR}/hlsl_4_0_project/src/color_vs.hlsl OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/max_assets/shaders_bank/hlsl_4_0)
install(FILES ${CMAKE_SOURCE_DIR}/hlsl_4_0_project/src/mat_input_constants.hlsl OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/max_assets/shaders_bank/hlsl_4_0)
install(FILES ${CMAKE_SOURCE_DIR}/hlsl_4_0_project/src/generic_rendering.hlsl OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/max_assets/shaders_bank/hlsl_4_0)

# deploy bellerophon model
install(DIRECTORY ${CMAKE_SOURCE_DIR}/samples/lua/lua_models/bellerophon/scripts_bank OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/max_assets)
install(DIRECTORY ${CMAKE_SOURCE_DIR}/samples/lua/lua_models/bellerophon/textures_bank OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/max_assets)
install(DIRECTORY ${CMAKE_SOURCE_DIR}/samples/lua/lua_models/bellerophon/meshes_bank OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/max_assets)
install(FILES ${CMAKE_SOURCE_DIR}/hlsl_4_0_project/src/lit_ps.hlsl OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/max_assets/shaders_bank/hlsl_4_0)
install(FILES ${CMAKE_SOURCE_DIR}/hlsl_4_0_project/src/lit_vs.hlsl OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/max_assets/shaders_bank/hlsl_4_0)
install(FILES ${CMAKE_SOURCE_DIR}/hlsl_4_0_project/src/mat_input_constants.hlsl OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/max_assets/shaders_bank/hlsl_4_0)
install(FILES ${CMAKE_SOURCE_DIR}/hlsl_4_0_project/src/generic_rendering.hlsl OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/max_assets/shaders_bank/hlsl_4_0)



# deploy raptor model
install(DIRECTORY ${CMAKE_SOURCE_DIR}/samples/lua/lua_models/raptor/scripts_bank OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/max_assets)
install(DIRECTORY ${CMAKE_SOURCE_DIR}/samples/lua/lua_models/raptor/textures_bank OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/max_assets)
install(DIRECTORY ${CMAKE_SOURCE_DIR}/samples/lua/lua_models/raptor/meshes_bank OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/max_assets)
install(FILES ${CMAKE_SOURCE_DIR}/hlsl_4_0_project/src/lit_ps.hlsl OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/max_assets/shaders_bank/hlsl_4_0)
install(FILES ${CMAKE_SOURCE_DIR}/hlsl_4_0_project/src/lit_vs.hlsl OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/max_assets/shaders_bank/hlsl_4_0)
install(FILES ${CMAKE_SOURCE_DIR}/hlsl_4_0_project/src/mat_input_constants.hlsl OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/max_assets/shaders_bank/hlsl_4_0)
install(FILES ${CMAKE_SOURCE_DIR}/hlsl_4_0_project/src/generic_rendering.hlsl OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/max_assets/shaders_bank/hlsl_4_0)

# deploy trex model
install(DIRECTORY ${CMAKE_SOURCE_DIR}/samples/lua/lua_models/trex/scripts_bank OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/max_assets)
install(DIRECTORY ${CMAKE_SOURCE_DIR}/samples/lua/lua_models/trex/textures_bank OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/max_assets)
install(DIRECTORY ${CMAKE_SOURCE_DIR}/samples/lua/lua_models/trex/meshes_bank OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/max_assets)
install(FILES ${CMAKE_SOURCE_DIR}/hlsl_4_0_project/src/lit_ps.hlsl OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/max_assets/shaders_bank/hlsl_4_0)
install(FILES ${CMAKE_SOURCE_DIR}/hlsl_4_0_project/src/lit_vs.hlsl OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/max_assets/shaders_bank/hlsl_4_0)
install(FILES ${CMAKE_SOURCE_DIR}/hlsl_4_0_project/src/mat_input_constants.hlsl OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/max_assets/shaders_bank/hlsl_4_0)
install(FILES ${CMAKE_SOURCE_DIR}/hlsl_4_0_project/src/generic_rendering.hlsl OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/max_assets/shaders_bank/hlsl_4_0)

# deploy skydome model in max tools assets directory
install(DIRECTORY ${CMAKE_SOURCE_DIR}/samples/lua/lua_models/skydome/scripts_bank OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/max_assets)
install(DIRECTORY ${CMAKE_SOURCE_DIR}/samples/lua/lua_models/skydome/textures_bank OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/max_assets)
install(DIRECTORY ${CMAKE_SOURCE_DIR}/samples/lua/lua_models/skydome/meshes_bank OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/max_assets)
install(FILES ${CMAKE_SOURCE_DIR}/hlsl_4_0_project/src/skydome_ps.hlsl OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/max_assets/shaders_bank/hlsl_4_0)
install(FILES ${CMAKE_SOURCE_DIR}/hlsl_4_0_project/src/skydome_vs.hlsl OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/max_assets/shaders_bank/hlsl_4_0)
install(FILES ${CMAKE_SOURCE_DIR}/hlsl_4_0_project/src/mat_input_constants.hlsl OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/max_assets/shaders_bank/hlsl_4_0)
install(FILES ${CMAKE_SOURCE_DIR}/hlsl_4_0_project/src/generic_rendering.hlsl OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/max_assets/shaders_bank/hlsl_4_0)
install(FILES ${CMAKE_SOURCE_DIR}/hlsl_4_0_project/src/spherelod_commons.hlsl OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/max_assets/shaders_bank/hlsl_4_0)
install(FILES ${CMAKE_SOURCE_DIR}/hlsl_4_0_project/src/atmospheric_scattering.hlsl OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/max_assets/shaders_bank/hlsl_4_0)


# deploy land model
install(DIRECTORY ${CMAKE_SOURCE_DIR}/samples/lua/lua_models/land/scripts_bank OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/max_assets)
install(DIRECTORY ${CMAKE_SOURCE_DIR}/samples/lua/lua_models/land/textures_bank OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/max_assets)
install(DIRECTORY ${CMAKE_SOURCE_DIR}/samples/lua/lua_models/land/meshes_bank OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/max_assets)
install(FILES ${CMAKE_SOURCE_DIR}/hlsl_4_0_project/src/lit_ps.hlsl OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/max_assets/shaders_bank/hlsl_4_0)
install(FILES ${CMAKE_SOURCE_DIR}/hlsl_4_0_project/src/lit_vs.hlsl OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/max_assets/shaders_bank/hlsl_4_0)
install(FILES ${CMAKE_SOURCE_DIR}/hlsl_4_0_project/src/mat_input_constants.hlsl OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/max_assets/shaders_bank/hlsl_4_0)
install(FILES ${CMAKE_SOURCE_DIR}/hlsl_4_0_project/src/generic_rendering.hlsl OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/max_assets/shaders_bank/hlsl_4_0)

# deploy ceresplanet model
install(DIRECTORY ${CMAKE_SOURCE_DIR}/samples/lua/lua_models/ceresplanet/scripts_bank OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/max_assets)
install(DIRECTORY ${CMAKE_SOURCE_DIR}/samples/lua/lua_models/ceresplanet/textures_bank OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/max_assets)
install(DIRECTORY ${CMAKE_SOURCE_DIR}/samples/lua/lua_models/ceresplanet/meshes_bank OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/max_assets)
install(FILES ${CMAKE_SOURCE_DIR}/hlsl_4_0_project/src/lit_ps.hlsl OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/max_assets/shaders_bank/hlsl_4_0)
install(FILES ${CMAKE_SOURCE_DIR}/hlsl_4_0_project/src/lit_vs.hlsl OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/max_assets/shaders_bank/hlsl_4_0)
install(FILES ${CMAKE_SOURCE_DIR}/hlsl_4_0_project/src/mat_input_constants.hlsl OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/max_assets/shaders_bank/hlsl_4_0)
install(FILES ${CMAKE_SOURCE_DIR}/hlsl_4_0_project/src/generic_rendering.hlsl OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/max_assets/shaders_bank/hlsl_4_0)


# deploy asteroid model
install(DIRECTORY ${CMAKE_SOURCE_DIR}/samples/lua/lua_models/asteroid/scripts_bank OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/max_assets)
install(DIRECTORY ${CMAKE_SOURCE_DIR}/samples/lua/lua_models/asteroid/textures_bank OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/max_assets)
install(DIRECTORY ${CMAKE_SOURCE_DIR}/samples/lua/lua_models/asteroid/meshes_bank OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/max_assets)
install(FILES ${CMAKE_SOURCE_DIR}/hlsl_4_0_project/src/lit_ps.hlsl OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/max_assets/shaders_bank/hlsl_4_0)
install(FILES ${CMAKE_SOURCE_DIR}/hlsl_4_0_project/src/lit_vs.hlsl OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/max_assets/shaders_bank/hlsl_4_0)
install(FILES ${CMAKE_SOURCE_DIR}/hlsl_4_0_project/src/mat_input_constants.hlsl OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/max_assets/shaders_bank/hlsl_4_0)
install(FILES ${CMAKE_SOURCE_DIR}/hlsl_4_0_project/src/generic_rendering.hlsl OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/max_assets/shaders_bank/hlsl_4_0)


##################################################
############################ Scenes installation
##################################################

# model raptor scene
install(FILES ${CMAKE_SOURCE_DIR}/samples/lua/lua_scenes/model_raptor_scene.lua OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/max_assets/scripts_bank)

# model trex scene
install(FILES ${CMAKE_SOURCE_DIR}/samples/lua/lua_scenes/model_trex_scene.lua OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/max_assets/scripts_bank)

# land model scene
install(FILES ${CMAKE_SOURCE_DIR}/samples/lua/lua_scenes/model_land_scene.lua OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/max_assets/scripts_bank)

# bellerophon model scene
install(FILES ${CMAKE_SOURCE_DIR}/samples/lua/lua_scenes/model_bellerophon_scene.lua OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/max_assets/scripts_bank)

# ceresplanet model scene
install(FILES ${CMAKE_SOURCE_DIR}/samples/lua/lua_scenes/model_ceresplanet_scene.lua OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/max_assets/scripts_bank)

# asteroid model scene
install(FILES ${CMAKE_SOURCE_DIR}/samples/lua/lua_scenes/model_asteroid_scene.lua OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/max_assets/scripts_bank)


# dinos scene
install(FILES ${CMAKE_SOURCE_DIR}/samples/lua/lua_scenes/dinos_scene.lua OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/max_assets/scripts_bank)




