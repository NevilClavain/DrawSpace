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

#include "renderstatesset.h"
#include "md5.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Utils;

RenderStatesSet::RenderStatesSet( void )
{
}

RenderStatesSet::~RenderStatesSet( void )
{
}

void RenderStatesSet::AddRenderStateIn( const RenderState& p_renderstate )
{
    m_renderstates_in.push_back( p_renderstate );
}

void RenderStatesSet::AddRenderStateOut( const RenderState& p_renderstate )
{
    m_renderstates_out.push_back( p_renderstate );
}

void RenderStatesSet::UpdateRenderStateIn( int p_index,const RenderState& p_renderstate )
{
    m_renderstates_in.at(p_index) = p_renderstate;
}

void RenderStatesSet::UpdateRenderStateOut( int p_index, const RenderState& p_renderstate )
{
    m_renderstates_out.at(p_index) = p_renderstate;
}

void RenderStatesSet::UpdateRenderStateInExtendedArgs(int p_index, const std::vector<dsstring>& p_args)
{
    m_renderstates_in.at(p_index).SetExtendedArgs(p_args);
}

void RenderStatesSet::UpdateRenderStateOutExtendedArgs(int p_index, const std::vector<dsstring>& p_args)
{
    m_renderstates_out.at(p_index).SetExtendedArgs(p_args);
}

void RenderStatesSet::ClearRenderStateInExtendedArgs(int p_index)
{
    m_renderstates_in.at(p_index).ClearExtendedArgs();
}

void RenderStatesSet::ClearRenderStateOutExtendedArgs(int p_index)
{
    m_renderstates_out.at(p_index).ClearExtendedArgs();
}

void RenderStatesSet::PushRenderStateInExtendedArgs(int p_index, const dsstring& p_arg)
{
    m_renderstates_in.at(p_index).PushExtendedArg(p_arg);
}

void RenderStatesSet::PushRenderStateOutExtendedArgs(int p_index, const dsstring& p_arg)
{
    m_renderstates_out.at(p_index).PushExtendedArg(p_arg);
}


RenderState RenderStatesSet::GetRenderStateIn( long p_index ) const
{
    return m_renderstates_in.at(p_index);
}

RenderState RenderStatesSet::GetRenderStateOut( long p_index ) const
{
    return m_renderstates_out.at(p_index);
}

size_t RenderStatesSet::GetRenderStatesInListSize( void ) const
{
    return m_renderstates_in.size();
}

size_t RenderStatesSet::GetRenderStatesOutListSize( void ) const
{
    return m_renderstates_out.size();
}

void RenderStatesSet::GetRenderStatesSetMD5( dsstring& p_md5 ) const
{
    MD5 md5;

    dsstring hash_rs = "";
    dsstring hash_rsargs = "";

    unsigned long* renderstates;
    renderstates = new unsigned long[m_renderstates_in.size() + m_renderstates_out.size()];

    int count = 0;
    for( size_t i = 0; i < m_renderstates_in.size(); i++ )
    {
        renderstates[count++] = (unsigned long)m_renderstates_in[i].GetOperation();
    }
    for( size_t i = 0; i < m_renderstates_out.size(); i++ )
    {
        renderstates[count++] = (unsigned long)m_renderstates_out[i].GetOperation();
    }

    if( count > 0 )
    {
        hash_rs = md5.digestMemory( (BYTE*)renderstates, count * sizeof( unsigned long ) );
    }

    delete[] renderstates;

    dsstring renderstates_arg = "";
    for( size_t i = 0; i < m_renderstates_in.size(); i++ )
    {
        const auto arg{ m_renderstates_in[i].GetArg() };
        renderstates_arg += arg;
    }

    if( m_renderstates_in.size() > 0 )
    {
        hash_rsargs = md5.digestMemory( (BYTE*)renderstates_arg.c_str(), (int)renderstates_arg.size() );
    }
    p_md5 = hash_rs + hash_rsargs + m_renderstate_unique_queue_id;
}

void RenderStatesSet::SetRenderStateUniqueQueueID( const dsstring& p_id )
{
    m_renderstate_unique_queue_id = p_id;
}
