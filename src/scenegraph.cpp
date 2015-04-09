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

#include "scenegraph.h"
#include "camerapoint.h"
#include "plugin.h"
#include "renderer.h"


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
    p_pass->GetSpecificName( pass_name );
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

void Scenegraph::GetCurrentCameraProj( Utils::Matrix& p_proj )
{
    if( m_current_camera != "" )
    {       
        DrawSpace::Dynamics::CameraPoint* camera = NULL; //TEMP en attendant de virer la presente class Scenegraph static_cast<DrawSpace::Dynamics::CameraPoint*>( m_cameras_list[m_current_camera] );
        camera->GetProjection( p_proj );        
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

std::map<dsstring, Core::TransformNode*>& Scenegraph::GetCamerasList( void )
{
    return m_cameras_list;
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

void Scenegraph::PointProjection( const DrawSpace::Utils::Vector& p_point, dsreal& p_outx, dsreal& p_outy, dsreal& p_outz )
{
    DrawSpace::Interface::Renderer* renderer = DrawSpace::Core::SingletonPlugin<DrawSpace::Interface::Renderer>::GetInstance()->m_interface;
    Matrix proj;
    Vector point = p_point;

    GetCurrentCameraProj( proj );
    renderer->PointProjection( m_view, proj, point, p_outx, p_outy, p_outz );       
}

void Scenegraph::GetCurrentCameraName( dsstring& p_outname )
{
    p_outname = m_current_camera;
}
