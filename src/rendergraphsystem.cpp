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

#include "rendergraphsystem.h"
#include "plugin.h"
#include "renderingqueue.h"

using namespace DrawSpace;
using namespace DrawSpace::ecs;


RendergraphSystem::RendergraphSystem( void )
{
    m_renderer = DrawSpace::Core::SingletonPlugin<DrawSpace::Interface::Renderer>::GetInstance()->m_interface;
}
    
RendergraphSystem::~RendergraphSystem( void )
{
}

void RendergraphSystem::on_entity_added_action( int p_actionid, ecs::BaseArguments* p_args, ecs::BaseComponent* p_src, ecs::BaseComponent* p_dst ) const
{
    switch( p_actionid )
    {
        case MakeRenderingQueueOnScreenOperation:
        {
            ecs::Component<DrawSpace::Core::RenderingQueue>* screen_renderingqueue_comp = static_cast<ecs::Component<DrawSpace::Core::RenderingQueue>*>( p_src );
            screen_renderingqueue_comp->MakePurpose();
        }
        break;

        case MakeScreenColorOperation:
        {
            ecs::Component<Color>* screen_color_comp = static_cast<ecs::Component<Color>*>( p_src );

            ecs::Arguments<Color>* args = static_cast<ecs::Arguments<Color>*>( p_args );
            Color rgba = std::get<0>( args->GetArg() );

            screen_color_comp->MakePurpose();

            screen_color_comp->getPurpose().r = rgba.r;
            screen_color_comp->getPurpose().g = rgba.g;
            screen_color_comp->getPurpose().b = rgba.b;
            screen_color_comp->getPurpose().a = rgba.a;
        }
        break;

        case InitScreenColor:
        {
            ecs::Component<DrawSpace::Core::RenderingQueue>* screen_renderingqueue_comp = static_cast<ecs::Component<DrawSpace::Core::RenderingQueue>*>( p_src );
            ecs::Component<Color>* screen_color = static_cast<ecs::Component<Color>*>( p_dst );
           
            Color rgba = screen_color->getPurpose();

            screen_renderingqueue_comp->getPurpose().EnableTargetClearing( true );
            screen_renderingqueue_comp->getPurpose().SetTargetClearingColor( rgba.r, rgba.g, rgba.b, rgba.a );        
        }
        break;

    }
}

void RendergraphSystem::on_entity_visited_action( int p_actionid, ecs::BaseArguments* p_args, ecs::BaseComponent* p_src, ecs::BaseComponent* p_dst ) const
{
    switch( p_actionid )
    {        
        case DrawRenderingQueueOperation:
        {
            ecs::Component<DrawSpace::Core::RenderingQueue>* screen_renderingqueue_comp = static_cast<ecs::Component<DrawSpace::Core::RenderingQueue>*>( p_src );
            screen_renderingqueue_comp->getPurpose().Draw();
        }
        break;

        case UpdateScreenColor:
        {
            ecs::Component<DrawSpace::Core::RenderingQueue>* screen_renderingqueue_comp = static_cast<ecs::Component<DrawSpace::Core::RenderingQueue>*>( p_src );
            ecs::Component<Color>* screen_color = static_cast<ecs::Component<Color>*>( p_dst );
           
            Color rgba = screen_color->getPurpose();

            screen_renderingqueue_comp->getPurpose().EnableTargetClearing( true );
            screen_renderingqueue_comp->getPurpose().SetTargetClearingColor( rgba.r, rgba.g, rgba.b, rgba.a );        
        }
        break;

        case UpdateColor:
        {
            ecs::Component<Color>* screen_color = static_cast<ecs::Component<Color>*>( p_src );
            
            ecs::Arguments<Color>* args = static_cast<ecs::Arguments<Color>*>( p_args );
            Color rgba = std::get<0>( args->GetArg() );

            screen_color->getPurpose().r = rgba.r;
            screen_color->getPurpose().g = rgba.g;
            screen_color->getPurpose().b = rgba.b;
            screen_color->getPurpose().a = rgba.a;
        }
        break;

        /*
        case DrawTextOperation:
        {
            ecs::Arguments<Text>* args = static_cast<ecs::Arguments<Text>*>( p_args );

            Text text_descr = std::get<0>( args->GetArg() );

            m_renderer->BeginScreen();
            m_renderer->DrawText( text_descr.r, text_descr.g, text_descr.b, text_descr.x, text_descr.y, text_descr.text.c_str() );
            m_renderer->EndScreen();
        }
        break;

        case RenderingQueueSetTargetClearingColorsOperation:
        {
            ecs::Component<DrawSpace::Core::RenderingQueue>* screen_renderingqueue_comp = static_cast<ecs::Component<DrawSpace::Core::RenderingQueue>*>( p_src );
            
            ecs::Arguments<Color>* args = static_cast<ecs::Arguments<Color>*>( p_args );

            Color rgba = std::get<0>( args->GetArg() );

            screen_renderingqueue_comp->getPurpose().EnableTargetClearing( true );
            screen_renderingqueue_comp->getPurpose().SetTargetClearingColor( rgba.r, rgba.g, rgba.b, rgba.a );

        }
        break;
        */
    }
}
