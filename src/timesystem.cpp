/* -*-LIC_BEGIN-*- */
/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2021                     
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

#include "timesystem.h"
#include "timeaspect.h"
#include "physicsaspect.h"
#include "transformaspect.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Utils;
using namespace DrawSpace::EntityGraph;
using namespace DrawSpace::Systems;
using namespace DrawSpace::Aspect;

TimeSystem::TimeSystem(void)
{
}

TimeSystem::~TimeSystem(void)
{
}

void TimeSystem::run( EntityNodeGraph* p_entitygraph )
{
    m_time_aspect = NULL;
    p_entitygraph->AcceptSystemRootToLeaf( this );
}

void TimeSystem::VisitEntity( Entity* p_parent, Entity* p_entity )
{   
    TimeAspect* time_aspect = p_entity->GetAspect<TimeAspect>();
    if( time_aspect )
    {
        m_time_aspect = time_aspect; // memorize for children & descendants

        m_time_aspect->Update();
    }

    PhysicsAspect* physics_aspect = p_entity->GetAspect<PhysicsAspect>();
    if( physics_aspect )
    {
        if( m_time_aspect )
        {
            physics_aspect->SetTimeAspect( m_time_aspect );
        }
        else
        {
            _DSEXCEPTION( "No time manager available to run physic world" );
        }
    }

    TransformAspect* transform_aspect = p_entity->GetAspect<TransformAspect>();
    if( transform_aspect )
    {
        if( m_time_aspect )
        {
            transform_aspect->SetTimeAspect( m_time_aspect );
        }
        else
        {
            _DSEXCEPTION( "No time manager available to run transformations" );
        }
    }
}
