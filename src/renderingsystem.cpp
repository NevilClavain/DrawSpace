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

#include "renderingsystem.h"
//#include "textrenderingaspect.h"
#include "renderingaspect.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;

RenderingSystem::RenderingSystem(void)
{
    m_renderer = DrawSpace::Core::SingletonPlugin<DrawSpace::Interface::Renderer>::GetInstance()->m_interface;
}

RenderingSystem::~RenderingSystem(void)
{
}

void RenderingSystem::Run( RenderPassNodeGraph* p_rendergraph, EntityNodeGraph* p_entitygraph )
{
    // rendu des passes du rendergraph
    p_rendergraph->Accept( this );

    // rendu des eventuels displaytexts dans l'entitygraph
    p_entitygraph->AcceptRenderingSystem( this );
}

void RenderingSystem::VisitRenderPassDescr( RenderPassNode::PassDescr* p_passdescr ) const
{
    p_passdescr->m_renderingqueue->Draw();
}

void RenderingSystem::VisitEntity( Entity* p_entity ) const
{
    RenderingAspect* rendering_aspect = p_entity->GetAspect<RenderingAspect>();
    if( rendering_aspect )
    {
        rendering_aspect->draw();
    }


    /*
    TextRenderingAspect* textrendering_aspect = p_entity->GetAspect<TextRenderingAspect>();
    if( textrendering_aspect )
    {
        // extraire tout les composants "texts display"
        std::vector<Component<DrawSpace::Core::TextRenderingAspect::TextDisplay>*> texts;

        textrendering_aspect->GetComponentsByType<DrawSpace::Core::TextRenderingAspect::TextDisplay>( texts );

        for( size_t i = 0; i < texts.size(); i++ )
        {
            DrawSpace::Core::TextRenderingAspect::TextDisplay text_descr = texts[i]->getPurpose();

            m_renderer->BeginScreen();
            m_renderer->DrawText( text_descr.m_r, text_descr.m_g, text_descr.m_b, text_descr.m_posx, text_descr.m_posy, text_descr.m_text.c_str() );
            m_renderer->EndScreen();
        }
    }
    */
}