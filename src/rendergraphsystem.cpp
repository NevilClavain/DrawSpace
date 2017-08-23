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
    switch (p_actionid)
    {
        /*
        case MakeColorParamOperation:
        {
            ecs::Arguments<unsigned char, unsigned char, unsigned char, unsigned char>* args = 
                static_cast<ecs::Arguments<unsigned char, unsigned char, unsigned char, unsigned char>*>(p_args);

            ecs::Component<ecs::RGBAColor>* rgbacolor_comp = static_cast<ecs::Component<ecs::RGBAColor>*>( p_src );

            unsigned char r = std::get<0>(args->GetArg());
            unsigned char g = std::get<1>(args->GetArg());
            unsigned char b = std::get<2>(args->GetArg());
            unsigned char a = std::get<3>(args->GetArg());

            rgbacolor_comp->MakePurpose( r, g, b, a );


        }
        break;

        */

        case MakeRenderingQueueOnScreenOperation:
            {
                ecs::Component<DrawSpace::Core::RenderingQueue>* screen_renderingqueue_comp = static_cast<ecs::Component<DrawSpace::Core::RenderingQueue>*>( p_src );
                screen_renderingqueue_comp->MakePurpose();
            }
            break;

    }


    /*
    switch (p_actionid)
    {
        case MakeTextOperation:
        {
            ecs::Arguments<ecs::Component<Text>*>* args = static_cast<ecs::Arguments<ecs::Component<Text>*>*>(p_args);
            ecs::Component<Text>* text_component = std::get<0>(args->Get());
            text_component->MakePurpose();
        }
        break;

        case MakeRenderingQueueOnScreenOperation:
        {
            ecs::Arguments<ecs::Component<DrawSpace::Core::RenderingQueue>*>* args = static_cast<ecs::Arguments<ecs::Component<DrawSpace::Core::RenderingQueue>*>*>(p_args);
            ecs::Component<DrawSpace::Core::RenderingQueue>* renderingqueue_component = std::get<0>(args->Get());
            renderingqueue_component->MakePurpose();
        }
        break;

        case MakeBoolParamOperation:
        {
            ecs::Arguments<ecs::Component<bool>*, bool>* args = static_cast<ecs::Arguments<ecs::Component<bool>*, bool>*>(p_args);
            ecs::Component<bool>* bool_component = std::get<0>(args->Get());
            bool init = std::get<1>(args->Get());
            bool_component->MakePurpose( init );        
        }
        break;

        case MakeColorParamOperation:
        {
            ecs::Arguments<ecs::Component<ecs::RGBAColor>*, unsigned char, unsigned char, unsigned char, unsigned char>* args = 
                static_cast<ecs::Arguments<ecs::Component<ecs::RGBAColor>*, unsigned char, unsigned char, unsigned char, unsigned char>*>(p_args);
            
            ecs::Component<ecs::RGBAColor>* rgba_component = std::get<0>(args->Get());

            unsigned char r = std::get<1>(args->Get());
            unsigned char g = std::get<2>(args->Get());
            unsigned char b = std::get<3>(args->Get());
            unsigned char a = std::get<4>(args->Get());

            rgba_component->MakePurpose( r, g, b, a );         
        }
        break;

        case MakeRenderingQueueOnTargetOperation:
        {
        }
        break;

        case RenderingQueueSetTargetClearingColorsOperation:
        {
            ecs::Arguments<ecs::Component<DrawSpace::Core::RenderingQueue>*, ecs::Component<ecs::RGBAColor>*>* args =
                static_cast<ecs::Arguments<ecs::Component<DrawSpace::Core::RenderingQueue>*, ecs::Component<ecs::RGBAColor>*>*>(p_args);           

            ecs::Component<DrawSpace::Core::RenderingQueue>* renderingqueue_component = std::get<0>(args->Get());
            ecs::Component<ecs::RGBAColor>* color = std::get<1>(args->Get());

            renderingqueue_component->getPurpose().SetTargetClearingColor( color->getPurpose().r, color->getPurpose().g, color->getPurpose().b, color->getPurpose().a );
        }
        break;

        case RenderingQueueEnableTargetClearingOperation:
        {
            ecs::Arguments<ecs::Component<DrawSpace::Core::RenderingQueue>*, ecs::Component<bool>*>* args =
                static_cast<ecs::Arguments<ecs::Component<DrawSpace::Core::RenderingQueue>*, ecs::Component<bool>*>*>(p_args);

            ecs::Component<DrawSpace::Core::RenderingQueue>* renderingqueue_component = std::get<0>(args->Get());
            ecs::Component<bool>* enable_target_clearing = std::get<1>(args->Get());

            renderingqueue_component->getPurpose().EnableTargetClearing( enable_target_clearing->getPurpose() );
        }
        break;

        case RenderingEnableDepthClearingOperation:
        {
            ecs::Arguments<ecs::Component<DrawSpace::Core::RenderingQueue>*, ecs::Component<bool>*>* args =
                static_cast<ecs::Arguments<ecs::Component<DrawSpace::Core::RenderingQueue>*, ecs::Component<bool>*>*>(p_args);

            ecs::Component<DrawSpace::Core::RenderingQueue>* renderingqueue_component = std::get<0>(args->Get());
            ecs::Component<bool>* enable_depth_clearing = std::get<1>(args->Get());

            renderingqueue_component->getPurpose().EnableTargetClearing( enable_depth_clearing->getPurpose() );   
        }
        break;
    }
    */
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
    }

    /*
    switch (p_actionid)
    {
        
        case DrawTextOperation:
        {
            ecs::Arguments<ecs::Component<Text>*>* args = static_cast<ecs::Arguments<ecs::Component<Text>*>*>(p_args);
            ecs::Component<Text>* text_component = std::get<0>(args->Get());

            m_renderer->BeginScreen();
            m_renderer->DrawText( text_component->getPurpose().m_r, text_component->getPurpose().m_g, text_component->getPurpose().m_b,
                                    text_component->getPurpose().m_x, text_component->getPurpose().m_y, 
                                    text_component->getPurpose().m_text.c_str() );                                                
            m_renderer->EndScreen();
        }
        break;

        case DrawRenderingQueueOperation:
        {
            ecs::Arguments<ecs::Component<DrawSpace::Core::RenderingQueue>*>* args = static_cast<ecs::Arguments<ecs::Component<DrawSpace::Core::RenderingQueue>*>*>(p_args);
            ecs::Component<DrawSpace::Core::RenderingQueue>* renderingqueue_component = std::get<0>(args->Get());

            renderingqueue_component->getPurpose().Draw();
        }
        break;
    }
    */
}
