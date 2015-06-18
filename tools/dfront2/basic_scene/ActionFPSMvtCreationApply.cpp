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

#include "ActionFPSMvtCreationApply.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Dynamics;
using namespace DrawSpace::Utils;

void ActionFPSMvtCreationApply::Execute( BasicSceneObjectPropertiesDialog* p_dialog )
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
    DIALOG_GET_BOOL_PROPERTY( "y movement", ymvt );

    /////////////////////////////////////////////////////////////////////////////////

    // create the fps mvt node

    SceneNode<FPSMovement>* fps_node;
    fps_node = new SceneNode<FPSMovement>( alias );
    fps_node->SetContent( new FPSMovement( ymvt ) );

    //fps_node->RegisterUpdateBeginEvtHandler( BasicSceneMainFrame::GetInstance()->m_nodeupdatebegin_cb );

    fps_node->GetContent()->Init( Vector( x, y, z, 1 ), Maths::DegToRad( init_theta ), Maths::DegToRad( init_phi ) );

    /////////////////////////////////////////////////////////////////////////////////

    // now we must found the scenenodegraph we belong to make the RegisterNode() call
    void* id = BasicSceneMainFrame::GetInstance()->find_scenenodegraph_id( p_dialog->GetTreeItem() );

    BasicSceneMainFrame::SceneNodeGraphEntry entry;

    entry = BasicSceneMainFrame::GetInstance()->m_scenenodegraphs[id];
    entry.scenenodegraph->RegisterNode( fps_node );

    ///////////////////////////////////////////////////////////////////////////////////////////////

    // link to the scenegraph hierarchy

    wxTreeItemId current;
    current = p_dialog->GetTreeItem(); //m_last_clicked_treeitem;
    id = current.GetID();

    if( BasicSceneMainFrame::GetInstance()->m_scenenodegraphs.count( id ) > 0 )
    {
        // parent is a scenegraph : use SceneNodeGraph::Add() method
        entry.scenenodegraph->AddNode( fps_node );
    }
    else
    {
        BaseSceneNode* parent_node = BasicSceneMainFrame::GetInstance()->m_tree_nodes[id];
        fps_node->LinkTo( parent_node );
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////

    // record the new node and associated metadata

    BasicSceneMainFrame::SceneNodeEntry<FPSMovement> f_entry;

    f_entry.name = alias;
    f_entry.scene_node = fps_node;
    f_entry.treeitemid = BasicSceneMainFrame::GetInstance()->AppendItem( p_dialog->GetTreeItem(), alias2, MOVEMENT_ICON_INDEX ); //treeitemid;


    BasicSceneMainFrame::GetInstance()->m_fps_nodes[f_entry.treeitemid.GetID()] = f_entry;

    BasicSceneMainFrame::GetInstance()->m_tree_nodes[f_entry.treeitemid.GetID()] = fps_node;
    BasicSceneMainFrame::GetInstance()->m_inv_tree_nodes[fps_node] = f_entry.treeitemid.GetID();


    //dsstring title;
    //dsstring* script_text;
    //bool * script_state;
    //title = "FPS movement node: ";
    //title += BasicSceneMainFrame::GetInstance()->m_fps_nodes[f_entry.treeitemid.GetID()].name;
    //script_text = &BasicSceneMainFrame::GetInstance()->m_fps_nodes[f_entry.treeitemid.GetID()].script;
    //script_state = &BasicSceneMainFrame::GetInstance()->m_fps_nodes[f_entry.treeitemid.GetID()].script_enabled;
    //BasicSceneScriptEditFrame* frame = new BasicSceneScriptEditFrame( BasicSceneMainFrame::GetInstance(), title, script_text, script_state );
    //BasicSceneMainFrame::GetInstance()->m_script_edit_frames[f_entry.treeitemid.GetID()] = frame;

    BasicSceneMainFrame::GetInstance()->m_menubuild_table[f_entry.treeitemid.GetID()] = FPSMOVEMENT_MASK;

    BasicSceneMainFrame::GetInstance()->m_inv_treeitemid[f_entry.treeitemid.GetID()] = &( BasicSceneMainFrame::GetInstance()->m_fps_nodes[f_entry.treeitemid.GetID()].treeitemid );

    /////////////////////////////////////////////////////////////////////////

    entry.scenenodegraph->SendNodeAppReadyEvent( fps_node );


    DIALOG_CLOSE

}