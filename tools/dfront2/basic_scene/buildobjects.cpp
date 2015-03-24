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

#include "buildobjects.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;

Spacebox* BuildSpaceBox( const DrawSpace::Utils::SpaceboxDescriptor& p_descriptor, dsstring& p_error )
{
    Spacebox* spacebox = new Spacebox();
    spacebox->SetSceneName( p_descriptor.scene_name );

    std::map<dsstring, DrawSpace::Utils::PassDescriptor> passes = p_descriptor.passes_slots;

    if( 0 == passes.size() )
    {
        p_error = "at least one pass required";
        return NULL;
    }

    for( std::map<dsstring, DrawSpace::Utils::PassDescriptor>::iterator it = passes.begin(); it != passes.end(); ++it )
    {
        Pass* current_pass = dynamic_cast<Pass*>( ConfigsBase::GetInstance()->GetConfigurableInstance( it->first ) );
        if( !current_pass )
        {
            p_error = "BuildSpaceBox : unknown Pass config name (" + dsstring( it->first.c_str() ) + dsstring( ")" );
            return NULL;
        }

        spacebox->RegisterPassSlot( current_pass );

        if( false == ConfigsBase::GetInstance()->ConfigurableInstanceExists( it->second.fx_name ) )
        {
            p_error = "BuildSpaceBox : unknown Fx config name (" + it->second.fx_name + dsstring( ")" );
            return NULL;
        }

        Fx* fx = dynamic_cast<Fx*>( ConfigsBase::GetInstance()->GetConfigurableInstance( it->second.fx_name ) );

        if( NULL == fx )
        {
            p_error = "BuildSpaceBox : specified config is not an Fx (" + it->second.fx_name + dsstring( ")" );
            return NULL;
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
                        return NULL;
                    }

                    Texture* texture = dynamic_cast<Texture*>( AssetsBase::GetInstance()->GetAsset( it->second.textures[i][j] ) );
                    if( NULL == texture )
                    {
                        p_error = "BuildSpaceBox : specified asset is not a Texture (" + it->second.textures[i][j] + dsstring( ")" );
                        return NULL;
                    }
                                       
                    spacebox->GetNodeFromPass( current_pass, i )->SetTexture( texture, j );

                }
            }

            spacebox->GetNodeFromPass( current_pass, i )->SetFx( fx );
            spacebox->GetNodeFromPass( current_pass, i )->SetOrderNumber( it->second.rendering_order );

            for( size_t j = 0; j < it->second.shader_params.size(); j++ )
            {
                spacebox->GetNodeFromPass( current_pass, i )->AddShaderParameter( it->second.shader_params[j].shader_index, it->second.shader_params[j].id, it->second.shader_params[j].shader_register );
                spacebox->GetNodeFromPass( current_pass, i )->SetShaderRealVector( it->second.shader_params[j].id, it->second.shader_params[j].value );
            }

        }
    }

    p_error = "";
    return spacebox;
}
