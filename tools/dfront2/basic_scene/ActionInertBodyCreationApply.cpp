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

#include <wx/wx.h>
#include "BasicSceneMainFrame.h"

#include "ActionInertBodyCreationApply.h"
#include "buildobjects.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Dynamics;
using namespace DrawSpace::Utils;


void ActionInertBodyCreationApply::Execute( BasicSceneObjectPropertiesDialog* p_dialog )
{
    DIALOG_GETGRID
    DIALOG_PROPERTIES_VARS

    DIALOG_GET_STRING_PROPERTY( "scene name", alias2 )

    DIALOG_WXSTRING_TO_DSSTRING( alias2, alias )

    if( "" == alias )
    {
        wxMessageBox( "'scene name' attribute cannot be void", "DrawFront error", wxICON_ERROR );
        return;
    }

    /////////////////////////////////////////////////////////////////////////////////

    DIALOG_GET_ENUM_PROPERTY( "world", selected_world_name )

    Body::Parameters params;

    std::vector<Matrix> new_chain;
    bool ok = true;
    Vector values;

    DIALOG_EXPLORE_NODES_BEGIN( "", "matrix %d", i, matrix_label )

        DIALOG_GET_ENUM_PROPERTY( DIALOG_INCREMENT_STRING( matrix_label, "type" ), matrix_type )
        DIALOG_GET_FLOAT_PROPERTY( DIALOG_INCREMENT_STRING( matrix_label, "x" ), x )
        DIALOG_GET_FLOAT_PROPERTY( DIALOG_INCREMENT_STRING( matrix_label, "y" ), y )
        DIALOG_GET_FLOAT_PROPERTY( DIALOG_INCREMENT_STRING( matrix_label, "z" ), z )
        DIALOG_GET_FLOAT_PROPERTY( DIALOG_INCREMENT_STRING( matrix_label, "angle" ), angle )

        DIALOG_WXSTRING_TO_DSSTRING( matrix_type, matrix_type2 )

        values[0] = x;
        values[1] = y;
        values[2] = z;
        values[3] = angle;
       
        Matrix mat;

        if( "identity" == matrix_type2 )
        {
            mat.Identity();            
        }
        else if( "translation" == matrix_type2 )
        {
            values[3] = 1.0;
            mat.Translation( values );            
        }
        else if( "rotation" == matrix_type2 )
        {
            Vector axis = values;
            axis[3] = 1.0;
            mat.Rotation( axis, Maths::DegToRad( values[3] ) );
        }
        else if( "scaling" == matrix_type2 )
        {
            values[3] = 1.0;
            mat.Scale( values );
        }
        else if( "zero" == matrix_type2 )
        {
            mat.Zero();            
        }
        else if( "undetermined" == matrix_type2 )
        {
            ok = false;
            break;
        }

        new_chain.push_back( mat );      


    DIALOG_EXPLORE_NODES_END( i )

    Transformation tdet;
    if( ok )
    {            
        for( size_t i = 0; i < new_chain.size(); i++ )
        {
            tdet.PushMatrix( new_chain[i] );
        }         
    }
    else
    {
        wxMessageBox( "Edited matrix chain has a undetermined matrix", "DrawFront error", wxICON_ERROR );
        return;
    }

    tdet.BuildResult();        
    tdet.GetResult( &params.initial_attitude );

    DIALOG_GET_FLOAT_PROPERTY( "mass", mass );
    params.mass = mass;

    DIALOG_GET_BOOL_PROPERTY( "active at creation", active );

    DIALOG_GET_ENUM_PROPERTY( "shape description.shape type", shape_type )

    if( shape_type == "box" )
    {
        DIALOG_GET_FLOAT_PROPERTY( "shape description.box dims.x", x )
        DIALOG_GET_FLOAT_PROPERTY( "shape description.box dims.y", y )
        DIALOG_GET_FLOAT_PROPERTY( "shape description.box dims.z", z )

        params.shape_descr.box_dims[0] = x;
        params.shape_descr.box_dims[1] = y;
        params.shape_descr.box_dims[2] = z;
        params.shape_descr.box_dims[3] = 1.0;

        params.shape_descr.shape = Body::BOX_SHAPE;
    }
    else if( shape_type == "sphere" )
    {
        DIALOG_GET_FLOAT_PROPERTY( "shape description.sphere radius", r )
        params.shape_descr.sphere_radius = r;

        params.shape_descr.shape = Body::SPHERE_SHAPE;
    }
    else if( shape_type == "meshe" )
    {
        DIALOG_GET_ENUM_PROPERTY( "shape description.meshe", meshe_name );  
        DIALOG_WXSTRING_TO_DSSTRING( meshe_name, meshe_name2 );

        Meshe* meshe = dynamic_cast<Meshe*>( AssetsBase::GetInstance()->GetAsset( meshe_name2 ) );

        params.shape_descr.meshe = *meshe;

        params.shape_descr.shape = Body::MESHE_SHAPE;
    }

    std::map<void*, BasicSceneMainFrame::WorldEntry> worlds = BasicSceneMainFrame::GetInstance()->m_worlds;
    
    BasicSceneMainFrame::WorldEntry we;

    // find world
    for( std::map<void*, BasicSceneMainFrame::WorldEntry>::iterator it = worlds.begin(); it != worlds.end(); ++it )
    {
        if( it->second.name == selected_world_name )
        {            
            we = it->second;
            break;
        }
    }

    InertBody* body = BuildInertBody( params, we.world );
    body->Enable( active );


    SceneNode<InertBody>* body_node = new SceneNode<InertBody>( alias );
    body_node->SetContent( body );
    //body_node->RegisterUpdateBeginEvtHandler( BasicSceneMainFrame::GetInstance()->m_nodeupdatebegin_cb );

    // now we must found the scenenodegraph we belong to make the RegisterNode() call
    void* id = BasicSceneMainFrame::GetInstance()->find_scenenodegraph_id( p_dialog->GetTreeItem() );

    BasicSceneMainFrame::SceneNodeGraphEntry entry;

    entry = BasicSceneMainFrame::GetInstance()->m_scenenodegraphs[id];
    entry.scenenodegraph->RegisterNode( body_node );

    // link to the scenegraph hierarchy

    wxTreeItemId current;
    current = p_dialog->GetTreeItem();
    id = current.GetID();

    if( BasicSceneMainFrame::GetInstance()->m_scenenodegraphs.count( id ) > 0 )
    {
        // parent is a scenegraph : use SceneNodeGraph::Add() method
        entry.scenenodegraph->AddNode( body_node );
    }
    else
    {
        BaseSceneNode* parent_node = BasicSceneMainFrame::GetInstance()->m_tree_nodes[id];
        body_node->LinkTo( parent_node );
    }

    BasicSceneMainFrame::SceneNodeEntry<InertBody> i_entry;

    i_entry.name = alias;
    i_entry.scene_node = body_node;
    i_entry.treeitemid = BasicSceneMainFrame::GetInstance()->AppendItem( p_dialog->GetTreeItem(), alias2, INERTBODY_ICON_INDEX ); 

    // ajouter aussi l'icone sous l'icone du world associe
    wxTreeItemId treeitemid_world = BasicSceneMainFrame::GetInstance()->AppendItem( we.treeitemid, alias2, INERTBODY_ICON_INDEX ); 

    BasicSceneMainFrame::GetInstance()->m_inertbody_nodes[i_entry.treeitemid.GetID()] = i_entry;
    BasicSceneMainFrame::GetInstance()->m_inertbody_nodes[treeitemid_world] = i_entry;

    BasicSceneMainFrame::GetInstance()->m_tree_nodes[i_entry.treeitemid.GetID()] = body_node;
    BasicSceneMainFrame::GetInstance()->m_inv_tree_nodes[body_node] = i_entry.treeitemid.GetID();

    // store body description
    BasicSceneMainFrame::GetInstance()->m_inertbody_descriptors[i_entry.treeitemid.GetID()] = params;
    BasicSceneMainFrame::GetInstance()->m_inertbody_descriptors[treeitemid_world] = params;

    /////////////////////////////////////////////////////////////////////////

    //dsstring title;
    //dsstring* script_text;
    //bool * script_state;
    //title = "Inertbody node: ";
    //title += BasicSceneMainFrame::GetInstance()->m_inertbody_nodes[i_entry.treeitemid.GetID()].name;
    //script_text = &BasicSceneMainFrame::GetInstance()->m_inertbody_nodes[i_entry.treeitemid.GetID()].script;
    //script_state = &BasicSceneMainFrame::GetInstance()->m_inertbody_nodes[i_entry.treeitemid.GetID()].script_enabled;
    //BasicSceneScriptEditFrame* frame = new BasicSceneScriptEditFrame( BasicSceneMainFrame::GetInstance(), title, script_text, script_state );
    //BasicSceneMainFrame::GetInstance()->m_script_edit_frames[i_entry.treeitemid.GetID()] = frame;

    //BasicSceneMainFrame::GetInstance()->m_script_edit_frames[treeitemid_world] = frame;

    /////////////////////////////////////////////////////////////////////////

    BasicSceneMainFrame::GetInstance()->m_menubuild_table[i_entry.treeitemid.GetID()] = INERTBODY_MASK;
    BasicSceneMainFrame::GetInstance()->m_menubuild_table[treeitemid_world] = INERTBODY_MASK;

    BasicSceneMainFrame::GetInstance()->m_inv_treeitemid[i_entry.treeitemid.GetID()] = &( BasicSceneMainFrame::GetInstance()->m_inertbody_nodes[i_entry.treeitemid.GetID()].treeitemid );

    /////////////////////////////////////////////////////////////////////////

    entry.scenenodegraph->SendNodeAppReadyEvent( body_node );
        
    DIALOG_CLOSE

}