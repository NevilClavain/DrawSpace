/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2014                                
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

#include "renderingqueue.h"
#include "renderer.h"
#include "plugin.h"

using namespace DrawSpace::Core;
using namespace DrawSpace::Interface;

RenderingQueue::RenderingQueue( void ) : 
m_target( NULL ),
m_clear_depth( false ),
m_clear_target( false ),
m_target_clear_color_r( 0 ),
m_target_clear_color_g( 0 ),
m_target_clear_color_b( 0 )
{


}

RenderingQueue::RenderingQueue( Texture* p_target ) : 
m_target( p_target ),
m_clear_depth( false ),
m_clear_target( false ),
m_target_clear_color_r( 0 ),
m_target_clear_color_g( 0 ),
m_target_clear_color_b( 0 )
{


}

RenderingQueue::~RenderingQueue( void )
{

}

void RenderingQueue::Draw( void )
{
    Renderer* renderer = SingletonPlugin<Renderer>::GetInstance()->m_interface;

    if( m_target )
    {
        renderer->BeginTarget( m_target );
    }
    else
    {
        renderer->BeginScreen();
    }

    if( m_clear_depth )
    {
        renderer->ClearDepth();
    }
    if( m_clear_target )
    {
        renderer->ClearScreen( m_target_clear_color_r, m_target_clear_color_g, m_target_clear_color_b );
    }

    for( unsigned long i = 0; i < m_nodes.size(); i++ )
    {    
        renderer->BeginNodeRender( m_nodes[i] );
        m_nodes[i]->OnDraw();
        renderer->EndNodeRender( m_nodes[i] );
    }

    if( m_target )
    {
        renderer->EndTarget( m_target );
    }
    else
    {
        renderer->EndScreen();
    }
}

void RenderingQueue::Add( RenderingNode* p_node )
{
    m_nodes.push_back( p_node );
}

void RenderingQueue::EnableDepthClearing( bool p_enable )
{
    m_clear_depth = p_enable;
}

void RenderingQueue::EnableTargetClearing( bool p_enable )
{
    m_clear_target = p_enable;
}

void RenderingQueue::SetTargetClearingColor( unsigned char p_r, unsigned char p_g, unsigned char p_b )
{
    m_target_clear_color_r = p_r;
    m_target_clear_color_g = p_g;
    m_target_clear_color_b = p_b;
}
