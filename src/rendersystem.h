/* -*-LIC_BEGIN-*- */
/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2019                     
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

#ifndef _RENDERSYSTEM_H_
#define _RENDERSYSTEM_H_

#include "systems.h"
#include "entitynodegraph.h"
#include "renderer.h"

namespace DrawSpace
{
namespace Systems
{
class RenderingSystem : public Interface::System
{
private:

    //bool init(EntityGraph::EntityNodeGraph* p_entitygraph);
    //void release(EntityGraph::EntityNodeGraph* p_entitygraph) {};
    void run(EntityGraph::EntityNodeGraph* p_entitygraph);

    DrawSpace::Interface::Renderer*  m_renderer;
    bool                             m_gui_enabled;
    bool                             m_draw_text_elements;
    //bool                             m_init_phase;
    //bool                             m_init_phase_status;
public:
    RenderingSystem( void );
    ~RenderingSystem( void );

    void EnableGUI( bool p_state );
    void VisitEntity( Core::Entity* p_parent, Core::Entity* p_entity );

    dsstring GetSystemId(void) const { return "RenderingSystem"; };
};

}
}

#endif
