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

#include <wx/wx.h>
#include "BasicSceneMainFrame.h"

#include "ActionCircularMvtCreationApply.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Dynamics;
using namespace DrawSpace::Utils;

void ActionCircularMvtCreationApply::Execute( BasicSceneObjectPropertiesDialog* p_dialog )
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

    DIALOG_GET_FLOAT_PROPERTY( "initial theta", init_theta );
    DIALOG_GET_FLOAT_PROPERTY( "initial phi", init_phi );
    DIALOG_GET_FLOAT_PROPERTY( "initial angle", init_angle );

    DIALOG_GET_FLOAT_PROPERTY( "center position.x", cx );
    DIALOG_GET_FLOAT_PROPERTY( "center position.y", cy );
    DIALOG_GET_FLOAT_PROPERTY( "center position.z", cz );

    DIALOG_GET_FLOAT_PROPERTY( "delta center position.x", dx );
    DIALOG_GET_FLOAT_PROPERTY( "delta center position.y", dy );
    DIALOG_GET_FLOAT_PROPERTY( "delta center position.z", dz );

    DIALOG_GET_FLOAT_PROPERTY( "axis.x", ax );
    DIALOG_GET_FLOAT_PROPERTY( "axis.y", ay );
    DIALOG_GET_FLOAT_PROPERTY( "axis.z", az );

    // create the circular mvt node

    SceneNode<CircularMovement>* circ_node;
    circ_node = new SceneNode<CircularMovement>( alias );
    circ_node->SetContent( new CircularMovement() );

    //circ_node->RegisterUpdateBeginEvtHandler( BasicSceneMainFrame::GetInstance()->m_nodeupdatebegin_cb );
    circ_node->GetContent()->Init( Vector( cx, cy, cz, 1 ), Vector( dx, dy, dz, 1 ), Vector( ax, ay, az, 1 ), init_angle, init_theta, init_phi );

    /////////////////////////////////////////////////////////////////////////////////

    // now we must found the scenenodegraph we belong to make the RegisterNode() call
    void* id = BasicSceneMainFrame::GetInstance()->find_scenenodegraph_id( p_dialog->GetTreeItem() );

    BasicSceneMainFrame::SceneNodeGraphEntry entry;

    entry = BasicSceneMainFrame::GetInstance()->m_scenenodegraphs[id];
    entry.scenenodegraph->RegisterNode( circ_node );

    /////////////////////////////////////////////////////////////////////////////////

    // link to the scenegraph hierarchy

    wxTreeItemId current;
    current = p_dialog->GetTreeItem();
    id = current.GetID();

    if( BasicSceneMainFrame::GetInstance()->m_scenenodegraphs.count( id ) > 0 )
    {
        // parent is a scenegraph : use SceneNodeGraph::Add() method
        entry.scenenodegraph->AddNode( circ_node );
    }
    else
    {
        BaseSceneNode* parent_node = BasicSceneMainFrame::GetInstance()->m_tree_nodes[id];
        circ_node->LinkTo( parent_node );
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////

    // record the new node and associated metadata

    BasicSceneMainFrame::SceneNodeEntry<CircularMovement> c_entry;

    c_entry.name = alias;
    c_entry.scene_node = circ_node;
    c_entry.treeitemid = BasicSceneMainFrame::GetInstance()->AppendItem( p_dialog->GetTreeItem(), alias2, MOVEMENT_ICON_INDEX ); //treeitemid;

    BasicSceneMainFrame::GetInstance()->m_circ_nodes[c_entry.treeitemid.GetID()] = c_entry;

    BasicSceneMainFrame::GetInstance()->m_tree_nodes[c_entry.treeitemid.GetID()] = circ_node;
    BasicSceneMainFrame::GetInstance()->m_inv_tree_nodes[circ_node] = c_entry.treeitemid.GetID();

    ///////////////////////////////////////////////////////////////////////////////////

    //dsstring title;
    //dsstring* script_text;
    //bool * script_state;
    //title = "Circular movement node: ";
    //title += BasicSceneMainFrame::GetInstance()->m_circ_nodes[c_entry.treeitemid.GetID()].name;
    //script_text = &BasicSceneMainFrame::GetInstance()->m_circ_nodes[c_entry.treeitemid.GetID()].script;
    //script_state = &BasicSceneMainFrame::GetInstance()->m_circ_nodes[c_entry.treeitemid.GetID()].script_enabled;
    //BasicSceneScriptEditFrame* frame = new BasicSceneScriptEditFrame( BasicSceneMainFrame::GetInstance(), title, script_text, script_state );
    //BasicSceneMainFrame::GetInstance()->m_script_edit_frames[c_entry.treeitemid.GetID()] = frame;

    BasicSceneMainFrame::GetInstance()->m_menubuild_table[c_entry.treeitemid.GetID()] = CIRCMOVEMENT_MASK;

    BasicSceneMainFrame::GetInstance()->m_inv_treeitemid[c_entry.treeitemid.GetID()] = &( BasicSceneMainFrame::GetInstance()->m_circ_nodes[c_entry.treeitemid.GetID()].treeitemid );

    /////////////////////////////////////////////////////////////////////////

    entry.scenenodegraph->SendNodeAppReadyEvent( circ_node );


    DIALOG_CLOSE
}
