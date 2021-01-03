/* -*-LIC_BEGIN-*- */
/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2021                     
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

#include "scenenodegraph.h"
#include "exceptions.h"
#include "camerapoint.h"
#include "plugin.h"
#include "renderer.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Dynamics;
using namespace DrawSpace::Utils;

SceneNodeGraph::SceneNodeGraph( void )
{
    //m_view.Identity();
}

SceneNodeGraph::~SceneNodeGraph( void )
{
}

void SceneNodeGraph::ComputeTransformations( Utils::TimeManager& p_timemanager )
{
    for( size_t i = 0; i < m_scenegraphevt_handlers.size(); i++ )
    {
        (* m_scenegraphevt_handlers[i] )( TRANSFORMATIONS_BEGIN, this );
    }
    for( auto it = m_nodes.begin(); it != m_nodes.end(); ++it )
    {
        it->second->ComputeTransformation( p_timemanager );
    }
    for( size_t i = 0; i < m_scenegraphevt_handlers.size(); i++ )
    {
        (* m_scenegraphevt_handlers[i] )( TRANSFORMATIONS_DONE, this );
    }

    /*
    m_view.Identity();
    if( m_current_camera != "" )
    {        
        m_cameras_list[m_current_camera]->GetFinalTransform( m_view );        
        m_view.Inverse();
    }
    */
}

bool SceneNodeGraph::AddNode( BaseSceneNode* p_node )
{
    dsstring name;
    p_node->GetSceneName( name );
    
    if( "" == name )
    {
        _DSEXCEPTION( "node has a void name !" );
    }

    if( m_nodes.count( name ) > 0 )
    {
        _DSEXCEPTION( "a node with the same name exists : " << name );
    }
    
    m_nodes[name] = p_node;
    return true;
}

void SceneNodeGraph::RemoveNode(BaseSceneNode* p_node)
{
	dsstring name;
	p_node->GetSceneName( name );

	if( m_nodes.count( name ) > 0 )
	{
		m_nodes.erase( name );
	}
}

std::map<dsstring, BaseSceneNode*>& SceneNodeGraph::GetNodesList( void )
{
    return m_nodes;
}

std::vector<BaseSceneNode*>& SceneNodeGraph::GetAllNodesList( void )
{
    return m_all_nodes;
}

void SceneNodeGraph::RegisterNode( BaseSceneNode* p_node )
{
    p_node->OnRegister( this );
    m_all_nodes.push_back( p_node );

    for( size_t i = 0; i < m_nodesevt_handlers.size(); i++ )
    {
        ( *m_nodesevt_handlers[i] )( NODE_REGISTERED, p_node );
    }
}

void SceneNodeGraph::UnregisterNode( BaseSceneNode* p_node )
{
    p_node->OnUnregister( this );

	for( auto it = m_all_nodes.begin(); it != m_all_nodes.end(); ++it )
	{
		if (*it == p_node)
		{
			m_all_nodes.erase(it);
			break;
		}
	}

	for (size_t i = 0; i < m_nodesevt_handlers.size(); i++)
	{
		(*m_nodesevt_handlers[i])( NODE_UNREGISTERED, p_node );
	}
}

Core::BaseSceneNode* SceneNodeGraph::GetCurrentCamera( void )
{    
    if( m_current_camera != "" )
    {
        return m_cameras_list[m_current_camera];
    }
    return NULL;
}

bool SceneNodeGraph::SetCurrentCamera( const dsstring& p_nodename )
{
    if( "" == p_nodename )
    {
        m_current_camera = "";
        for( std::vector<CameraEventHandler*>::iterator it = m_cameraevt_handlers.begin(); it != m_cameraevt_handlers.end(); ++it )
        {
            ( **it )( ACTIVE, NULL );
        }
        return true;
    }
    if( m_cameras_list.count( p_nodename ) > 0 )
    {
        m_current_camera = p_nodename;

        for( std::vector<CameraEventHandler*>::iterator it = m_cameraevt_handlers.begin(); it != m_cameraevt_handlers.end(); ++it )
        {
            ( **it )( ACTIVE, m_cameras_list[m_current_camera] );
        }
        return true;
    }
	_DSEXCEPTION( "Unknown camera '" + p_nodename + "'" );
    return false;
}

void SceneNodeGraph::GetCurrentCameraView( Utils::Matrix& p_view )
{
    Matrix view;
    view.Identity();    
    if( m_current_camera != "" )
    {        
        m_cameras_list[m_current_camera]->GetFinalTransform( view );        
        view.Inverse();
    }
    p_view = view;
}

void SceneNodeGraph::GetCurrentCameraTranform( Utils::Matrix& p_mat )
{    
    Matrix mat;
    mat.Identity();

    if( m_current_camera != "" )
    {
        m_cameras_list[m_current_camera]->GetFinalTransform( mat );
    }
    p_mat = mat;
}

void SceneNodeGraph::GetCurrentCameraProj( Utils::Matrix& p_proj )
{
    if( m_current_camera != "" )
    {
        SceneNode<CameraPoint>* camera_node; 

        camera_node = dynamic_cast<SceneNode<CameraPoint>*>( m_cameras_list[m_current_camera] );
        if( NULL == camera_node )
        {            
            _DSEXCEPTION( "scene node is not a camerapoint !" );
        }
        else
        {
            CameraPoint* camera = camera_node->GetContent();
            camera->GetProjection( p_proj );
        }
    }
    else
    {
        // prepare default projection matrix
        DrawSpace::Interface::Renderer::Characteristics characteristics;
        DrawSpace::Interface::Renderer* renderer = DrawSpace::Core::SingletonPlugin<DrawSpace::Interface::Renderer>::GetInstance()->m_interface;
        renderer->GetRenderCharacteristics( characteristics );
        p_proj.Perspective( characteristics.width_viewport, characteristics.height_viewport, 1.0, 100000000000.0 );        
    }
}

std::map<dsstring, BaseSceneNode*>& SceneNodeGraph::GetCamerasList( void )
{
    return m_cameras_list;
}

void SceneNodeGraph::RegisterCameraEvtHandler( CameraEventHandler* p_handler )
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

void SceneNodeGraph::UnregisterCameraEvtHandler( CameraEventHandler* p_handler )
{
    for( auto it = m_cameraevt_handlers.begin(); it != m_cameraevt_handlers.end(); ++it )
    {
        if( (*it) == p_handler )
        {
            m_cameraevt_handlers.erase( it );
            break;
        }
    }
}


void SceneNodeGraph::RegisterNodesEvtHandler( NodesEventHandler* p_handler )
{
    m_nodesevt_handlers.push_back( p_handler );

    // balancer la liste des nodes deja enregistres
    
    for( size_t i = 0; i < m_all_nodes.size(); i++ )
    {
        (*p_handler)( NODE_REGISTERED, m_all_nodes[i] );
    }
}

void SceneNodeGraph::UnregisterNodesEvtHandler( NodesEventHandler* p_handler )
{
    for( auto it = m_nodesevt_handlers.begin(); it != m_nodesevt_handlers.end(); ++it )
    {
        if( (*it) == p_handler )
        {
            m_nodesevt_handlers.erase( it );
            break;
        }
    }
}

void SceneNodeGraph::RegisterScenegraphEvtHandler( ScenegraphEventHandler* p_handler )
{
    m_scenegraphevt_handlers.push_back( p_handler );
}


void SceneNodeGraph::UnregisterScenegraphEvtHandler( ScenegraphEventHandler* p_handler )
{
    for( auto it = m_scenegraphevt_handlers.begin(); it != m_scenegraphevt_handlers.end(); ++it )
    {
        if( (*it) == p_handler )
        {
            m_scenegraphevt_handlers.erase( it );
            break;
        }
    }
}


void SceneNodeGraph::PointProjection( const Vector& p_point, dsreal& p_outx, dsreal& p_outy, dsreal& p_outz )
{
    DrawSpace::Interface::Renderer* renderer = DrawSpace::Core::SingletonPlugin<DrawSpace::Interface::Renderer>::GetInstance()->m_interface;
    Matrix proj;
    Vector point = p_point;

    GetCurrentCameraProj( proj );

    Matrix view;
    GetCurrentCameraView( view );
    renderer->PointProjection( view, proj, point, p_outx, p_outy, p_outz );
}

void SceneNodeGraph::GetCurrentCameraName( dsstring& p_outname )
{
    p_outname = m_current_camera;
}

void SceneNodeGraph::SendNodeAppReadyEvent( BaseSceneNode* p_node )
{
    for( size_t i = 0; i < m_all_nodes.size(); i++ )
    {
        if( m_all_nodes[i] == p_node )
        {
            for( size_t j = 0; j < m_nodesevt_handlers.size(); j++ )
            {
                ( *m_nodesevt_handlers[j] )( NODE_APP_READY, p_node );
            }            
            return;
        }
    }
}
