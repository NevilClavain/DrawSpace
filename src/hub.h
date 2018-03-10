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


#ifndef _HUB_H_
#define _HUB_H_

#include "transformsystem.h"
#include "timesystem.h"
#include "physicssystem.h"
#include "rendersystem.h"
#include "tracesystem.h"

namespace DrawSpace
{
namespace Systems
{
class Hub : public Interface::System
{
public:
    typedef enum
    {
        SYSTEMS_UPDATE_BEGIN,
        SYSTEMS_UPDATE_END

    } SystemsUpdateEvent;

    typedef DrawSpace::Core::BaseCallback<void, SystemsUpdateEvent>                                               SystemsUpdateEventHandler;
    

private:
    DrawSpace::Systems::RenderingSystem             m_renderingSystem;
    DrawSpace::Systems::TransformSystem             m_transformSystem;
    DrawSpace::Systems::PhysicsSystem               m_physicsSystem;
    DrawSpace::Systems::TimeSystem                  m_timeSystem;
    DrawSpace::Systems::TraceSystem                 m_traceSystem;


    std::vector<DrawSpace::Interface::System*>      m_systems;

    std::set<SystemsUpdateEventHandler*>            m_systems_update_evt_handlers;


public:
    Hub( void );

    bool Init( EntityGraph::EntityNodeGraph* p_entitygraph );
    void Release( EntityGraph::EntityNodeGraph* p_entitygraph );
    void Run( EntityGraph::EntityNodeGraph* p_entitygraph );

    void SetLogConf( DrawSpace::Logger::Configuration* p_logconf );

    void EnableGUI( bool p_state );

    void RegisterSystemsUpdateEvtHandler( SystemsUpdateEventHandler* p_handler );
    void UnregisterSystemsUpdateEvtHandler( SystemsUpdateEventHandler* p_handler );

};
}
}

#endif