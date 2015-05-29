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

#include "ActionSpaceBoxLinkTo.h"
#include "buildobjects.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Dynamics;
using namespace DrawSpace::Utils;


void ActionSpaceBoxLinkTo::Execute( DrawSpace::Core::PropertyPool& p_propertypool )
{
    dsstring scene_name = p_propertypool.GetPropValue<dsstring>( "scene_name" );
    dsstring scenegraph_name = p_propertypool.GetPropValue<dsstring>( "scenegraph_name" );
    dsstring parent_name = p_propertypool.GetPropValue<dsstring>( "parent_name" );
    BaseSceneNode* node = p_propertypool.GetPropValue<BaseSceneNode*>( "node" );
    DrawSpace::Utils::SpaceboxDescriptor sb_descr = p_propertypool.GetPropValue<DrawSpace::Utils::SpaceboxDescriptor>( "descriptor" );

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
        wxMessageBox( "Spacebox node, unknown scenegraph name : " + scenegraph_name, "Script error", wxICON_ERROR );
        return;           
    }

    else if( !parent_found )
    {
        wxMessageBox( "Spacebox node, unknown parent name : " + parent_name, "Script error", wxICON_ERROR );
        return;
    }

    SceneNode<Spacebox>* sb_node = static_cast<SceneNode<Spacebox>*>( node );

    sb_node->RegisterUpdateBeginEvtHandler( BasicSceneMainFrame::GetInstance()->m_nodeupdatebegin_cb );
        
    //sb_node->SetContent( new Spacebox );
    dsstring sb_error;
    Spacebox* sb = BuildSpaceBox( sb_descr, sb_error );
    if( NULL == sb )
    {
        wxMessageBox( "Spacebox node creation error : " + sb_error, "Script error", wxICON_ERROR );
        return;
    }
    else
    {
        sb_node->SetContent( sb );

        scenenodegraph_entry.scenenodegraph->RegisterNode( node );

        if( parent )
        {            
            sb_node->LinkTo( parent );
            parent_tree_item = BasicSceneMainFrame::GetInstance()->searchTreeItemIdInNodes( parent_id );
        }
        else
        {
            scenenodegraph_entry.scenenodegraph->AddNode( node );
            parent_tree_item = scenenodegraph_entry.treeitemid;
        }
               
        // record the new spacebox node and associated metadata

        BasicSceneMainFrame::SceneNodeEntry<Spacebox> t_entry;

        t_entry.name = scene_name;
        t_entry.scene_node = sb_node;
        t_entry.treeitemid = BasicSceneMainFrame::GetInstance()->AppendItem( parent_tree_item, scene_name.c_str(), SPACEBOX_ICON_INDEX );

        BasicSceneMainFrame::GetInstance()->m_spacebox_nodes[t_entry.treeitemid.GetID()] = t_entry;
        BasicSceneMainFrame::GetInstance()->m_tree_nodes[t_entry.treeitemid.GetID()] = sb_node;
        BasicSceneMainFrame::GetInstance()->m_inv_tree_nodes[sb_node] = t_entry.treeitemid.GetID();

        // update passes output queues
        for( std::map<dsstring, SpaceboxPassDescriptor>::iterator it = sb_descr.passes_slots.begin(); it != sb_descr.passes_slots.end(); ++it )
        {
            Pass* current_pass = dynamic_cast<Pass*>( ConfigsBase::GetInstance()->GetConfigurableInstance( it->first ) );
            current_pass->GetRenderingQueue()->UpdateOutputQueue();
        }

        // store spacebox description

        BasicSceneMainFrame::GetInstance()->m_spacebox_descriptors[t_entry.treeitemid.GetID()] = sb_descr;

        //////////////////////////////////////

        dsstring title;
        dsstring* script_text;
        bool * script_state;
        title = "Spacebox node: ";
        title += BasicSceneMainFrame::GetInstance()->m_spacebox_nodes[t_entry.treeitemid.GetID()].name;
        script_text = &BasicSceneMainFrame::GetInstance()->m_spacebox_nodes[t_entry.treeitemid.GetID()].script;
        script_state = &BasicSceneMainFrame::GetInstance()->m_spacebox_nodes[t_entry.treeitemid.GetID()].script_enabled;
        BasicSceneScriptEditFrame* frame = new BasicSceneScriptEditFrame( BasicSceneMainFrame::GetInstance(), title, script_text, script_state );
        BasicSceneMainFrame::GetInstance()->m_script_edit_frames[t_entry.treeitemid.GetID()] = frame;

    }

}