/* -*-LIC_BEGIN-*- */
/*
*
* DrawSpace Rendering engine
* Emmanuel Chaumont Copyright (c) 2013-2023
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

#include "stringrenderingaspectimpl.h"
#include "renderingaspect.h"
#include "plugin.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Aspect;

StringRenderingAspectImpl::StringRenderingAspectImpl( void )
{
    m_renderer = DrawSpace::Core::SingletonPlugin<DrawSpace::Interface::Renderer>::GetInstance()->m_interface;
}

void StringRenderingAspectImpl::Run( Entity* p_entity )
{
    RenderingAspect* rendering_aspect = p_entity->GetAspect<RenderingAspect>();
    // NB : rendering_aspect ne peut pas etre NULL (si on arrive ici c'est qu'il y a forcement un RenderingAspect dans l'entite

    // extraire tout les composants "texts display"
    ComponentList<TextDisplay> texts;
    rendering_aspect->GetComponentsByType<TextDisplay>( texts );

    for( size_t i = 0; i < texts.size(); i++ )
    {
        TextDisplay text_descr = texts[i]->getPurpose();

        m_renderer->BeginScreen();
        m_renderer->DrawText( text_descr.m_r, text_descr.m_g, text_descr.m_b, text_descr.m_posx, text_descr.m_posy, text_descr.m_text.c_str() );
        m_renderer->EndScreen();
    }

    ComponentList<std::vector<TextDisplay>> texts_v;
    rendering_aspect->GetComponentsByType<std::vector<TextDisplay>>( texts_v );

    for( size_t i = 0; i < texts_v.size(); i++ )
    {
        std::vector<TextDisplay> texts = texts_v[i]->getPurpose();

        for( size_t j = 0; j < texts.size(); j++ )
        {
            TextDisplay text_descr = texts[j];

            m_renderer->BeginScreen();
            m_renderer->DrawText( text_descr.m_r, text_descr.m_g, text_descr.m_b, text_descr.m_posx, text_descr.m_posy, text_descr.m_text.c_str() );
            m_renderer->EndScreen();
        }
    }
}
