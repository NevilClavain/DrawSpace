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

#include "shaderscontroller.h"



DrawSpace::Core::ShadersController* DrawSpace::Core::ShadersController::m_instance = NULL;

using namespace DrawSpace;
using namespace DrawSpace::Core;


ShadersController::ShadersController( void )
{
}

ShadersController::~ShadersController( void )
{
}

void ShadersController::RegisterRenderingNode( DrawSpace::Core::RenderingNode* p_rnode )
{
    std::map<dsstring, RenderingNode::ShadersParams*> node_shaders_params;
    p_rnode->GetShadersParams( node_shaders_params );

    for( std::map<dsstring, RenderingNode::ShadersParams*>::iterator it = node_shaders_params.begin(); it != node_shaders_params.end(); ++it )
    {
        m_shader_nodes[it->first] = p_rnode;
    }
}

bool ShadersController::Update( const dsstring& p_id, const DrawSpace::Utils::Vector& p_value )
{
    if( m_shader_nodes.count( p_id )> 0 )
    {
        m_shader_nodes[p_id]->SetShaderRealVector( p_id, p_value );
        return true;
    }
    return false;
}


