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

#include "ActionLinearMvtCreationApply.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Dynamics;
using namespace DrawSpace::Utils;

void ActionLinearMvtCreationApply::Execute( BasicSceneObjectPropertiesDialog* p_dialog )
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

    DIALOG_GET_FLOAT_PROPERTY( "initial position.x", x );
    DIALOG_GET_FLOAT_PROPERTY( "initial position.y", y );
    DIALOG_GET_FLOAT_PROPERTY( "initial position.z", z );

    DIALOG_GET_FLOAT_PROPERTY( "direction.x", xdir );
    DIALOG_GET_FLOAT_PROPERTY( "direction.y", ydir );
    DIALOG_GET_FLOAT_PROPERTY( "direction.z", zdir );

    /////////////////////////////////////////////////////////////////////////////////

    // create the linear mvt node

    SceneNode<LinearMovement>* lin_node;
    lin_node = new SceneNode<LinearMovement>( alias );
    lin_node->SetContent( new LinearMovement() );

    //lin_node->RegisterUpdateBeginEvtHandler( BasicSceneMainFrame::GetInstance()->m_nodeupdatebegin_cb );
    lin_node->GetContent()->Init( Vector( x, y, z, 1 ), Vector( xdir, ydir, zdir, 1 ), init_theta, init_phi );

    /////////////////////////////////////////////////////////////////////////////////

    // now we must found the scenenodegraph we belong to make the RegisterNode() call
    void* id = BasicSceneMainFrame::GetInstance()->find_scenenodegraph_id( p_dialog->GetTreeItem() );

    BasicSceneMainFrame::SceneNodeGraphEntry entry;

    entry = BasicSceneMainFrame::GetInstance()->m_scenenodegraphs[id];
    entry.scenenodegraph->RegisterNode( lin_node );

    /////////////////////////////////////////////////////////////////////////////////

    // link to the scenegraph hierarchy

    wxTreeItemId current;
    current = p_dialog->GetTreeItem();
    id = current.GetID();

    if( BasicSceneMainFrame::GetInstance()->m_scenenodegraphs.count( id ) > 0 )
    {
        // parent is a scenegraph : use SceneNodeGraph::Add() method
        entry.scenenodegraph->AddNode( lin_node );
    }
    else
    {
        BaseSceneNode* parent_node = BasicSceneMainFrame::GetInstance()->m_tree_nodes[id];
        lin_node->LinkTo( parent_node );
    }

    /////////////////////////////////////////////////////////////////////////////////

    // record the new node and associated metadata

    BasicSceneMainFrame::SceneNodeEntry<LinearMovement> l_entry;

    l_entry.name = alias;
    l_entry.scene_node = lin_node;
    l_entry.treeitemid = BasicSceneMainFrame::GetInstance()->AppendItem( p_dialog->GetTreeItem(), alias2, MOVEMENT_ICON_INDEX ); //treeitemid;

    BasicSceneMainFrame::GetInstance()->m_lin_nodes[l_entry.treeitemid.GetID()] = l_entry;

    BasicSceneMainFrame::GetInstance()->m_tree_nodes[l_entry.treeitemid.GetID()] = lin_node;
    BasicSceneMainFrame::GetInstance()->m_inv_tree_nodes[lin_node] = l_entry.treeitemid.GetID();

    /////////////////////////////////////////////////////////////////////////

    //dsstring title;
    //dsstring* script_text;
    //bool * script_state;
    //title = "Linear movement node: ";
    //title += BasicSceneMainFrame::GetInstance()->m_lin_nodes[l_entry.treeitemid.GetID()].name;
    //script_text = &BasicSceneMainFrame::GetInstance()->m_lin_nodes[l_entry.treeitemid.GetID()].script;
    //script_state = &BasicSceneMainFrame::GetInstance()->m_lin_nodes[l_entry.treeitemid.GetID()].script_enabled;
    //BasicSceneScriptEditFrame* frame = new BasicSceneScriptEditFrame( BasicSceneMainFrame::GetInstance(), title, script_text, script_state );
    //BasicSceneMainFrame::GetInstance()->m_script_edit_frames[l_entry.treeitemid.GetID()] = frame;


    /////////////////////////////////////////////////////////////////////////

    BasicSceneMainFrame::GetInstance()->m_menubuild_table[l_entry.treeitemid.GetID()] = LINMOVEMENT_MASK;

    BasicSceneMainFrame::GetInstance()->m_inv_treeitemid[l_entry.treeitemid.GetID()] = &( BasicSceneMainFrame::GetInstance()->m_lin_nodes[l_entry.treeitemid.GetID()].treeitemid );

    /////////////////////////////////////////////////////////////////////////

    entry.scenenodegraph->SendNodeAppReadyEvent( lin_node );


    DIALOG_CLOSE

}