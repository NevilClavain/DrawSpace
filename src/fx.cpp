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

#include "fx.h"
#include "renderer.h"
#include "plugin.h"

using namespace DrawSpace::Core;
using namespace DrawSpace::Utils;
using namespace DrawSpace::Interface;

Fx::Fx( void )
{

}

Fx::~Fx( void )
{


}

Shader* Fx::GetShader( long p_index )
{
    return m_shaders[p_index];
}

RenderState Fx::GetRenderStateIn( long p_index )
{
    return m_renderstates_in[p_index];
}

RenderState Fx::GetRenderStateOut( long p_index )
{
    return m_renderstates_out[p_index];
}

long Fx::GetShadersListSize( void )
{
    return (long)m_shaders.size();
}

long Fx::GetRenderStatesInListSize( void )
{
    return (long)m_renderstates_in.size();
}

long Fx::GetRenderStatesOutListSize( void )
{
    return (long)m_renderstates_out.size();
}

void Fx::AddShader( Shader* p_shader )
{
    m_shaders.push_back( p_shader );
}

void Fx::AddRenderStateIn( const RenderState& p_renderstate )
{
    m_renderstates_in.push_back( p_renderstate );
}

void Fx::AddRenderStateOut( const RenderState& p_renderstate )
{
    m_renderstates_out.push_back( p_renderstate );
}

void Fx::AddShaderRealParameter( long p_shader_index, const dsstring& p_id, long p_register )
{
    m_shader_realparams[p_id].first = p_shader_index;
    m_shader_realparams[p_id].second = p_register;
}

void Fx::AddShaderRealVectorParameter( long p_shader_index, const dsstring& p_id, long p_register )
{
    m_shader_realvectorparams[p_id].first = p_shader_index;
    m_shader_realvectorparams[p_id].second = p_register;
}

void Fx::AddShaderBoolParameter( long p_shader_index, const dsstring& p_id, long p_register )
{
    m_shader_boolparams[p_id].first = p_shader_index;
    m_shader_boolparams[p_id].second = p_register;
}

void Fx::SetShaderReal( const dsstring& p_id, dsreal p_value )
{
    if( m_shader_realparams.count( p_id ) > 0 )
    {
        Utils::Vector vec;
        vec[0] = p_value;
        vec[1] = 0.0;
        vec[2] = 0.0;
        vec[3] = 1.0;
        m_shaders[m_shader_realparams[p_id].first]->SetParam( m_shader_realparams[p_id].second, vec );
    }
}

void Fx::SetShaderRealVector( const dsstring& p_id, const Vector& p_value )
{
    if( m_shader_realvectorparams.count( p_id ) > 0 )
    {
        Utils::Vector vec = p_value;
        m_shaders[m_shader_realvectorparams[p_id].first]->SetParam( m_shader_realvectorparams[p_id].second, vec );
    }
}

void Fx::SetShaderBool( const dsstring& p_id, bool p_value )
{
    if( m_shader_boolparams.count( p_id ) > 0 )
    {
        Utils::Vector vec;
        vec[0] = ( p_value ? 1.0 : 0.0 );
        vec[1] = 0.0;
        vec[2] = 0.0;
        vec[3] = 1.0;
        m_shaders[m_shader_boolparams[p_id].first]->SetParam( m_shader_boolparams[p_id].second, vec );
    }
}


void Fx::Serialize( Factory& p_factory, Archive& p_archive )
{

}
void Fx::Unserialize( Factory& p_factory, Archive& p_archive )
{

}




