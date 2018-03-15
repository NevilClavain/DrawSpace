/* -*-LIC_BEGIN-*- */
/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2018                        
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


#include "fx.h"
#include "renderer.h"
#include "md5.h"
#include "plugin.h"
#include "exceptions.h"

using namespace DrawSpace;
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
    //return m_renderstates_in[p_index];
    return m_renderstates.GetRenderStateIn( p_index );
}

RenderState Fx::GetRenderStateOut( long p_index )
{
    //return m_renderstates_out[p_index];
    return m_renderstates.GetRenderStateOut( p_index );
}

long Fx::GetShadersListSize( void )
{
    return (long)m_shaders.size();
}

void Fx::ClearShaders( void )
{
    m_shaders.clear();
}

long Fx::GetRenderStatesInListSize( void )
{
    //return (long)m_renderstates_in.size();
    return m_renderstates.GetRenderStatesInListSize();
}

long Fx::GetRenderStatesOutListSize( void )
{
    //return (long)m_renderstates_out.size();
    return m_renderstates.GetRenderStatesOutListSize();
}

void Fx::AddShader( Shader* p_shader )
{
    m_shaders.push_back( p_shader );
}


void Fx::SetRenderStates( const RenderStatesSet& p_renderstates )
{
    m_renderstates = p_renderstates;
}


void Fx::GetShadersMD5( dsstring& p_md5 )
{
    MD5 md5;

    unsigned char* shaders;

    dsstring hash_shaders = "";


    size_t total_shaders_data_size = 0;
    for( size_t i = 0; i < m_shaders.size(); i++ )
    {
        total_shaders_data_size += m_shaders[i]->GetDataSize();
    }

    shaders = new unsigned char[total_shaders_data_size];

    unsigned char* curr = shaders;
    for( size_t i = 0; i < m_shaders.size(); i++ )
    {
        memcpy( curr, m_shaders[i]->GetData(), m_shaders[i]->GetDataSize() );

        curr += m_shaders[i]->GetDataSize();
    }

    if( m_shaders.size() > 0 )
    {
        hash_shaders = md5.digestMemory( (BYTE*)shaders, (int)( total_shaders_data_size ) );
    }

    delete[] shaders;
    
    p_md5 = hash_shaders;
}

void Fx::GetRenderStatesSetMD5( dsstring& p_md5 )
{
    m_renderstates.GetRenderStatesSetMD5( p_md5 );
}



void Fx::SetRenderStateUniqueQueueID( const dsstring& p_id )
{
    //m_renderstate_unique_queue_id = p_id;
    m_renderstates.SetRenderStateUniqueQueueID( p_id );
}

RenderStatesSet* Fx::GetRenderStatesSetRef( void )
{
    return &m_renderstates;
}
