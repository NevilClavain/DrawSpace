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

#include <random>
#include "lod_patch.h"
#include "lod_heightmapsubpass.h"
#include "csts.h"
#include "lod_drawing.h"
#include "lod_config.h"
#include "renderer.h"
#include "plugin.h"
#include "maths.h"
#include "exceptions.h"


using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Utils;
using namespace LOD;

Patch::Patch( dsreal p_ray, int p_orientation, Patch* p_parent, int p_nodeid, BaseQuadtreeNode* p_owner, 
                SubPass::SubPassCreationHandler* p_handler, 
                Config* p_config, int p_layer_index, int p_nbLODRanges ) : 

m_orientation( p_orientation ),
m_ray( p_ray ),
m_owner( p_owner ),
m_datatexture_pass( NULL ),
m_config( p_config ),
m_subpasscreation_handler( p_handler ),
m_parent( p_parent ),
m_nodeid( p_nodeid ),
//m_subpass_entry_infos_valid( false ),
m_nbLODRanges( p_nbLODRanges ),
m_layer_index( p_layer_index )
/*,
m_subpassDoneCb(this, &Patch::on_subpassdone),
m_subpassAbortedCb(this, &Patch::on_subpassaborted)
*/
{
    m_enable_datatexture = m_config->m_layers_descr[p_layer_index].enable_datatextures;
    m_enable_foliage = m_config->m_layers_descr[p_layer_index].enable_foliage;

    for( long i = 0; i < 8; i++ )
    {
        m_neighbours[i] = NULL;
    }

    dsreal ui1, vi1, ui2, vi2;

    if( NULL == p_parent )
    {
        m_lod_level = m_nbLODRanges - 1;
        m_xpos = m_ypos = 0.0;
        m_sidelength = 2.0;    // on travaille sur une sphere de rayon = 1.0, donc diametre = 2.0
    }
    else
    {
        m_lod_level = p_parent->m_lod_level - 1;
        m_sidelength = p_parent->m_sidelength / 2.0;

        switch( p_nodeid )
        {
            case BaseQuadtreeNode::NorthWestNode:

                m_xpos = -p_parent->m_sidelength / 4.0;
                m_xpos += p_parent->m_xpos;
                m_ypos = p_parent->m_sidelength / 4.0;
                m_ypos += p_parent->m_ypos;

                ui1 = 0.0;
                vi1 = 0.0;
                ui2 = 0.5;
                vi2 = 0.5;

                break;

            case BaseQuadtreeNode::NorthEastNode:

                m_xpos = p_parent->m_sidelength / 4.0;
                m_xpos += p_parent->m_xpos;
                m_ypos = p_parent->m_sidelength / 4.0;
                m_ypos += p_parent->m_ypos;

                ui1 = 0.5;
                vi1 = 0.0;
                ui2 = 1.0;
                vi2 = 0.5;

                break;

            case BaseQuadtreeNode::SouthEastNode:

                m_xpos = p_parent->m_sidelength / 4.0;
                m_xpos += p_parent->m_xpos;
                m_ypos = -p_parent->m_sidelength / 4.0;
                m_ypos += p_parent->m_ypos;

                ui1 = 0.5;
                vi1 = 0.5;
                ui2 = 1.0;
                vi2 = 1.0;

                break;

            case BaseQuadtreeNode::SouthWestNode:

                m_xpos = -p_parent->m_sidelength / 4.0;
                m_xpos += p_parent->m_xpos;
                m_ypos = -p_parent->m_sidelength / 4.0;
                m_ypos += p_parent->m_ypos;

                ui1 = 0.0;
                vi1 = 0.5;
                ui2 = 0.5;
                vi2 = 1.0;

                break;

            default:
                m_xpos = 0.0; m_ypos = 0.0;
                break;
        }       
    }

    /////////////////////////////////////////////////////

    if (cst::FoliageRootLODLevel == m_lod_level)
    {
        // generate coords list for foliage

        const auto seed{ 144 };
        std::default_random_engine rand_engine(seed);
        std::uniform_real_distribution<dsreal> rand_source(-0.5, 0.5);

        for (int i = 0; i < 100; i++)
        {
            const auto xp{ rand_source(rand_engine) };
            const auto yp{ rand_source(rand_engine) };

            m_foliagesCoordinates.push_back({ xp, yp });
        }
    }
    else if (cst::FoliageRootLODLevel > m_lod_level)
    {
        // take some foliage coords from parent and convert in local space
        
        dsreal local_center_x;
        dsreal local_center_y;

        for (const auto &coord : p_parent->m_foliagesCoordinates)
        {
            switch (p_nodeid)
            {
                case BaseQuadtreeNode::NorthWestNode:

                    local_center_x = -0.25;
                    local_center_y = 0.25;
                    break;

                case BaseQuadtreeNode::NorthEastNode:

                    local_center_x = 0.25;
                    local_center_y = 0.25;
                    break;

                case BaseQuadtreeNode::SouthEastNode:

                    local_center_x = 0.25;
                    local_center_y = -0.25;
                    break;

                case BaseQuadtreeNode::SouthWestNode:

                    local_center_x = -0.25;
                    local_center_y = -0.25;
                    break;
            }

            const FoliagesCoordinates local_coords{ (coord.x - local_center_x) / 0.5, (coord.y - local_center_y) / 0.5 };

            if (-0.5 < local_coords.x && local_coords.x <= 0.5 && -0.5 < local_coords.y && local_coords.y <= 0.5)
            {
                m_foliagesCoordinates.push_back(local_coords);
            }
        }
    }

    /////////////////////////////////////////////////////
    bool register_subpass{ false };
    SubPass::Destination subpass_dest;

    if (m_nbLODRanges - 1 == m_lod_level)
    {
        subpass_dest = SubPass::Destination::IMMEDIATE_SINGLE_SUBPASS;
    }
    else if (m_lod_level >= m_nbLODRanges - 8)
    {
        subpass_dest = SubPass::Destination::DELAYED_SINGLE_SUBPASS;
    }

    if( m_enable_datatexture )
    {
        if( m_nbLODRanges - 1 == m_lod_level )
        {
            prepare_data_texture( p_layer_index);
            register_subpass = true;
        }
        else if( m_lod_level >= m_nbLODRanges - 8 )
        {
            prepare_data_texture( p_layer_index);
            register_subpass = true;
        }
    }

    if (register_subpass && m_subpasscreation_handler)
    {
        m_subpass_entry_infos_list.push_back((*m_subpasscreation_handler)(this, subpass_dest));
    }

    if( p_parent )
    {
        m_texture_referent = p_parent->m_texture_referent;

        m_global_ref_u1 = ( ui1 * ( p_parent->m_global_ref_u2 - p_parent->m_global_ref_u1 ) ) + p_parent->m_global_ref_u1;
        m_global_ref_v1 = ( vi1 * ( p_parent->m_global_ref_v2 - p_parent->m_global_ref_v1 ) ) + p_parent->m_global_ref_v1;

        m_global_ref_u2 = ( ui2 * ( p_parent->m_global_ref_u2 - p_parent->m_global_ref_u1 ) ) + p_parent->m_global_ref_u1;
        m_global_ref_v2 = ( vi2 * ( p_parent->m_global_ref_v2 - p_parent->m_global_ref_v1 ) ) + p_parent->m_global_ref_v1;


        m_global_u1 = ( ui1 * ( p_parent->m_global_u2 - p_parent->m_global_u1 ) ) + p_parent->m_global_u1;
        m_global_v1 = ( vi1 * ( p_parent->m_global_v2 - p_parent->m_global_v1 ) ) + p_parent->m_global_v1;

        m_global_u2 = ( ui2 * ( p_parent->m_global_u2 - p_parent->m_global_u1 ) ) + p_parent->m_global_u1;
        m_global_v2 = ( vi2 * ( p_parent->m_global_v2 - p_parent->m_global_v1 ) ) + p_parent->m_global_v1;

    }
    else
    {
        m_texture_referent = this;

        m_global_ref_u1 = 0.0;
        m_global_ref_v1 = 0.0;
        m_global_ref_u2 = 1.0;
        m_global_ref_v2 = 1.0;

        m_global_u1 = 0.0;
        m_global_v1 = 0.0;
        m_global_u2 = 1.0;
        m_global_v2 = 1.0;
    }

    /////////////////////////
    /*
    if (cst::SurfaceLayer == p_layer_index)
    {
        generate_heightmap();
    } 
    */
}

Patch::~Patch( void )
{
    if (m_heightmap)
    {
        _DRAWSPACE_DELETE_N_(m_heightmap);
    }

    for (auto e : m_related_subpasses)
    {
        _DRAWSPACE_DELETE_(e);
    }
}

std::vector<Patch::FoliagesCoordinates> Patch::GetFoliageCoordsList(void) const
{
    return m_foliagesCoordinates;
}

int Patch::GetLayerIndex(void) const
{
    return m_layer_index;
}

void Patch::prepare_data_texture( /*SubPass::SubPassCreationHandler* p_handler, SubPass::Destination p_subpass_dest,*/ int p_layer_index)
{
    m_datatexture_pass = create_data_texture_pass();

    std::vector<Patch*> dl;
    dl.push_back( this );

    // creation/preparation du node

    const auto renderer{ SingletonPlugin<DrawSpace::Interface::Renderer>::GetInstance()->m_interface };
    const auto node{ _DRAWSPACE_NEW_(FaceDrawingNode, FaceDrawingNode(renderer, m_config, p_layer_index)) };
            
    node->SetMeshe( Body::m_patch2_meshe );
    node->SetDisplayList( dl );

    node->SetBinder( m_config->m_layers_descr[p_layer_index].patchTexturesBinder[m_orientation] );
               
    void* tx_data;
    if( false == renderer->CreateTexture( m_datatexture_pass->GetTargetTexture(), &tx_data ) )
    {
        _DSEXCEPTION( "failed to create subpasstarget texture in renderer" );
    }
            
    ////////////////////////

    m_subpass_node_list.push_back(node);
    m_subpass_list.push_back(m_datatexture_pass);
    
    /*
    // appel handler pour enregistrer et executer la passe
    
    if( p_handler )
    {
        m_subpass_entry_infos_list.push_back((*p_handler)(this, p_subpass_dest));
    }
    */
}

void Patch::prepare_hm_texture(/*SubPass::SubPassCreationHandler* p_handler, SubPass::Destination p_subpass_dest,*/ int p_layer_index)
{
    m_heightmap_pass = create_heightmap_pass();

    std::vector<Patch*> dl;
    dl.push_back(this);


    // creation/preparation du node

    auto renderer{ SingletonPlugin<DrawSpace::Interface::Renderer>::GetInstance()->m_interface };
    const auto node{ _DRAWSPACE_NEW_(FaceDrawingNode, FaceDrawingNode(renderer, m_config, p_layer_index)) };

    node->SetMeshe(LOD::Body::m_patch_meshe);
    node->SetDisplayList(dl);

    node->SetBinder(m_config->m_layers_descr[p_layer_index].heightmapGenerationBinder[m_orientation]);

    void* tx_data;
    if (false == renderer->CreateTexture(m_heightmap_pass->GetTargetTexture(), &tx_data))
    {
        _DSEXCEPTION("failed to create hm subpasstarget texture in renderer");
    }

    ////////////////////////

    m_heightmap_pass->GetTargetTexture()->AllocTextureContent();

    m_subpass_node_list.push_back(node);
    m_subpass_list.push_back(m_heightmap_pass);

}



void Patch::CleanupSubpasses( void )
{
    // remove texture on renderer side
    /*
    if( m_subpass && m_subpass_node )
    {
        if( m_subpass_entry_infos_valid )
        {
            try
            {
                remove_entry_from_queue( m_subpass_entry_infos );
            } 
            catch( ... )
            {
                _DSEXCEPTION( "unexpected error while trying to remove subpass queue entry" );
            }
        }

        DrawSpace::Interface::Renderer* renderer = SingletonPlugin<DrawSpace::Interface::Renderer>::GetInstance()->m_interface;
        renderer->DestroyTexture( m_subpass->GetTargetTexture()->GetRenderData() );

        // remove node
        _DRAWSPACE_DELETE_( m_subpass_node );

        // remove pass
        _DRAWSPACE_DELETE_( m_subpass );
    }
    */

    for (const auto& e : m_subpass_entry_infos_list)
    {
        try
        {
            remove_entry_from_queue(e);
        }
        catch (...)
        {
            _DSEXCEPTION("unexpected error while trying to remove subpass queue entry");
        }

    }

    const auto renderer { SingletonPlugin<DrawSpace::Interface::Renderer>::GetInstance()->m_interface };

    for (const auto s : m_subpass_list)
    {
        renderer->DestroyTexture(s->GetTargetTexture()->GetRenderData());
    }

    for (auto n : m_subpass_node_list)
    {
        _DRAWSPACE_DELETE_(n);
    }

    for (auto s : m_subpass_list)
    {
        _DRAWSPACE_DELETE_(s);
    }
}

void Patch::SetNeighbour( DrawSpace::Utils::BaseQuadtreeNode* p_patch, int p_id )
{
    m_neighbours[p_id] = p_patch;
}

DrawSpace::Utils::BaseQuadtreeNode* Patch::GetNeighbour( int p_id ) const
{
    return m_neighbours[p_id];
}

int Patch::GetLodLevel( void ) const
{
    return m_lod_level;
}

void Patch::CubeToSphere( const Vector& p_in, Vector& p_out )
{
    dsreal x = p_in[0];
    dsreal y = p_in[1];
    dsreal z = p_in[2];

    p_out[0] = x * sqrt( 1.0 - y * y * 0.5 - z * z * 0.5 + y * y * z * z / 3.0 );
    p_out[1] = y * sqrt( 1.0 - z * z * 0.5 - x * x * 0.5 + x * x * z * z / 3.0 );
    p_out[2] = z * sqrt( 1.0 - x * x * 0.5 - y * y * 0.5 + x * x * y * y / 3.0 );
}

void Patch::SphereToCube( const Vector& p_in, Vector& p_out )
{
    dsreal nx, ny, nz;
    nx = ny = nz = 0;

    dsreal x, y, z;

    x = p_in[0];
    y = p_in[1];
    z = p_in[2];

    dsreal fx, fy, fz;
    fx = fabs( p_in[0] );
    fy = fabs( p_in[1] );
    fz = fabs( p_in[2] );

    int nbIter = 6;

    if( fy >= fx && fy >= fz )
    {
        if( y > 0 )
        {
            for( int n = 0; n < nbIter; n++ )
            {
                nx = x / sqrt( 1.0 - ny * ny * 0.5 - nz * nz * 0.5 + ny * ny * nz * nz / 3.0 );
                ny = y / sqrt( 1.0 - nz * nz * 0.5 - nx * nx * 0.5 + nz * nz * nx * nx / 3.0 );
                nz = z / sqrt( 1.0 - nx * nx * 0.5 - ny * ny * 0.5 + nx * nx * ny * ny / 3.0 );

                nx = nx / ny;
                nz = nz / ny;
                ny = 1.0;
            }
        }
        else
        {
            for( int n = 0; n < nbIter; n++ )
            {
                nx = x / sqrt( 1.0 - ny * ny * 0.5 - nz * nz * 0.5 + ny * ny * nz * nz / 3.0 );
                ny = y / sqrt( 1.0 - nz * nz * 0.5 - nx * nx * 0.5 + nz * nz * nx * nx / 3.0 );
                nz = z / sqrt( 1.0 - nx * nx * 0.5 - ny * ny * 0.5 + nx * nx * ny * ny / 3.0 );

                nx = nx / (-ny);
                nz = nz / (-ny);
                ny = -1.0;
            }
        }
    }
    else if( fx >= fy && fx >= fz )
    {
        if( x > 0 )
        {
            for( int n = 0; n < nbIter; n++ )
            {
                nx = x / sqrt( 1.0 - ny * ny * 0.5 - nz * nz * 0.5 + ny * ny * nz * nz / 3.0 );
                ny = y / sqrt( 1.0 - nz * nz * 0.5 - nx * nx * 0.5 + nz * nz * nx * nx / 3.0 );
                nz = z / sqrt( 1.0 - nx * nx * 0.5 - ny * ny * 0.5 + nx * nx * ny * ny / 3.0 );

                nz = nz / nx;
                ny = ny / nx;
                nx = 1.0;
            }
        }
        else
        {
            for( int n = 0; n < nbIter; n++ )
            {
                nx = x / sqrt( 1.0 - ny * ny * 0.5 - nz * nz * 0.5 + ny * ny * nz * nz / 3.0 );
                ny = y / sqrt( 1.0 - nz * nz * 0.5 - nx * nx * 0.5 + nz * nz * nx * nx / 3.0 );
                nz = z / sqrt( 1.0 - nx * nx * 0.5 - ny * ny * 0.5 + nx * nx * ny * ny / 3.0 );

                nz = nz / (-nx);
                ny = ny / (-nx);
                nx = -1.0;
            }
        }
    }
    else
    {
        if( z > 0 )
        {
            for( int n = 0; n < nbIter; n++ )
            {
                nx = x / sqrt( 1.0 - ny * ny * 0.5 - nz * nz * 0.5 + ny * ny * nz * nz / 3.0 );
                ny = y / sqrt( 1.0 - nz * nz * 0.5 - nx * nx * 0.5 + nz * nz * nx * nx / 3.0 );
                nz = z / sqrt( 1.0 - nx * nx * 0.5 - ny * ny * 0.5 + nx * nx * ny * ny / 3.0 );

                nx = nx / nz;
                ny = ny / nz;
                nz = 1.0;
            }
        }
        else
        {
            for( int n = 0; n < nbIter; n++ )
            {
                nx = x / sqrt( 1.0 - ny * ny * 0.5 - nz * nz * 0.5 + ny * ny * nz * nz / 3.0 );
                ny = y / sqrt( 1.0 - nz * nz * 0.5 - nx * nx * 0.5 + nz * nz * nx * nx / 3.0 );
                nz = z / sqrt( 1.0 - nx * nx * 0.5 - ny * ny * 0.5 + nx * nx * ny * ny / 3.0 );

                nx = nx / (-nz);
                ny = ny / (-nz);
                nz = -1.0;
            }
        }
    }

    p_out[0] = nx;
    p_out[1] = ny;
    p_out[2] = nz;
}

dsreal Patch::GetSideLength( void ) const
{
    return m_sidelength * m_ray;
}

dsreal Patch::GetUnitSideLenght( void ) const
{
    return m_sidelength;
}

dsreal Patch::GetTriangleSideLength( void ) const
{
    return ( ( m_sidelength * m_ray ) / ( cst::patchResolution - 1 ) );
}

void Patch::GetPos( dsreal& p_xpos, dsreal& p_ypos ) const
{
    p_xpos = m_xpos * m_ray;
    p_ypos = m_ypos * m_ray;
}

void Patch::GetUnitPos( dsreal& p_xpos, dsreal& p_ypos ) const
{
    p_xpos = m_xpos;
    p_ypos = m_ypos;
}

BaseQuadtreeNode* Patch::GetOwner( void ) const
{
    return m_owner;
}

int Patch::GetOrientation( void ) const
{
    return m_orientation;
}

void Patch::XYToXYZ( int p_orientation, dsreal p_x, dsreal p_y, Vector& p_out )
{
    switch( p_orientation )
    {
        case FrontPlanetFace:

            p_out[0] = p_x;
            p_out[1] = p_y;
            p_out[2] = 1.0;
            break;

        case RearPlanetFace:

            p_out[0] = -p_x;
            p_out[1] = p_y;
            p_out[2] = -1.0;
            break;

        case LeftPlanetFace:

            p_out[0] = -1.0;
            p_out[1] = p_y;
            p_out[2] = p_x;
            break;

        case RightPlanetFace:

            p_out[0] = 1.0;
            p_out[1] = p_y;
            p_out[2] = -p_x;
            break;

        case TopPlanetFace:

            p_out[0] = p_x;
            p_out[1] = 1.0;
            p_out[2] = -p_y;
            break;

        case BottomPlanetFace:

            p_out[0] = p_x;
            p_out[1] = -1.0;
            p_out[2] = p_y;
            break;
    }
}

void Patch::ConvertVectorToFrontFaceCoords( int p_orientation, const DrawSpace::Utils::Vector& p_in, DrawSpace::Utils::Vector& p_out )
{
    if( p_orientation == Patch::FrontPlanetFace )
    {
        p_out[0] = p_in[0];
        p_out[1] = p_in[1];
        p_out[2] = p_in[2];
        p_out[3] = 0.0;
    }

    if( p_orientation == Patch::RearPlanetFace )
    {
        p_out[0] = -p_in[0];
        p_out[1] = p_in[1];
        p_out[2] = -p_in[2];
        p_out[3] = 0.0;
    }

    if( p_orientation == Patch::TopPlanetFace )
    {
        p_out[0] = p_in[0];
        p_out[1] = -p_in[2];
        p_out[2] = p_in[1];
        p_out[3] = 0.0;
    }

    if( p_orientation == Patch::BottomPlanetFace )
    {
        p_out[0] = p_in[0];
        p_out[1] = p_in[2];
        p_out[2] = -p_in[1];
        p_out[3] = 0.0;
    }


    if( p_orientation == Patch::RightPlanetFace )
    {
        p_out[0] = -p_in[2];
        p_out[1] = p_in[1];
        p_out[2] = p_in[0];
        p_out[3] = 0.0;
    }

    if( p_orientation == Patch::LeftPlanetFace )
    {
        p_out[0] = p_in[2];
        p_out[1] = p_in[1];
        p_out[2] = -p_in[0];
        p_out[3] = 0.0;
    }
}

void Patch::ProjectVertex( const DrawSpace::Utils::Vector& p_in, DrawSpace::Utils::Vector& p_out )
{
    DrawSpace::Utils::Vector in = p_in;
    DrawSpace::Utils::Vector v2, v3;

    // sidelenght scaling
    in.Scale( m_sidelength / 2.0 );

    // patch positionning
    in[0] = in[0] + m_xpos;
    in[1] = in[1] + m_ypos;
    in[2] = 0.0;
    in[3] = 1.0;

    // patch reorientation
    XYToXYZ( m_orientation, in[0], in[1], v2 );
    v2[3] = 1.0;

    CubeToSphere( v2, v3 );
    v3[3] = 1.0;
    p_out = v3;
}

void Patch::GetGlobalRelUVCoords( DrawSpace::Utils::Vector& p_uv ) const
{
    p_uv[0] = m_global_ref_u1;
    p_uv[1] = m_global_ref_v1;
    p_uv[2] = m_global_ref_u2;
    p_uv[3] = m_global_ref_v2;
}

void Patch::GetGlobalUVCoords( DrawSpace::Utils::Vector& p_uv ) const
{
    p_uv[0] = m_global_u1;
    p_uv[1] = m_global_v1;
    p_uv[2] = m_global_u2;
    p_uv[3] = m_global_v2;
}

void Patch::GetNormalVector( int p_orientation, DrawSpace::Utils::Vector& p_vector )
{
    Vector face_dir;
    switch( p_orientation )
    {
        case Patch::FrontPlanetFace:

            face_dir[0] = 0.0;
            face_dir[1] = 0.0;
            face_dir[2] = 1.0;
            face_dir[3] = 1.0;
            break;

        case Patch::RearPlanetFace:

            face_dir[0] = 0.0;
            face_dir[1] = 0.0;
            face_dir[2] = -1.0;
            face_dir[3] = 1.0;
            break;

        case Patch::TopPlanetFace:

            face_dir[0] = 0.0;
            face_dir[1] = 1.0;
            face_dir[2] = 0.0;
            face_dir[3] = 1.0;
            break;

        case Patch::BottomPlanetFace:

            face_dir[0] = 0.0;
            face_dir[1] = -1.0;
            face_dir[2] = 0.0;
            face_dir[3] = 1.0;
            break;

        case Patch::RightPlanetFace:

            face_dir[0] = 1.0;
            face_dir[1] = 0.0;
            face_dir[2] = 0.0;
            face_dir[3] = 1.0;
            break;

        case Patch::LeftPlanetFace:

            face_dir[0] = -1.0;
            face_dir[1] = 0.0;
            face_dir[2] = 0.0;
            face_dir[3] = 1.0;
            break;
    }
    p_vector = face_dir;
}

bool Patch::IsCircleIntersection( dsreal p_centerx, dsreal p_centery, dsreal p_ray )
{    
    dsreal xpos = m_xpos * m_ray;
    dsreal ypos = m_ypos * m_ray;
    dsreal sidelength = m_sidelength * m_ray;

    Utils::Vector dist( xpos - p_centerx, ypos - p_centery, 0.0, 0.0 );

    if( dist.Length() < p_ray + sidelength )
    {
        return true;
    }
    return false;
}

DrawSpace::IntermediatePass* Patch::create_data_texture_pass( void )
{
    char thisname[32];

    sprintf( thisname, "patch_%x", this );

    dsstring complete_name = dsstring( thisname ) + dsstring( "_colortexture_pass" );
    IntermediatePass* ipass = _DRAWSPACE_NEW_( IntermediatePass, IntermediatePass( complete_name ) );

    ipass->SetTargetDimsFromRenderer( false );    
    ipass->SetTargetDims( cst::patchHighResolution, cst::patchHighResolution );
    ipass->SetRenderPurpose( Texture::RENDERPURPOSE_FLOATVECTOR );
    
    ipass->Initialize();
    ipass->GetRenderingQueue()->EnableDepthClearing( false );
    ipass->GetRenderingQueue()->EnableTargetClearing( false );
    return ipass;
}

DrawSpace::IntermediatePass* Patch::create_heightmap_pass(void)
{
    const auto thisname{ dsstring("layer_") + std::to_string((int)this) };
    const auto complete_name{ thisname + dsstring("_heightmap_pass") };

    const auto ipass{ _DRAWSPACE_NEW_(IntermediatePass, IntermediatePass(complete_name)) };

    ipass->SetTargetDimsFromRenderer(false);
    ipass->SetTargetDims(heightmapTextureSize, heightmapTextureSize);
    ipass->SetRenderPurpose(Texture::RENDERPURPOSE_FLOAT32);
    ipass->SetRenderTarget(Texture::RENDERTARGET_CPU);

    ipass->Initialize();
    ipass->GetRenderingQueue()->EnableDepthClearing(true);

    return ipass;
}

DrawSpace::Core::Texture* Patch::GetDataTexture( void ) const
{
    if( m_datatexture_pass )
    {
        return m_datatexture_pass->GetTargetTexture();
    }
    return NULL;
}

Patch* Patch::GetParent(void) const
{    
    BaseQuadtreeNode* parent{ m_owner->GetParent() };    
    if (parent)
    {
        QuadtreeNode<Patch>* node{ nullptr };
        node = static_cast<QuadtreeNode<Patch>*>(parent);
        return node->GetContent();
    }
    return nullptr;
}

Patch* Patch::GetTextureReferent( void ) const
{
    return m_texture_referent;
}

void Patch::SubPassDone( void )
{
    // subpass effectuee, l'entree dans la queue n'existe donc plus...
    
    //m_subpass_entry_infos_valid = false;
    m_subpass_entry_infos_list.clear();

    if( m_parent )
    {
        m_texture_referent = this;
        m_global_ref_u1 = 0.0;
        m_global_ref_v1 = 0.0;
        m_global_ref_u2 = 1.0;
        m_global_ref_v2 = 1.0;

        if( m_owner->HasChildren() )
        {
            for( long i = 0; i < 4; i++ )
            {
                QuadtreeNode<Patch>* child = static_cast<QuadtreeNode<Patch>*>( m_owner->GetChild( i ) );
           
                child->GetContent()->recurs_update_texture_referent( m_texture_referent );
            }
        }
    }
}

void Patch::SubPassAborted(void)
{
    //m_subpass_entry_infos_valid = false;
}

void Patch::AddRelatedSubpasses(HeighmapSubPass* p_subpass)
{
    m_related_subpasses.insert(p_subpass);
}

void Patch::RemoveRelatedSubpasses(HeighmapSubPass* p_subpass)
{
    if (m_related_subpasses.count(p_subpass))
    {
        m_related_subpasses.erase(p_subpass);
    }
}


void Patch::SetHeightMap(float* p_hm)
{
    m_heightmap = p_hm;
}

bool Patch::HasHeightMap(void)
{
    return (m_heightmap != nullptr);
}

float* Patch::GetHeightMap(void) const
{
    return m_heightmap;
}


dsstring Patch::DumpInfos(void) const
{
    dsstring infos;

    infos = ">>>> Patch " + std::to_string((int)this);

    infos += " Orientation = " + std::to_string(m_orientation);
    infos += " Unit sidelenght = " + std::to_string(m_sidelength);
    infos += " ray = " + std::to_string(m_ray);
    infos += " unit pos = " + std::to_string(m_xpos) + " " + std::to_string(m_ypos);
   
    return infos;
}

/*
void Patch::generate_heightmap()
{
    
    // launch hm generation
    
    std::vector<LOD::Patch*> display_list;
    display_list.push_back(this);

    LOD::HeighmapSubPass* current_hm{ _DRAWSPACE_NEW_(HeighmapSubPass, HeighmapSubPass(m_subpasscreation_handler, m_config, GetOrientation(), m_layer_index, HeighmapSubPass::Purpose::FOR_FOLIAGE)) };

    current_hm->RegisterSubpassDoneHandler(&m_subpassDoneCb);
    //current_hm->RegisterSubpassAbortedHandler(&m_subpassAbortedCb);

    const auto node{ static_cast<LOD::FaceDrawingNode*>(current_hm->GetNode()) };
    node->SetDisplayList(display_list);
    
    //AddRelatedSubpasses(current_hm);
    

}
*/

/*
void Patch::on_subpassdone(LOD::HeighmapSubPass* p_subpass)
{
    p_subpass->GetHMTexture()->CopyTextureContent();
    const auto heightmap{ (float*)p_subpass->GetHMTextureContent() };

    if (HasHeightMap())
    {
        auto old_buffer{ GetHeightMap() };
        _DRAWSPACE_DELETE_N_(old_buffer);
    }

    const auto hm_buffer_size{ HeighmapSubPass::heightmapTextureSize * HeighmapSubPass::heightmapTextureSize };
    const auto patch_hm_buffer{ _DRAWSPACE_NEW_EXPLICIT_SIZE_WITH_COMMENT(float, float[hm_buffer_size], hm_buffer_size, "heightmap for patch") };
    memcpy(patch_hm_buffer, heightmap, hm_buffer_size * sizeof(float));

    SetHeightMap(patch_hm_buffer);

    RemoveRelatedSubpasses(p_subpass);
    _DRAWSPACE_DELETE_(p_subpass);
}
*/

/*
void Patch::on_subpassaborted(LOD::HeighmapSubPass* p_subpass)
{
    _DRAWSPACE_DELETE_(p_subpass);
}
*/



void Patch::recurs_update_texture_referent( Patch* p_texture_referent )
{
    m_texture_referent = p_texture_referent;

    dsreal ui1, vi1, ui2, vi2;

    switch( m_nodeid )
    {
        case BaseQuadtreeNode::NorthWestNode:

            ui1 = 0.0;
            vi1 = 0.0;
            ui2 = 0.5;
            vi2 = 0.5;

            break;

        case BaseQuadtreeNode::NorthEastNode:

            ui1 = 0.5;
            vi1 = 0.0;
            ui2 = 1.0;
            vi2 = 0.5;

            break;

        case BaseQuadtreeNode::SouthEastNode:

            ui1 = 0.5;
            vi1 = 0.5;
            ui2 = 1.0;
            vi2 = 1.0;

            break;

        case BaseQuadtreeNode::SouthWestNode:

            ui1 = 0.0;
            vi1 = 0.5;
            ui2 = 0.5;
            vi2 = 1.0;

            break;
    }

    m_global_ref_u1 = ( ui1 * ( m_parent->m_global_ref_u2 - m_parent->m_global_ref_u1 ) ) + m_parent->m_global_ref_u1;
    m_global_ref_v1 = ( vi1 * ( m_parent->m_global_ref_v2 - m_parent->m_global_ref_v1 ) ) + m_parent->m_global_ref_v1;

    m_global_ref_u2 = ( ui2 * ( m_parent->m_global_ref_u2 - m_parent->m_global_ref_u1 ) ) + m_parent->m_global_ref_u1;
    m_global_ref_v2 = ( vi2 * ( m_parent->m_global_ref_v2 - m_parent->m_global_ref_v1 ) ) + m_parent->m_global_ref_v1;

    if( m_owner->HasChildren() )
    {
        for( long i = 0; i < 4; i++ )
        {
            QuadtreeNode<Patch>* child = static_cast<QuadtreeNode<Patch>*>( m_owner->GetChild( i ) );           
            child->GetContent()->recurs_update_texture_referent( p_texture_referent );
        }
    }
}
