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

#include "ActionCloudsCreationApply.h"
#include "buildobjects.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Dynamics;
using namespace DrawSpace::Utils;

void ActionCloudsCreationApply::Execute( BasicSceneObjectPropertiesDialog* p_dialog )
{
    DIALOG_GETGRID
    DIALOG_PROPERTIES_VARS

    DrawSpace::Utils::CloudsDescriptor descr;

    DIALOG_GET_STRING_PROPERTY( "scene name", alias2 )
    DIALOG_WXSTRING_TO_DSSTRING( alias2, alias )

    if( "" == alias )
    {
        wxMessageBox( "'scene name' attribute cannot be void", "DrawFront error", wxICON_ERROR );
        return;
    }

    descr.chunk_descriptor.scene_name = alias;

    DIALOG_GET_FILE_PROPERTY( "procedural rules", rules_filepath2 )
    DIALOG_WXSTRING_TO_DSSTRING( rules_filepath2, rules_filepath )

    if( "" == rules_filepath )
    {
        wxMessageBox( "'procedural rules' attribute cannot be void", "DrawFront error", wxICON_ERROR );
        return;
    }
    descr.rules_filepath = rules_filepath;

    DIALOG_GET_BOOL_PROPERTY( "details", details)
    descr.details = details;

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
            descr.chunk_descriptor.passes_slots[pass_name2] = pass_descr;

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

            if( descr.chunk_descriptor.passes_slots.count( pass_name2 ) > 0 )
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

                descr.chunk_descriptor.passes_slots[pass_name2].shader_params.push_back( psp );
            }

        DIALOG_EXPLORE_NODES_END( i )
    }

    dsstring clouds_error;
    Clouds* clouds = BuildClouds( descr, clouds_error );

    if( clouds )
    {
        SceneNode<Clouds>* clouds_node = new SceneNode<Clouds>( alias );
        clouds_node->SetContent( clouds );

        // now we must found the scenenodegraph we belong to make the RegisterNode() call
        void* id = BasicSceneMainFrame::GetInstance()->find_scenenodegraph_id( p_dialog->GetTreeItem() );

        BasicSceneMainFrame::SceneNodeGraphEntry entry;

        entry = BasicSceneMainFrame::GetInstance()->m_scenenodegraphs[id];
        entry.scenenodegraph->RegisterNode( clouds_node );

        // link to the scenegraph hierarchy

        wxTreeItemId current;
        current = p_dialog->GetTreeItem();
        id = current.GetID();

        if( BasicSceneMainFrame::GetInstance()->m_scenenodegraphs.count( id ) > 0 )
        {
            // parent is a scenegraph : use SceneNodeGraph::Add() method
            entry.scenenodegraph->AddNode( clouds_node );
        }
        else
        {
            BaseSceneNode* parent_node = BasicSceneMainFrame::GetInstance()->m_tree_nodes[id];
            clouds_node->LinkTo( parent_node );
        }

        // GUI : add item in the tree

        // record the new chunk node and associated metadata

        BasicSceneMainFrame::SceneNodeEntry<Clouds> c_entry;

        c_entry.name = alias;
        c_entry.scene_node = clouds_node;
        c_entry.treeitemid = BasicSceneMainFrame::GetInstance()->AppendItem( p_dialog->GetTreeItem(), alias2, CLOUDS_ICON_INDEX );

        BasicSceneMainFrame::GetInstance()->m_clouds_nodes[c_entry.treeitemid.GetID()] = c_entry;

        BasicSceneMainFrame::GetInstance()->m_tree_nodes[c_entry.treeitemid.GetID()] = clouds_node;
        BasicSceneMainFrame::GetInstance()->m_inv_tree_nodes[clouds_node] = c_entry.treeitemid.GetID();

        // update passes output queues
        for( std::map<dsstring, ChunkPassDescriptor>::iterator it = descr.chunk_descriptor.passes_slots.begin(); it != descr.chunk_descriptor.passes_slots.end(); ++it )
        {
            Pass* current_pass = dynamic_cast<Pass*>( ConfigsBase::GetInstance()->GetConfigurableInstance( it->first ) );
            current_pass->GetRenderingQueue()->UpdateOutputQueue();
        }

        // store chunk description

        BasicSceneMainFrame::GetInstance()->m_clouds_descriptors[c_entry.treeitemid.GetID()] = descr;


        /////////////////////////////////////////////////////////////////////////

        BasicSceneMainFrame::GetInstance()->m_menubuild_table[c_entry.treeitemid.GetID()] = CLOUDS_MASK;

        BasicSceneMainFrame::GetInstance()->m_inv_treeitemid[c_entry.treeitemid.GetID()] = &( BasicSceneMainFrame::GetInstance()->m_clouds_nodes[c_entry.treeitemid.GetID()].treeitemid );

        /////////////////////////////////////////////////////////////////////////

        entry.scenenodegraph->SendNodeAppReadyEvent( clouds_node );
        
        DIALOG_CLOSE

    }
    else
    {
        wxMessageBox( wxString( "clouds creation failure : " ) + wxString( clouds_error.c_str() ) , "DrawFront error", wxICON_ERROR );
    }


}