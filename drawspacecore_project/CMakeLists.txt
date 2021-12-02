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
project(drawspace_core)

include_directories(${physfs_include_dir})
include_directories(${assimp_include_dir})
include_directories(${bullet_include_dir})
include_directories(${jsonjsmn_include_dir})

file(
        GLOB_RECURSE
        source_files
		${CMAKE_SOURCE_DIR}/src/*.h
        ${CMAKE_SOURCE_DIR}/src/*.cpp
		
)

source_group(logging FILES 
${CMAKE_SOURCE_DIR}/src/logconf.cpp 
${CMAKE_SOURCE_DIR}/src/logconf.h
${CMAKE_SOURCE_DIR}/src/logoutput.h
${CMAKE_SOURCE_DIR}/src/logsink.cpp 
${CMAKE_SOURCE_DIR}/src/logsink.h
${CMAKE_SOURCE_DIR}/src/logoutputfile.cpp 
${CMAKE_SOURCE_DIR}/src/logoutputfile.h
${CMAKE_SOURCE_DIR}/src/tracedefs.h
)

source_group(aspect FILES 
${CMAKE_SOURCE_DIR}/src/animationsaspect.cpp 
${CMAKE_SOURCE_DIR}/src/animationsaspect.h
${CMAKE_SOURCE_DIR}/src/cameraaspect.cpp 
${CMAKE_SOURCE_DIR}/src/cameraaspect.h
${CMAKE_SOURCE_DIR}/src/collisionaspect.cpp 
${CMAKE_SOURCE_DIR}/src/collisionaspect.h
${CMAKE_SOURCE_DIR}/src/informationsaspect.h
${CMAKE_SOURCE_DIR}/src/physicsaspect.cpp 
${CMAKE_SOURCE_DIR}/src/physicsaspect.h
${CMAKE_SOURCE_DIR}/src/renderingaspect.cpp 
${CMAKE_SOURCE_DIR}/src/renderingaspect.h
${CMAKE_SOURCE_DIR}/src/resourcesaspect.cpp 
${CMAKE_SOURCE_DIR}/src/resourcesaspect.h
${CMAKE_SOURCE_DIR}/src/serviceaspect.cpp 
${CMAKE_SOURCE_DIR}/src/serviceaspect.h
${CMAKE_SOURCE_DIR}/src/timeaspect.cpp 
${CMAKE_SOURCE_DIR}/src/timeaspect.h
${CMAKE_SOURCE_DIR}/src/transformaspect.cpp 
${CMAKE_SOURCE_DIR}/src/transformaspect.h
)

source_group(aspectimplementations FILES 
${CMAKE_SOURCE_DIR}/src/rigidbodytransformaspectimpl.cpp 
${CMAKE_SOURCE_DIR}/src/rigidbodytransformaspectimpl.h
${CMAKE_SOURCE_DIR}/src/mesherenderingaspectimpl.cpp 
${CMAKE_SOURCE_DIR}/src/mesherenderingaspectimpl.h
${CMAKE_SOURCE_DIR}/src/orbittransformaspectimpl.cpp 
${CMAKE_SOURCE_DIR}/src/orbittransformaspectimpl.h
${CMAKE_SOURCE_DIR}/src/passesrenderingaspectimpl.cpp 
${CMAKE_SOURCE_DIR}/src/passesrenderingaspectimpl.h
${CMAKE_SOURCE_DIR}/src/quadrenderingaspectimpl.cpp 
${CMAKE_SOURCE_DIR}/src/quadrenderingaspectimpl.h
${CMAKE_SOURCE_DIR}/src/rawtransformaspectimpl.cpp 
${CMAKE_SOURCE_DIR}/src/rawtransformaspectimpl.h
${CMAKE_SOURCE_DIR}/src/revolutiontransformaspectimpl.cpp 
${CMAKE_SOURCE_DIR}/src/revolutiontransformaspectimpl.h
${CMAKE_SOURCE_DIR}/src/textrenderingaspectimpl.cpp 
${CMAKE_SOURCE_DIR}/src/textrenderingaspectimpl.h
)

source_group(core FILES
${CMAKE_SOURCE_DIR}/src/componentcontainer.cpp
${CMAKE_SOURCE_DIR}/src/componentcontainer.h
${CMAKE_SOURCE_DIR}/src/callback.h
${CMAKE_SOURCE_DIR}/src/circularmovement.cpp
${CMAKE_SOURCE_DIR}/src/circularmovement.h
${CMAKE_SOURCE_DIR}/src/component.h
${CMAKE_SOURCE_DIR}/src/entity.h
${CMAKE_SOURCE_DIR}/src/fpsmovement.cpp
${CMAKE_SOURCE_DIR}/src/fpsmovement.h
${CMAKE_SOURCE_DIR}/src/freemovement.cpp
${CMAKE_SOURCE_DIR}/src/freemovement.h
${CMAKE_SOURCE_DIR}/src/fx.cpp
${CMAKE_SOURCE_DIR}/src/fx.h
${CMAKE_SOURCE_DIR}/src/headmovement.cpp
${CMAKE_SOURCE_DIR}/src/headmovement.h
${CMAKE_SOURCE_DIR}/src/linearmovement.cpp
${CMAKE_SOURCE_DIR}/src/linearmovement.h
${CMAKE_SOURCE_DIR}/src/longlatmovement.cpp
${CMAKE_SOURCE_DIR}/src/longlatmovement.h
${CMAKE_SOURCE_DIR}/src/meshe.cpp
${CMAKE_SOURCE_DIR}/src/meshe.h
${CMAKE_SOURCE_DIR}/src/movement.cpp
${CMAKE_SOURCE_DIR}/src/movement.h
${CMAKE_SOURCE_DIR}/src/plugin.h
${CMAKE_SOURCE_DIR}/src/renderingnode.cpp
${CMAKE_SOURCE_DIR}/src/renderingnode.h
${CMAKE_SOURCE_DIR}/src/renderingqueue.cpp
${CMAKE_SOURCE_DIR}/src/renderingqueue.h
${CMAKE_SOURCE_DIR}/src/renderstate.cpp
${CMAKE_SOURCE_DIR}/src/renderstate.h
${CMAKE_SOURCE_DIR}/src/renderstatesset.cpp
${CMAKE_SOURCE_DIR}/src/renderstatesset.h
${CMAKE_SOURCE_DIR}/src/scenenode.cpp
${CMAKE_SOURCE_DIR}/src/scenenode.h
${CMAKE_SOURCE_DIR}/src/scenenodegraph.cpp
${CMAKE_SOURCE_DIR}/src/scenenodegraph.h
${CMAKE_SOURCE_DIR}/src/shader.cpp
${CMAKE_SOURCE_DIR}/src/shader.h
${CMAKE_SOURCE_DIR}/src/spectatormovement.cpp
${CMAKE_SOURCE_DIR}/src/spectatormovement.h
${CMAKE_SOURCE_DIR}/src/texture.cpp
${CMAKE_SOURCE_DIR}/src/texture.h
${CMAKE_SOURCE_DIR}/src/transformation.cpp
${CMAKE_SOURCE_DIR}/src/transformation.h
${CMAKE_SOURCE_DIR}/src/triangle.h
${CMAKE_SOURCE_DIR}/src/vertex.h
)

source_group(dynamics FILES
${CMAKE_SOURCE_DIR}/src/body.cpp
${CMAKE_SOURCE_DIR}/src/body.h
${CMAKE_SOURCE_DIR}/src/calendar.cpp
${CMAKE_SOURCE_DIR}/src/calendar.h
${CMAKE_SOURCE_DIR}/src/camerapoint.cpp
${CMAKE_SOURCE_DIR}/src/camerapoint.h
${CMAKE_SOURCE_DIR}/src/collider.cpp
${CMAKE_SOURCE_DIR}/src/collider.h
${CMAKE_SOURCE_DIR}/src/inertbody.cpp
${CMAKE_SOURCE_DIR}/src/inertbody.h
${CMAKE_SOURCE_DIR}/src/orbiter.cpp
${CMAKE_SOURCE_DIR}/src/orbiter.h
${CMAKE_SOURCE_DIR}/src/rocket.cpp
${CMAKE_SOURCE_DIR}/src/rocket.h
${CMAKE_SOURCE_DIR}/src/world.cpp
${CMAKE_SOURCE_DIR}/src/world.h
)

source_group(entitygraph FILES
${CMAKE_SOURCE_DIR}/src/entitynode.cpp
${CMAKE_SOURCE_DIR}/src/entitynode.h
${CMAKE_SOURCE_DIR}/src/entitynodegraph.cpp
${CMAKE_SOURCE_DIR}/src/entitynodegraph.h
)

source_group(framework FILES
${CMAKE_SOURCE_DIR}/src/dsapp.cpp
${CMAKE_SOURCE_DIR}/src/dsapp.h
)

source_group(interface/aspectimplementations FILES
${CMAKE_SOURCE_DIR}/src/renderingaspectimpl.h
${CMAKE_SOURCE_DIR}/src/serviceaspectimpl.h
${CMAKE_SOURCE_DIR}/src/transformaspectimpl.h
)

source_group(interface/module FILES
${CMAKE_SOURCE_DIR}/src/module_root.cpp
${CMAKE_SOURCE_DIR}/src/module_root.h
${CMAKE_SOURCE_DIR}/src/module_service.h
)

source_group(interface/system FILES
${CMAKE_SOURCE_DIR}/src/systems.h
)

source_group(interface FILES
${CMAKE_SOURCE_DIR}/src/mesheimport.h
${CMAKE_SOURCE_DIR}/src/renderer.h
)

source_group(module FILES
${CMAKE_SOURCE_DIR}/src/module_keys.cpp
${CMAKE_SOURCE_DIR}/src/module_keys.h
)

source_group(procedural FILES
${CMAKE_SOURCE_DIR}/src/procedural_blocs.cpp
${CMAKE_SOURCE_DIR}/src/procedural_blocs.h
${CMAKE_SOURCE_DIR}/src/procedural_parser.cpp
${CMAKE_SOURCE_DIR}/src/procedural_parser.h
${CMAKE_SOURCE_DIR}/src/procedural_seeds.cpp
${CMAKE_SOURCE_DIR}/src/procedural_seeds.h
)

source_group(rendergraph FILES
${CMAKE_SOURCE_DIR}/src/renderpassnode.cpp
${CMAKE_SOURCE_DIR}/src/renderpassnode.h
${CMAKE_SOURCE_DIR}/src/renderpassnodegraph.cpp
${CMAKE_SOURCE_DIR}/src/renderpassnodegraph.h
)

source_group(systems FILES
${CMAKE_SOURCE_DIR}/src/animationssystem.cpp
${CMAKE_SOURCE_DIR}/src/animationssystem.h
${CMAKE_SOURCE_DIR}/src/hub.cpp
${CMAKE_SOURCE_DIR}/src/hub.h
${CMAKE_SOURCE_DIR}/src/physicssystem.cpp
${CMAKE_SOURCE_DIR}/src/physicssystem.h
${CMAKE_SOURCE_DIR}/src/rendersystem.cpp
${CMAKE_SOURCE_DIR}/src/rendersystem.h
${CMAKE_SOURCE_DIR}/src/resourcessystem.cpp
${CMAKE_SOURCE_DIR}/src/resourcessystem.h
${CMAKE_SOURCE_DIR}/src/servicesystem.cpp
${CMAKE_SOURCE_DIR}/src/servicesystem.h
${CMAKE_SOURCE_DIR}/src/timesystem.cpp
${CMAKE_SOURCE_DIR}/src/timesystem.h
${CMAKE_SOURCE_DIR}/src/tracesystem.cpp
${CMAKE_SOURCE_DIR}/src/tracesystem.h
${CMAKE_SOURCE_DIR}/src/transformsystem.cpp
${CMAKE_SOURCE_DIR}/src/transformsystem.h
${CMAKE_SOURCE_DIR}/src/runnersystem.cpp
${CMAKE_SOURCE_DIR}/src/runnersystem.h
${CMAKE_SOURCE_DIR}/src/loadfiletask.cpp
${CMAKE_SOURCE_DIR}/src/loadfiletask.h
${CMAKE_SOURCE_DIR}/src/loadshaderfiletask.cpp
${CMAKE_SOURCE_DIR}/src/loadshaderfiletask.h
${CMAKE_SOURCE_DIR}/src/loadfiletoassimptask.cpp
${CMAKE_SOURCE_DIR}/src/loadfiletoassimptask.h
${CMAKE_SOURCE_DIR}/src/readshadermd5task.cpp
${CMAKE_SOURCE_DIR}/src/readshadermd5task.h
${CMAKE_SOURCE_DIR}/src/compileshadertask.cpp
${CMAKE_SOURCE_DIR}/src/compileshadertask.h
${CMAKE_SOURCE_DIR}/src/createdirectorytask.cpp
${CMAKE_SOURCE_DIR}/src/createdirectorytask.h
${CMAKE_SOURCE_DIR}/src/updatebcfilestask.cpp
${CMAKE_SOURCE_DIR}/src/updatebcfilestask.h
${CMAKE_SOURCE_DIR}/src/fillmeshesownernamestask.cpp
${CMAKE_SOURCE_DIR}/src/fillmeshesownernamestask.h
${CMAKE_SOURCE_DIR}/src/loadanimationstask.cpp
${CMAKE_SOURCE_DIR}/src/loadanimationstask.h
${CMAKE_SOURCE_DIR}/src/buildemeshetask.cpp
${CMAKE_SOURCE_DIR}/src/buildemeshetask.h
)

source_group(threading FILES
${CMAKE_SOURCE_DIR}/src/task.h
${CMAKE_SOURCE_DIR}/src/mailbox.h
${CMAKE_SOURCE_DIR}/src/runner.h
${CMAKE_SOURCE_DIR}/src/runner.cpp
)

source_group(utils FILES
${CMAKE_SOURCE_DIR}/src/ac3dmeshe.cpp
${CMAKE_SOURCE_DIR}/src/ac3dmeshe.h
${CMAKE_SOURCE_DIR}/src/exceptions.h
${CMAKE_SOURCE_DIR}/src/file.cpp
${CMAKE_SOURCE_DIR}/src/file.h
${CMAKE_SOURCE_DIR}/src/filesystem.cpp
${CMAKE_SOURCE_DIR}/src/filesystem.h
${CMAKE_SOURCE_DIR}/src/jsonparser.cpp
${CMAKE_SOURCE_DIR}/src/jsonparser.h
${CMAKE_SOURCE_DIR}/src/maths.cpp
${CMAKE_SOURCE_DIR}/src/maths.h
${CMAKE_SOURCE_DIR}/src/matrix.cpp
${CMAKE_SOURCE_DIR}/src/matrix.h
${CMAKE_SOURCE_DIR}/src/md5.h
${CMAKE_SOURCE_DIR}/src/memalloc.cpp
${CMAKE_SOURCE_DIR}/src/memalloc.h
${CMAKE_SOURCE_DIR}/src/misc_utils.cpp
${CMAKE_SOURCE_DIR}/src/misc_utils.h
${CMAKE_SOURCE_DIR}/src/parser.cpp
${CMAKE_SOURCE_DIR}/src/parser.h
${CMAKE_SOURCE_DIR}/src/piload.cpp
${CMAKE_SOURCE_DIR}/src/piload.h
${CMAKE_SOURCE_DIR}/src/pimanager.h
${CMAKE_SOURCE_DIR}/src/pimanager_impl.h
${CMAKE_SOURCE_DIR}/src/quadtree.cpp
${CMAKE_SOURCE_DIR}/src/quadtree.h
${CMAKE_SOURCE_DIR}/src/quaternion.cpp
${CMAKE_SOURCE_DIR}/src/quaternion.h
${CMAKE_SOURCE_DIR}/src/timemanager.cpp
${CMAKE_SOURCE_DIR}/src/timemanager.h
${CMAKE_SOURCE_DIR}/src/vector.cpp
${CMAKE_SOURCE_DIR}/src/vector.h
${CMAKE_SOURCE_DIR}/src/crtp_singleton.h
)

add_definitions(-DBT_USE_DOUBLE_PRECISION )
add_definitions( -D_FROMCMAKE )

add_library(drawspace_core ${source_files})

install(FILES ${assimp_lib_dir}/$<CONFIGURATION>/assimp-vc140-mt.dll DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>)
install(FILES ${CMAKE_SOURCE_DIR}/config_deployment/renderer/appgraphics.conf OPTIONAL DESTINATION ${CMAKE_SOURCE_DIR}/apps/$<CONFIGURATION>)


