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

#include "ActionCameraPointCreationApply.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Dynamics;
using namespace DrawSpace::Utils;

void ActionCameraPointCreationApply::Execute( BasicSceneObjectPropertiesDialog* p_dialog )
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

    // create the camera node

    SceneNode<CameraPoint>* camera_node;
    camera_node = new SceneNode<CameraPoint>( alias );
    camera_node->SetContent( new CameraPoint );

    camera_node->RegisterUpdateBeginEvtHandler( BasicSceneMainFrame::GetInstance()->m_nodeupdatebegin_cb );

    /////////////////////////////////////////////////////////////////////////////////

    // now we must found the scenenodegraph we belong to make the RegisterNode() call
    void* id = BasicSceneMainFrame::GetInstance()->find_scenenodegraph_id(  p_dialog->GetTreeItem() );

    BasicSceneMainFrame::SceneNodeGraphEntry entry;

    entry = BasicSceneMainFrame::GetInstance()->m_scenenodegraphs[id];
    entry.scenenodegraph->RegisterNode( camera_node );

    ///////////////////////////////////////////////////////////////////////////////////////////////

    // link to the scenegraph hierarchy

    wxTreeItemId current;
    current = p_dialog->GetTreeItem(); //m_last_clicked_treeitem;
    id = current.GetID();

    if( BasicSceneMainFrame::GetInstance()->m_scenenodegraphs.count( id ) > 0 )
    {
        // parent is a scenegraph : use SceneNodeGraph::Add() method
        entry.scenenodegraph->AddNode( camera_node );
    }
    else
    {
        BaseSceneNode* parent_node = BasicSceneMainFrame::GetInstance()->m_tree_nodes[id];
        camera_node->LinkTo( parent_node );
    }

   
    /////////////////////////////////////////////////////////////////////////////////

    // record the new transformation node and associated metadata

    BasicSceneMainFrame::SceneNodeEntry<CameraPoint> c_entry;

    c_entry.name = alias;
    c_entry.scene_node = camera_node;
    c_entry.treeitemid = BasicSceneMainFrame::GetInstance()->AppendItem( p_dialog->GetTreeItem(), alias2, CAMERA_ICON_INDEX ); //treeitemid;


    BasicSceneMainFrame::GetInstance()->m_camera_nodes[c_entry.treeitemid.GetID()] = c_entry;

    BasicSceneMainFrame::GetInstance()->m_tree_nodes[c_entry.treeitemid.GetID()] = camera_node;
    BasicSceneMainFrame::GetInstance()->m_inv_tree_nodes[camera_node] = c_entry.treeitemid.GetID();


    dsstring title;
    dsstring* script_text;
    bool * script_state;
    title = "CameraPoint node: ";
    title += BasicSceneMainFrame::GetInstance()->m_camera_nodes[c_entry.treeitemid.GetID()].name;
    script_text = &BasicSceneMainFrame::GetInstance()->m_camera_nodes[c_entry.treeitemid.GetID()].script;
    script_state = &BasicSceneMainFrame::GetInstance()->m_camera_nodes[c_entry.treeitemid.GetID()].script_enabled;
    BasicSceneScriptEditFrame* frame = new BasicSceneScriptEditFrame( BasicSceneMainFrame::GetInstance(), title, script_text, script_state );
    BasicSceneMainFrame::GetInstance()->m_script_edit_frames[c_entry.treeitemid.GetID()] = frame;

    BasicSceneMainFrame::GetInstance()->m_menubuild_table[c_entry.treeitemid.GetID()] = CAMERA_MASK;

    DIALOG_CLOSE
}