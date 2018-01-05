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

#include "rendersystem.h"
#include "renderingaspect.h"
#include "plugin.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::EntityGraph;
using namespace DrawSpace::Aspect;
using namespace DrawSpace::Systems;

RenderingSystem::RenderingSystem(void) :
m_gui_enabled( false )
{
    m_renderer = DrawSpace::Core::SingletonPlugin<DrawSpace::Interface::Renderer>::GetInstance()->m_interface;
}

RenderingSystem::~RenderingSystem(void)
{
}

void RenderingSystem::EnableGUI( bool p_state )
{
    m_gui_enabled = p_state;
}

void RenderingSystem::Run( EntityNodeGraph* p_entitygraph )
{
    p_entitygraph->AcceptSystemLeafToRoot( this );
    if( m_gui_enabled )
    {
        m_renderer->GUI_Render();
    }
    m_renderer->FlipScreen();
}

void RenderingSystem::VisitEntity( Entity* p_parent, Entity* p_entity )
{
    RenderingAspect* rendering_aspect = p_entity->GetAspect<RenderingAspect>();
    if( rendering_aspect )
    {
        rendering_aspect->Draw( p_entity );
    }
}