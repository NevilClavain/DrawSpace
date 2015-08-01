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

#include "ActionChunkEditionDialog.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Dynamics;
using namespace DrawSpace::Utils;

void ActionChunkEditionDialog::Execute( void )
{
    void* id = BasicSceneMainFrame::GetInstance()->m_last_clicked_treeitem.GetID();

    if( BasicSceneMainFrame::GetInstance()->m_chunk_descriptors.count( id ) > 0 )
    {
        BasicSceneMainFrame::SceneNodeEntry<DrawSpace::Chunk> chunk_node = BasicSceneMainFrame::GetInstance()->m_chunk_nodes[id];

        DrawSpace::Utils::ChunkDescriptor chunk_descr = BasicSceneMainFrame::GetInstance()->m_chunk_descriptors[id];

        DIALOG_ACTION_DECLARE( DIALOG_CHUNK_EDITION_TITLE )
       
        DIALOG_APPENDROOT_STRING( "scene name", chunk_descr.scene_name )

        for( std::map<dsstring, ChunkPassDescriptor>::iterator it = chunk_descr.passes_slots.begin(); it != chunk_descr.passes_slots.end(); ++it )
        {
            ChunkPassDescriptor pass_descr = it->second;
            IntermediatePass* ipass = static_cast<IntermediatePass*>( ConfigsBase::GetInstance()->GetConfigurableInstance( it->first ) );

            DrawSpace::Core::RenderingNode* rendering_node = chunk_node.scene_node->GetContent()->GetNodeFromPass( ipass );

            std::map<dsstring, RenderingNode::ShadersParams*> shaders_params_list;
            rendering_node->GetShadersParams( shaders_params_list );

            DIALOG_APPENDROOT_NODE( it->first, pass_root );

            DIALOG_BUILD_LABELS( pass_descr.shader_params.size(), "shader parameter %d", params_list )

            DIALOG_APPENDNODE_ITERATE_NODE_BEGIN( pass_root, i, params_list, param_root )

                
                DIALOG_APPENDNODE_STRING( param_root, "param id", pass_descr.shader_params[i].id );

                DIALOG_APPENDNODE_NODE( param_root, "values", shader_param_values_root )
                    
                    /*
                DIALOG_APPENDNODE_FLOAT( shader_param_values_root, "x", pass_descr.shader_params[i].value[0] )
                DIALOG_APPENDNODE_FLOAT( shader_param_values_root, "y", pass_descr.shader_params[i].value[1] )
                DIALOG_APPENDNODE_FLOAT( shader_param_values_root, "z", pass_descr.shader_params[i].value[2] )
                DIALOG_APPENDNODE_FLOAT( shader_param_values_root, "w", pass_descr.shader_params[i].value[3] )
                    */

                Vector params_value = shaders_params_list[pass_descr.shader_params[i].id]->param_values;

                DIALOG_APPENDNODE_FLOAT( shader_param_values_root, "x", params_value[0] )
                DIALOG_APPENDNODE_FLOAT( shader_param_values_root, "y", params_value[1] )
                DIALOG_APPENDNODE_FLOAT( shader_param_values_root, "z", params_value[2] )
                DIALOG_APPENDNODE_FLOAT( shader_param_values_root, "w", params_value[3] )

               
            DIALOG_APPENDNODE_ITERATE_NODE_END
        }

        DIALOG_APPLY

        DIALOG_SHOW
    }
}