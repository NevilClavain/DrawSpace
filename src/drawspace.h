/* -*-LIC_BEGIN-*- */
/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2018                        
*                                                                          
* This file is part of DrawSpace.                                          
*                                                                          
*    DrawSpace is free software: you can redistribute it and/or modify     
*    it under the terms of the GNU General Public License as published by  
*    the Free Software Foundation, either version 3 of the License, or     
*    (at your option) any later version.                                   
*                                                                          
*    DrawSpace is distributed in the hope that it will be useful,          
*    but WITHOUT ANY WARRANTY; without even the implied warranty of        
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         
*    GNU General Public License for more details.                          
*                                                                          
*    You should have received a copy of the GNU General Public License     
*    along with DrawSpace.  If not, see <http://www.gnu.org/licenses/>.    
*
*/
/* -*-LIC_END-*- */


#ifndef _DRAWSPACE_H_
#define _DRAWSPACE_H_

#include "tracedefs.h"
#include "exceptions.h"
#include "maths.h"
#include "task.h"
#include "mutex.h"
#include "parser.h"
#include "vector.h"
#include "matrix.h"
#include "quaternion.h"
#include "vertex.h"
#include "triangle.h"
#include "meshe.h"
#include "file.h"
#include "md5.h"
#include "transformation.h"
#include "timemanager.h"
#include "transformation.h"
#include "renderingnode.h"
#include "renderingqueue.h"
#include "texture.h"
#include "shader.h"
#include "plugin.h"
#include "pimanager.h"
#include "piload.h"
#include "renderstate.h"
#include "renderstatesset.h"
#include "renderer.h"
#include "fx.h"
#include "viewportquad.h"
#include "pass.h"
#include "scenenodegraph.h"
#include "memalloc.h"
#include "fpsmovement.h"
#include "freemovement.h"
#include "linearmovement.h"
#include "circularmovement.h"
#include "longlatmovement.h"
#include "spectatormovement.h"
#include "headmovement.h"
#include "quadtree.h"
#include "misc_utils.h"
#include "world.h"
#include "inertbody.h"
#include "orbiter.h"
#include "rocket.h"
#include "collider.h"
#include "calendar.h"
#include "camerapoint.h"
#include "spacebox.h"
#include "spherelod_drawing.h"
#include "chunk.h"
#include "mediator.h"
#include "runner.h"
#include "spherelod_root.h"
#include "ac3dmeshe.h"
#include "procedural_blocs.h"
#include "procedural_parser.h"
#include "procedural_seeds.h"
#include "impostorsdisplaylist.h"
#include "clouds.h"
#include "module_root.h"
#include "module_service.h"
#include "module_keys.h"
#include "jsonparser.h"

#include "entitynodegraph.h"
#include "renderpassnodegraph.h"

#include "renderingaspect.h"
#include "passesrenderingaspectimpl.h"
#include "mesherenderingaspectimpl.h"
#include "quadrenderingaspectimpl.h"
#include "impostorsrenderingaspectimpl.h"
#include "textrenderingaspectimpl.h"


#include "transformaspect.h"
#include "rawtransformaspectimpl.h"
#include "bodytransformaspectimpl.h"
#include "orbittransformaspectimpl.h"
#include "revolutiontransformaspectimpl.h"

#include "serviceaspect.h"
#include "serviceaspectimpl.h"

#include "cameraaspect.h"
#include "bodyaspect.h"
#include "physicsaspect.h"
#include "timeaspect.h"
#include "informationsaspect.h"

#include "rendersystem.h"
#include "transformsystem.h"
#include "physicssystem.h"
#include "timesystem.h"
#include "servicesystem.h"
#include "hub.h"

#include "tracesystem.h"

#endif
