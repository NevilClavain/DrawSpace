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

#include "transformqueue.h"
#include "tracedefs.h"
_DECLARE_DS_LOGGER( logger, "TransformQueue" )

using namespace DrawSpace::Core;
using namespace DrawSpace::Utils;

TransformQueue::TransformQueue( void )
{
}

TransformQueue::~TransformQueue( void )
{

}

void TransformQueue::ComputeTransformations( void )
{
    Matrix view;
    view.Identity();

    if( "" != m_camera_node )
    {
        TransformNode* view_node = m_nodes[m_camera_node];
        view_node->GetSceneWorld( view );
    }

    for( std::map<dsstring, TransformNode*>::iterator it = m_nodes.begin(); it != m_nodes.end(); ++it )
    {
        (*it).second->ComputeFinalTransform();
    }
}

bool TransformQueue::Add( TransformNode* p_node )
{
    dsstring name;
    p_node->GetName( name );

    if( "" == name )
    {
        // mettre une trace error
        _DSERROR( logger, "node has a void name !" )
        return false;
    }

    if( m_nodes.count( name ) > 0 )
    {
        // mettre une trace warning
        _DSWARN( logger, "a node with the same name exists!!!" )
    }

    _DSDEBUG( logger, "adding node : " << name.c_str() )
    m_nodes[name] = p_node;
    return true;
}

bool TransformQueue::SetNodeLocalTransformation( const dsstring& p_nodename, const Matrix& p_mat )
{
    if( m_nodes.count( p_nodename ) > 0 )
    {
        m_nodes[p_nodename]->SetLocalTransform( p_mat );
        return true;
    }	
    _DSERROR( logger, "no node with this name found !" )
    return false;
}

bool TransformQueue::SetCamera( const dsstring& p_nodename )
{
    if( m_nodes.count( p_nodename ) > 0 )
    {
        m_camera_node = p_nodename;
        return true;
    }
    _DSERROR( logger, "no node with this name found !" )
    return false;
}