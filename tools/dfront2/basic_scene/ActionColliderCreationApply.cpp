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

#include "ActionColliderCreationApply.h"
#include "buildobjects.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Dynamics;
using namespace DrawSpace::Utils;

void ActionColliderCreationApply::Execute( BasicSceneObjectPropertiesDialog* p_dialog )
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

    Collider* body = BuildCollider( params, we.world );

    SceneNode<Collider>* body_node = new SceneNode<Collider>( alias );
    body_node->SetContent( body );

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


    BasicSceneMainFrame::SceneNodeEntry<Collider> c_entry;

    c_entry.name = alias;
    c_entry.scene_node = body_node;
    c_entry.treeitemid = BasicSceneMainFrame::GetInstance()->AppendItem( p_dialog->GetTreeItem(), alias2, COLLIDER_ICON_INDEX ); 

    // ajouter aussi l'icone sous l'icone du world associe
    wxTreeItemId treeitemid_world = BasicSceneMainFrame::GetInstance()->AppendItem( we.treeitemid, alias2, COLLIDER_ICON_INDEX ); 

    BasicSceneMainFrame::GetInstance()->m_collider_nodes[c_entry.treeitemid.GetID()] = c_entry;
    BasicSceneMainFrame::GetInstance()->m_collider_nodes[treeitemid_world] = c_entry;

    BasicSceneMainFrame::GetInstance()->m_tree_nodes[c_entry.treeitemid.GetID()] = body_node;
    BasicSceneMainFrame::GetInstance()->m_inv_tree_nodes[body_node] = c_entry.treeitemid.GetID();

    // store body description
    BasicSceneMainFrame::GetInstance()->m_collider_descriptors[c_entry.treeitemid.GetID()] = params;
    BasicSceneMainFrame::GetInstance()->m_collider_descriptors[treeitemid_world] = params;

    /////////////////////////////////////////////////////////////////////////

    BasicSceneMainFrame::GetInstance()->m_menubuild_table[c_entry.treeitemid.GetID()] = COLLIDER_MASK;
    BasicSceneMainFrame::GetInstance()->m_menubuild_table[treeitemid_world] = COLLIDER_MASK;

    BasicSceneMainFrame::GetInstance()->m_inv_treeitemid[c_entry.treeitemid.GetID()] = &( BasicSceneMainFrame::GetInstance()->m_collider_nodes[c_entry.treeitemid.GetID()].treeitemid );

    /////////////////////////////////////////////////////////////////////////

    entry.scenenodegraph->SendNodeAppReadyEvent( body_node );
        
    DIALOG_CLOSE


}