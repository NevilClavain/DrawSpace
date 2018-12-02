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

#ifndef _SERVICESYSTEM_H_
#define _SERVICESYSTEM_H_

#include "systems.h"
#include "entitynodegraph.h"

namespace DrawSpace
{
namespace Systems
{
class ServiceSystem : public Interface::System
{
protected:

    bool init(EntityGraph::EntityNodeGraph* p_entitygraph);
    void release(EntityGraph::EntityNodeGraph* p_entitygraph);
    void run(EntityGraph::EntityNodeGraph* p_entitygraph);


    dsstring    m_callsource;
    bool        m_init_status;

    long        m_key;
    long        m_char;
    long        m_scan;
    long        m_xm, m_ym, m_dx, m_dy;
    long        m_delta;
    WPARAM      m_wParam;
    LPARAM      m_lParam;

public:
    ServiceSystem( void );
    ~ServiceSystem( void );

    void VisitEntity( Core::Entity* p_parent, Core::Entity* p_entity );

    dsstring GetSystemId(void) const { return "ServiceSystem"; };

    void OnKeyPress( EntityGraph::EntityNodeGraph* p_entitygraph, long p_key );
    void OnEndKeyPress( EntityGraph::EntityNodeGraph* p_entitygraph, long p_key );
    void OnKeyPulse( EntityGraph::EntityNodeGraph* p_entitygraph, long p_key );
    void OnChar( EntityGraph::EntityNodeGraph* p_entitygraph, long p_char, long p_scan );
    void OnMouseMove( EntityGraph::EntityNodeGraph* p_entitygraph, long p_xm, long p_ym, long p_dx, long p_dy );
    void OnMouseWheel( EntityGraph::EntityNodeGraph* p_entitygraph, long p_delta );
    void OnMouseLeftButtonDown( EntityGraph::EntityNodeGraph* p_entitygraph, long p_xm, long p_ym );
    void OnMouseLeftButtonUp( EntityGraph::EntityNodeGraph* p_entitygraph, long p_xm, long p_ym );
    void OnMouseRightButtonDown( EntityGraph::EntityNodeGraph* p_entitygraph, long p_xm, long p_ym );
    void OnMouseRightButtonUp( EntityGraph::EntityNodeGraph* p_entitygraph, long p_xm, long p_ym );
    void OnAppEvent( EntityGraph::EntityNodeGraph* p_entitygraph, WPARAM p_wParam, LPARAM p_lParam );
};

}
}

#endif
