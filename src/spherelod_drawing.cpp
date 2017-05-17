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

#include "renderer.h"
#include "plugin.h"
#include "memalloc.h"
#include "exceptions.h"
#include "spherelod_drawing.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Utils;
using namespace DrawSpace::SphericalLOD;


FaceDrawingNode::FaceDrawingNode( DrawSpace::Interface::Renderer* p_renderer, DrawSpace::SphericalLOD::Config* p_config, int p_layer_index ) :
m_renderer( p_renderer ),
m_current_patch( NULL ),
m_config( p_config ),
m_binder( NULL ),
m_layer_index( p_layer_index ),
m_drawpatch_mode( DRAW_ALL )
{
    ZeroMemory( &m_stats, sizeof( Stats ) );
}

FaceDrawingNode::~FaceDrawingNode( void )
{
}

void FaceDrawingNode::SetDisplayList( const std::vector<Patch*>& p_list )
{
    m_display_list = p_list;
}

void FaceDrawingNode::SetDrawPatchMode( DrawPatchMode p_mode )
{
    m_drawpatch_mode = p_mode;
}

void FaceDrawingNode::draw_single_patch( Patch* p_patch, dsreal p_ray, dsreal p_rel_alt, const DrawSpace::Utils::Vector& p_invariant_view_pos,
                                            const DrawSpace::Utils::Matrix& p_world, const DrawSpace::Utils::Matrix& p_view, const DrawSpace::Utils::Matrix& p_proj )
{
    Vector flag0;
    flag0[0] = p_patch->GetOrientation();
    flag0[1] = p_patch->GetUnitSideLenght();
    flag0[2] = p_ray;
    flag0[3] = p_rel_alt;

    Vector patch_pos;
    dsreal xp, yp;
    p_patch->GetUnitPos( xp, yp );

    patch_pos[0] = xp;
    patch_pos[1] = yp;
    patch_pos[2] = ( DRAW_LANDPLACEPATCH_ONLY == m_drawpatch_mode ? 1.0 : 0.0 );


    Vector globalrel_uvcoords;
    p_patch->GetGlobalRelUVCoords( globalrel_uvcoords );

    Vector global_uvcoords;
    p_patch->GetGlobalUVCoords( global_uvcoords );

    Vector view_pos = p_invariant_view_pos;

    m_renderer->SetFxShaderParams( 0, 24, flag0 );
    m_renderer->SetFxShaderParams( 0, 25, patch_pos );
    m_renderer->SetFxShaderParams( 0, 26, globalrel_uvcoords );
    m_renderer->SetFxShaderParams( 0, 27, global_uvcoords );
    m_renderer->SetFxShaderParams( 0, 28, view_pos );

    Vector pixels_flags;
    pixels_flags[0] = p_rel_alt;
    pixels_flags[1] = p_patch->GetUnitSideLenght();
    pixels_flags[2] = p_ray;
    pixels_flags[3] = p_patch->GetOrientation();

    Vector pixels_flags_2;
    pixels_flags_2[0] = PATCH_HIGH_RESOLUTION;

    m_renderer->SetFxShaderParams( 1, 0, pixels_flags );
    m_renderer->SetFxShaderParams( 1, 1, pixels_flags_2 );
    m_renderer->SetFxShaderParams( 1, 2, view_pos );

    m_renderer->DrawMeshe( p_world, p_view, p_proj );
    //m_stats.nb_patchs++;       
}

void FaceDrawingNode::UpdateRelativeHotPoint( const Utils::Vector p_hotpoint )
{
    m_relativehotpoint = p_hotpoint;
}

void FaceDrawingNode::Draw( dsreal p_ray, dsreal p_rel_alt, const DrawSpace::Utils::Vector& p_invariant_view_pos, 
                            const Matrix& p_world, const DrawSpace::Utils::Matrix& p_view, const Matrix& p_proj, bool p_bind_ht_texture )
{
    //ZeroMemory( &m_stats, sizeof( Stats ) );

    Texture* current_texture = NULL;
        
    for( size_t i = 0; i < m_display_list.size(); i++ )
    {
        Patch* ref_patch = m_display_list[i]->GetTextureReferent();
        Texture* refpatchtexture = ref_patch->GetDataTexture();

        if( p_bind_ht_texture && ( refpatchtexture != current_texture ) )
        {
            m_renderer->SetTexture( refpatchtexture->GetRenderData(), 7 );
            current_texture = refpatchtexture;
        }

        if( DRAW_ALL == m_drawpatch_mode )
        {
            draw_single_patch( m_display_list[i], p_ray, p_rel_alt, p_invariant_view_pos, p_world, p_view, p_proj );
        }
        else
        {
            if( DRAW_ALL_BUTLANDPLACEPATCH == m_drawpatch_mode )
            {
                /*
                if( !check_view_in_patch( p_ray, m_relativehotpoint, m_display_list[i] ) && 0 == m_display_list[i]->GetLodLevel() )
                {
                    // dessiner les patch de niveau LOD 0 qui ne contiennent PAS la camera
                    draw_single_patch( m_display_list[i], p_ray, p_rel_alt, p_invariant_view_pos, p_world, p_view, p_proj );
                }
                */

                if( !check_view_in_patch( p_ray, m_relativehotpoint, m_display_list[i] ) && 0 == m_display_list[i]->GetLodLevel() || 0 != m_display_list[i]->GetLodLevel()  )
                {
                    // dessiner les patchs de niveau LOD 0 qui ne contiennent PAS la camera... et tout les autres
                    draw_single_patch( m_display_list[i], p_ray, p_rel_alt, p_invariant_view_pos, p_world, p_view, p_proj );
                }
            }
            else if( DRAW_LANDPLACEPATCH_ONLY == m_drawpatch_mode && 0 == m_display_list[i]->GetLodLevel() )
            {
                if( check_view_in_patch( p_ray, m_relativehotpoint, m_display_list[i] ) )
                {
                    // dessiner LE patch de niveau LOD 0 qui contient  la camera
                    draw_single_patch( m_display_list[i], p_ray, p_rel_alt, p_invariant_view_pos, p_world, p_view, p_proj );
                }
            }
        }
    }
}

void FaceDrawingNode::GetStats( FaceDrawingNode::Stats& p_stats )
{
    p_stats = m_stats;
}

void FaceDrawingNode::SetCurrentPatch( DrawSpace::SphericalLOD::Patch* p_patch )
{
    m_current_patch = p_patch;
}

void FaceDrawingNode::SetBinder( DrawSpace::SphericalLOD::Binder* p_binder )
{
    m_binder = p_binder;

    SetFx( p_binder->GetFx() );
    for( long i = 0; i < RenderingNode::GetTextureListSize(); i++ )
    {
        Texture* texture = p_binder->GetTexture( i );
        if( texture )
        {
            SetTexture( texture, i );
        }

        Texture* vtexture = p_binder->GetVertexTexture( i );
        if( vtexture )
        {
            SetVertexTexture( vtexture, i );
        }
    }
}

DrawSpace::SphericalLOD::Binder* FaceDrawingNode::GetBinder( void )
{
    return m_binder;
}

int FaceDrawingNode::GetLayerIndex( void )
{
    return m_layer_index;
}

bool FaceDrawingNode::check_view_in_patch( dsreal p_ray, const Utils::Vector& p_view, Patch* p_patch )
{
    Vector viewer;
    Patch::ConvertVectorToFrontFaceCoords( p_patch->GetOrientation(), p_view, viewer );
 
    viewer.Normalize();
    Vector projected_viewer;
    Patch::SphereToCube( viewer, projected_viewer );    
    projected_viewer.Scale( p_ray );

    dsreal patch_xpos, patch_ypos;
    p_patch->GetPos( patch_xpos, patch_ypos );

    dsreal patch_side_size = p_patch->GetSideLength();

    if( ( patch_xpos - ( patch_side_size * 0.5 ) ) <= projected_viewer[0] && ( patch_xpos + ( patch_side_size * 0.5 ) ) >= projected_viewer[0] &&
        ( patch_ypos - ( patch_side_size * 0.5 ) ) <= projected_viewer[1] && ( patch_ypos + ( patch_side_size * 0.5 ) ) >= projected_viewer[1] )
    {

        return true;
    }

    return false;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Drawing::Drawing( SphericalLOD::Config* p_config ) :
m_renderer( NULL ),
m_config( p_config )
{
    m_singlenode_draw_handler = _DRAWSPACE_NEW_( RenderingNodeDrawCallback, RenderingNodeDrawCallback( this, &Drawing::on_rendering_singlenode_draw ) );
}

Drawing::~Drawing( void )
{
    _DRAWSPACE_DELETE_( m_singlenode_draw_handler );

    for( size_t i = 0; i < m_drawing_handlers.size(); i++ )
    {
        _DRAWSPACE_DELETE_( m_drawing_handlers[i] );
    }

    for( size_t i = 0; i < m_facedrawingnodes.size(); i++ )
    {
        _DRAWSPACE_DELETE_( m_facedrawingnodes[i] );
    }
}

void Drawing::SetCurrentPlanetBodies( const std::vector<Body*>& p_planetbodies )
{
    m_planetbodies = p_planetbodies;
}

void Drawing::SetRenderer( DrawSpace::Interface::Renderer* p_renderer )
{
    m_renderer = p_renderer;
}


void Drawing::OnRegister( DrawSpace::Core::SceneNodeGraph* p_scenegraph, DrawSpace::Core::BaseSceneNode* p_node )
{  
    for( auto it = m_passesnodes.begin(); it != m_passesnodes.end(); ++it )
    {
        std::pair<Pass*, FaceDrawingNode*> curr_pair = *it;
        curr_pair.first->GetRenderingQueue()->Add( curr_pair.second );
    }

    m_scenenodegraph = p_scenegraph;
}

void Drawing::OnUnregister( DrawSpace::Core::SceneNodeGraph* p_scenegraph, DrawSpace::Core::BaseSceneNode* p_node )
{
	for (auto it = m_passesnodes.begin(); it != m_passesnodes.end(); ++it)
	{
		std::pair<Pass*, FaceDrawingNode*> curr_pair = *it;
		curr_pair.first->GetRenderingQueue()->Remove( curr_pair.second );
	}

	m_scenenodegraph = NULL;
}

void Drawing::on_renderingnode_draw( RenderingNode* p_rendering_node )
{
    if( 0 == m_planetbodies.size() )
    {
        return;
    }

    DrawSpace::Utils::Matrix view;
    DrawSpace::Utils::Matrix proj;

    if( m_scenenodegraph )
    {
        m_scenenodegraph->GetCurrentCameraView( view );
        m_scenenodegraph->GetCurrentCameraProj( proj );
    }

    FaceDrawingNode* face_node = static_cast<FaceDrawingNode*>( p_rendering_node );

    std::vector<Patch*> dl;
   
    Body* planetbody = m_planetbodies[face_node->GetLayerIndex()];

    planetbody->GetFace( m_nodes[face_node] )->GetDisplayList( dl );
    Patch* current_patch = planetbody->GetFace( m_nodes[face_node] )->GetCurrentPatch();
 
    face_node->SetCurrentPatch( current_patch );
    face_node->SetDisplayList( dl );

    Binder* node_binder = face_node->GetBinder();
    node_binder->Bind();
    
    // recup relative alt de la face
    dsreal rel_alt = planetbody->GetFace( m_nodes[face_node] )->GetRelativeAltSphere();

    Vector view_pos;
    planetbody->GetInvariantViewerPos( view_pos );


    Vector hotpoint;
    planetbody->GetFace( m_nodes[face_node] )->GetRelativeHotPoint( hotpoint );
    face_node->UpdateRelativeHotPoint( hotpoint );


    face_node->Draw( planetbody->GetDiameter() / 2.0, rel_alt, view_pos, m_globaltransformation, view, proj, true );
    node_binder->Unbind();
}

void Drawing::on_rendering_singlenode_draw( DrawSpace::Core::RenderingNode* p_rendering_node )
{
    DrawSpace::Utils::Matrix world;
    DrawSpace::Utils::Matrix view;
    DrawSpace::Utils::Matrix proj;

    world.Translation( 0.0, 0.0, -1.0 );
    view.Identity();
    proj.Perspective( 2.0, 2.0, 1.0, 10.0 );

    FaceDrawingNode* face_node = static_cast<FaceDrawingNode*>( p_rendering_node ); 
    face_node->SetCurrentPatch( NULL );

    Binder* node_binder = face_node->GetBinder();
    node_binder->Bind();

    Body* planetbody = m_planetbodies[face_node->GetLayerIndex()];
    dsreal rel_alt = planetbody->GetFace( m_nodes[face_node] )->GetRelativeAlt();

    Vector view_pos;
    planetbody->GetInvariantViewerPos( view_pos );

    face_node->Draw( 1.0, rel_alt, view_pos, world, view, proj, false );   
    node_binder->Unbind();
}

void Drawing::RegisterSinglePassSlot( Pass* p_pass, SphericalLOD::Binder* p_binder, int p_orientation, 
                                                DrawSpace::SphericalLOD::Body::MesheType p_meshe_type, int p_layer_index, int p_rendering_order )
{

    FaceDrawingNode* node = _DRAWSPACE_NEW_( FaceDrawingNode, FaceDrawingNode( m_renderer, m_config, p_layer_index ) );
    m_facedrawingnodes.push_back( node );

    FaceDrawingNode* node_skirts = NULL;

    FaceDrawingNode* node_landplace = NULL;

    switch( p_meshe_type )
    {
        case SphericalLOD::Body::LOWRES_MESHE:

            // node patch terrain
            node->SetMeshe( Body::m_patch_meshe );
            break;

        case SphericalLOD::Body::AVGRES_MESHE:

            // node patch terrain
            node->SetMeshe( Body::m_patch3_meshe );
            break;

        case SphericalLOD::Body::LOWRES_SKIRT_MESHE:

            node_skirts = _DRAWSPACE_NEW_( FaceDrawingNode, FaceDrawingNode( m_renderer, m_config, p_layer_index ) );
            m_facedrawingnodes.push_back( node_skirts );

            // node patch terrain
            node->SetMeshe( Body::m_patch_meshe );
            node->SetDrawPatchMode( FaceDrawingNode::DRAW_ALL_BUTLANDPLACEPATCH );

            // plus un node jupes terrain
            node_skirts->SetMeshe( Body::m_skirt_meshe );
            node_skirts->SetDrawPatchMode( FaceDrawingNode::DRAW_ALL_BUTLANDPLACEPATCH );


            node_landplace = _DRAWSPACE_NEW_( FaceDrawingNode, FaceDrawingNode( m_renderer, m_config, p_layer_index ) );
            node_landplace->SetMeshe( Body::m_patch_meshe );
            node_landplace->SetDrawPatchMode( FaceDrawingNode::DRAW_LANDPLACEPATCH_ONLY );

            break;

        case SphericalLOD::Body::HIRES_MESHE:

            //node patch terrain
            node->SetMeshe( Body::m_patch2_meshe );
            break;
    }
        
    RenderingNodeDrawCallback* cb = _DRAWSPACE_NEW_( RenderingNodeDrawCallback, RenderingNodeDrawCallback( this, &Drawing::on_renderingnode_draw ) );

    m_drawing_handlers.push_back( cb );

    if( node_skirts )
    {
        // enregistrer le node jupes terrain
        node_skirts->RegisterHandler( cb );
      
        std::pair<Pass*, FaceDrawingNode*> p_s( p_pass, node_skirts );
        m_passesnodes.push_back( p_s );

        m_nodes[node_skirts] = p_orientation;

        node_skirts->SetBinder( p_binder );

        // pour faire le rendu des nodes jupes terrains AVANT le rendu des nodes patch terrains
        node_skirts->SetOrderNumber( p_rendering_order - 1 );
    }

    // enregistrer le node patch terrain
    node->RegisterHandler( cb );
      
    std::pair<Pass*, FaceDrawingNode*> p( p_pass, node );
    m_passesnodes.push_back( p );

    m_nodes[node] = p_orientation;

    node->SetBinder( p_binder );
    node->SetOrderNumber( p_rendering_order );


    if( node_landplace )
    {
        node_landplace->RegisterHandler( cb );
    
        std::pair<Pass*, FaceDrawingNode*> p_s( p_pass, node_landplace );
        m_passesnodes.push_back( p_s );

        m_nodes[node_landplace] = p_orientation;

        node_landplace->SetBinder( p_binder );

        // pour faire le rendu landplace terrains APRES le rendu des nodes patch terrains
        node_landplace->SetOrderNumber( p_rendering_order + 1 );
    }
}

Drawing::RenderingNodeDrawCallback* Drawing::GetSingleNodeDrawHandler( void )
{
    return m_singlenode_draw_handler;
}

void Drawing::SetFinalTransform( const DrawSpace::Utils::Matrix& p_mat )
{
    m_globaltransformation = p_mat;
}

void Drawing::SetLayerNodeDrawingState( int p_layer_index, bool p_drawing_state )
{  
    for( size_t i = 0; i < m_facedrawingnodes.size(); i++ )
    {
        if( p_layer_index == m_facedrawingnodes[i]->GetLayerIndex() )
        {
            m_facedrawingnodes[i]->SetDrawingState( p_drawing_state );
        }
    }
}
