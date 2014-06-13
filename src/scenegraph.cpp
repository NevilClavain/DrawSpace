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
using namespace DrawSpace::Utils;

Scenegraph::Scenegraph( void ) /*: m_camera( NULL )*/
{
    m_view.Identity();
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
    if( false == add( p_node ) )
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
    //return m_camera;

    if( m_current_camera != "" )
    {
        return m_cameras_list[m_current_camera];
    }
    return NULL;
}

bool Scenegraph::SetCurrentCamera( const dsstring& p_nodename )
{
    if( m_cameras_list.count( p_nodename ) > 0 )
    {
        m_current_camera = p_nodename;

        for( std::vector<CameraEventHandler*>::iterator it = m_cameraevt_handlers.begin(); it != m_cameraevt_handlers.end(); ++it )
        {
            ( **it )( ACTIVE, m_cameras_list[m_current_camera] );
        }
        return true;
    }
    return false;
}

void Scenegraph::ComputeTransformations( Utils::TimeManager& p_timemanager )
{
    TransformQueue::ComputeTransformations( p_timemanager );

    for( std::map<dsstring, TransformNode*>::iterator it = m_nodes.begin(); it != m_nodes.end(); ++it )
    {
        it->second->ComputeLod();
    }

    m_view.Identity();

    if( m_current_camera != "" )
    {
        m_cameras_list[m_current_camera]->GetSceneWorld( m_view );
        m_view.Inverse();
    }
}

void Scenegraph::GetCurrentCameraView( Utils::Matrix& p_view )
{
    p_view = m_view;
}

void Scenegraph::GetCurrentCameraTranform( Utils::Matrix& p_mat )
{
    Matrix mat;
    mat.Identity();

    if( m_current_camera != "" )
    {
        m_cameras_list[m_current_camera]->GetSceneWorld( mat );
    }
    p_mat = mat;
}

std::map<dsstring, Core::TransformNode*>& Scenegraph::GetCamerasList( void )
{
    return m_cameras_list;
}

void Scenegraph::GetCamerasList( std::vector<dsstring>& p_list )
{    
    for( std::map<dsstring, Core::TransformNode*>::iterator it = m_cameras_list.begin(); it != m_cameras_list.end(); ++it )
    {
        p_list.push_back( it->first );
    }
}

void Scenegraph::RegisterCameraEvtHandler( CameraEventHandler* p_handler )
{
    m_cameraevt_handlers.push_back( p_handler );
    if( m_current_camera != "" )
    {
        (*p_handler)( ACTIVE, m_cameras_list[m_current_camera] );
    }
    else
    {
        (*p_handler)( ACTIVE, NULL );
    }
}