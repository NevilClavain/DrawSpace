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
    m_view.Identity();
}

SceneNodeGraph::~SceneNodeGraph( void )
{
}

void SceneNodeGraph::ComputeTransformations( Utils::TimeManager& p_timemanager )
{
    for( std::map<dsstring, BaseSceneNode*>::iterator it = m_nodes.begin(); it != m_nodes.end(); ++it )
    {
        it->second->ComputeTransformation( p_timemanager );
    }

    // TODO
    /*
    for( std::map<dsstring, BaseSceneNode*>::iterator it = m_nodes.begin(); it != m_nodes.end(); ++it )
    {
        it->second->ComputeLod();
    }
    */

    m_view.Identity();

    if( m_current_camera != "" )
    {
        //m_cameras_list[m_current_camera]->GetSceneWorld( m_view );
        m_cameras_list[m_current_camera]->GetFinalTransform( m_view );        
        m_view.Inverse();
    }
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
        _DSEXCEPTION( "a node with the same name exists!!!" );
    }
    
    m_nodes[name] = p_node;
    return true;
}

std::map<dsstring, BaseSceneNode*>& SceneNodeGraph::GetNodesList( void )
{
    return m_nodes;
}


void SceneNodeGraph::RegisterPass( Pass* p_pass )
{
    dsstring pass_name;
    p_pass->GetSpecificName( pass_name );
    m_passes[pass_name] = p_pass;    
}

void SceneNodeGraph::RegisterNode( BaseSceneNode* p_node )
{
    p_node->OnRegister( this );
    m_all_nodes.push_back( p_node );

    for( size_t i = 0; i < m_nodesevt_handlers.size(); i++ )
    {
        ( *m_nodesevt_handlers[i] )( NODE_ADDED, p_node );
    }
}

Pass* SceneNodeGraph::GetPass( const dsstring& p_passname )
{
    if( m_passes.count( p_passname ) > 0 )
    {
        return m_passes[p_passname];
    }
    return NULL;
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
    return false;
}

void SceneNodeGraph::GetCurrentCameraView( Utils::Matrix& p_view )
{
    p_view = m_view;
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
        /*
        DrawSpace::Dynamics::CameraPoint* camera = static_cast<DrawSpace::Dynamics::CameraPoint*>( m_cameras_list[m_current_camera] );
        camera->GetProjection( p_proj );
        */

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

void SceneNodeGraph::RegisterNodesEvtHandler( NodesEventHandler* p_handler )
{
    m_nodesevt_handlers.push_back( p_handler );

    // balancer la liste des nodes deja enregistres
    
    for( size_t i = 0; i < m_all_nodes.size(); i++ )
    {
        (*p_handler)( NODE_ADDED, m_all_nodes[i] );
    }
}

void SceneNodeGraph::PointProjection( const Vector& p_point, dsreal& p_outx, dsreal& p_outy, dsreal& p_outz )
{
    DrawSpace::Interface::Renderer* renderer = DrawSpace::Core::SingletonPlugin<DrawSpace::Interface::Renderer>::GetInstance()->m_interface;
    Matrix proj;
    Vector point = p_point;

    GetCurrentCameraProj( proj );
    renderer->PointProjection( m_view, proj, point, p_outx, p_outy, p_outz );       
}

void SceneNodeGraph::GetCurrentCameraName( dsstring& p_outname )
{
    p_outname = m_current_camera;
}
