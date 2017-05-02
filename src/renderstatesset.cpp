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

#include "renderstatesset.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;

RenderStatesSet::RenderStatesSet( void )
{
}

RenderStatesSet::RenderStatesSet( const dsstring& p_filepath ) :
m_path( p_filepath )
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
    m_renderstates_in[p_index] = p_renderstate;
}

void RenderStatesSet::UpdateRenderStateOut( int p_index, const RenderState& p_renderstate )
{
    m_renderstates_out[p_index] = p_renderstate;
}

bool RenderStatesSet::LoadFromFile( void )
{
    return true;
}

dsstring RenderStatesSet::compute_final_path( void )
{
    dsstring final_path = m_rootpath + "/";
    
    final_path += m_path;
    return final_path;
}

void RenderStatesSet::SetRootPath( const dsstring& p_path )
{
    m_rootpath = p_path;
}


RenderState RenderStatesSet::GetRenderStateIn( long p_index )
{
    return m_renderstates_in[p_index];
}

RenderState RenderStatesSet::GetRenderStateOut( long p_index )
{
    return m_renderstates_out[p_index];
}

long RenderStatesSet::GetRenderStatesInListSize( void )
{
    return (long)m_renderstates_in.size();
}

long RenderStatesSet::GetRenderStatesOutListSize( void )
{
    return (long)m_renderstates_out.size();
}