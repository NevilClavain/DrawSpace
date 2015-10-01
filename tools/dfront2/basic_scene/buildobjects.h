/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2015                              
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

#include "drawspace.h"
#include "BasicSceneMainFrame.h"

DrawSpace::Spacebox* BuildSpaceBox( const DrawSpace::Utils::SpaceboxDescriptor& p_descriptor, dsstring& p_error );
DrawSpace::Chunk* BuildChunk( const DrawSpace::Utils::ChunkDescriptor& p_descriptor, dsstring& p_error );
DrawSpace::Clouds* BuildClouds( DrawSpace::Utils::CloudsDescriptor& p_descriptor, dsstring& p_error );
DrawSpace::Dynamics::InertBody* BuildInertBody( const DrawSpace::Dynamics::Body::Parameters& p_params, DrawSpace::Dynamics::World* p_world );
DrawSpace::Dynamics::Collider* BuildCollider( const DrawSpace::Dynamics::Body::Parameters& p_params, DrawSpace::Dynamics::World* p_world );