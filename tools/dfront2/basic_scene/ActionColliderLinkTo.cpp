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

#include "buildobjects.h"
#include "ActionColliderLinkTo.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Dynamics;
using namespace DrawSpace::Utils;


void ActionColliderLinkTo::Execute( DrawSpace::Core::PropertyPool& p_propertypool )
{
    dsstring scene_name = p_propertypool.GetPropValue<dsstring>( "scene_name" );
    dsstring world_name = p_propertypool.GetPropValue<dsstring>( "world_name" );
    dsstring scenegraph_name = p_propertypool.GetPropValue<dsstring>( "scenegraph_name" );
    dsstring parent_name = p_propertypool.GetPropValue<dsstring>( "parent_name" );
    BaseSceneNode* node = p_propertypool.GetPropValue<BaseSceneNode*>( "node" );

    Body::Parameters params = p_propertypool.GetPropValue<Body::Parameters>( "descriptor" );


    wxTreeItemId parent_tree_item;
    void* parent_id = NULL;


    // search for scenenodegraph

    bool scene_found = false;
    BasicSceneMainFrame::SceneNodeGraphEntry scenenodegraph_entry;

    for( std::map<void*, BasicSceneMainFrame::SceneNodeGraphEntry>::iterator it = BasicSceneMainFrame::GetInstance()->m_scenenodegraphs.begin(); it != BasicSceneMainFrame::GetInstance()->m_scenenodegraphs.end(); ++it )
    {
        if( it->second.name == scenegraph_name )
        {
            scenenodegraph_entry = it->second;
            scene_found = true;                
            break;
        }
    }

    bool parent_found = false;
    BaseSceneNode* parent = NULL;
    bool world_found = false;

    for( std::map<void*, DrawSpace::Core::BaseSceneNode*>::iterator it = BasicSceneMainFrame::GetInstance()->m_tree_nodes.begin(); it != BasicSceneMainFrame::GetInstance()->m_tree_nodes.end(); ++it )
    {
        dsstring node_scenename;
        it->second->GetSceneName( node_scenename );

        if( node_scenename == parent_name )
        {
            parent_found = true;
            parent = it->second;
            parent_id = it->first;
            break;
        }
    }

    if( !parent_found )
    {
        for( std::map<void*, BasicSceneMainFrame::SceneNodeGraphEntry>::iterator it = BasicSceneMainFrame::GetInstance()->m_scenenodegraphs.begin(); it != BasicSceneMainFrame::GetInstance()->m_scenenodegraphs.end(); ++it )
        {
            if( it->second.name == parent_name )
            {
                parent_found = true;
                parent_id = it->first;
                break;
            }
        }
    }

    std::map<void*, BasicSceneMainFrame::WorldEntry> worlds = BasicSceneMainFrame::GetInstance()->m_worlds;
    BasicSceneMainFrame::WorldEntry we;
    // find world
    for( std::map<void*, BasicSceneMainFrame::WorldEntry>::iterator it = worlds.begin(); it != worlds.end(); ++it )
    {
        if( it->second.name == world_name )
        {            
            we = it->second;
            world_found = true;
            break;
        }
    }

    if( !scene_found )
    {
        wxMessageBox( "Collider, unknown scenegraph name : " + scenegraph_name, "Script error", wxICON_ERROR );
        return;           
    }

    else if( !parent_found )
    {
        wxMessageBox( "Collider, unknown parent name : " + parent_name, "Script error", wxICON_ERROR );
        return;
    }

    else if( !world_found )
    {
        wxMessageBox( "Collider, unknown world name : " + world_name, "Script error", wxICON_ERROR );
        return;               
    }


    SceneNode<Collider>* body_node = static_cast<SceneNode<Collider>*>( node );
    //body_node->RegisterUpdateBeginEvtHandler( BasicSceneMainFrame::GetInstance()->m_nodeupdatebegin_cb );

    Collider* body = BuildCollider( params, we.world );
    body_node->SetContent( body );

    scenenodegraph_entry.scenenodegraph->RegisterNode( node );

    if( parent )
    {            
        body_node->LinkTo( parent );
        parent_tree_item = BasicSceneMainFrame::GetInstance()->searchTreeItemIdInNodes( parent_id );
    }
    else
    {
        scenenodegraph_entry.scenenodegraph->AddNode( node );
        parent_tree_item = scenenodegraph_entry.treeitemid;
    }

    BasicSceneMainFrame::SceneNodeEntry<Collider> c_entry;

    c_entry.name = scene_name;
    c_entry.scene_node = body_node;
    c_entry.treeitemid = BasicSceneMainFrame::GetInstance()->AppendItem( parent_tree_item, scene_name.c_str(), COLLIDER_ICON_INDEX );

    // ajouter aussi l'icone sous l'icone du world associe
    wxTreeItemId treeitemid_world = BasicSceneMainFrame::GetInstance()->AppendItem( we.treeitemid, scene_name.c_str(), COLLIDER_ICON_INDEX ); 


    BasicSceneMainFrame::GetInstance()->m_collider_nodes[c_entry.treeitemid.GetID()] = c_entry;
    BasicSceneMainFrame::GetInstance()->m_collider_nodes[treeitemid_world] = c_entry;
    BasicSceneMainFrame::GetInstance()->m_tree_nodes[c_entry.treeitemid.GetID()] = body_node;
    BasicSceneMainFrame::GetInstance()->m_inv_tree_nodes[body_node] = c_entry.treeitemid.GetID();

    BasicSceneMainFrame::GetInstance()->m_collider_descriptors[c_entry.treeitemid.GetID()] = params;
    BasicSceneMainFrame::GetInstance()->m_collider_descriptors[treeitemid_world] = params;

    /////////////////////////////////////////////////////////////////////////

    BasicSceneMainFrame::GetInstance()->m_menubuild_table[c_entry.treeitemid.GetID()] = COLLIDER_MASK;
    BasicSceneMainFrame::GetInstance()->m_menubuild_table[treeitemid_world] = COLLIDER_MASK;

    BasicSceneMainFrame::GetInstance()->m_inv_treeitemid[c_entry.treeitemid.GetID()] = &( BasicSceneMainFrame::GetInstance()->m_collider_nodes[c_entry.treeitemid.GetID()].treeitemid );

    /////////////////////////////////////////////////////////////////////////

    scenenodegraph_entry.scenenodegraph->SendNodeAppReadyEvent( body_node );

}