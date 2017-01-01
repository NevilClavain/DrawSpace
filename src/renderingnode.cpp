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

#include "renderingnode.h"
#include "memalloc.h"
#include "shaderscontroller.h"

using namespace DrawSpace::Core;
using namespace DrawSpace::Utils;

RenderingNode::RenderingNode( void ) : 
m_order( 10000 ), 
m_handler( NULL ), 
m_meshe( NULL ),
m_fx( NULL ),
m_drawing_enabled( true )
{
    //m_fx = _DRAWSPACE_NEW_( Fx, Fx );
    
    for( long i = 0; i < NbMaxTextures; i++ )
    {
        m_textures[i] = NULL;
        m_vertextextures[i] = NULL;
    }
}

RenderingNode::~RenderingNode( void )
{
    //_DRAWSPACE_DELETE_( m_fx );
}

long RenderingNode::GetOrderNumber( void )
{
    return m_order;
}

void RenderingNode::SetOrderNumber( long p_order )
{
    m_order = p_order;
}

void RenderingNode::SetTexture( Texture* p_texture, long p_stage )
{
    m_textures[p_stage] = p_texture;
}

void RenderingNode::SetVertexTexture( Texture* p_texture, long p_stage )
{
    m_vertextextures[p_stage] = p_texture;
}

long RenderingNode::GetTextureListSize( void )
{
    return NbMaxTextures;
}

Texture* RenderingNode::GetTexture( long p_index )
{
    return m_textures[p_index];
}

Texture* RenderingNode::GetVertexTexture( long p_index )
{
    return m_vertextextures[p_index];
}

void RenderingNode::OnDraw( void )
{
    (*m_handler)( this );
}

void RenderingNode::RegisterHandler( BaseCallback<void, RenderingNode*>* p_handler )
{
    m_handler = p_handler;
}

void RenderingNode::AddShaderParameter( long p_shader_index, const dsstring& p_id, long p_register )
{
    ShadersParams* sp = _DRAWSPACE_NEW_( ShadersParams, ShadersParams );

    sp->shader_index = p_shader_index;
    sp->param_register = p_register;

    m_shader_params[p_id] = sp;

    ShadersController::GetInstance()->RegisterRenderingNode( this );
}

void RenderingNode::SetShaderReal( const dsstring& p_id, dsreal p_value )
{
    if( m_shader_params.count( p_id ) > 0 )
    {
        Vector vec;
        vec[0] = p_value;
        vec[1] = 0.0;
        vec[2] = 0.0;
        vec[3] = 1.0;
        
        m_shader_params[p_id]->param_values = vec;
        m_shader_params[p_id]->vector = true;
    }
}

void RenderingNode::SetShaderRealVector( const dsstring& p_id, const Vector& p_value )
{
    if( m_shader_params.count( p_id ) > 0 )
    {
        Vector vec = p_value;

        m_shader_params[p_id]->param_values = vec;
        m_shader_params[p_id]->vector = true;
    }
}

void RenderingNode::SetShaderRealMatrix( const dsstring& p_id, const Matrix& p_value )
{
    if( m_shader_params.count( p_id ) > 0 )
    {
        m_shader_params[p_id]->mat = p_value;
        m_shader_params[p_id]->vector = false;
    }
}

void RenderingNode::SetShaderBool( const dsstring& p_id, bool p_value )
{
    if( m_shader_params.count( p_id ) > 0 )
    {
        Vector vec;
        vec[0] = ( p_value ? 1.0 : 0.0 );
        vec[1] = 0.0;
        vec[2] = 0.0;
        vec[3] = 1.0;
        
        m_shader_params[p_id]->param_values = vec;
        m_shader_params[p_id]->vector = true;
    }
}

void RenderingNode::GetShadersParams( std::map<dsstring, ShadersParams*>& p_outlist )
{
    p_outlist = m_shader_params;
}

void RenderingNode::SetDrawingState( bool p_drawing )
{
    m_drawing_enabled = p_drawing;
}
