/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2017                        
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

#include "worldsystem.h"
#include "worldaspect.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Utils;

WorldSystem::WorldSystem( void )
{
}

WorldSystem::~WorldSystem( void )
{
}

void WorldSystem::Run( EntityNodeGraph* p_entitygraph )
{
    p_entitygraph->AcceptWorldSystem( this );
}

void WorldSystem::VisitEntity( Entity* p_parent, Entity* p_entity ) const
{
    WorldAspect* world_aspect = p_entity->GetAspect<WorldAspect>();

    if( world_aspect )
    {
        world_aspect->compute_transforms( p_parent, p_entity );
    }
}
