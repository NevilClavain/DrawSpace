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

#include "renderingnode.h"
#include "memalloc.h"
#include "exceptions.h"

using namespace DrawSpace::Core;
using namespace DrawSpace::Utils;

RenderingNode::RenderingNode( void ) : 
m_order( 10000 ), 
m_handler( NULL ), 
m_meshe( NULL ),
m_fx( NULL ),
m_drawing_enabled( true )
{ 
    for( long i = 0; i < NbMaxTextures; i++ )
    {
        m_textures[i] = NULL;
        m_vertextextures[i] = NULL;
    }
}

RenderingNode::~RenderingNode( void )
{
    CleanupShaderParams();
}

long RenderingNode::GetOrderNumber( void ) const
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

Texture* RenderingNode::GetTexture( long p_index ) const
{
    return m_textures[p_index];
}

Texture* RenderingNode::GetVertexTexture( long p_index ) const
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

void RenderingNode::AddShaderArrayParameter(long p_shader_index, const dsstring& p_id, long p_begin_register)
{
	ShadersArrayParam* sap = _DRAWSPACE_NEW_(ShadersArrayParam, ShadersArrayParam);
	sap->begin_register = p_begin_register;
	sap->shader_index = p_shader_index;
	m_shaders_array_params[p_id] = sap;
}

void RenderingNode::SetShaderArrayParameter(const dsstring& p_id, const std::vector<DrawSpace::Utils::Vector>& p_array)
{
	if (m_shaders_array_params.count(p_id) > 0)
	{
		m_shaders_array_params.at(p_id)->array = p_array;
	}
	else
	{
		_DSEXCEPTION("Unknown shader array param id")
	}
}

void RenderingNode::CleanupShaderArrayParams(void)
{
	for (auto& e : m_shaders_array_params)
	{
		_DRAWSPACE_DELETE_(e.second);
	}
	m_shaders_array_params.clear();
}

void RenderingNode::GetShadersArrayParams(std::map<dsstring, ShadersArrayParam*>& p_outlist) const
{
	p_outlist = m_shaders_array_params;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////

void RenderingNode::AddShaderParameter( long p_shader_index, const dsstring& p_id, long p_register )
{
    ShadersParams* sp = _DRAWSPACE_NEW_( ShadersParams, ShadersParams );

    sp->shader_index = p_shader_index;
    sp->param_register = p_register;

    m_shader_params[p_id] = sp;
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
        
        m_shader_params.at(p_id)->param_values = vec;
        m_shader_params.at(p_id)->vector = true;
    }
    else
    {
        _DSEXCEPTION( "Unknown shader param id" )
    }
}

void RenderingNode::SetShaderRealVector( const dsstring& p_id, const Vector& p_value )
{
    if( m_shader_params.count( p_id ) > 0 )
    {
        Vector vec = p_value;

        m_shader_params.at(p_id)->param_values = vec;
        m_shader_params.at(p_id)->vector = true;
    }
    else
    {
        _DSEXCEPTION( "Unknown shader param id" )
    }
}

void RenderingNode::SetShaderRealInVector(const dsstring& p_id, int p_index_in_vector, dsreal p_value)
{
	if (m_shader_params.count(p_id) > 0)
	{
		if (p_index_in_vector < 4)
		{
			m_shader_params.at(p_id)->param_values[p_index_in_vector] = p_value;
			m_shader_params.at(p_id)->vector = true;
		}
		else
		{
			_DSEXCEPTION("Bad scalar index in vector")
		}
	}
	else
	{
		_DSEXCEPTION("Unknown shader param id")
	}
}

void RenderingNode::SetShaderRealMatrix( const dsstring& p_id, const Matrix& p_value )
{
    if( m_shader_params.count( p_id ) > 0 )
    {
        m_shader_params.at(p_id)->mat = p_value;
        m_shader_params.at(p_id)->vector = false;
    }
    else
    {
        _DSEXCEPTION( "Unknown shader param id" )
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
        
        m_shader_params.at(p_id)->param_values = vec;
        m_shader_params.at(p_id)->vector = true;
    }
    else
    {
        _DSEXCEPTION( "Unknown shader param id" )
    }
}

void RenderingNode::UpdateShaderParams( const dsstring& p_id, RenderingNode::ShadersParams& p_params )
{
    if( 0 == m_shader_params.count( p_id ) > 0 )
    {
        m_shader_params[p_id] = _DRAWSPACE_NEW_( ShadersParams, ShadersParams );
    }

    *( m_shader_params[p_id] ) = p_params;
}

void RenderingNode::GetShadersParams( std::map<dsstring, ShadersParams*>& p_outlist ) const
{
    p_outlist = m_shader_params;
}

void RenderingNode::CleanupShaderParams( void )
{
	for (auto& e : m_shader_params)
	{
		_DRAWSPACE_DELETE_(e.second);
	}
    m_shader_params.clear();

	for (auto& e : m_shaders_array_params)
	{
		_DRAWSPACE_DELETE_(e.second);
	}
	m_shaders_array_params.clear();
}

void RenderingNode::SetDrawingState( bool p_drawing )
{
    m_drawing_enabled = p_drawing;
}
