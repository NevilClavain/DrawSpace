/* -*-LIC_BEGIN-*- */
/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2020                     
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

#ifndef _SYSTEMS_H_
#define _SYSTEMS_H_

#include "entitynodegraph.h"

namespace DrawSpace
{
namespace Interface
{
class System
{
public:
    enum Event
    {
        SYSTEM_RUN_BEGIN,
        SYSTEM_RUN_END,
    };

    using SystemEventsHandler = DrawSpace::Core::BaseCallback2<void, Event, dsstring>;

protected:

    std::set<SystemEventsHandler*>            m_system_evt_handlers;

    virtual void run(EntityGraph::EntityNodeGraph* p_entitygraph) = 0;

public:

    // NVI pattern
    void Run(EntityGraph::EntityNodeGraph* p_entitygraph)
    {
        for (auto& e : m_system_evt_handlers)
        {
            (*e)(SYSTEM_RUN_BEGIN, GetSystemId());
        }

        run(p_entitygraph);

        for (auto& e : m_system_evt_handlers)
        {
            (*e)(SYSTEM_RUN_END, GetSystemId());
        }
    }

    virtual dsstring GetSystemId(void) const = 0;
    virtual void VisitEntity( Core::Entity* p_parent, Core::Entity* p_entity ) {};

    void RegisterSystemEvtHandler(SystemEventsHandler* p_handler) 
    { 
        m_system_evt_handlers.insert(p_handler);
    }

    void UnregisterSystemEvtHandler(SystemEventsHandler* p_handler)
    {
        m_system_evt_handlers.erase(p_handler);
    }
};
}
}

#endif
