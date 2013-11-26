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

#include "renderingnode.h"
#include "memalloc.h"

using namespace DrawSpace::Core;
using namespace DrawSpace::Utils;

RenderingNode::RenderingNode( void ) : m_order( 1000 ), m_handler( NULL )
{
    m_fx = _DRAWSPACE_NEW_( Fx, Fx );
    for( long i = 0; i < 32; i++ )
    {
        m_textures[i] = NULL;
    }
}

RenderingNode::~RenderingNode( void )
{
    _DRAWSPACE_DELETE_( m_fx );
}

long RenderingNode::GetOrderNumber( void )
{
    return m_order;
}

void RenderingNode::SetFx( Fx* p_fx )
{
    m_fx = p_fx;
}
void RenderingNode::SetTexture( Texture* p_texture, long p_stage )
{
    m_textures[p_stage] = p_texture;
}

long RenderingNode::GetTextureListSize( void )
{
    return 32;
}

Texture* RenderingNode::GetTexture( long p_index )
{
    return m_textures[p_index];
}

void RenderingNode::OnDraw( void )
{
    (*m_handler)( this );
}

void RenderingNode::RegisterHandler( BaseCallback<void, RenderingNode*>* p_handler )
{
    m_handler = p_handler;
}