/* -*-LIC_BEGIN-*- */
/*
*
* DrawSpace Rendering engine
* Emmanuel Chaumont Copyright (c) 2013-2023
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

//#include <random>
#include "lod_drawing.h"
#include "csts.h"
#include "lod_binder.h"
#include "lod_config.h"
#include "lod_layer.h"

#include "entity.h"
#include "renderer.h"
#include "plugin.h"
#include "memalloc.h"
#include "exceptions.h"
#include "maths.h"

#include "transformaspect.h"

#include "quaternion.h"
#include "lod_heightmapsubpass.h"

#include "foliage_config.h"



using namespace DrawSpace;
using namespace DrawSpace::Commons;
using namespace DrawSpace::Core;
using namespace DrawSpace::Utils;
using namespace DrawSpace::Maths;
using namespace DrawSpace::Aspect;
using namespace LOD;

DrawSpace::Logger::Sink planetdrawing_logger("PlanetDrawing", DrawSpace::Logger::Configuration::getInstance());


std::map<int, FoliageDrawingNode::CoordsGenerationParams> FoliageDrawingNode::m_seeds;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CollisionMesheDrawingNode::CollisionMesheDrawingNode(DrawSpace::Interface::Renderer* p_renderer):
m_renderer(p_renderer)
{
}

CollisionMesheDrawingNode::~CollisionMesheDrawingNode(void)
{
}

void CollisionMesheDrawingNode::Draw(const DrawSpace::Maths::Matrix& p_world, const DrawSpace::Maths::Matrix& p_view, const DrawSpace::Maths::Matrix& p_proj)
{
    // red color
    m_renderer->SetFxShaderParams(1, 0, Maths::Vector(1.0, 0.0, 0.0, 1.0));
    m_renderer->DrawMeshe(p_world, p_view, p_proj);
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


FaceDrawingNode::FaceDrawingNode( DrawSpace::Interface::Renderer* p_renderer, Config* p_config, int p_layer_index ) :
m_renderer( p_renderer ),
m_config( p_config ),
m_layer_index( p_layer_index )
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

void FaceDrawingNode::SetDrawPatchMode( DrawPatchMode p_mode, int maxlodlevel_to_draw)
{
    m_drawpatch_mode = p_mode;
    m_maxlodlevel_to_draw = maxlodlevel_to_draw;
}


void FaceDrawingNode::SetCurrentBodyDescription(const dsstring& p_descr)
{
    m_current_body_description = p_descr;
}

void FaceDrawingNode::draw_single_patch( Patch* p_patch, dsreal p_ray, dsreal p_rel_alt, const DrawSpace::Maths::Vector& p_invariant_view_pos,
                                            const DrawSpace::Maths::Matrix& p_world, const DrawSpace::Maths::Matrix& p_view, const DrawSpace::Maths::Matrix& p_proj )
{
    const auto patch_dim{ p_patch->GetUnitSideLenght() / 2.0 * p_ray };
    const auto patch_scale{ cst::detailsPatchScaling };

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
    patch_pos[2] = (DrawPatchMode::DRAW_LANDPLACEPATCH_ONLY == m_drawpatch_mode ? 1.0 : 0.0 );


    Vector globalrel_uvcoords;
    p_patch->GetGlobalRelUVCoords( globalrel_uvcoords );

    Vector global_uvcoords;
    p_patch->GetGlobalUVCoords( global_uvcoords );

    if(DrawPatchMode::DRAW_LANDPLACEPATCH_ONLY == m_drawpatch_mode )
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
    

    // uncomment to see current patch
    /*
    if (p_patch == m_current_patch)
    {
        // highlight current patch
        pixels_flags_2[1] = 1.0;
    }
    */
    
    
    
    
    
    
    
    pixels_flags_2[2] = (dsreal)p_patch->GetLodLevel();

    m_renderer->SetFxShaderParams( 1, 0, pixels_flags );
    m_renderer->SetFxShaderParams( 1, 1, pixels_flags_2 );
    m_renderer->SetFxShaderParams( 1, 2, view_pos );

    Matrix world;
    
    if(DrawPatchMode::DRAW_LANDPLACEPATCH_ONLY == m_drawpatch_mode )
    {
        dsreal rot_phi, rot_theta;

        Vector cube_pos( xp, yp, 1.0, 1.0 );
        Vector sphere_pos;
        Vector spos;

        Maths::cubeToSphere( cube_pos, sphere_pos );

        Maths::cartesiantoSpherical( sphere_pos, spos );

        rot_phi = spos[2];
        rot_theta = spos[1];

        Matrix local_mat;
        Matrix local_scale;
        Matrix local_mat_trans;
        Matrix local_mat_rot_theta;
        Matrix local_mat_rot_phi;
       
        local_scale.scale( Vector( patch_scale * patch_dim, patch_scale * patch_dim, patch_scale * patch_dim, 1.0 ) );

        switch( p_patch->GetOrientation() )
        {   
            case Patch::FrontPlanetFace:
                
                local_mat_trans.translation( 0.0, 0.0, p_ray );
                local_mat_rot_theta.rotation( Vector( 0.0, 1.0, 0.0, 1.0 ), rot_theta );        
                local_mat_rot_phi.rotation( Vector( -1.0, 0.0, 0.0, 1.0 ), rot_phi );

                break;

            case Patch::RightPlanetFace:
                
                local_mat_trans.translation( p_ray, 0.0, 0.0 );
                local_mat_rot_theta.rotation( Vector( 0.0, 1.0, 0.0, 1.0 ), rot_theta );        
                local_mat_rot_phi.rotation( Vector( 0.0, 0.0, 1.0, 1.0 ), rot_phi );

                break;

            case Patch::LeftPlanetFace:
                
                local_mat_trans.translation( -p_ray, 0.0, 0.0 );
                local_mat_rot_theta.rotation( Vector( 0.0, 1.0, 0.0, 1.0 ), rot_theta );        
                local_mat_rot_phi.rotation( Vector( 0.0, 0.0, -1.0, 1.0 ), rot_phi );
                break;

            case Patch::RearPlanetFace:
                
                local_mat_trans.translation( 0.0, 0.0, -p_ray );
                local_mat_rot_theta.rotation( Vector( 0.0, 1.0, 0.0, 1.0 ), rot_theta );        
                local_mat_rot_phi.rotation( Vector( 1.0, 0.0, 0.0, 1.0 ), rot_phi );

                break;

            case Patch::TopPlanetFace:

                local_mat_trans.translation( 0.0, p_ray, 0.0 );
                local_mat_rot_theta.rotation( Vector( 0.0, 0.0, -1.0, 1.0 ), rot_theta );        
                local_mat_rot_phi.rotation( Vector( -1.0, 0.0, 0.0, 1.0 ), rot_phi );

                break;


            case Patch::BottomPlanetFace:

                local_mat_trans.translation( 0.0, -p_ray, 0.0 );
                local_mat_rot_theta.rotation( Vector( 0.0, 0.0, 1.0, 1.0 ), rot_theta );        
                local_mat_rot_phi.rotation( Vector( -1.0, 0.0, 0.0, 1.0 ), rot_phi );

                break;

        }

        local_mat = local_scale * local_mat_trans * local_mat_rot_phi * local_mat_rot_theta;

        auto local_mat_transp{ local_mat };
        local_mat_transp.transpose();

        m_renderer->SetFxShaderMatrix( 0, 29, local_mat_transp );

        world = local_mat * p_world;
        
        Matrix world = p_world;
        world.transpose();
        m_renderer->SetFxShaderMatrix( 0, 34, world );


        //////////////////////////////////////////////////////////////////////////////

        auto landplace_normale_mat{ local_mat_rot_phi * local_mat_rot_theta * p_world * p_view };
        landplace_normale_mat.clearTranslation();
        Vector landplace_normale( 0.0, 0.0, 1.0, 1.0 );

        Vector landplace_normale_orient;
        Maths::vectorPlanetOrientation( p_patch->GetOrientation(), landplace_normale, landplace_normale_orient );

        Vector landplace_normale_t;

        landplace_normale_mat.transform( &landplace_normale_orient, &landplace_normale_t );

        m_renderer->SetFxShaderParams( 0, 33, landplace_normale_t );

      
    }
    else
    {    
        world = p_world;
    }

    m_renderer->DrawMeshe( world, p_view, p_proj );

    //m_stats.nb_patchs++;       
}

void FaceDrawingNode::UpdateRelativeHotPoint( const Maths::Vector p_hotpoint )
{
    m_relativehotpoint = p_hotpoint;
}

void FaceDrawingNode::Draw( dsreal p_ray, dsreal p_rel_alt, const DrawSpace::Maths::Vector& p_invariant_view_pos,
                            const Matrix& p_world, const DrawSpace::Maths::Matrix& p_view, const Matrix& p_proj, bool p_bind_ht_texture )
{
    Texture* current_texture{ nullptr };

    int min_lod_level = -1;
        
    for( size_t i = 0; i < m_display_list.size(); i++ )
    {
        const auto curr_lod_level{ m_display_list[i]->GetLodLevel() };
        if( -1 == min_lod_level || curr_lod_level < min_lod_level )
        {
            min_lod_level = curr_lod_level;
        }

        const auto ref_patch{ m_display_list[i]->GetTextureReferent() };
        const auto refpatchtexture{ ref_patch->GetDataTexture() };

        if( p_bind_ht_texture && refpatchtexture && (refpatchtexture != current_texture))
        {
            m_renderer->SetTexture( refpatchtexture->GetRenderData(), 7 );
            current_texture = refpatchtexture;
        }

        //apply IN-renderstate required for current pass...
        if (m_renderstate_per_passes.count(m_current_pass))
        {
            auto& rs_list{ m_renderstate_per_passes.at(m_current_pass) };
            for (auto& rs_pair : rs_list)
            {
                auto rs_in{ rs_pair.first };
                m_renderer->SetRenderState(&rs_in);
            }
        }

        switch (m_drawpatch_mode)
        {
            case DrawPatchMode::DRAW_ALL:

                draw_single_patch(m_display_list[i], p_ray, p_rel_alt, p_invariant_view_pos, p_world, p_view, p_proj);
                break;

            case DrawPatchMode::DRAW_ALL_BUTLANDPLACEPATCH:

                if (!check_view_in_patch(p_ray, m_relativehotpoint, m_display_list[i]) && 0 == m_display_list[i]->GetLodLevel() || 0 != m_display_list[i]->GetLodLevel())
                {
                    draw_single_patch(m_display_list[i], p_ray, p_rel_alt, p_invariant_view_pos, p_world, p_view, p_proj);
                }
                break;

            case DrawPatchMode::DRAW_LANDPLACEPATCH_ONLY:

                if(0 == m_display_list[i]->GetLodLevel())
                {
                    if (check_view_in_patch(p_ray, m_relativehotpoint, m_display_list[i]))
                    {
                        // not needed ?
                        //m_renderer->ClearDepth();

                        // dessiner LE patch de niveau LOD 0 qui contient la camera
                        draw_single_patch(m_display_list[i], p_ray, p_rel_alt, p_invariant_view_pos, p_world, p_view, p_proj);
                    }
                }
                break;

            case DrawPatchMode::DRAW_MAXLODLEVEL:

                int lodlevel{ m_display_list[i]->GetLodLevel() };
                if (lodlevel <= m_maxlodlevel_to_draw)
                {
                    draw_single_patch(m_display_list[i], p_ray, p_rel_alt, p_invariant_view_pos, p_world, p_view, p_proj);
                }
                break;
        }


        //apply OUT-renderstate required for current pass...
        if (m_renderstate_per_passes.count(m_current_pass))
        {
            const auto& rs_list{ m_renderstate_per_passes.at(m_current_pass) };
            for (auto& rs_pair : rs_list)
            {
                auto rs_out{ rs_pair.second };
                m_renderer->SetRenderState(&rs_out);
            }
        }
    }
}

void FaceDrawingNode::GetStats( FaceDrawingNode::Stats& p_stats ) const
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

Binder* FaceDrawingNode::GetBinder( void ) const
{
    return m_binder;
}

int FaceDrawingNode::GetLayerIndex( void ) const
{
    return m_layer_index;
}

bool FaceDrawingNode::check_view_in_patch( dsreal p_ray, const Maths::Vector& p_view, Patch* p_patch )
{
    Vector viewer;
    Patch::ConvertVectorToFrontFaceCoords( p_patch->GetOrientation(), p_view, viewer );
 
    viewer.normalize();
    Vector projected_viewer;
    Patch::SphereToCube( viewer, projected_viewer );    
    projected_viewer.scale( p_ray );

    dsreal patch_xpos, patch_ypos;
    p_patch->GetPos( patch_xpos, patch_ypos );

    const auto patch_side_size{ p_patch->GetSideLength() };

    if( ( patch_xpos - ( patch_side_size * 0.5 ) ) <= projected_viewer[0] && ( patch_xpos + ( patch_side_size * 0.5 ) ) >= projected_viewer[0] &&
        ( patch_ypos - ( patch_side_size * 0.5 ) ) <= projected_viewer[1] && ( patch_ypos + ( patch_side_size * 0.5 ) ) >= projected_viewer[1] )
    {

        return true;
    }

    return false;
}

void FaceDrawingNode::SetRenderStatePerPassTable(const std::map<dsstring, std::vector<std::pair<DrawSpace::Core::RenderState, DrawSpace::Core::RenderState>>>& p_table)
{
    m_renderstate_per_passes = p_table;
}

void FaceDrawingNode::SetCurrentPass(const dsstring& p_pass)
{
    m_current_pass = p_pass;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

FoliageDrawingNode::FoliageDrawingNode(DrawSpace::Interface::Renderer* p_renderer, const FoliageConfig& p_config) :
m_renderer( p_renderer )
{
    SetMeshe(p_config.foliages_meshes);

    m_global_lit = p_config.foliages_global_lits;
    m_detailed_lit = p_config.foliages_detailed_lits;

    const CoordsGenerationParams coords_generation_params
    {
        p_config.nb_poles_min, p_config.nb_poles_max,
        p_config.pole_ray_min, p_config.pole_ray_max,
        p_config.nbpoints_per_pole_min, p_config.nbpoints_per_pole_max
    };

    m_seeds[p_config.foliages_local_seeds] = coords_generation_params;
    m_local_seed = p_config.foliages_local_seeds;

    m_temperature_range_min = p_config.temperature_range_min;
    m_temperature_range_max = p_config.temperature_range_max;

    m_humidity_range_min = p_config.humidity_range_min;
    m_humidity_range_max = p_config.humidity_range_max;

    m_appearance = p_config.appearance;

    m_altitud_max = p_config.altitud_max;
}

void FoliageDrawingNode::SetBinder(Binder* p_binder)
{
    m_binder = p_binder;

    SetFx(m_binder->GetFx());
    for (long i = 0; i < RenderingNode::GetTextureListSize(); i++)
    {
        Texture* texture = p_binder->GetTexture(i);
        if (texture)
        {
            SetTexture(texture, i);
        }

        Texture* vtexture = p_binder->GetVertexTexture(i);
        if (vtexture)
        {
            SetVertexTexture(vtexture, i);
        }
    }
}

Binder* FoliageDrawingNode::GetBinder(void) const
{
    return m_binder;
}

void FoliageDrawingNode::Draw(dsreal p_ray, LOD::Body* p_body, const DrawSpace::Maths::Vector& p_invariant_view_pos,
                                const DrawSpace::Maths::Matrix& p_world, const DrawSpace::Maths::Matrix& p_view, const DrawSpace::Maths::Matrix& p_proj)
{
    const auto current_face{ p_body->GetCurrentFace() };
    if (current_face > -1)
    {
        std::vector<Patch*> dl;
        p_body->GetFace(p_body->GetCurrentFace())->GetDisplayList(dl);

        for (auto e : dl)
        {
            const auto patch_temperature{ e->GetTemperature() };
            const auto patch_humidity{ e->GetHumidity() };

            const bool temperature_in_range{ m_temperature_range_min < patch_temperature && patch_temperature < m_temperature_range_max };
            const bool humidity_in_range{ m_humidity_range_min < patch_humidity && patch_humidity < m_humidity_range_max };

            if (temperature_in_range && humidity_in_range)
            {                       
                draw_foliages_batch_on_patch(e, p_ray, p_invariant_view_pos, p_world, p_view, p_proj);
            }
        }
    }
}


// render many foliage meshe
void FoliageDrawingNode::draw_foliages_batch_on_patch(Patch* p_patch, dsreal p_ray, 
                            const DrawSpace::Maths::Vector& p_invariant_view_pos, const DrawSpace::Maths::Matrix& p_world, const DrawSpace::Maths::Matrix& p_view, const DrawSpace::Maths::Matrix& p_proj)
{
    auto foliage_patch{ p_patch };

    if (foliage_patch->HasHeightMap())
    {
        const auto foliages_coords{ foliage_patch->GetFoliageCoordsList() };

        if (foliages_coords.count(m_local_seed) > 0)
        {
            const auto foliage_coords{ foliages_coords.at(m_local_seed) };
            for (const auto& coords : foliage_coords)
            {
                const auto must_appear{ coords.appearance_threshold > 1.0 - m_appearance };

                if (must_appear) 
                {
                    draw_foliage_on_patch(foliage_patch, p_ray, p_invariant_view_pos, p_world, p_view, p_proj, coords.x, coords.y, coords.orientation);
                }                
            }
        }
    }
}

// render one foliage meshe
void FoliageDrawingNode::draw_foliage_on_patch(Patch* p_patch, dsreal p_ray,
    const DrawSpace::Maths::Vector& p_invariant_view_pos,
    const DrawSpace::Maths::Matrix& p_world, const DrawSpace::Maths::Matrix& p_view, const DrawSpace::Maths::Matrix& p_proj,
    dsreal p_xpos, dsreal p_ypos, dsreal p_orientation)
{
    const dsreal xpos{ p_xpos }; // [-0.5, 0.5 ]
    const dsreal ypos{ p_ypos }; // [-0.5, 0.5 ]

    //// compute height

    const auto hm { p_patch->GetHeightMap() };

    // translate to [0.0, 1.0] range for heighmap access
    //
    const auto x_hm{ (int)((xpos + 0.5) * cst::patchLowResolution) };
    const auto y_hm{ (int)((ypos + 0.5) * cst::patchLowResolution) };
    const auto index_hm{ (cst::patchLowResolution * (cst::patchLowResolution - 1 - y_hm)) + x_hm };

    const auto hm_height{ hm[index_hm] };

    if (hm_height < 0.0)
    {
        return;
    }

    if (m_altitud_max != -1.0 && hm_height > m_altitud_max)
    {
        return;
    }

    ////////////////////

    Vector view_pos = p_invariant_view_pos;

    m_renderer->SetFxShaderParams(0, 28, view_pos);

    dsreal xp, yp;
    p_patch->GetUnitPos(xp, yp);

    const auto patch_unit_sidelenght{ p_patch->GetUnitSideLenght() };
    xp += patch_unit_sidelenght * xpos;
    yp += patch_unit_sidelenght * ypos;

    Vector v1, v2;
    Patch::XYToXYZ(p_patch->GetOrientation(), xp, yp, v1);
    v1[3] = 1.0;

    Patch::CubeToSphere(v1, v2);
    v2[3] = 1.0;

    // final scaling
    v2.scale(p_ray);
    v2.scale(1.0 + (hm_height / p_ray));

    m_renderer->SetFxShaderParams(1, 3, v2);


    // lit model
    // enable global lit, disable detailed lit
    //Vector lit_model(1.0, 0.0, 0.0, 0.0);
    //Vector lit_model(1.0, 1.0, 0.0, 0.0);

    Vector lit_model(m_global_lit, m_detailed_lit, 0.0, 0.0);

    m_renderer->SetFxShaderParams(1, 4, lit_model);

    //////////////////////////////////////////////////////////

    // axe vertical de l'objet, ici c'est l'axe z ! :-p
    Matrix vertical_rot;
    vertical_rot.rotation(Vector(0.0, 0.0, 1.0, 1.0), p_orientation);




    Matrix local_t;
    local_t.translation(v2);

    Maths::Quaternion q;
    q.lookAt(v2, Vector(0.0, 0.0, 0.0, 1.0));

    Matrix local_r;
    q.rotationMatFrom(local_r);

    Matrix world = vertical_rot * local_r * local_t * p_world;
       
    m_renderer->DrawMeshe(world, p_view, p_proj);
}

const std::map<int, FoliageDrawingNode::CoordsGenerationParams>& FoliageDrawingNode::GetLocalSeeds(void)
{
    return m_seeds;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Drawing::Drawing( Config* p_config ) :
m_config( p_config )
{
    m_collisionmeshe.SetPath("Collision display meshe");
}

Drawing::~Drawing( void )
{
}

void Drawing::Startup( Core::Entity* p_entity )
{
    m_singlenode_draw_handler = _DRAWSPACE_NEW_(RenderingNodeDrawCallback, RenderingNodeDrawCallback(this, &Drawing::on_rendering_singlenode_draw));

    m_newcollisionmeshecreation_cb = _DRAWSPACE_NEW_(NewCollisionMesheCreationCb, NewCollisionMesheCreationCb(this, &Drawing::on_new_collisionmeshe_creation));

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

    ////////////////////////////////////////////////////////

    _DRAWSPACE_DELETE_(m_singlenode_draw_handler);

    _DRAWSPACE_DELETE_(m_newcollisionmeshecreation_cb);

    for (size_t i = 0; i < m_drawing_handlers.size(); i++)
    {
        _DRAWSPACE_DELETE_(m_drawing_handlers[i]);
    }

    ////////////////////////////////////////////////////////

    for (auto& e : m_facedrawingnodes)
    {
        _DRAWSPACE_DELETE_(e);
    }

    for (auto& e : m_collisionmeshedrawingnodes)
    {
        _DRAWSPACE_DELETE_(e);
    }

    for (auto& e : m_foliagedrawingnodes)
    {
        _DRAWSPACE_DELETE_(e);
    }
}


void Drawing::SetLayers(const std::vector<LOD::Layer*>& p_layers)
{
    m_layers = p_layers;

    m_planetbodies.clear();
    for (auto e : m_layers)
    {
        m_planetbodies.push_back(e->GetBody());
    }    
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

    for( auto& e : m_passescollisionsdrawingnodes )
    {
        if (e.first == p_passname)
        {
            p_passqueue->Add(e.second);
        }
    }

    for (auto& e : m_passesfoliagenodes)
    {
        if (e.first == p_passname)
        {
            p_passqueue->Add(e.second);
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

    for (auto& e : m_passescollisionsdrawingnodes)
    {
        if (e.first == p_passname)
        {
            p_passqueue->Remove(e.second);
        }
    }

    for (auto& e : m_passesfoliagenodes)
    {
        if (e.first == p_passname)
        {
            p_passqueue->Remove(e.second);
        }
    }
}

void Drawing::on_collisionmeshe_draw(RenderingNode* p_rendering_node)
{
    if (m_collisionmeshe_valid)
    {
        DrawSpace::Maths::Matrix view;
        DrawSpace::Maths::Matrix proj;
        DrawSpace::Maths::Matrix world;

        const auto transform_aspect{ m_owner_entity->GetAspect<TransformAspect>() };
        if (!transform_aspect)
        {
            _DSEXCEPTION("Owner entity has no transform aspect!");
        }

        transform_aspect->GetViewTransform(view);
        transform_aspect->GetProjTransform(proj);
        transform_aspect->GetWorldTransform(world);

        auto collisionmeshe_node{ static_cast<CollisionMesheDrawingNode*>(p_rendering_node) };
        collisionmeshe_node->Draw(world, view, proj);
    }
}


void Drawing::on_renderingnode_draw( RenderingNode* p_rendering_node )
{
    if( 0 == m_planetbodies.size() )
    {
        return;
    }

    DrawSpace::Maths::Matrix view;
    DrawSpace::Maths::Matrix proj;
    DrawSpace::Maths::Matrix world;

    const auto transform_aspect{ m_owner_entity->GetAspect<TransformAspect>() };
    if( !transform_aspect )
    {
        _DSEXCEPTION( "Owner entity has no transform aspect!" );
    }

    transform_aspect->GetViewTransform(view);
    transform_aspect->GetProjTransform(proj);
    transform_aspect->GetWorldTransform(world);

    const auto face_node{ static_cast<FaceDrawingNode*>(p_rendering_node) };

    std::vector<Patch*> dl;
    const auto planetbody{ m_planetbodies[face_node->GetLayerIndex()] };


    const auto current_face_index{ m_nodes[face_node] };
    

    planetbody->GetFace(current_face_index)->GetDisplayList( dl );
    const auto current_patch{ planetbody->GetFace(current_face_index)->GetCurrentPatch() };

    if (m_current_patchs.count(face_node) == 0)
    {
        m_current_patchs[face_node] = current_patch;
    }
    else
    {
        if (m_current_patchs.at(face_node) != current_patch)
        {
            m_current_patchs[face_node] = current_patch;
                                   
        }
    }

    //////////////////////////////////////////
 
    face_node->SetCurrentPatch( current_patch );
    face_node->SetDisplayList( dl );
    face_node->SetCurrentBodyDescription( planetbody->GetDescription() );

    const auto node_binder{ face_node->GetBinder() };

    node_binder->BindToShader();
    
    // recup relative alt de la face
    const auto rel_alt{ planetbody->GetFace(m_nodes[face_node])->GetRelativeAltSphere() };

    Vector view_pos;
    planetbody->GetInvariantViewerPos( view_pos );

    Vector hotpoint;
    planetbody->GetFace( m_nodes[face_node] )->GetRelativeHotPoint( hotpoint );
    face_node->UpdateRelativeHotPoint( hotpoint );

    face_node->Draw( planetbody->GetDiameter() / 2.0, rel_alt, view_pos, world, view, proj, true );
}

// used for subpasses :)
void Drawing::on_rendering_singlenode_draw( DrawSpace::Core::RenderingNode* p_rendering_node )
{
    if (0 == m_planetbodies.size())
    {
        return;
    }

    DrawSpace::Maths::Matrix world;
    DrawSpace::Maths::Matrix view;
    DrawSpace::Maths::Matrix proj;

    world.translation( 0.0, 0.0, -1.0 );
    view.identity();
    proj.perspective( 2.0, 2.0, 1.0, 10.0 );

    auto face_node{ static_cast<FaceDrawingNode*>(p_rendering_node) };
    face_node->SetCurrentPatch( NULL );

    const auto node_binder{ face_node->GetBinder() };

    node_binder->BindToShader();

    const auto planetbody{ m_planetbodies[face_node->GetLayerIndex()] };
    const auto rel_alt{ planetbody->GetFace(m_nodes[face_node])->GetRelativeAlt() };

    Vector view_pos;
    planetbody->GetInvariantViewerPos( view_pos );

    face_node->Draw( 1.0, rel_alt, view_pos, world, view, proj, false );       
}

void Drawing::on_foliagerenderingnode_draw(DrawSpace::Core::RenderingNode* p_rendering_node)
{    
    const auto planetbody{ m_planetbodies.at(cst::SurfaceLayer) };

    DrawSpace::Maths::Matrix world;
    DrawSpace::Maths::Matrix view;
    DrawSpace::Maths::Matrix proj;

    const auto transform_aspect{ m_owner_entity->GetAspect<TransformAspect>() };
    if (!transform_aspect)
    {
        _DSEXCEPTION("Owner entity has no transform aspect!");
    }

    transform_aspect->GetViewTransform(view);
    transform_aspect->GetProjTransform(proj);
    transform_aspect->GetWorldTransform(world);

    const auto foliage_node{ static_cast<FoliageDrawingNode*>(p_rendering_node) };

    const auto node_binder{ foliage_node->GetBinder() };
    node_binder->BindToShader();

    Vector view_pos;
    planetbody->GetInvariantViewerPos(view_pos);



    foliage_node->Draw(planetbody->GetDiameter() / 2.0, planetbody, view_pos, world, view, proj);
}

void Drawing::RegisterSinglePassSlot( const dsstring& p_pass, Binder* p_binder, int p_orientation, Body::MesheType p_meshe_type, int p_layer_index, int p_rendering_order, int maxlodlevel_to_draw)
{

    const auto node{ _DRAWSPACE_NEW_(FaceDrawingNode, FaceDrawingNode(m_renderer, m_config, p_layer_index)) };

    m_facedrawingnodes.push_back( node );

    FaceDrawingNode* node_skirts{ nullptr };
    FaceDrawingNode* node_landplace{ nullptr };

    switch( p_meshe_type )
    {
        case Body::MesheType::LOWRES_MESHE:

            // node patch terrain
            node->SetMeshe( Body::m_patch_meshe );
            node->m_debug_id = "LOWRES_MESHE for layer : " + std::to_string(p_layer_index);

            if( m_config->m_landplace_patch && p_layer_index == LOD::cst::SurfaceLayer)
            {
                node->SetDrawPatchMode( FaceDrawingNode::DrawPatchMode::DRAW_ALL_BUTLANDPLACEPATCH );

                node_landplace = _DRAWSPACE_NEW_( FaceDrawingNode, FaceDrawingNode( m_renderer, m_config, p_layer_index ) );
                node_landplace->m_debug_id = "LOWRES_MESHE_node_landplace for layer : " + std::to_string(p_layer_index);
                node_landplace->SetMeshe( m_landplace_meshes[p_orientation] );
                node_landplace->SetDrawPatchMode( FaceDrawingNode::DrawPatchMode::DRAW_LANDPLACEPATCH_ONLY );

                m_facedrawingnodes.push_back(node_landplace);
            }
            break;

        case Body::MesheType::AVGRES_MESHE:
           
            // node patch terrain
            node->SetMeshe( Body::m_patch3_meshe );
            node->m_debug_id = "AVGRES_MESHE for layer : " + std::to_string(p_layer_index);

            break;

        case Body::MesheType::LOWRES_SKIRT_MESHE:

            node_skirts = _DRAWSPACE_NEW_( FaceDrawingNode, FaceDrawingNode( m_renderer, m_config, p_layer_index ) );
            node_skirts->m_debug_id = "LOWRES_SKIRT_MESHE_skirts for layer : " + std::to_string(p_layer_index);
            m_facedrawingnodes.push_back( node_skirts );

            // node patch terrain
            node->SetMeshe( Body::m_patch_meshe );
            node->m_debug_id = "LOWRES_SKIRT_MESHE for layer : " + std::to_string(p_layer_index);

            // plus un node jupes terrain
            node_skirts->SetMeshe( Body::m_skirt_meshe );

            if( m_config->m_landplace_patch && p_layer_index == LOD::cst::SurfaceLayer)
            {
                node->SetDrawPatchMode( FaceDrawingNode::DrawPatchMode::DRAW_ALL_BUTLANDPLACEPATCH );
                node_skirts->SetDrawPatchMode( FaceDrawingNode::DrawPatchMode::DRAW_ALL_BUTLANDPLACEPATCH );

                node_landplace = _DRAWSPACE_NEW_( FaceDrawingNode, FaceDrawingNode( m_renderer, m_config, p_layer_index ) );
                node_landplace->m_debug_id = "LOWRES_SKIRT_MESHE_node_landplace";
                node_landplace->SetMeshe( m_landplace_meshes[p_orientation] );
                node_landplace->SetDrawPatchMode( FaceDrawingNode::DrawPatchMode::DRAW_LANDPLACEPATCH_ONLY );

                m_facedrawingnodes.push_back(node_landplace);
            }
            break;

        case Body::MesheType::HIRES_MESHE:

            //node patch terrain
            node->SetMeshe( Body::m_patch2_meshe );
            node->m_debug_id = "HIRES_MESHE for layer : " + std::to_string(p_layer_index);
            break;
    }

    if (maxlodlevel_to_draw > -1)
    {
        node->SetDrawPatchMode(FaceDrawingNode::DrawPatchMode::DRAW_MAXLODLEVEL, maxlodlevel_to_draw);
    }
        
    const auto cb{ _DRAWSPACE_NEW_(RenderingNodeDrawCallback, RenderingNodeDrawCallback(this, &Drawing::on_renderingnode_draw)) };
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
      
    const auto p{ std::make_pair(p_pass, node) };
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

void Drawing::RegisterSinglePassSlotForCollisionDisplay(const dsstring& p_pass, DrawSpace::Core::Fx* p_fx, long p_rendering_order)
{
    auto node{ _DRAWSPACE_NEW_(CollisionMesheDrawingNode, CollisionMesheDrawingNode(m_renderer)) };

    node->SetFx(p_fx);
    node->m_debug_id = "COLLISIONDISPLAY_MESHE";

    // we cannot create a void meshe, so set whatever meshes in it
    // this meshe will be updated later, when collision meshe will be generated from Layer 0 (Drawing::on_new_collisionmeshe_creation callback)
    create_collision_meshe_from(*(LOD::Body::GetPatcheMeshe()));

    node->SetMeshe(&m_collisionmeshe);

    node->SetOrderNumber(p_rendering_order);

    auto cb{ _DRAWSPACE_NEW_(RenderingNodeDrawCallback, RenderingNodeDrawCallback(this, &Drawing::on_collisionmeshe_draw)) };

    node->RegisterHandler(cb);

    m_drawing_handlers.push_back(cb);

    auto p{ std::make_pair(p_pass, node) };
    m_passescollisionsdrawingnodes.push_back(p);
   
    m_collisionmeshedrawingnodes.push_back(node);
}


void Drawing::RegisterFoliageSinglePassSlot(const dsstring& p_pass, Binder* p_binder, int p_ro, int p_foliage_layer, const FoliageConfig& p_foliage_config)
{
    const auto node{ _DRAWSPACE_NEW_(FoliageDrawingNode, FoliageDrawingNode(m_renderer, p_foliage_config)) };
    node->m_debug_id = dsstring("Foliage") + std::to_string(p_foliage_layer);

    const auto cb{ _DRAWSPACE_NEW_(RenderingNodeDrawCallback, RenderingNodeDrawCallback(this, &Drawing::on_foliagerenderingnode_draw)) };
    m_drawing_handlers.push_back(cb);

    node->RegisterHandler(cb);
    node->SetOrderNumber(p_ro);
    node->SetBinder(p_binder);
    
    const auto p{ std::make_pair(p_pass, node) };
    m_passesfoliagenodes.push_back(p);

    m_foliagedrawingnodes.push_back(node);
}

void Drawing::on_new_collisionmeshe_creation(const DrawSpace::Core::Meshe& p_meshe)
{
    // update meshe with new vertices/triangles
    create_collision_meshe_from(p_meshe);

    //and then update in renderer side
    m_collisionmeshe.UpdateIndexes();
    m_collisionmeshe.UpdateVertices();
}


Drawing::RenderingNodeDrawCallback* Drawing::GetSingleNodeDrawHandler( void ) const
{
    return m_singlenode_draw_handler;
}

void Drawing::SetLayerNodeDrawingState( int p_layer_index, bool p_drawing_state )
{  
    for (auto& e : m_facedrawingnodes)
    {
        if( p_layer_index == e->GetLayerIndex() )
        {
            e->SetDrawingState( p_drawing_state );
        }
    }
}

void Drawing::SetRenderStatePerPassTableForLayer(int p_layer_index, const std::map<dsstring, std::vector<std::pair<DrawSpace::Core::RenderState, DrawSpace::Core::RenderState>>>& p_table)
{
    for (auto& e : m_facedrawingnodes)
    {
        if (p_layer_index == e->GetLayerIndex())
        {
            e->SetRenderStatePerPassTable(p_table);
        }
    }
}

void Drawing::create_collision_meshe_from(const DrawSpace::Core::Meshe& p_src_meshe)
{
    m_collisionmeshe.ClearVertices();
    m_collisionmeshe.ClearTriangles();

    auto v_list{ p_src_meshe.GetVertices() };
    auto t_list{ p_src_meshe.GetTriangles() };

    for (auto& e : v_list)
    {
        m_collisionmeshe.AddVertex(e);
    }
    for (auto& e : t_list)
    {
        m_collisionmeshe.AddTriangle(e);
    }

    m_collisionmeshe_valid = true;
}


void Drawing::create_landplace_meshe( long p_patch_resol, int p_orientation, DrawSpace::Core::Meshe* p_meshe_dest )
{
    int main_patch_nbv = 0;
    dsreal xcurr, ycurr;
    const auto patch_resolution{ p_patch_resol };

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


            Maths::vectorPlanetOrientation( p_orientation, v, v_orient );


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
            TrianglePrimitive<unsigned int> triangle;

            triangle.at(0) = current_index;
            triangle.at(1) = current_index + 1;
            triangle.at(2) = current_index + patch_resolution;
            p_meshe_dest->AddTriangle( triangle );
            
            triangle.at(0) = current_index + 1;
            triangle.at(1) = current_index + 1 + patch_resolution;
            triangle.at(2) = current_index + patch_resolution;
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

        create_landplace_meshe( /*cst::patchAvgResolution*/ cst::patchResolution, i, m_landplace_meshes[i]);

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

Drawing::NewCollisionMesheCreationCb* Drawing::GetNewCollisionMesheCreationCb(void) const
{
    return m_newcollisionmeshecreation_cb;
}

void Drawing::ResetCollisionMesheValidity(void)
{
    m_collisionmeshe_valid = false;
}

void Drawing::SetCurrentPass(const dsstring& p_pass)
{
    for (auto& e : m_facedrawingnodes)
    {
        e->SetCurrentPass(p_pass);
    }
}

std::vector<std::pair<dsstring, FaceDrawingNode*>> Drawing::GetFaceDrawingNode(void) const
{
    return m_passesnodes;
}
