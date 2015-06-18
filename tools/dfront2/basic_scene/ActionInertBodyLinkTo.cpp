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
#include "ActionInertBodyLinkTo.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Dynamics;
using namespace DrawSpace::Utils;


void ActionInertBodyLinkTo::Execute( DrawSpace::Core::PropertyPool& p_propertypool )
{
    dsstring scene_name = p_propertypool.GetPropValue<dsstring>( "scene_name" );
    dsstring world_name = p_propertypool.GetPropValue<dsstring>( "world_name" );
    dsstring scenegraph_name = p_propertypool.GetPropValue<dsstring>( "scenegraph_name" );
    dsstring parent_name = p_propertypool.GetPropValue<dsstring>( "parent_name" );
    BaseSceneNode* node = p_propertypool.GetPropValue<BaseSceneNode*>( "node" );
    bool init_state = p_propertypool.GetPropValue<bool>( "init_state" );

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
        wxMessageBox( "Inert body, unknown scenegraph name : " + scenegraph_name, "Script error", wxICON_ERROR );
        return;           
    }

    else if( !parent_found )
    {
        wxMessageBox( "Inert body, unknown parent name : " + parent_name, "Script error", wxICON_ERROR );
        return;
    }

    else if( !world_found )
    {
        wxMessageBox( "Inert body, unknown world name : " + world_name, "Script error", wxICON_ERROR );
        return;               
    }

    SceneNode<InertBody>* body_node = static_cast<SceneNode<InertBody>*>( node );
    //body_node->RegisterUpdateBeginEvtHandler( BasicSceneMainFrame::GetInstance()->m_nodeupdatebegin_cb );

    InertBody* body = BuildInertBody( params, we.world );
    body->Enable( init_state );

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

    BasicSceneMainFrame::SceneNodeEntry<InertBody> i_entry;

    i_entry.name = scene_name;
    i_entry.scene_node = body_node;
    i_entry.treeitemid = BasicSceneMainFrame::GetInstance()->AppendItem( parent_tree_item, scene_name.c_str(), INERTBODY_ICON_INDEX );

    // ajouter aussi l'icone sous l'icone du world associe
    wxTreeItemId treeitemid_world = BasicSceneMainFrame::GetInstance()->AppendItem( we.treeitemid, scene_name.c_str(), INERTBODY_ICON_INDEX ); 


    BasicSceneMainFrame::GetInstance()->m_inertbody_nodes[i_entry.treeitemid.GetID()] = i_entry;
    BasicSceneMainFrame::GetInstance()->m_inertbody_nodes[treeitemid_world] = i_entry;
    BasicSceneMainFrame::GetInstance()->m_tree_nodes[i_entry.treeitemid.GetID()] = body_node;
    BasicSceneMainFrame::GetInstance()->m_inv_tree_nodes[body_node] = i_entry.treeitemid.GetID();

    BasicSceneMainFrame::GetInstance()->m_inertbody_descriptors[i_entry.treeitemid.GetID()] = params;
    BasicSceneMainFrame::GetInstance()->m_inertbody_descriptors[treeitemid_world] = params;


    /////////////////////////////////////////////

    //dsstring title;
    //dsstring* script_text;
    //bool * script_state;
    //title = "Chunk node: ";
    //title += BasicSceneMainFrame::GetInstance()->m_inertbody_nodes[i_entry.treeitemid.GetID()].name;
    //script_text = &BasicSceneMainFrame::GetInstance()->m_inertbody_nodes[i_entry.treeitemid.GetID()].script;
    //script_state = &BasicSceneMainFrame::GetInstance()->m_inertbody_nodes[i_entry.treeitemid.GetID()].script_enabled;
    //BasicSceneScriptEditFrame* frame = new BasicSceneScriptEditFrame( BasicSceneMainFrame::GetInstance(), title, script_text, script_state );
    //BasicSceneMainFrame::GetInstance()->m_script_edit_frames[i_entry.treeitemid.GetID()] = frame;

    //BasicSceneMainFrame::GetInstance()->m_script_edit_frames[treeitemid_world] = frame;

    BasicSceneMainFrame::GetInstance()->m_menubuild_table[i_entry.treeitemid.GetID()] = INERTBODY_MASK;
    BasicSceneMainFrame::GetInstance()->m_menubuild_table[treeitemid_world] = INERTBODY_MASK;

    BasicSceneMainFrame::GetInstance()->m_inv_treeitemid[i_entry.treeitemid.GetID()] = &( BasicSceneMainFrame::GetInstance()->m_inertbody_nodes[i_entry.treeitemid.GetID()].treeitemid );

    /////////////////////////////////////////////////////////////////////////

    scenenodegraph_entry.scenenodegraph->SendNodeAppReadyEvent( body_node );
}