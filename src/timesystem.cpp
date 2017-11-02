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

void TimeSystem::Run( EntityNodeGraph* p_entitygraph )
{
    m_currtm = NULL;
    p_entitygraph->AcceptSystemRootToLeaf( this );
}

void TimeSystem::VisitEntity( Entity* p_parent, Entity* p_entity )
{   
    TimeAspect* time_aspect = p_entity->GetAspect<TimeAspect>();
    if( time_aspect )
    {
        ComponentList<TimeManager> tms;

        time_aspect->GetComponentsByType<TimeManager>( tms );

        if( tms.size() > 0 )
        {
            m_currtm = &tms[0]->getPurpose();
            m_currtm->Update();
        }
    }
    else
    {    
        PhysicsAspect* physics_aspect = p_entity->GetAspect<PhysicsAspect>();
        if( physics_aspect )
        {
            if( m_currtm )
            {
                physics_aspect->SetTimeParameters( m_currtm, 1.0 );

            }
            else
            {
                _DSEXCEPTION( "No time manager available to run physic world" );
            }
        }
        else
        {

            TransformAspect* transform_aspect = p_entity->GetAspect<TransformAspect>();
            if( transform_aspect )
            {
                if( m_currtm )
                {
                    transform_aspect->SetTimeParameters( m_currtm, 1.0 );

                }
                else
                {
                    _DSEXCEPTION( "No time manager available to run transformations" );
                }
            }
        }
    }
}