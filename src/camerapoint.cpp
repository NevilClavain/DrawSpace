/* -*-LIC_BEGIN-*- */
/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2019                     
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

#include "camerapoint.h"
#include "maths.h"
#include "rocket.h"
#include "fpsmovement.h"
#include "freemovement.h"
#include "linearmovement.h"
#include "circularmovement.h"
#include "spectatormovement.h"
#include "headmovement.h"
#include "collider.h"
#include "plugin.h"
#include "renderer.h"



using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Dynamics;
using namespace DrawSpace::Utils;

CameraPoint::CameraPoint( void ) :
//m_locked_body( NULL ),
m_locked_node( NULL ),
m_relative_orbiter( NULL ),
m_relative_altitud( 0.0 ),
m_znear( 1.0 ),
m_lockedobject_distance( 0.0 ),
m_owner( NULL ),
m_referent_body( NULL )
{
    // prepare projection matrix    
    DrawSpace::Interface::Renderer* renderer = DrawSpace::Core::SingletonPlugin<DrawSpace::Interface::Renderer>::GetInstance()->m_interface;
    renderer->GetRenderCharacteristics( m_rendercharacteristics );
    m_projection.Perspective( m_rendercharacteristics.width_viewport, m_rendercharacteristics.height_viewport, m_znear, 100000000000.0 );

    m_globaltransformation.Identity();
}


CameraPoint::~CameraPoint( void )
{

}



void CameraPoint::OnRegister( SceneNodeGraph* p_scenegraph, BaseSceneNode* p_node )
{
    std::map<dsstring, Core::BaseSceneNode*>& camera_list = p_scenegraph->GetCamerasList();
    
    dsstring scenename;
    p_node->GetSceneName( scenename );
    camera_list[scenename] = p_node;

    m_owner = static_cast<SceneNode<CameraPoint>*>( p_node );
}


void CameraPoint::OnUnregister( SceneNodeGraph* p_scenegraph, DrawSpace::Core::BaseSceneNode* p_node )
{
    // supprimer de la listes cameras scenegraph
	std::map<dsstring, Core::BaseSceneNode*>& camera_list = p_scenegraph->GetCamerasList();

	dsstring scenename;
	p_node->GetSceneName( scenename );
	if( camera_list.count( scenename ) )
	{
		camera_list.erase(scenename);
	}
}

void CameraPoint::GetInfos( CameraPoint::Infos& p_infos )
{
    p_infos.locked_on_node = ( m_locked_node != NULL ? true : false );

    if( m_locked_node )
    {
        dsstring scene_name;
        m_locked_node->GetSceneName( scene_name );
        p_infos.locked_node_alias = scene_name;
    }
    else
    {
        p_infos.locked_node_alias = "";
    }

    p_infos.relative_orbiter = m_relative_orbiter;
    p_infos.altitud = m_relative_altitud;
}

void CameraPoint::Update( DrawSpace::Utils::TimeManager& p_timemanager )
{
    Matrix body_transf;

    m_localtransformation.Identity();

    if( m_locked_node )
    {
        Matrix temp_global;


        //////////////////////
       
        // recup transfo parent

        BaseSceneNode* parent = m_owner->GetParentNode();
        if( parent )
        {
            parent->GetFinalTransform( body_transf );
            temp_global = m_localtransformation * body_transf;
        }
        else
        {
            temp_global = m_localtransformation;
        }

        //////////////////////

        m_locked_node->GetFinalTransform( body_transf );

        m_body_transf = body_transf;

        Matrix camera_transf = temp_global;
        camera_transf.Inverse();

        Matrix res = body_transf * camera_transf;

        // point (0,0,0) local au body, exprime dans le repère de la camera

        Vector body_center( 0.0, 0.0, 0.0, 1.0 );
        Vector body_center_2;

        res.Transform( &body_center, &body_center_2 );

        m_locked_body_center = body_center_2;


        body_center_2.Normalize();

        dsreal theta = atan2( body_center_2[0], body_center_2[2] );

        
        theta = PI + theta;
        if( theta > 2* PI )
        {
            theta -= 2 * PI;
        }
        
 
        Matrix roty;
        roty.Rotation( Vector( 0.0, 1.0, 0.0, 1.0 ), theta );

        Vector theta_dir( body_center_2[0], 0.0, body_center_2[2], 1.0 );

        dsreal phi = atan2( body_center_2[1], theta_dir.Length() );

        if( phi < 0 )
        {
            phi = ( 2 * PI ) + phi;
        }
        
        Matrix rotx;
        rotx.Rotation( Vector( 1.0, 0.0, 0.0, 1.0 ), phi );

        Matrix final_lock;
        final_lock = rotx * roty;
        
        Matrix final_res = final_lock * m_localtransformation;
        m_localtransformation = final_res;      
    }
}

void CameraPoint::Update2( DrawSpace::Utils::TimeManager& p_timemanager )
{
    // calcul de la distance de l'objet suivi
    if( m_locked_node )
    {
        Vector body_center( 0.0, 0.0, 0.0, 1.0 );
        Vector body_center_2;

        Matrix view = m_globaltransformation;
        view.Inverse();
        Matrix final = m_body_transf * view;
        final.Transform( &body_center, &body_center_2 );
        m_lockedobject_distance = body_center_2.Length();
    }
    else
    {
        m_lockedobject_distance = 0.0;
    }
}

void CameraPoint::Lock( DrawSpace::Core::BaseSceneNode* p_locked_node )
{
    m_locked_node = p_locked_node;
}

void CameraPoint::Unlock( void )
{
    m_locked_node = NULL;
}

void CameraPoint::GetLockedBodyCenter( Vector& p_vector )
{
    p_vector = m_locked_body_center;
}

void CameraPoint::GetLocalTransform( DrawSpace::Utils::Matrix& p_localtransf )
{
    p_localtransf = m_localtransformation;
}

void CameraPoint::SetRelativeOrbiter( DrawSpace::Dynamics::Orbiter* p_relative_orbiter )
{
    m_relative_orbiter = p_relative_orbiter;
}

void CameraPoint::SetRelativeAltitude( dsreal p_relative_altitud )
{
    m_relative_altitud = p_relative_altitud;
}

void CameraPoint::GetProjection( DrawSpace::Utils::Matrix& p_mat )
{
    p_mat = m_projection;
}

dsreal CameraPoint::GetZNear( void )
{
    return m_znear;
}

void CameraPoint::UpdateProjectionZNear( dsreal p_znear )
{
    m_znear = p_znear;
    m_projection.Perspective( m_rendercharacteristics.width_viewport, m_rendercharacteristics.height_viewport, m_znear, 100000000000.0 );
}

dsreal CameraPoint::GetLockedObjectDistance( void )
{
    return m_lockedobject_distance;
}

/////////////////////

void CameraPoint::GetBaseTransform( DrawSpace::Utils::Matrix& p_mat )
{
    p_mat = m_localtransformation;
}


void CameraPoint::SetFinalTransform( const DrawSpace::Utils::Matrix& p_mat )
{
    m_globaltransformation = p_mat;
}

SceneNode<CameraPoint>* CameraPoint::GetOwner( void )
{
    return m_owner;
}


// NE PAS SUPPRIMER !!!
void CameraPoint::SetReferentBody( DrawSpace::Dynamics::Body* p_refbody )
{
    m_referent_body = p_refbody;
}

// NE PAS SUPPRIMER !!!
Body* CameraPoint::GetReferentBody( void )
{
    return m_referent_body;
}
