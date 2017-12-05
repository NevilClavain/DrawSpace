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

#include "servicesystem.h"
#include "serviceaspect.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::EntityGraph;
using namespace DrawSpace::Aspect;
using namespace DrawSpace::Systems;

ServiceSystem::ServiceSystem(void)
{
}

ServiceSystem::~ServiceSystem(void)
{
}

bool ServiceSystem::Init( EntityGraph::EntityNodeGraph* p_entitygraph )
{
    m_init_status = true;
    m_callsource = 0;
    p_entitygraph->AcceptSystemLeafToRoot( this );

    return m_init_status;
}

void ServiceSystem::Release( EntityGraph::EntityNodeGraph* p_entitygraph )
{
    m_callsource = 2;
    p_entitygraph->AcceptSystemLeafToRoot( this );
}

void ServiceSystem::Run( EntityNodeGraph* p_entitygraph )
{
    m_callsource = 1;
    p_entitygraph->AcceptSystemLeafToRoot( this );
}

void ServiceSystem::VisitEntity( Entity* p_parent, Entity* p_entity )
{
    ServiceAspect* service_aspect = p_entity->GetAspect<ServiceAspect>();
    if( service_aspect )
    {
        switch( m_callsource )
        {
            case 0:
                if( false == service_aspect->Init() )
                {
                    m_init_status = false;
                }
                break;

            case 1:
                service_aspect->Run();
                break;

            case 2:
                service_aspect->Release();
                break;
        }        
    }
}