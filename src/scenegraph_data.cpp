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

#include "scenegraph_data.h"

#include "components_ids.h"
#include "meshe_component.h"
#include "renderingnode_component.h"
#include "draw_component.h"


#include "ac3dmeshe.h"

#include "memalloc.h"
#include "exceptions.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;


SceneGraphData::SceneGraphData( void )
{
    m_meshe_import = new DrawSpace::Utils::AC3DMesheImport();
}

SceneGraphData::~SceneGraphData( void )
{
    delete m_meshe_import;
}

void SceneGraphData::initialize_new_entity( Entity* p_entity )
{
    ///////////////////////////////////////////////////////////////////////////////
    MesheComponent* meshe_comp = NULL;
    if( p_entity->CheckComponent( MesheComponentType ) )
    {
        meshe_comp = p_entity->ExtractComponent<MesheComponent>( MesheComponentType, 0 );

        meshe_comp->m_meshe = _DRAWSPACE_NEW_( Meshe, Meshe );
        meshe_comp->m_meshe->SetImporter( m_meshe_import );

        if( meshe_comp->m_filepath != "" )
        {
            if( false == meshe_comp->m_meshe->LoadFromFile( meshe_comp->m_filepath, meshe_comp->m_index_in_file ) )
            {                
                _DSEXCEPTION( "Scenegraph system init phase : cannot load meshe : " + meshe_comp->m_filepath );               
            }
        }     
    }

    ///////////////////////////////////////////////////////////////////////////////

    int nb_rendering_node = p_entity->CheckComponent( RenderingNodeComponentType );
    for( int i = 0; i < nb_rendering_node; i++ )
    {
        RenderingNodeComponent* renderingnode_comp = p_entity->ExtractComponent<RenderingNodeComponent>( RenderingNodeComponentType, i );

        renderingnode_comp->m_rendering_node = _DRAWSPACE_NEW_( RenderingNode, RenderingNode ); 

        if( renderingnode_comp->m_meshe )
        {
            renderingnode_comp->m_rendering_node->SetMeshe( renderingnode_comp->m_meshe->m_meshe );
        }

        if( p_entity->CheckComponent( DrawComponentType ) )
        {
            DrawComponent* draw_comp = p_entity->ExtractComponent<DrawComponent>( DrawComponentType, 0 );

            DrawComponent::Callback* cb = _DRAWSPACE_NEW_( DrawComponent::Callback, DrawComponent::Callback( draw_comp, &DrawComponent::on_renderingnode_draw ) );            
            renderingnode_comp->m_rendering_node->RegisterHandler( cb );
        }

        if( renderingnode_comp->m_queue )
        {
            renderingnode_comp->m_queue->m_queue->Add( renderingnode_comp->m_rendering_node );
        }
    }

    ///////////////////////////////////////////////////////////////////////////////
}

void SceneGraphData::AddRoot( Entity* p_elt )
{
    initialize_new_entity( p_elt );
    EntitySet::AddRoot( p_elt );
}
