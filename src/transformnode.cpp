/***************************************************************************
*                                                                          *
* DrawSpace Rendering engine                                               *
* Emmanuel Chaumont Copyright (c) 2013-2014                                *
*                                                                          *
* This file is part of DrawSpace.                                          *
*                                                                          *
*    DrawSpace is free software: you can redistribute it and/or modify     *
*    it under the terms of the GNU General Public License as published by  *
*    the Free Software Foundation, either version 3 of the License, or     *
*    (at your option) any later version.                                   *
*                                                                          *
*    DrawSpace is distributed in the hope that it will be useful,          *
*    but WITHOUT ANY WARRANTY; without even the implied warranty of        *
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
*    GNU General Public License for more details.                          *
*                                                                          *
*    You should have received a copy of the GNU General Public License     *
*    along with DrawSpace.  If not, see <http://www.gnu.org/licenses/>.    *
*                                                                          *
***************************************************************************/

#include "transformnode.h"

using namespace DrawSpace::Core;
using namespace DrawSpace::Utils;


TransformNode::TransformNode( const dsstring& p_name, bool p_req_view ) : 
m_parent( NULL ), 
m_scenename( p_name ),
m_request_camera_view( p_req_view )
{
    m_localtransformation.Identity();
    m_globaltransformation.Identity();
	m_view.Identity();
}

TransformNode::~TransformNode( void )
{

}

void TransformNode::AddChild( TransformNode* p_node )
{
    m_children.push_back( p_node );
    p_node->m_parent = this;
}

void TransformNode::ComputeFinalTransform( void )
{
    if( m_parent )
    {
        m_globaltransformation = m_localtransformation * m_parent->m_globaltransformation;
    }
    else
    {
        m_globaltransformation = m_localtransformation;
    }

    for( unsigned long i = 0; i < m_children.size(); i++ )
    {
        m_children[i]->ComputeFinalTransform();
    }
}

void TransformNode::SetLocalTransform( const DrawSpace::Utils::Matrix& p_mat )
{
	m_localtransformation = p_mat;
}

void TransformNode::GetName( dsstring& p_name )
{
	p_name = m_scenename;
}

void TransformNode::GetSceneWorld( DrawSpace::Utils::Matrix& p_mat )
{
	p_mat = m_globaltransformation;
}

void TransformNode::GetSceneView( Matrix& p_mat )
{
	p_mat = m_view;
}

void TransformNode::SetView( const Matrix& p_mat )
{
	m_view = p_mat;
}

bool TransformNode::IsCameraViewRequested( void )
{
	return m_request_camera_view;
}

