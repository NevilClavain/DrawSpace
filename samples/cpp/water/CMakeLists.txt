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
project(samples_water)

set(sample_name waterdemo)

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
		${CMAKE_SOURCE_DIR}/samples/cpp/water/src/*.h
        ${CMAKE_SOURCE_DIR}/samples/cpp/water/src/*.cpp
		
)

add_library(${sample_name} SHARED ${source_files})

target_link_libraries(${sample_name} physfs jsmn assimp-vc140-mt BulletDynamics_vs2017 BulletCollision_vs2017 LinearMath_vs2017 drawspace_core)

install(TARGETS ${sample_name} CONFIGURATIONS Debug RUNTIME DESTINATION ${CMAKE_SOURCE_DIR}/apps/Debug)
install(TARGETS ${sample_name} CONFIGURATIONS Release RUNTIME DESTINATION ${CMAKE_SOURCE_DIR}/apps/Release)
install(DIRECTORY ${CMAKE_SOURCE_DIR}/samples/cpp/water/waterdemo_assets DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>)

#CEGUI stuff : gui skin
# install(DIRECTORY ${CMAKE_SOURCE_DIR}/gui_skins/testskin DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>)
install(DIRECTORY ${CMAKE_SOURCE_DIR}/gui_skins/testskin/fonts DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/guiskins)
install(DIRECTORY ${CMAKE_SOURCE_DIR}/gui_skins/testskin/imagesets DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/guiskins)
install(DIRECTORY ${CMAKE_SOURCE_DIR}/gui_skins/testskin/layouts DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/guiskins)
install(DIRECTORY ${CMAKE_SOURCE_DIR}/gui_skins/testskin/looknfeel DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/guiskins)
install(DIRECTORY ${CMAKE_SOURCE_DIR}/gui_skins/testskin/schemes DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/guiskins)
install(DIRECTORY ${CMAKE_SOURCE_DIR}/gui_skins/testskin/xml_schemas DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/guiskins)


# required shaders for this demo
install(FILES ${CMAKE_SOURCE_DIR}/hlsl_4_0_project/src/mat_input_constants.hlsl OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/waterdemo_assets/shaders_bank/hlsl_4_0)
install(FILES ${CMAKE_SOURCE_DIR}/hlsl_4_0_project/src/generic_rendering.hlsl OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/waterdemo_assets/shaders_bank/hlsl_4_0)
install(FILES ${CMAKE_SOURCE_DIR}/hlsl_4_0_project/src/spherelod_commons.hlsl OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/waterdemo_assets/shaders_bank/hlsl_4_0)
install(FILES ${CMAKE_SOURCE_DIR}/hlsl_4_0_project/src/atmospheric_scattering.hlsl OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/waterdemo_assets/shaders_bank/hlsl_4_0)
install(FILES ${CMAKE_SOURCE_DIR}/hlsl_4_0_project/src/fbm.hlsl OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/waterdemo_assets/shaders_bank/hlsl_4_0)


install(FILES ${CMAKE_SOURCE_DIR}/hlsl_4_0_project/src/texture_ps.hlsl OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/waterdemo_assets/shaders_bank/hlsl_4_0)
install(FILES ${CMAKE_SOURCE_DIR}/hlsl_4_0_project/src/texture_vs.hlsl OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/waterdemo_assets/shaders_bank/hlsl_4_0)

install(FILES ${CMAKE_SOURCE_DIR}/hlsl_4_0_project/src/texture_mirror_ps.hlsl OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/waterdemo_assets/shaders_bank/hlsl_4_0)
install(FILES ${CMAKE_SOURCE_DIR}/hlsl_4_0_project/src/texture_mirror_vs.hlsl OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/waterdemo_assets/shaders_bank/hlsl_4_0)

install(FILES ${CMAKE_SOURCE_DIR}/hlsl_4_0_project/src/color_ps.hlsl OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/waterdemo_assets/shaders_bank/hlsl_4_0)
install(FILES ${CMAKE_SOURCE_DIR}/hlsl_4_0_project/src/color_vs.hlsl OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/waterdemo_assets/shaders_bank/hlsl_4_0)

install(FILES ${CMAKE_SOURCE_DIR}/hlsl_4_0_project/src/water_bump_ps.hlsl OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/waterdemo_assets/shaders_bank/hlsl_4_0)
install(FILES ${CMAKE_SOURCE_DIR}/hlsl_4_0_project/src/water_bump_vs.hlsl OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/waterdemo_assets/shaders_bank/hlsl_4_0)

install(FILES ${CMAKE_SOURCE_DIR}/hlsl_4_0_project/src/water_mask_ps.hlsl OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/waterdemo_assets/shaders_bank/hlsl_4_0)
install(FILES ${CMAKE_SOURCE_DIR}/hlsl_4_0_project/src/water_mask_vs.hlsl OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/waterdemo_assets/shaders_bank/hlsl_4_0)

install(FILES ${CMAKE_SOURCE_DIR}/hlsl_4_0_project/src/water_waves_ps.hlsl OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/waterdemo_assets/shaders_bank/hlsl_4_0)
install(FILES ${CMAKE_SOURCE_DIR}/hlsl_4_0_project/src/water_waves_vs.hlsl OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>/waterdemo_assets/shaders_bank/hlsl_4_0)

