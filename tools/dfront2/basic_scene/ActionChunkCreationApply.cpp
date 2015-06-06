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

#include "ActionChunkCreationApply.h"
#include "buildobjects.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Dynamics;
using namespace DrawSpace::Utils;

void ActionChunkCreationApply::Execute( BasicSceneObjectPropertiesDialog* p_dialog )
{
    DIALOG_GETGRID
    DIALOG_PROPERTIES_VARS

    DrawSpace::Utils::ChunkDescriptor descr;

    DIALOG_GET_STRING_PROPERTY( "scene name", alias2 )
    DIALOG_WXSTRING_TO_DSSTRING( alias2, alias )

    if( "" == alias )
    {
        wxMessageBox( "'scene name' attribute cannot be void", "DrawFront error", wxICON_ERROR );
        return;
    }

    descr.scene_name = alias;

    DIALOG_GET_ENUM_PROPERTY( "meshe", meshe_name )
    DIALOG_WXSTRING_TO_DSSTRING( meshe_name, meshe_name2 )

    descr.meshe = meshe_name2;

    {
        DIALOG_EXPLORE_NODES_BEGIN( "", "pass %d", i, pass_slot )

            DrawSpace::Utils::ChunkPassDescriptor pass_descr;

            DIALOG_GET_ENUM_PROPERTY( DIALOG_INCREMENT_STRING( pass_slot, "pass" ), pass_name )
            DIALOG_WXSTRING_TO_DSSTRING( pass_name, pass_name2 )

            DIALOG_GET_ENUM_PROPERTY( DIALOG_INCREMENT_STRING( pass_slot, "fx" ), fx_name )
            DIALOG_GET_INT_PROPERTY( DIALOG_INCREMENT_STRING( pass_slot, "rendering order" ), rendering_order )

            dsstring texture_root_name;
            texture_root_name = DIALOG_INCREMENT_STRING( pass_slot, "textures" );

            DIALOG_EXPLORE_NODES_BEGIN( texture_root_name, "stage %d", j, texture_stage )
                DIALOG_GET_ENUM_PROPERTY( texture_stage, texture_name )
                DIALOG_WXSTRING_TO_DSSTRING( texture_name, texture_name2 )

                if( texture_name2 != "..." )
                {
                    pass_descr.textures[j] = texture_name2;
                }
                else
                {
                    pass_descr.textures[j] = "";
                }
            DIALOG_EXPLORE_NODES_END( j )

            pass_descr.fx_name = fx_name;
            pass_descr.rendering_order = rendering_order;          
            descr.passes_slots[pass_name2] = pass_descr;

        DIALOG_EXPLORE_NODES_END( i )
    }

    {
        DIALOG_EXPLORE_NODES_BEGIN( "", "shader params %d", i, sp_slot )

            DIALOG_GET_ENUM_PROPERTY( DIALOG_INCREMENT_STRING( sp_slot, "pass" ), pass_name )
            DIALOG_WXSTRING_TO_DSSTRING( pass_name, pass_name2 )

            DIALOG_GET_INT_PROPERTY( DIALOG_INCREMENT_STRING( sp_slot, "shader index" ), shader_index )
            DIALOG_GET_STRING_PROPERTY( DIALOG_INCREMENT_STRING( sp_slot, "param id" ), param_id )
            DIALOG_GET_INT_PROPERTY( DIALOG_INCREMENT_STRING( sp_slot, "register" ), shader_register )

            DIALOG_GET_FLOAT_PROPERTY( DIALOG_INCREMENT_STRING( sp_slot, "values.x" ), val_x )
            DIALOG_GET_FLOAT_PROPERTY( DIALOG_INCREMENT_STRING( sp_slot, "values.y" ), val_y )
            DIALOG_GET_FLOAT_PROPERTY( DIALOG_INCREMENT_STRING( sp_slot, "values.z" ), val_z )
            DIALOG_GET_FLOAT_PROPERTY( DIALOG_INCREMENT_STRING( sp_slot, "values.w" ), val_w )

            if( descr.passes_slots.count( pass_name2 ) > 0 )
            {
                if( "" == param_id )
                {
                    wxMessageBox( "'param id' shader params attribute cannot be void", "DrawFront error", wxICON_ERROR );
                    return;
                }

                PassShaderParam psp;

                psp.id = param_id;
                psp.shader_index = shader_index;
                psp.shader_register = shader_register;
                psp.value[0] = val_x;
                psp.value[1] = val_y;
                psp.value[2] = val_z;
                psp.value[3] = val_w;

                descr.passes_slots[pass_name2].shader_params.push_back( psp );
            }

        DIALOG_EXPLORE_NODES_END( i )
    }

    dsstring chunk_error;
    Chunk* chunk = BuildChunk( descr, chunk_error );

    if( chunk )
    {
        SceneNode<Chunk>* chunk_node = new SceneNode<Chunk>( alias );
        chunk_node->SetContent( chunk );
        chunk_node->RegisterUpdateBeginEvtHandler( BasicSceneMainFrame::GetInstance()->m_nodeupdatebegin_cb );

        // now we must found the scenenodegraph we belong to make the RegisterNode() call
        void* id = BasicSceneMainFrame::GetInstance()->find_scenenodegraph_id( p_dialog->GetTreeItem() );

        BasicSceneMainFrame::SceneNodeGraphEntry entry;

        entry = BasicSceneMainFrame::GetInstance()->m_scenenodegraphs[id];
        entry.scenenodegraph->RegisterNode( chunk_node );

        // link to the scenegraph hierarchy

        wxTreeItemId current;
        current = p_dialog->GetTreeItem(); //m_last_clicked_treeitem;
        id = current.GetID();

        if( BasicSceneMainFrame::GetInstance()->m_scenenodegraphs.count( id ) > 0 )
        {
            // parent is a scenegraph : use SceneNodeGraph::Add() method
            entry.scenenodegraph->AddNode( chunk_node );
        }
        else
        {
            BaseSceneNode* parent_node = BasicSceneMainFrame::GetInstance()->m_tree_nodes[id];
            chunk_node->LinkTo( parent_node );
        }

        // GUI : add item in the tree

        //wxTreeItemId treeitemid = m_scenegraphs_treeCtrl->AppendItem( p_dialog->GetTreeItem(), alias2, CHUNK_ICON_INDEX );
        //m_scenegraphs_treeCtrl->ExpandAllChildren( p_dialog->GetTreeItem() );

        // record the new chunk node and associated metadata

        BasicSceneMainFrame::SceneNodeEntry<Chunk> c_entry;

        c_entry.name = alias;
        c_entry.scene_node = chunk_node;
        c_entry.treeitemid = BasicSceneMainFrame::GetInstance()->AppendItem( p_dialog->GetTreeItem(), alias2, CHUNK_ICON_INDEX ); //treeitemid;

        BasicSceneMainFrame::GetInstance()->m_chunk_nodes[c_entry.treeitemid.GetID()] = c_entry;

        BasicSceneMainFrame::GetInstance()->m_tree_nodes[c_entry.treeitemid.GetID()] = chunk_node;
        BasicSceneMainFrame::GetInstance()->m_inv_tree_nodes[chunk_node] = c_entry.treeitemid.GetID();

        // update passes output queues
        for( std::map<dsstring, ChunkPassDescriptor>::iterator it = descr.passes_slots.begin(); it != descr.passes_slots.end(); ++it )
        {
            Pass* current_pass = dynamic_cast<Pass*>( ConfigsBase::GetInstance()->GetConfigurableInstance( it->first ) );
            current_pass->GetRenderingQueue()->UpdateOutputQueue();
        }

        // store chunk description

        BasicSceneMainFrame::GetInstance()->m_chunk_descriptors[c_entry.treeitemid.GetID()] = descr;

        /////////////////////////////////////////////////////////////////////////

        dsstring title;
        dsstring* script_text;
        bool * script_state;
        title = "Chunk node: ";
        title += BasicSceneMainFrame::GetInstance()->m_chunk_nodes[c_entry.treeitemid.GetID()].name;
        script_text = &BasicSceneMainFrame::GetInstance()->m_chunk_nodes[c_entry.treeitemid.GetID()].script;
        script_state = &BasicSceneMainFrame::GetInstance()->m_chunk_nodes[c_entry.treeitemid.GetID()].script_enabled;
        BasicSceneScriptEditFrame* frame = new BasicSceneScriptEditFrame( BasicSceneMainFrame::GetInstance(), title, script_text, script_state );
        BasicSceneMainFrame::GetInstance()->m_script_edit_frames[c_entry.treeitemid.GetID()] = frame;

        /////////////////////////////////////////////////////////////////////////

        BasicSceneMainFrame::GetInstance()->m_menubuild_table[c_entry.treeitemid.GetID()] = CHUNK_MASK;

        BasicSceneMainFrame::GetInstance()->m_inv_treeitemid[c_entry.treeitemid.GetID()] = &( BasicSceneMainFrame::GetInstance()->m_chunk_nodes[c_entry.treeitemid.GetID()].treeitemid );

        /////////////////////////////////////////////////////////////////////////

        entry.scenenodegraph->SendNodeAppReadyEvent( chunk_node );
        
        DIALOG_CLOSE

    }
    else
    {
        wxMessageBox( wxString( "chunk creation failure : " ) + wxString( chunk_error.c_str() ) , "DrawFront error", wxICON_ERROR );
    }
}