/* -*-LIC_BEGIN-*- */
/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2019                     
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
/* -*-LIC_END-*- */

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

#include "spherelod_layer.h"
#include "spherelod_collisions.h"

using namespace DrawSpace;
using namespace DrawSpace::SphericalLOD;
using namespace DrawSpace::Core;


Collisions::Collisions( Layer* p_owner, DrawSpace::SphericalLOD::Config* p_config, int p_orientation, int p_node_layer_index ) :
m_layer( p_owner ),
m_collidingheightmap_texture( NULL ),
m_collidingheightmap_content( NULL ),
m_enable( true )
{
    m_collidingheightmap_pass = create_colliding_heightmap_pass();

    // creation/preparation du node

    DrawSpace::Interface::Renderer* renderer = SingletonPlugin<DrawSpace::Interface::Renderer>::GetInstance()->m_interface;
    FaceDrawingNode* node = _DRAWSPACE_NEW_( FaceDrawingNode, FaceDrawingNode( renderer, p_config, p_node_layer_index ) );
                
    node->SetMeshe( SphericalLOD::Body::m_patch_meshe );

    //node->SetBinder( p_config->m_groundCollisionsBinder[p_orientation] );
    node->SetBinder( p_config->m_layers_descr[p_node_layer_index].groundCollisionsBinder[p_orientation] );

    void* tx_data;
    if( false == renderer->CreateTexture( m_collidingheightmap_pass->GetTargetTexture(), &tx_data ) )
    {
        _DSEXCEPTION( "failed to create subpasstarget texture in renderer" );
    }
    
    m_collidingheightmap_pass->GetTargetTexture()->AllocTextureContent();

    ////////////////////////

    m_subpass = m_collidingheightmap_pass;
    m_subpass_node = node;


    std::vector<DrawSpace::SphericalLOD::Patch*> dl;
    // appel handler pour enregistrer et executer la passe

    Layer::SubPassCreationHandler* handler = p_owner->GetSubPassCreationHandler();

    if( handler )
    {
        (*handler)( this, 2 );

        m_collidingheightmap_texture = m_collidingheightmap_pass->GetTargetTexture();
        m_collidingheightmap_content = m_collidingheightmap_texture->GetTextureContentPtr();
    }
}

Collisions::~Collisions( void )
{
    _DRAWSPACE_DELETE_( m_subpass );
    _DRAWSPACE_DELETE_( m_subpass_node );
}

DrawSpace::IntermediatePass* Collisions::create_colliding_heightmap_pass( void )
{
    char thisname[32];
    sprintf( thisname, "layer_%x", this );

    dsstring complete_name = dsstring( thisname ) + dsstring( "_collisionheightmap_pass" );
    IntermediatePass* ipass = _DRAWSPACE_NEW_( IntermediatePass, IntermediatePass( complete_name ) );

    ipass->SetTargetDimsFromRenderer( false );    
    ipass->SetTargetDims( PATCH_RESOLUTION, PATCH_RESOLUTION );
    ipass->SetRenderPurpose( Texture::RENDERPURPOSE_FLOAT32 );

    ipass->Initialize();
    ipass->GetRenderingQueue()->EnableDepthClearing( true );
    ipass->GetRenderingQueue()->EnableTargetClearing( true );
    ipass->GetRenderingQueue()->SetTargetClearingColor( 0, 0, 0, 255 );

    return ipass;
}

void Collisions::DrawSubPass( void )
{
    if( m_enable )
    {
        SubPass::DrawSubPass();
    }
}

void Collisions::SubPassDone( void )
{
    if( m_enable )
    {
        m_layer->SubPassDone( this );
    }
}

void Collisions::Enable( void )
{
    m_enable = true;
}

void Collisions::Disable( void )
{
    m_enable = false;
}

DrawSpace::Core::Texture* Collisions::GetHMTexture( void )
{
    return m_collidingheightmap_texture;
}

void* Collisions::GetHMTextureContent( void )
{
    return m_collidingheightmap_content;
}
