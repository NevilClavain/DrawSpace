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



#ifndef _PROCEDURALSYSTEM_H_
#define _PROCEDURALSYSTEM_H_

#include "systems.h"
#include "entitynodegraph.h"
#include "proceduralaspect.h"

namespace DrawSpace
{
namespace Systems
{
class ProceduralSystem : public Interface::System
{
public:

    typedef DrawSpace::Core::BaseCallback2<void, const dsstring&, Aspect::ProceduralAspect::ProceduralBloc*> ProceduralPublicationEventHandler;

protected:

    
    bool                                                m_init_phase;

    Aspect::ProceduralAspect::ProceduralBlocsFactory    m_factory;
    
    std::set<ProceduralPublicationEventHandler*>        m_proc_pub_evt_handlers;


    void visit_entity_on_init( Core::Entity* p_parent, Core::Entity* p_entity );
    void visit_entity_on_run( Core::Entity* p_parent, Core::Entity* p_entity );

public:
    ProceduralSystem( void );
    ~ProceduralSystem( void );

    bool Init( EntityGraph::EntityNodeGraph* p_entitygraph );
    void Release( EntityGraph::EntityNodeGraph* p_entitygraph );

    void Run( EntityGraph::EntityNodeGraph* p_entitygraph );
    void VisitEntity( Core::Entity* p_parent, Core::Entity* p_entity );

    void RegisterProceduralPublicationEvtHandler( ProceduralPublicationEventHandler* p_handler );
    void UnregisterProceduralPublicationEvtHandler( ProceduralPublicationEventHandler* p_handler );

    Aspect::ProceduralAspect::ProceduralBlocsFactory& GetFactory( void );
};

}
}

#endif
