/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2014                                
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

#include "buildobjects.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;

bool BuildSpaceBox( const DrawSpace::Spacebox::Descriptor& p_descriptor, BasicSceneMainFrame::MetadataScenegraphEntry& p_entry, dsstring& p_error )
{
    Spacebox* spacebox = new Spacebox();
    spacebox->SetSceneName( p_descriptor.scene_name );

    std::map<dsstring, Spacebox::PassDescriptor> passes = p_descriptor.passes_slots;

    for( std::map<dsstring, Spacebox::PassDescriptor>::iterator it = passes.begin(); it != passes.end(); ++it )
    {
        spacebox->RegisterPassSlot( it->first.c_str() );

        if( false == ConfigsBase::GetInstance()->ConfigurableInstanceExists( it->second.fx_name ) )
        {
            p_error = "BuildSpaceBox : unknown Fx config name (" + it->second.fx_name + dsstring( ")" );
            return false;
        }

        Fx* fx = dynamic_cast<Fx*>( ConfigsBase::GetInstance()->GetConfigurableInstance( it->second.fx_name ) );

        if( NULL == fx )
        {
            p_error = "BuildSpaceBox : specified config is not an Fx (" + it->second.fx_name + dsstring( ")" );
            return false;
        }

        for( long i = 0; i < 6; i++ )
        {
            for( long j = 0; j < RenderingNode::NbMaxTextures; j++ )
            {
                if( it->second.textures[i][j] != "" )
                {
                    if( false == AssetsBase::GetInstance()->AssetIdExists( it->second.textures[i][j] ) )
                    {
                        p_error = "BuildSpaceBox : unknown texture asset name (" + it->second.textures[i][j] + dsstring( ")" );
                        return false;
                    }

                    Texture* texture = dynamic_cast<Texture*>( AssetsBase::GetInstance()->GetAsset( it->second.textures[i][j] ) );
                    if( NULL == texture )
                    {
                        p_error = "BuildSpaceBox : specified asset is not a Texture (" + it->second.textures[i][j] + dsstring( ")" );
                        return false;
                    }

                    spacebox->GetNodeFromPass( it->first, i )->SetTexture( texture, j );
                }
            }

            spacebox->GetNodeFromPass( it->first, i )->SetFx( fx );
            spacebox->GetNodeFromPass( it->first, i )->SetOrderNumber( it->second.rendering_order );
        }
    }

    p_entry.node = spacebox;
    p_entry.transformation_source_type = BasicSceneMainFrame::TRANSFORMATIONSOURCE_MATRIXSTACK;
    p_entry.propose_matrixstack = true;
    p_entry.propose_body = false;
    p_entry.propose_movement = false;
    
    BasicSceneMainFrame::TransformationMatrixDescriptor descr;

    descr.ope = BasicSceneMainFrame::TRANSFORMATIONMATRIX_SCALE;

    descr.arg.translation_vals_link[0].value = 0.0;
    descr.arg.translation_vals_link[0].var_alias = "...";
    descr.arg.translation_vals_link[1].value = 0.0;
    descr.arg.translation_vals_link[1].var_alias = "...";
    descr.arg.translation_vals_link[2].value = 0.0;
    descr.arg.translation_vals_link[2].var_alias = "...";

    descr.arg.rotation_vals_link[0].value = 0.0;
    descr.arg.rotation_vals_link[0].var_alias = "...";
    descr.arg.rotation_vals_link[1].value = 0.0;
    descr.arg.rotation_vals_link[1].var_alias = "...";
    descr.arg.rotation_vals_link[2].value = 0.0;
    descr.arg.rotation_vals_link[2].var_alias = "...";

    descr.arg.angle_val_link.value = 0.0;
    descr.arg.angle_val_link.var_alias = "...";

    descr.arg.scale_vals_link[0].value = 20.0;
    descr.arg.scale_vals_link[0].var_alias = "...";
    descr.arg.scale_vals_link[1].value = 20.0;
    descr.arg.scale_vals_link[1].var_alias = "...";
    descr.arg.scale_vals_link[2].value = 20.0;
    descr.arg.scale_vals_link[2].var_alias = "...";
    
    p_entry.matrix_stack_descr.push_back( descr );
    
    p_entry.node = spacebox;

    p_error = "";
    return true;
}