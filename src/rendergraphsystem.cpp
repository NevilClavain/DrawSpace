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

void RendergraphSystem::on_entity_added_action( int p_actionid, ecs::BaseArguments* p_args, ecs::BaseComponent* p_c1, ecs::BaseComponent* p_c2 ) const
{
    switch( p_actionid )
    {
        case MakeRenderingQueueOnScreenOperation:
        {
            ecs::Component<DrawSpace::Core::RenderingQueue>* screen_renderingqueue_comp = static_cast<ecs::Component<DrawSpace::Core::RenderingQueue>*>( p_c1 );
            screen_renderingqueue_comp->MakePurpose();
        }
        break;

        /*
        case MakeTextOperation:
        {
            ecs::ComponentMultiple<Text>* text_comp = static_cast<ecs::ComponentMultiple<Text>*>( p_c1 );
                       
            ecs::Arguments<Text>* args = static_cast<ecs::Arguments<Text>*>( p_args );
            Text text = std::get<0>( args->GetArg() );

            text_comp->MakePurpose();

            text_comp->GetLast().r = text.r;
            text_comp->GetLast().g = text.g;
            text_comp->GetLast().b = text.b;
            text_comp->GetLast().x = text.x;
            text_comp->GetLast().y = text.y;
            text_comp->GetLast().text = text.text;            
        }
        break;

        case MakeRenderingQueueStatesOperation:
        {
            ecs::Component<RenderingQueueStates>* rq_states = static_cast<ecs::Component<RenderingQueueStates>*>( p_c1 );

            ecs::Arguments<RenderingQueueStates>* args = static_cast<ecs::Arguments<RenderingQueueStates>*>( p_args );
            RenderingQueueStates states = std::get<0>( args->GetArg() );

            rq_states->MakePurpose(); 

            rq_states->GetPurpose() = states;
        }
        break;
        */

        case SetRenderingQueueStates:
        {
            ecs::Component<DrawSpace::Core::RenderingQueue>* renderingqueue_comp = static_cast<ecs::Component<DrawSpace::Core::RenderingQueue>*>( p_c1 );
            ecs::Component<RenderingQueueStates>* rq_states = static_cast<ecs::Component<RenderingQueueStates>*>( p_c2 );  

            RenderingQueueStates states = rq_states->GetPurpose();

            renderingqueue_comp->GetPurpose().EnableDepthClearing( states.depth_clearing_enabled );
            renderingqueue_comp->GetPurpose().EnableTargetClearing( states.target_clearing_enabled );
            renderingqueue_comp->GetPurpose().SetTargetClearingColor( states.target_clear_r, states.target_clear_g, states.target_clear_b, states.target_clear_a );        
        }

    }
}

void RendergraphSystem::on_entity_visited_action( int p_actionid, ecs::BaseArguments* p_args, ecs::BaseComponent* p_c1, ecs::BaseComponent* p_c2 ) const
{
    switch( p_actionid )
    {        
        case DrawRenderingQueueOperation:
        {
            ecs::Component<DrawSpace::Core::RenderingQueue>* screen_renderingqueue_comp = static_cast<ecs::Component<DrawSpace::Core::RenderingQueue>*>( p_c1 );
            screen_renderingqueue_comp->GetPurpose().Draw();
        }
        break;

        case DrawTextsOperation:
        {
            ecs::ComponentMultiple<Text>* texts = static_cast<ecs::ComponentMultiple<Text>*>( p_c1 );

            size_t nb_texts = texts->GetSize();

            m_renderer->BeginScreen();
            for( size_t i = 0; i < nb_texts; i++ )
            {
                Text text_descr = texts->GetPurpose( i );
                m_renderer->DrawText( text_descr.r, text_descr.g, text_descr.b, text_descr.x, text_descr.y, text_descr.text.c_str() );
            }
            m_renderer->EndScreen();        
        }
        break;
    }
}
