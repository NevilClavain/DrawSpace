/* -*-LIC_BEGIN-*- */
/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2021                     
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

#include "lod_drawing.h"
#include "csts.h"
#include "lod_binder.h"
#include "lod_config.h"

#include "entity.h"
#include "renderer.h"
#include "plugin.h"
#include "memalloc.h"
#include "exceptions.h"
#include "maths.h"

#include "transformaspect.h"


using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Utils;
using namespace DrawSpace::Aspect;
using namespace LOD;


FaceDrawingNode::FaceDrawingNode( DrawSpace::Interface::Renderer* p_renderer, Config* p_config, int p_layer_index ) :
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


void FaceDrawingNode::SetCurrentBodyDescription(const dsstring& p_descr)
{
    m_current_body_description = p_descr;
}

void FaceDrawingNode::draw_single_patch( Patch* p_patch, dsreal p_ray, dsreal p_rel_alt, const DrawSpace::Utils::Vector& p_invariant_view_pos,
                                            const DrawSpace::Utils::Matrix& p_world, const DrawSpace::Utils::Matrix& p_view, const DrawSpace::Utils::Matrix& p_proj )
{
    dsreal patch_dim = p_patch->GetUnitSideLenght() / 2.0 * p_ray;
    dsreal patch_scale = 1.5;

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

    if( DRAW_LANDPLACEPATCH_ONLY == m_drawpatch_mode )
    {
        // dilatation des coords uv rel et global suivant le facteur d'echelle du patch landplace
        dsreal middle_x, middle_y;

        middle_x = 0.5 * ( globalrel_uvcoords[2] + globalrel_uvcoords[0] );
        middle_y = 0.5 * ( globalrel_uvcoords[3] + globalrel_uvcoords[1] );

        globalrel_uvcoords[0] = patch_scale * ( globalrel_uvcoords[0] - middle_x ) + middle_x;
        globalrel_uvcoords[2] = patch_scale * ( globalrel_uvcoords[2] - middle_x ) + middle_x;

        globalrel_uvcoords[1] = patch_scale * ( globalrel_uvcoords[1] - middle_y ) + middle_y;
        globalrel_uvcoords[3] = patch_scale * ( globalrel_uvcoords[3] - middle_y ) + middle_y;

        ///////////////////////////////////////////////////////////////////////////////////////////////


        middle_x = 0.5 * ( global_uvcoords[2] + global_uvcoords[0] );
        middle_y = 0.5 * ( global_uvcoords[3] + global_uvcoords[1] );

        global_uvcoords[0] = patch_scale * ( global_uvcoords[0] - middle_x ) + middle_x;
        global_uvcoords[2] = patch_scale * ( global_uvcoords[2] - middle_x ) + middle_x;

        global_uvcoords[1] = patch_scale * ( global_uvcoords[1] - middle_y ) + middle_y;
        global_uvcoords[3] = patch_scale * ( global_uvcoords[3] - middle_y ) + middle_y;
    }

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
    pixels_flags_2[0] = cst::patchHighResolution;

    m_renderer->SetFxShaderParams( 1, 0, pixels_flags );
    m_renderer->SetFxShaderParams( 1, 1, pixels_flags_2 );
    m_renderer->SetFxShaderParams( 1, 2, view_pos );

    Matrix world;
    
    if( DRAW_LANDPLACEPATCH_ONLY == m_drawpatch_mode )
    {
        dsreal rot_phi, rot_theta;

        Vector cube_pos( xp, yp, 1.0, 1.0 );
        Vector sphere_pos;
        Vector spos;

        Maths::CubeToSphere( cube_pos, sphere_pos );

        Maths::CartesiantoSpherical( sphere_pos, spos );

        rot_phi = spos[2];
        rot_theta = spos[1];

        Matrix local_mat;
        Matrix local_scale;
        Matrix local_mat_trans;
        Matrix local_mat_rot_theta;
        Matrix local_mat_rot_phi;
       
        local_scale.Scale( Vector( patch_scale * patch_dim, patch_scale * patch_dim, patch_scale * patch_dim, 1.0 ) );

        switch( p_patch->GetOrientation() )
        {   
            case Patch::FrontPlanetFace:
                
                local_mat_trans.Translation( 0.0, 0.0, p_ray );
                local_mat_rot_theta.Rotation( Vector( 0.0, 1.0, 0.0, 1.0 ), rot_theta );        
                local_mat_rot_phi.Rotation( Vector( -1.0, 0.0, 0.0, 1.0 ), rot_phi );

                break;

            case Patch::RightPlanetFace:
                
                local_mat_trans.Translation( p_ray, 0.0, 0.0 );
                local_mat_rot_theta.Rotation( Vector( 0.0, 1.0, 0.0, 1.0 ), rot_theta );        
                local_mat_rot_phi.Rotation( Vector( 0.0, 0.0, 1.0, 1.0 ), rot_phi );

                break;

            case Patch::LeftPlanetFace:
                
                local_mat_trans.Translation( -p_ray, 0.0, 0.0 );
                local_mat_rot_theta.Rotation( Vector( 0.0, 1.0, 0.0, 1.0 ), rot_theta );        
                local_mat_rot_phi.Rotation( Vector( 0.0, 0.0, -1.0, 1.0 ), rot_phi );
                break;

            case Patch::RearPlanetFace:
                
                local_mat_trans.Translation( 0.0, 0.0, -p_ray );
                local_mat_rot_theta.Rotation( Vector( 0.0, 1.0, 0.0, 1.0 ), rot_theta );        
                local_mat_rot_phi.Rotation( Vector( 1.0, 0.0, 0.0, 1.0 ), rot_phi );

                break;

            case Patch::TopPlanetFace:

                local_mat_trans.Translation( 0.0, p_ray, 0.0 );
                local_mat_rot_theta.Rotation( Vector( 0.0, 0.0, -1.0, 1.0 ), rot_theta );        
                local_mat_rot_phi.Rotation( Vector( -1.0, 0.0, 0.0, 1.0 ), rot_phi );

                break;


            case Patch::BottomPlanetFace:

                local_mat_trans.Translation( 0.0, -p_ray, 0.0 );
                local_mat_rot_theta.Rotation( Vector( 0.0, 0.0, 1.0, 1.0 ), rot_theta );        
                local_mat_rot_phi.Rotation( Vector( -1.0, 0.0, 0.0, 1.0 ), rot_phi );

                break;

        }

        local_mat = local_scale * local_mat_trans * local_mat_rot_phi * local_mat_rot_theta;

        Matrix local_mat_transp = local_mat;
        local_mat_transp.Transpose();

        m_renderer->SetFxShaderMatrix( 0, 29, local_mat_transp );

        world = local_mat * p_world;


        m_renderer->SetFxShaderMatrix( 0, 29, local_mat_transp );
        Matrix world = p_world;
        world.Transpose();
        m_renderer->SetFxShaderMatrix( 0, 34, world );


        //////////////////////////////////////////////////////////////////////////////

        Matrix landplace_normale_mat = local_mat_rot_phi * local_mat_rot_theta * p_world * p_view;
        landplace_normale_mat.ClearTranslation();
        Vector landplace_normale( 0.0, 0.0, 1.0, 1.0 );

        Vector landplace_normale_orient;
        Utils::Maths::VectorPlanetOrientation( p_patch->GetOrientation(), landplace_normale, landplace_normale_orient );

        Vector landplace_normale_t;

        landplace_normale_mat.Transform( &landplace_normale_orient, &landplace_normale_t );

        m_renderer->SetFxShaderParams( 0, 33, landplace_normale_t );

      
    }
    else
    {    
        world = p_world;
    }

    m_renderer->DrawMeshe( world, p_view, p_proj );

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

    int min_lod_level = -1;
        
    for( size_t i = 0; i < m_display_list.size(); i++ )
    {
        int curr_lod_level = m_display_list[i]->GetLodLevel();
        if( -1 == min_lod_level || curr_lod_level < min_lod_level )
        {
            min_lod_level = curr_lod_level;
        }

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
                // dessiner les patchs de niveau LOD 0 qui ne contiennent PAS la camera... et tout les autres
                if( !check_view_in_patch( p_ray, m_relativehotpoint, m_display_list[i] ) && 0 == m_display_list[i]->GetLodLevel() || 0 != m_display_list[i]->GetLodLevel() )
                //if( 0 != m_display_list[i]->GetLodLevel()  )
                {
                    
                    draw_single_patch( m_display_list[i], p_ray, p_rel_alt, p_invariant_view_pos, p_world, p_view, p_proj );
                }
            }
            else if( DRAW_LANDPLACEPATCH_ONLY == m_drawpatch_mode && 0 == m_display_list[i]->GetLodLevel() )
            {
                if( check_view_in_patch( p_ray, m_relativehotpoint, m_display_list[i] ) )
                {

                    m_renderer->ClearDepth();
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

void FaceDrawingNode::SetCurrentPatch( Patch* p_patch )
{
    m_current_patch = p_patch;
}

void FaceDrawingNode::SetBinder( Binder* p_binder )
{
    m_binder = p_binder;

    SetFx( m_binder->GetFx() );
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

Binder* FaceDrawingNode::GetBinder( void )
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

Drawing::Drawing( Config* p_config ) :
m_renderer( NULL ),
m_config( p_config ),
m_owner_entity( NULL )
{
}

Drawing::~Drawing( void )
{
}

void Drawing::Startup( Core::Entity* p_entity )
{
    m_singlenode_draw_handler = _DRAWSPACE_NEW_(RenderingNodeDrawCallback, RenderingNodeDrawCallback(this, &Drawing::on_rendering_singlenode_draw));

    if (m_config->m_landplace_patch)
    {
        create_all_landplace_meshes();
    }

    m_owner_entity = p_entity;
}

void Drawing::Shutdown(void)
{
    if (m_config->m_landplace_patch)
    {
        destroy_all_landplace_meshes();
    }
    _DRAWSPACE_DELETE_(m_singlenode_draw_handler);

    for (size_t i = 0; i < m_drawing_handlers.size(); i++)
    {
        _DRAWSPACE_DELETE_(m_drawing_handlers[i]);
    }

    for (size_t i = 0; i < m_facedrawingnodes.size(); i++)
    {
        _DRAWSPACE_DELETE_(m_facedrawingnodes[i]);
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


void Drawing::AddInRendergraph(const dsstring& p_passname, DrawSpace::Core::RenderingQueue* p_passqueue)
{
    for( auto& e : m_passesnodes )
    {
        if( e.first == p_passname )
        {
            p_passqueue->Add( e.second );
        }
    }
}

void Drawing::RemoveFromRendergraph(const dsstring& p_passname, DrawSpace::Core::RenderingQueue* p_passqueue)
{
    for( auto& e : m_passesnodes )
    {
        if( e.first == p_passname )
        {
            p_passqueue->Remove(e.second);
        }
    }
}




void Drawing::on_renderingnode_draw( RenderingNode* p_rendering_node )
{
    if( 0 == m_planetbodies.size() )
    {
        return;
    }

    DrawSpace::Utils::Matrix view;
    DrawSpace::Utils::Matrix proj;
    DrawSpace::Utils::Matrix world;

    TransformAspect* transform_aspect = m_owner_entity->GetAspect<TransformAspect>();
    if( !transform_aspect )
    {
        _DSEXCEPTION( "Owner entity has no transform aspect!" );
    }

    transform_aspect->GetViewTransform(view);
    transform_aspect->GetProjTransform(proj);
    transform_aspect->GetWorldTransform(world);

    FaceDrawingNode* face_node = static_cast<FaceDrawingNode*>( p_rendering_node );

    std::vector<Patch*> dl;
   
    Body* planetbody = m_planetbodies[face_node->GetLayerIndex()];

    planetbody->GetFace( m_nodes[face_node] )->GetDisplayList( dl );
    Patch* current_patch = planetbody->GetFace( m_nodes[face_node] )->GetCurrentPatch();
 
    face_node->SetCurrentPatch( current_patch );
    face_node->SetDisplayList( dl );
    face_node->SetCurrentBodyDescription( planetbody->GetDescription() );

    Binder* node_binder = face_node->GetBinder();
    node_binder->Bind();
    
    // recup relative alt de la face
    dsreal rel_alt = planetbody->GetFace( m_nodes[face_node] )->GetRelativeAltSphere();

    Vector view_pos;
    planetbody->GetInvariantViewerPos( view_pos );


    Vector hotpoint;
    planetbody->GetFace( m_nodes[face_node] )->GetRelativeHotPoint( hotpoint );
    face_node->UpdateRelativeHotPoint( hotpoint );


    face_node->Draw( planetbody->GetDiameter() / 2.0, rel_alt, view_pos, world, view, proj, true );
    node_binder->Unbind();
}

void Drawing::on_rendering_singlenode_draw( DrawSpace::Core::RenderingNode* p_rendering_node )
{
    if (0 == m_planetbodies.size())
    {
        return;
    }

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

void Drawing::RegisterSinglePassSlot( /*Pass* p_pass,*/ const dsstring& p_pass, Binder* p_binder, int p_orientation, Body::MesheType p_meshe_type, int p_layer_index, int p_rendering_order )
{

    FaceDrawingNode* node = _DRAWSPACE_NEW_( FaceDrawingNode, FaceDrawingNode( m_renderer, m_config, p_layer_index ) );
    node->m_debug_id = "planet main node";
    m_facedrawingnodes.push_back( node );

    FaceDrawingNode* node_skirts = NULL;

    FaceDrawingNode* node_landplace = NULL;

    switch( p_meshe_type )
    {
        case Body::LOWRES_MESHE:

            // node patch terrain
            node->SetMeshe( Body::m_patch_meshe );


            if( m_config->m_landplace_patch )
            {
                node->SetDrawPatchMode( FaceDrawingNode::DRAW_ALL_BUTLANDPLACEPATCH );

                node_landplace = _DRAWSPACE_NEW_( FaceDrawingNode, FaceDrawingNode( m_renderer, m_config, p_layer_index ) );
                node_landplace->m_debug_id = "LOWRES_MESHE_node_landplace";
                node_landplace->SetMeshe( m_landplace_meshes[p_orientation] );
                node_landplace->SetDrawPatchMode( FaceDrawingNode::DRAW_LANDPLACEPATCH_ONLY );

                m_facedrawingnodes.push_back(node_landplace);
            }

            break;

        case Body::AVGRES_MESHE:

            // node patch terrain
            node->SetMeshe( Body::m_patch3_meshe );
            break;

        case Body::LOWRES_SKIRT_MESHE:

            node_skirts = _DRAWSPACE_NEW_( FaceDrawingNode, FaceDrawingNode( m_renderer, m_config, p_layer_index ) );
            node_skirts->m_debug_id = "LOWRES_SKIRT_MESHE_skirts";
            m_facedrawingnodes.push_back( node_skirts );

            // node patch terrain
            node->SetMeshe( Body::m_patch_meshe );

            // plus un node jupes terrain
            node_skirts->SetMeshe( Body::m_skirt_meshe );

            if( m_config->m_landplace_patch )
            {
                node->SetDrawPatchMode( FaceDrawingNode::DRAW_ALL_BUTLANDPLACEPATCH );
                node_skirts->SetDrawPatchMode( FaceDrawingNode::DRAW_ALL_BUTLANDPLACEPATCH );

                node_landplace = _DRAWSPACE_NEW_( FaceDrawingNode, FaceDrawingNode( m_renderer, m_config, p_layer_index ) );
                node_landplace->m_debug_id = "LOWRES_SKIRT_MESHE_node_landplace";
                node_landplace->SetMeshe( m_landplace_meshes[p_orientation] );
                node_landplace->SetDrawPatchMode( FaceDrawingNode::DRAW_LANDPLACEPATCH_ONLY );

                m_facedrawingnodes.push_back(node_landplace);
            }
            break;

        case Body::HIRES_MESHE:

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
      
        std::pair<dsstring, FaceDrawingNode*> p_s = std::make_pair( p_pass, node_skirts );
        m_passesnodes.push_back( p_s );

        m_nodes[node_skirts] = p_orientation;

        node_skirts->SetBinder( p_binder );

        // pour faire le rendu des nodes jupes terrains AVANT le rendu des nodes patch terrains
        node_skirts->SetOrderNumber( p_rendering_order - 1 );
    }

    // enregistrer le node patch terrain
    node->RegisterHandler( cb );
      
    std::pair<dsstring, FaceDrawingNode*> p = std::make_pair( p_pass, node );
    m_passesnodes.push_back( p );

    m_nodes[node] = p_orientation;

    node->SetBinder( p_binder );
    node->SetOrderNumber( p_rendering_order );


    if( node_landplace )
    {
        node_landplace->RegisterHandler( cb );
    
        std::pair<dsstring, FaceDrawingNode*> p_s = std::make_pair( p_pass, node_landplace );
        m_passesnodes.push_back( p_s );

        m_nodes[node_landplace] = p_orientation;

        node_landplace->SetBinder( p_binder );

        // pour faire le rendu landplace terrains APRES le rendu des nodes patch terrains
        node_landplace->SetOrderNumber( p_rendering_order + 1 );
    }
}

Drawing::RenderingNodeDrawCallback* Drawing::GetSingleNodeDrawHandler( void ) const
{
    return m_singlenode_draw_handler;
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

void Drawing::create_landplace_meshe( long p_patch_resol, int p_orientation, DrawSpace::Core::Meshe* p_meshe_dest )
{
    int main_patch_nbv = 0;
    dsreal xcurr, ycurr;
    long patch_resolution = p_patch_resol;

    // on travaille sur une sphere de rayon = 1.0, donc diametre = 2.0
    dsreal interval = 2.0 / ( patch_resolution - 1 );

    float delta_uv0 = 1.0f / ( patch_resolution - 1 );
    float current_u0 = 0.0f;
    float current_v0 = 0.0f;

    for( long i = 0; i < patch_resolution; i++ )
    {
        for( long j = 0; j < patch_resolution; j++ )
        {
            xcurr = j * interval - 1.0;
            ycurr = i * interval - 1.0;

            Vertex vertex;

            Vector v_scaled;
            Vector v_orient;

            Vector v( xcurr, ycurr, 0.0, 1.0 );


            Utils::Maths::VectorPlanetOrientation( p_orientation, v, v_orient );


            vertex.x = v_orient[0];
            vertex.y = v_orient[1];
            vertex.z = v_orient[2];

            vertex.tu[0] = current_u0;
            vertex.tv[0] = 1.0 - current_v0; // coin inferieur gauche de la grille correspond a la coord texture u = 0.0, v = 1.0 !!!!
                                            // le v des coords textures et le y du repere patch sont en sens opposes

            vertex.tw[0] = 0.0;

            p_meshe_dest->AddVertex( vertex );
            main_patch_nbv++;

            current_u0 += delta_uv0;
        }

        current_v0 += delta_uv0;
        current_u0 = 0.0;
    }

    long current_index;

    for( long i = 0; i < patch_resolution - 1; i++  )
    {
        current_index = i * patch_resolution;

        for( long j = 0; j < patch_resolution - 1; j++ )
        {
            Triangle triangle;

            triangle.vertex1 = current_index;
            triangle.vertex2 = current_index + 1;
            triangle.vertex3 = current_index + patch_resolution;
            p_meshe_dest->AddTriangle( triangle );
            
            triangle.vertex1 = current_index + 1;
            triangle.vertex2 = current_index + 1 + patch_resolution;
            triangle.vertex3 = current_index + patch_resolution;
            p_meshe_dest->AddTriangle( triangle );
            
            current_index++;
        }        
    }
}

void Drawing::create_all_landplace_meshes( void )
{
    for( long i = 0; i < 6; i++ )
    {
        m_landplace_meshes[i] = _DRAWSPACE_NEW_( Meshe, Meshe );

        create_landplace_meshe( cst::patchAvgResolution, i, m_landplace_meshes[i] );

        m_landplace_meshes[i]->SetPath( "sphereLOD landplace meshe" );
    }
}


void Drawing::destroy_all_landplace_meshes( void )
{
    for( long i = 0; i < 6; i++ )
    {
        _DRAWSPACE_DELETE_( m_landplace_meshes[i] );
    }
}
