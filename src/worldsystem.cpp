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

    DrawSpace::Utils::Matrix base_mat;
    DrawSpace::Utils::Matrix finaltransform_mat;

    base_mat.Identity();

    WorldAspect* world_aspect = p_entity->GetAspect<WorldAspect>();
    if( world_aspect )
    {
        std::vector<Component<Matrix>*> mats;
        world_aspect->GetComponentsByType<Matrix>( mats );

        Matrix curr;
        curr.Identity();
        for( size_t i = 0; i < mats.size(); i++ )
        {
            curr = curr * mats[i]->getPurpose();
        }

        base_mat = base_mat * curr;
    }

    if( p_parent )
    {
        DrawSpace::Utils::Matrix parent_final_transform;
        // lire parent_final_transform
        WorldAspect* parent_world_aspect = p_parent->GetAspect<WorldAspect>();

        if( parent_world_aspect )
        {
            finaltransform_mat = base_mat * parent_world_aspect->m_globaltransform;
        }
    }
    else
    {
        finaltransform_mat = base_mat;
    }

    if( world_aspect )
    {
        world_aspect->m_globaltransform = finaltransform_mat;
    }
}
