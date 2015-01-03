/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2015                              
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
#include "exceptions.h"

_DECLARE_DS_LOGGER( logger, "TransformQueue" )

using namespace DrawSpace::Core;
using namespace DrawSpace::Utils;

TransformQueue::TransformQueue( void )
{
}

TransformQueue::~TransformQueue( void )
{

}

void TransformQueue::ComputeTransformations( TimeManager& p_timemanager )
{
    for( std::map<dsstring, TransformNode*>::iterator it = m_nodes.begin(); it != m_nodes.end(); ++it )
    {
        (*it).second->ComputeFinalTransform( p_timemanager );
    }
}

bool TransformQueue::add( TransformNode* p_node )
{
    dsstring name;
    p_node->GetSceneName( name );

    
    if( "" == name )
    {
        _DSEXCEPTION( "node has a void name !" );

        // mettre une trace error
        //_DSERROR( logger, "node has a void name !" )
        //return false;
    }

    if( m_nodes.count( name ) > 0 )
    {
        // mettre une trace warning
        //_DSWARN( logger, "a node with the same name exists!!!" )

        _DSEXCEPTION( "a node with the same name exists!!!" );
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

bool TransformQueue::GetNodeGlobalTransform( const dsstring& p_nodename, DrawSpace::Utils::Matrix& p_mat )
{
    if( m_nodes.count( p_nodename ) > 0 )
    {
        m_nodes[p_nodename]->GetSceneWorld( p_mat );
        return true;
    }	
    _DSERROR( logger, "no node with this name found !" )
    return false;
}

std::map<dsstring, TransformNode*>& TransformQueue::GetNodesList( void )
{
    return m_nodes;
}
