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

#include "buildobjects.h"
#include "ActionCloudsLinkTo.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Dynamics;
using namespace DrawSpace::Utils;


void ActionCloudsLinkTo::Execute( DrawSpace::Core::PropertyPool& p_propertypool )
{
    dsstring scene_name = p_propertypool.GetPropValue<dsstring>( "scene_name" );
    dsstring scenegraph_name = p_propertypool.GetPropValue<dsstring>( "scenegraph_name" );
    dsstring parent_name = p_propertypool.GetPropValue<dsstring>( "parent_name" );
    BaseSceneNode* node = p_propertypool.GetPropValue<BaseSceneNode*>( "node" );
    DrawSpace::Utils::CloudsDescriptor clouds_descr = p_propertypool.GetPropValue<DrawSpace::Utils::CloudsDescriptor>( "descriptor" );

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

    if( !scene_found )
    {
        wxMessageBox( "Clouds node, unknown scenegraph name : " + scenegraph_name, "Script error", wxICON_ERROR );
        return;           
    }

    else if( !parent_found )
    {
        wxMessageBox( "Clouds node, unknown parent name : " + parent_name, "Script error", wxICON_ERROR );
        return;
    }

    SceneNode<Clouds>* clouds_node = static_cast<SceneNode<Clouds>*>( node );


    dsstring clouds_error;
    Clouds* clouds = BuildClouds( clouds_descr, clouds_error );
    if( NULL == clouds )
    {
        wxMessageBox( "Clouds node creation error : " + clouds_error, "Script error", wxICON_ERROR );
        return;
    }
    else
    {
        clouds_node->SetContent( clouds );

        scenenodegraph_entry.scenenodegraph->RegisterNode( node );

        if( parent )
        {            
            clouds_node->LinkTo( parent );
            parent_tree_item = BasicSceneMainFrame::GetInstance()->searchTreeItemIdInNodes( parent_id );
        }
        else
        {
            scenenodegraph_entry.scenenodegraph->AddNode( node );
            parent_tree_item = scenenodegraph_entry.treeitemid;
        }

        // record the new chunk node and associated metadata

        BasicSceneMainFrame::SceneNodeEntry<Clouds> c_entry;

        c_entry.name = scene_name;
        c_entry.scene_node = clouds_node;
        c_entry.treeitemid = BasicSceneMainFrame::GetInstance()->AppendItem( parent_tree_item, scene_name.c_str(), CLOUDS_ICON_INDEX );//treeitemid;

        BasicSceneMainFrame::GetInstance()->m_clouds_nodes[c_entry.treeitemid.GetID()] = c_entry;
        BasicSceneMainFrame::GetInstance()->m_tree_nodes[c_entry.treeitemid.GetID()] = clouds_node;
        BasicSceneMainFrame::GetInstance()->m_inv_tree_nodes[clouds_node] = c_entry.treeitemid.GetID();

        // update passes output queues
        for( std::map<dsstring, ChunkPassDescriptor>::iterator it = clouds_descr.chunk_descriptor.passes_slots.begin(); it != clouds_descr.chunk_descriptor.passes_slots.end(); ++it )
        {
            Pass* current_pass = dynamic_cast<Pass*>( ConfigsBase::GetInstance()->GetConfigurableInstance( it->first ) );
            current_pass->GetRenderingQueue()->UpdateOutputQueue();
        }

        // store chunk description

        BasicSceneMainFrame::GetInstance()->m_clouds_descriptors[c_entry.treeitemid.GetID()] = clouds_descr;

        BasicSceneMainFrame::GetInstance()->m_menubuild_table[c_entry.treeitemid.GetID()] = CLOUDS_MASK;

        BasicSceneMainFrame::GetInstance()->m_inv_treeitemid[c_entry.treeitemid.GetID()] = &( BasicSceneMainFrame::GetInstance()->m_clouds_nodes[c_entry.treeitemid.GetID()].treeitemid );

        /////////////////////////////////////////////////////////////////////////

        scenenodegraph_entry.scenenodegraph->SendNodeAppReadyEvent( clouds_node );
    }
}
