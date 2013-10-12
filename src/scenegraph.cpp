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

#include "scenegraph.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;

Scenegraph::Scenegraph( void ) : m_camera( NULL )
{
}

Scenegraph::~Scenegraph( void )
{
}

void Scenegraph::RegisterPass( Pass* p_pass )
{
    dsstring pass_name;
    p_pass->GetName( pass_name );
    m_passes[pass_name] = p_pass;    
}

bool Scenegraph::RegisterNode( TransformNode* p_node )
{
    if( false == TransformQueue::Add( p_node ) )
    {
        return false;
    }
    p_node->OnRegister( this );
    return true;
}

Pass* Scenegraph::GetPass( const dsstring& p_passname )
{
    if( m_passes.count( p_passname ) > 0 )
    {
        return m_passes[p_passname];
    }
    return NULL;
}

TransformNode* Scenegraph::GetCurrentCamera( void )
{
    return m_camera;
}

bool Scenegraph::SetCurrentCamera( const dsstring& p_nodename )
{
    if( m_nodes.count( p_nodename ) > 0 )
    {
        m_camera = m_nodes[p_nodename];
        return true;
    }
    return false;
}