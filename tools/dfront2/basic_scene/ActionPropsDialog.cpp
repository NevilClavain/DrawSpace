/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2016                              
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

#include <wx/wx.h>
#include "BasicSceneMainFrame.h"

#include "ActionPropsDialog.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Dynamics;
using namespace DrawSpace::Utils;


void ActionPropsDialog::Execute( void )
{
    void* id = BasicSceneMainFrame::GetInstance()->m_last_clicked_treeitem.GetID();
    if( BasicSceneMainFrame::GetInstance()->m_spacebox_descriptors.count( id ) > 0 )
    {
        DrawSpace::Utils::SpaceboxDescriptor sb_descr = BasicSceneMainFrame::GetInstance()->m_spacebox_descriptors[id];
        BasicSceneMainFrame::SceneNodeEntry<DrawSpace::Spacebox> sb_node = BasicSceneMainFrame::GetInstance()->m_spacebox_nodes[id];

        DIALOG_ACTION_DECLARE( DIALOG_SPACEBOX_PROPS_TITLE )

        DIALOG_APPENDROOT_STRING( "scene name", sb_descr.scene_name )

        for( std::map<dsstring, SpaceboxPassDescriptor>::iterator it = sb_descr.passes_slots.begin(); it != sb_descr.passes_slots.end(); ++it )
        {
            SpaceboxPassDescriptor pass_descr = it->second;                        
            IntermediatePass* ipass = static_cast<IntermediatePass*>( ConfigsBase::GetInstance()->GetConfigurableInstance( it->first ) );


            DIALOG_APPENDROOT_NODE( it->first, pass_root )

            DIALOG_APPENDNODE_STRING( pass_root, "fx name", pass_descr.fx_name )
            DIALOG_APPENDNODE_INTEGER( pass_root, "rendering order", pass_descr.rendering_order )

            DIALOG_BUILD_LABELS( RenderingNode::NbMaxTextures, "stage %d", texture_stages )


            {
                DIALOG_APPENDNODE_NODE( pass_root, "front textures", front_root )
                for( size_t i = 0; i < texture_stages.size(); i++ )
                {
                    if( pass_descr.textures[Spacebox::FrontQuad][i] != "" )
                    {
                        DIALOG_APPENDNODE_STRING( front_root, texture_stages[i], pass_descr.textures[Spacebox::FrontQuad][i] )
                    }
                }
            }

            {
                DIALOG_APPENDNODE_NODE( pass_root, "rear textures", rear_root )
                for( size_t i = 0; i < texture_stages.size(); i++ )
                {
                    if( pass_descr.textures[Spacebox::RearQuad][i] != "" )
                    {
                        DIALOG_APPENDNODE_STRING( rear_root, texture_stages[i], pass_descr.textures[Spacebox::RearQuad][i] )
                    }
                }
            }

            {
                DIALOG_APPENDNODE_NODE( pass_root, "left textures", left_root )
                for( size_t i = 0; i < texture_stages.size(); i++ )
                {
                    if( pass_descr.textures[Spacebox::LeftQuad][i] != "" )
                    {
                        DIALOG_APPENDNODE_STRING( left_root, texture_stages[i], pass_descr.textures[Spacebox::LeftQuad][i] )
                    }
                }
            }

            {
                DIALOG_APPENDNODE_NODE( pass_root, "right textures", right_root )
                for( size_t i = 0; i < texture_stages.size(); i++ )
                {
                    if( pass_descr.textures[Spacebox::RightQuad][i] != "" )
                    {
                        DIALOG_APPENDNODE_STRING( right_root, texture_stages[i], pass_descr.textures[Spacebox::RightQuad][i] )
                    }
                }
            }

            {
                DIALOG_APPENDNODE_NODE( pass_root, "top textures", top_root )
                for( size_t i = 0; i < texture_stages.size(); i++ )
                {
                    if( pass_descr.textures[Spacebox::TopQuad][i] != "" )
                    {
                        DIALOG_APPENDNODE_STRING( top_root, texture_stages[i], pass_descr.textures[Spacebox::TopQuad][i] )
                    }
                }
            }

            {
                DIALOG_APPENDNODE_NODE( pass_root, "bottom textures", bottom_root )
                for( size_t i = 0; i < texture_stages.size(); i++ )
                {
                    if( pass_descr.textures[Spacebox::BottomQuad][i] != "" )
                    {
                        DIALOG_APPENDNODE_STRING( bottom_root, texture_stages[i], pass_descr.textures[Spacebox::BottomQuad][i] )
                    }
                }
            }

            // les 6 rendering_node ont tous les m�mes valeurs de shaders, on peut donc se contenter d'un lire
            // un seul (ici le 0)
            DrawSpace::Core::RenderingNode* rendering_node = sb_node.scene_node->GetContent()->GetNodeFromPass( ipass, 0 );

            std::map<dsstring, RenderingNode::ShadersParams*> shaders_params_list;
            rendering_node->GetShadersParams( shaders_params_list );


            for( size_t i = 0; i < pass_descr.shader_params.size(); i++ )
            {
                
                DIALOG_APPENDNODE_NODE( pass_root, pass_descr.shader_params[i].id, shader_param_root )

                DIALOG_APPENDNODE_INTEGER( shader_param_root, "shader index", pass_descr.shader_params[i].shader_index )
                DIALOG_APPENDNODE_INTEGER( shader_param_root, "shader register", pass_descr.shader_params[i].shader_register )

                DIALOG_APPENDNODE_NODE( shader_param_root, "values", shader_param_values_root )

                Vector params_value = shaders_params_list[pass_descr.shader_params[i].id]->param_values;

                DIALOG_APPENDNODE_FLOAT( shader_param_values_root, "x", params_value[0] )
                DIALOG_APPENDNODE_FLOAT( shader_param_values_root, "y", params_value[1] )
                DIALOG_APPENDNODE_FLOAT( shader_param_values_root, "z", params_value[2] )
                DIALOG_APPENDNODE_FLOAT( shader_param_values_root, "w", params_value[3] )


                /*
                DIALOG_APPENDNODE_FLOAT( shader_param_values_root, "x", pass_descr.shader_params[i].value[0] )
                DIALOG_APPENDNODE_FLOAT( shader_param_values_root, "y", pass_descr.shader_params[i].value[1] )
                DIALOG_APPENDNODE_FLOAT( shader_param_values_root, "z", pass_descr.shader_params[i].value[2] )
                DIALOG_APPENDNODE_FLOAT( shader_param_values_root, "w", pass_descr.shader_params[i].value[3] )
                */
            }
        }
     
        DIALOG_SHOW
    }
    else if( BasicSceneMainFrame::GetInstance()->m_chunk_descriptors.count( id ) > 0 )
    {
        DrawSpace::Utils::ChunkDescriptor chunk_descr = BasicSceneMainFrame::GetInstance()->m_chunk_descriptors[id];
        BasicSceneMainFrame::SceneNodeEntry<DrawSpace::Chunk> chunk_node = BasicSceneMainFrame::GetInstance()->m_chunk_nodes[id];

        DIALOG_ACTION_DECLARE( DIALOG_CHUNK_PROPS_TITLE )

        DIALOG_APPENDROOT_STRING( "scene name", chunk_descr.scene_name )

        if( chunk_descr.impostors.size() > 0 )
        {
            DIALOG_APPENDROOT_INTEGER( "impostors", chunk_descr.impostors.size() )
        }
        else
        {
            DIALOG_APPENDROOT_STRING( "meshe", chunk_descr.meshe )
        }

        for( std::map<dsstring, ChunkPassDescriptor>::iterator it = chunk_descr.passes_slots.begin(); it != chunk_descr.passes_slots.end(); ++it )
        {
            ChunkPassDescriptor pass_descr = it->second; 
            IntermediatePass* ipass = static_cast<IntermediatePass*>( ConfigsBase::GetInstance()->GetConfigurableInstance( it->first ) );
            DrawSpace::Core::RenderingNode* rendering_node = chunk_node.scene_node->GetContent()->GetNodeFromPass( ipass );

            std::map<dsstring, RenderingNode::ShadersParams*> shaders_params_list;
            rendering_node->GetShadersParams( shaders_params_list );


            DIALOG_APPENDROOT_NODE( it->first, pass_root )

            DIALOG_APPENDNODE_STRING( pass_root, "fx name", pass_descr.fx_name )
            DIALOG_APPENDNODE_INTEGER( pass_root, "rendering order", pass_descr.rendering_order )

            DIALOG_BUILD_LABELS( RenderingNode::NbMaxTextures, "stage %d", texture_stages )

            DIALOG_APPENDNODE_NODE( pass_root, "textures", textures_root )

            for( size_t i = 0; i < texture_stages.size(); i++ )
            {
                if( pass_descr.textures[i] != "" )
                {
                    DIALOG_APPENDNODE_STRING( textures_root, texture_stages[i], pass_descr.textures[i] )
                }
            }

            for( size_t i = 0; i < pass_descr.shader_params.size(); i++ )
            {
                DIALOG_APPENDNODE_NODE( pass_root, pass_descr.shader_params[i].id, shader_param_root )

                DIALOG_APPENDNODE_INTEGER( shader_param_root, "shader index", pass_descr.shader_params[i].shader_index )
                DIALOG_APPENDNODE_INTEGER( shader_param_root, "shader register", pass_descr.shader_params[i].shader_register )

                DIALOG_APPENDNODE_NODE( shader_param_root, "values", shader_param_values_root )

                Vector params_value = shaders_params_list[pass_descr.shader_params[i].id]->param_values;

                DIALOG_APPENDNODE_FLOAT( shader_param_values_root, "x", params_value[0] )
                DIALOG_APPENDNODE_FLOAT( shader_param_values_root, "y", params_value[1] )
                DIALOG_APPENDNODE_FLOAT( shader_param_values_root, "z", params_value[2] )
                DIALOG_APPENDNODE_FLOAT( shader_param_values_root, "w", params_value[3] )

            }
        }

        DIALOG_SHOW
    }

    else if( BasicSceneMainFrame::GetInstance()->m_clouds_descriptors.count( id ) > 0 )
    {
        DrawSpace::Utils::CloudsDescriptor clouds_descr = BasicSceneMainFrame::GetInstance()->m_clouds_descriptors[id];
        BasicSceneMainFrame::SceneNodeEntry<DrawSpace::Clouds> clouds_node = BasicSceneMainFrame::GetInstance()->m_clouds_nodes[id];

        DIALOG_ACTION_DECLARE( DIALOG_CLOUDS_PROPS_TITLE )

        DIALOG_APPENDROOT_STRING( "scene name", clouds_descr.chunk_descriptor.scene_name )

        DrawSpace::ImpostorsDisplayList idl;
        clouds_node.scene_node->GetContent()->GetImpostorsDisplayList( idl );
        DIALOG_APPENDROOT_INTEGER( "impostors", idl.size() )

        DIALOG_APPENDROOT_INTEGER( "seed", clouds_descr.seed )
        DIALOG_APPENDROOT_BOOL( "details", clouds_descr.details )
        DIALOG_APPENDROOT_STRING( "procedural rules", clouds_descr.rules_filepath )
        DIALOG_APPENDROOT_FLOAT( "sorting distance", clouds_node.scene_node->GetContent()->GetSortingDistance() )

        for( std::map<dsstring, ChunkPassDescriptor>::iterator it = clouds_descr.chunk_descriptor.passes_slots.begin(); it != clouds_descr.chunk_descriptor.passes_slots.end(); ++it )
        {
            ChunkPassDescriptor pass_descr = it->second; 
            IntermediatePass* ipass = static_cast<IntermediatePass*>( ConfigsBase::GetInstance()->GetConfigurableInstance( it->first ) );
            DrawSpace::Core::RenderingNode* rendering_node = clouds_node.scene_node->GetContent()->GetNodeFromPass( ipass );

            std::map<dsstring, RenderingNode::ShadersParams*> shaders_params_list;
            rendering_node->GetShadersParams( shaders_params_list );


            DIALOG_APPENDROOT_NODE( it->first, pass_root )

            DIALOG_APPENDNODE_STRING( pass_root, "fx name", pass_descr.fx_name )
            DIALOG_APPENDNODE_INTEGER( pass_root, "rendering order", pass_descr.rendering_order )

            DIALOG_BUILD_LABELS( RenderingNode::NbMaxTextures, "stage %d", texture_stages )

            DIALOG_APPENDNODE_NODE( pass_root, "textures", textures_root )

            for( size_t i = 0; i < texture_stages.size(); i++ )
            {
                if( pass_descr.textures[i] != "" )
                {
                    DIALOG_APPENDNODE_STRING( textures_root, texture_stages[i], pass_descr.textures[i] )
                }
            }

            for( size_t i = 0; i < pass_descr.shader_params.size(); i++ )
            {
                DIALOG_APPENDNODE_NODE( pass_root, pass_descr.shader_params[i].id, shader_param_root )

                DIALOG_APPENDNODE_INTEGER( shader_param_root, "shader index", pass_descr.shader_params[i].shader_index )
                DIALOG_APPENDNODE_INTEGER( shader_param_root, "shader register", pass_descr.shader_params[i].shader_register )

                DIALOG_APPENDNODE_NODE( shader_param_root, "values", shader_param_values_root )

                Vector params_value = shaders_params_list[pass_descr.shader_params[i].id]->param_values;

                DIALOG_APPENDNODE_FLOAT( shader_param_values_root, "x", params_value[0] )
                DIALOG_APPENDNODE_FLOAT( shader_param_values_root, "y", params_value[1] )
                DIALOG_APPENDNODE_FLOAT( shader_param_values_root, "z", params_value[2] )
                DIALOG_APPENDNODE_FLOAT( shader_param_values_root, "w", params_value[3] )

            }
        }

        DIALOG_SHOW
    }


    else if( BasicSceneMainFrame::GetInstance()->m_camera_nodes.count( id ) > 0 )
    {
        CameraPoint* camera = BasicSceneMainFrame::GetInstance()->m_camera_nodes[id].scene_node->GetContent();

        CameraPoint::Infos infos;
        camera->GetInfos( infos );

        DIALOG_ACTION_DECLARE( DIALOG_CAMERA_PROPS_TITLE )

        DIALOG_APPENDROOT_STRING( "scene name", BasicSceneMainFrame::GetInstance()->m_camera_nodes[id].name );

        DIALOG_APPENDROOT_BOOL( "relative to an orbiter", ( infos.relative_orbiter != NULL ? true : false ) );
        if( infos.relative_orbiter != NULL )
        {
            DIALOG_APPENDROOT_FLOAT( "altitud", infos.altitud );
        }

        DIALOG_APPENDROOT_BOOL( "locked on node", infos.locked_on_node );
        if( infos.locked_on_node )
        {
            DIALOG_APPENDROOT_STRING( "locked on", infos.locked_node_alias );
            DIALOG_APPENDROOT_FLOAT( "locked object distance", camera->GetLockedObjectDistance() );
        }
        
        DIALOG_SHOW
    }
    else if( BasicSceneMainFrame::GetInstance()->m_fps_nodes.count( id ) > 0 )
    {
        FPSMovement* fps = BasicSceneMainFrame::GetInstance()->m_fps_nodes[id].scene_node->GetContent();

        Vector pos;
        dsreal curr_yaw, curr_pitch;
        fps->GetCurrentPos( pos );
        curr_yaw = fps->GetCurrentYaw();
        curr_pitch = fps->GetCurrentPitch();
       
        DIALOG_ACTION_DECLARE( DIALOG_FPSMVT_PROPS_TITLE )
        DIALOG_APPENDROOT_STRING( "scene name", BasicSceneMainFrame::GetInstance()->m_fps_nodes[id].name );
        DIALOG_APPENDROOT_STRING( "movement type ", "fps" );
        DIALOG_APPENDROOT_FLOAT( "current yaw", Maths::RadToDeg( curr_yaw ) );
        DIALOG_APPENDROOT_FLOAT( "current pitch", Maths::RadToDeg( curr_pitch ) );
        DIALOG_APPENDROOT_NODE( "current position", curr_pos_root );
        DIALOG_APPENDNODE_FLOAT( curr_pos_root, "x", pos[0] );
        DIALOG_APPENDNODE_FLOAT( curr_pos_root, "y", pos[1] );
        DIALOG_APPENDNODE_FLOAT( curr_pos_root, "z", pos[2] );

        DIALOG_SHOW
    }

    else if( BasicSceneMainFrame::GetInstance()->m_lin_nodes.count( id ) > 0 )
    {
        LinearMovement* lin = BasicSceneMainFrame::GetInstance()->m_lin_nodes[id].scene_node->GetContent();

        Vector init_pos;
        Vector curr_pos;
        Vector dir;

        dsreal curr_theta, curr_phi, curr_speed;

        lin->GetCurrentPos( curr_pos );
        lin->GetInitPos( init_pos );
        lin->GetDirection( dir );

        curr_theta = lin->GetCurrentTheta();
        curr_phi = lin->GetCurrentPhi();
        curr_speed = lin->GetCurrentSpeed();

        DIALOG_ACTION_DECLARE( DIALOG_LINMVT_PROPS_TITLE )
        DIALOG_APPENDROOT_STRING( "scene name", BasicSceneMainFrame::GetInstance()->m_lin_nodes[id].name );
        DIALOG_APPENDROOT_STRING( "movement type ", "linear" );

        DIALOG_APPENDROOT_FLOAT( "current theta", curr_theta );
        DIALOG_APPENDROOT_FLOAT( "current phi", curr_phi );
        DIALOG_APPENDROOT_FLOAT( "current speed", curr_speed );

        DIALOG_APPENDROOT_NODE( "initial position", init_pos_root );
        DIALOG_APPENDNODE_FLOAT( init_pos_root, "x", init_pos[0] );
        DIALOG_APPENDNODE_FLOAT( init_pos_root, "y", init_pos[1] );
        DIALOG_APPENDNODE_FLOAT( init_pos_root, "z", init_pos[2] );

        DIALOG_APPENDROOT_NODE( "current position", curr_pos_root );
        DIALOG_APPENDNODE_FLOAT( curr_pos_root, "x", curr_pos[0] );
        DIALOG_APPENDNODE_FLOAT( curr_pos_root, "y", curr_pos[1] );
        DIALOG_APPENDNODE_FLOAT( curr_pos_root, "z", curr_pos[2] );

        DIALOG_APPENDROOT_NODE( "direction", dir_root );
        DIALOG_APPENDNODE_FLOAT( dir_root, "x", dir[0] );
        DIALOG_APPENDNODE_FLOAT( dir_root, "y", dir[1] );
        DIALOG_APPENDNODE_FLOAT( dir_root, "z", dir[2] );

        DIALOG_SHOW                    
    }
    else if( BasicSceneMainFrame::GetInstance()->m_free_nodes.count( id ) > 0 )
    {
        FreeMovement* free = BasicSceneMainFrame::GetInstance()->m_free_nodes[id].scene_node->GetContent();
        Vector curr_pos;

        free->GetCurrentPos( curr_pos );

        DIALOG_ACTION_DECLARE( DIALOG_FREEMVT_PROPS_TITLE )
        DIALOG_APPENDROOT_STRING( "scene name", BasicSceneMainFrame::GetInstance()->m_free_nodes[id].name );
        DIALOG_APPENDROOT_STRING( "movement type ", "free" );

        DIALOG_APPENDROOT_NODE( "current position", curr_pos_root );
        DIALOG_APPENDNODE_FLOAT( curr_pos_root, "x", curr_pos[0] );
        DIALOG_APPENDNODE_FLOAT( curr_pos_root, "y", curr_pos[1] );
        DIALOG_APPENDNODE_FLOAT( curr_pos_root, "z", curr_pos[2] );

        DIALOG_SHOW
    }
    else if( BasicSceneMainFrame::GetInstance()->m_circ_nodes.count( id ) > 0 )
    {
        CircularMovement* circ = BasicSceneMainFrame::GetInstance()->m_circ_nodes[id].scene_node->GetContent();

        Vector center_pos;
        Vector delta_center_pos;
        Vector axis;

        dsreal angle, theta, phi, curr_speed;

        circ->GetCenterPos( center_pos );
        circ->GetDeltaCenter( delta_center_pos );
        circ->GetRotAxis( axis );

        theta = circ->GetCurrentTheta();
        phi = circ->GetCurrentPhi();
        angle = circ->GetCurrentAngle();

        curr_speed = circ->GetAngularSpeed();

        DIALOG_ACTION_DECLARE( DIALOG_CIRCMVT_PROPS_TITLE )
        DIALOG_APPENDROOT_STRING( "scene name", BasicSceneMainFrame::GetInstance()->m_circ_nodes[id].name );
        DIALOG_APPENDROOT_STRING( "movement type ", "circular" );

        DIALOG_APPENDROOT_FLOAT( "current theta", theta );
        DIALOG_APPENDROOT_FLOAT( "current phi", phi );
        DIALOG_APPENDROOT_FLOAT( "current angle", angle );
        DIALOG_APPENDROOT_FLOAT( "current angular speed", curr_speed );

        DIALOG_APPENDROOT_NODE( "center position", center_pos_root );
        DIALOG_APPENDNODE_FLOAT( center_pos_root, "x", center_pos[0] );
        DIALOG_APPENDNODE_FLOAT( center_pos_root, "y", center_pos[1] );
        DIALOG_APPENDNODE_FLOAT( center_pos_root, "z", center_pos[2] );

        DIALOG_APPENDROOT_NODE( "delta center position", dcenter_pos_root );
        DIALOG_APPENDNODE_FLOAT( dcenter_pos_root, "x", delta_center_pos[0] );
        DIALOG_APPENDNODE_FLOAT( dcenter_pos_root, "y", delta_center_pos[1] );
        DIALOG_APPENDNODE_FLOAT( dcenter_pos_root, "z", delta_center_pos[2] );

        DIALOG_APPENDROOT_NODE( "axis", axis_root );
        DIALOG_APPENDNODE_FLOAT( axis_root, "x", axis[0] );
        DIALOG_APPENDNODE_FLOAT( axis_root, "y", axis[1] );
        DIALOG_APPENDNODE_FLOAT( axis_root, "z", axis[2] );

        DIALOG_SHOW
    }
    else if( BasicSceneMainFrame::GetInstance()->m_ll_nodes.count( id ) > 0 )
    {
        LongLatMovement* ll = BasicSceneMainFrame::GetInstance()->m_ll_nodes[id].scene_node->GetContent();

        dsreal longitud, latitud, altitud, theta, phi;

        longitud = ll->GetCurrentLongitud();
        latitud = ll->GetCurrentLatitud();
        altitud = ll->GetCurrentAltitud();
        theta = ll->GetCurrentTheta();
        phi = ll->GetCurrentPhi();

        DIALOG_ACTION_DECLARE( DIALOG_LONGLATMVT_PROPS_TITLE )
        DIALOG_APPENDROOT_STRING( "scene name", BasicSceneMainFrame::GetInstance()->m_ll_nodes[id].name );
        DIALOG_APPENDROOT_STRING( "movement type ", "long/lat" );

        DIALOG_APPENDROOT_FLOAT( "current theta", theta );
        DIALOG_APPENDROOT_FLOAT( "current phi", phi );
        DIALOG_APPENDROOT_FLOAT( "current longitud", longitud );
        DIALOG_APPENDROOT_FLOAT( "current latitud", latitud );
        DIALOG_APPENDROOT_FLOAT( "current altitud", altitud );

        DIALOG_SHOW
    }

    else if( BasicSceneMainFrame::GetInstance()->m_inertbody_descriptors.count( id ) > 0 )
    {
        Body::Parameters params = BasicSceneMainFrame::GetInstance()->m_inertbody_descriptors[id];
        BasicSceneMainFrame::SceneNodeEntry<InertBody> body_entry = BasicSceneMainFrame::GetInstance()->m_inertbody_nodes[id];

        DIALOG_ACTION_DECLARE( DIALOG_INERTBODY_PROPS_TITLE )

        DIALOG_APPENDROOT_STRING( "scene name", body_entry.name );

        DIALOG_APPENDROOT_BOOL( "active", body_entry.scene_node->GetContent()->IsEnabled() );
        DIALOG_APPENDROOT_FLOAT( "mass", params.mass );

        DIALOG_APPENDROOT_NODE( "shape description", shape_descr )

        switch( params.shape_descr.shape )
        {
            case Body::BOX_SHAPE:
                {

                    DIALOG_APPENDNODE_STRING( shape_descr, "shape type", "box" );

                    DIALOG_APPENDNODE_NODE( shape_descr, "box dims", box_dims )
                    DIALOG_APPENDNODE_FLOAT( box_dims, "x", params.shape_descr.box_dims[0] );
                    DIALOG_APPENDNODE_FLOAT( box_dims, "y", params.shape_descr.box_dims[1] );
                    DIALOG_APPENDNODE_FLOAT( box_dims, "z", params.shape_descr.box_dims[2] );

                }
                break;

            case Body::SPHERE_SHAPE:
                {

                    DIALOG_APPENDNODE_STRING( shape_descr, "shape type", "sphere" );
                    DIALOG_APPENDNODE_FLOAT( shape_descr, "radius", params.shape_descr.sphere_radius );

                }
                break;

            case Body::MESHE_SHAPE:
                {
                    DIALOG_APPENDNODE_STRING( shape_descr, "shape type", "meshe" );

                    DrawSpace::Asset::PropertiesMap pm;
                    params.shape_descr.meshe.GetPropertiesMap( pm );
                    dsstring name = pm["assetname"].GetPropValue<dsstring>();

                    DIALOG_APPENDNODE_STRING( shape_descr, "meshe name", name );

                }
                break;
        }

        DIALOG_SHOW
    }

    else if( BasicSceneMainFrame::GetInstance()->m_collider_descriptors.count( id ) > 0 )
    {
        Body::Parameters params = BasicSceneMainFrame::GetInstance()->m_collider_descriptors[id];
        BasicSceneMainFrame::SceneNodeEntry<Collider> body_entry = BasicSceneMainFrame::GetInstance()->m_collider_nodes[id];

        DIALOG_ACTION_DECLARE( DIALOG_COLLIDER_PROPS_TITLE )

        DIALOG_APPENDROOT_STRING( "scene name", body_entry.name );

        DIALOG_APPENDROOT_NODE( "shape description", shape_descr )

        switch( params.shape_descr.shape )
        {
            case Body::BOX_SHAPE:
                {

                    DIALOG_APPENDNODE_STRING( shape_descr, "shape type", "box" );

                    DIALOG_APPENDNODE_NODE( shape_descr, "box dims", box_dims )
                    DIALOG_APPENDNODE_FLOAT( box_dims, "x", params.shape_descr.box_dims[0] );
                    DIALOG_APPENDNODE_FLOAT( box_dims, "y", params.shape_descr.box_dims[1] );
                    DIALOG_APPENDNODE_FLOAT( box_dims, "z", params.shape_descr.box_dims[2] );

                }
                break;

            case Body::SPHERE_SHAPE:
                {

                    DIALOG_APPENDNODE_STRING( shape_descr, "shape type", "sphere" );
                    DIALOG_APPENDNODE_FLOAT( shape_descr, "radius", params.shape_descr.sphere_radius );

                }
                break;

            case Body::MESHE_SHAPE:
                {
                    DIALOG_APPENDNODE_STRING( shape_descr, "shape type", "meshe" );

                    DrawSpace::Asset::PropertiesMap pm;
                    params.shape_descr.meshe.GetPropertiesMap( pm );
                    dsstring name = pm["assetname"].GetPropValue<dsstring>();

                    DIALOG_APPENDNODE_STRING( shape_descr, "meshe name", name );

                }
                break;
        }

        DIALOG_SHOW

    }
}

