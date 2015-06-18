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

#include "ActionFreeMvtCreationApply.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Dynamics;
using namespace DrawSpace::Utils;

void ActionFreeMvtCreationApply::Execute( BasicSceneObjectPropertiesDialog* p_dialog )
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

    DIALOG_GET_FLOAT_PROPERTY( "initial position.x", x );
    DIALOG_GET_FLOAT_PROPERTY( "initial position.y", y );
    DIALOG_GET_FLOAT_PROPERTY( "initial position.z", z );


    /////////////////////////////////////////////////////////////////////////////////

    // create the free mvt node

    SceneNode<FreeMovement>* free_node;
    free_node = new SceneNode<FreeMovement>( alias );
    free_node->SetContent( new FreeMovement() );

    //free_node->RegisterUpdateBeginEvtHandler( BasicSceneMainFrame::GetInstance()->m_nodeupdatebegin_cb );
    free_node->GetContent()->Init( Vector( x, y, z, 1 ) );

    /////////////////////////////////////////////////////////////////////////////////

    // now we must found the scenenodegraph we belong to make the RegisterNode() call
    void* id = BasicSceneMainFrame::GetInstance()->find_scenenodegraph_id( p_dialog->GetTreeItem() );

    BasicSceneMainFrame::SceneNodeGraphEntry entry;

    entry = BasicSceneMainFrame::GetInstance()->m_scenenodegraphs[id];
    entry.scenenodegraph->RegisterNode( free_node );

    /////////////////////////////////////////////////////////////////////////////////

    // link to the scenegraph hierarchy

    wxTreeItemId current;
    current = p_dialog->GetTreeItem();
    id = current.GetID();

    if( BasicSceneMainFrame::GetInstance()->m_scenenodegraphs.count( id ) > 0 )
    {
        // parent is a scenegraph : use SceneNodeGraph::Add() method
        entry.scenenodegraph->AddNode( free_node );
    }
    else
    {
        BaseSceneNode* parent_node = BasicSceneMainFrame::GetInstance()->m_tree_nodes[id];
        free_node->LinkTo( parent_node );
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////

    // record the new node and associated metadata

    BasicSceneMainFrame::SceneNodeEntry<FreeMovement> f_entry;

    f_entry.name = alias;
    f_entry.scene_node = free_node;
    f_entry.treeitemid = BasicSceneMainFrame::GetInstance()->AppendItem( p_dialog->GetTreeItem(), alias2, MOVEMENT_ICON_INDEX ); //treeitemid;

    BasicSceneMainFrame::GetInstance()->m_free_nodes[f_entry.treeitemid.GetID()] = f_entry;

    BasicSceneMainFrame::GetInstance()->m_tree_nodes[f_entry.treeitemid.GetID()] = free_node;
    BasicSceneMainFrame::GetInstance()->m_inv_tree_nodes[free_node] = f_entry.treeitemid.GetID();

    ///////////////////////////////////////////////////////////////////////////

    //dsstring title;
    //dsstring* script_text;
    //bool * script_state;
    //title = "Free movement node: ";
    //title += BasicSceneMainFrame::GetInstance()->m_free_nodes[f_entry.treeitemid.GetID()].name;
    //script_text = &BasicSceneMainFrame::GetInstance()->m_free_nodes[f_entry.treeitemid.GetID()].script;
    //script_state = &BasicSceneMainFrame::GetInstance()->m_free_nodes[f_entry.treeitemid.GetID()].script_enabled;
    //BasicSceneScriptEditFrame* frame = new BasicSceneScriptEditFrame( BasicSceneMainFrame::GetInstance(), title, script_text, script_state );
    //BasicSceneMainFrame::GetInstance()->m_script_edit_frames[f_entry.treeitemid.GetID()] = frame;

    BasicSceneMainFrame::GetInstance()->m_menubuild_table[f_entry.treeitemid.GetID()] = FREEMOVEMENT_MASK;

    BasicSceneMainFrame::GetInstance()->m_inv_treeitemid[f_entry.treeitemid.GetID()] = &( BasicSceneMainFrame::GetInstance()->m_free_nodes[f_entry.treeitemid.GetID()].treeitemid );


    /////////////////////////////////////////////////////////////////////////

    entry.scenenodegraph->SendNodeAppReadyEvent( free_node );


    DIALOG_CLOSE
}