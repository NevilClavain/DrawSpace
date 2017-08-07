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

using namespace DrawSpace;
using namespace DrawSpace::ecs;

RendergraphSystem::RendergraphSystem( void )
{
    m_renderer = DrawSpace::Core::SingletonPlugin<DrawSpace::Interface::Renderer>::GetInstance()->m_interface;
}
    
RendergraphSystem::~RendergraphSystem( void )
{
}

void RendergraphSystem::on_entity_visited_action( int p_actionid, ecs::BaseArguments* p_args ) const
{
    switch (p_actionid)
    {
        case DrawTextOperation:
        {
            ecs::Arguments<ecs::Component<Text>*>* args = static_cast<ecs::Arguments<ecs::Component<Text>*>*>(p_args);
            ecs::Component<Text>* text_component = std::get<0>(args->Get());

            m_renderer->DrawText( text_component->getPurpose().m_r, text_component->getPurpose().m_g, text_component->getPurpose().m_b,
                                    text_component->getPurpose().m_x, text_component->getPurpose().m_y, 
                                    text_component->getPurpose().m_text.c_str() );                                                
        }
    }
}
void RendergraphSystem::on_entity_added_action( int p_actionid, ecs::BaseArguments* p_args ) const
{
    switch (p_actionid)
    {
        case MakeTextOperation:
        {
            ecs::Arguments<ecs::Component<Text>*>* args = static_cast<ecs::Arguments<ecs::Component<Text>*>*>(p_args);
            ecs::Component<Text>* text_component = std::get<0>(args->Get());
            text_component->MakePurpose();
        }
    }
}
