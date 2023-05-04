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

#include "lod_layer.h"
#include "lod_body.h"
#include "lod_config.h"
#include "lod_drawing.h"

#include "collisionaspect.h"
#include "entitynodegraph.h"

#include "maths.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Utils;
using namespace DrawSpace::Aspect;

using namespace LOD;

DrawSpace::Logger::Sink planetlayer_logger("PlanetLayer", DrawSpace::Logger::Configuration::GetInstance());

Layer::Layer(DrawSpace::EntityGraph::EntityNodeGraph* p_eg, Config* p_config, Body* p_body, 
                SubPass::SubPassCreationHandler* p_subpass_creation_handler,
                CollisionMesheUpdateHandler* p_collision_meshe_update_handler,
                int p_index, bool p_freecamera) :
m_entitynodegraph(p_eg),
m_config(p_config),
m_body(p_body),
m_subpass_creation_handler(p_subpass_creation_handler),
m_collision_meshe_update_handler(p_collision_meshe_update_handler),
m_hot(false),
m_current_lod(-1),
m_meshe_collision_shape(m_hm_meshe),
m_layer_index(p_index),
m_subpassDoneCb(this, &Layer::on_subpassdone),
m_subpassAbortedCb(this, &Layer::on_subpassaborted)
{   
    if (m_config->m_layers_descr[p_index].enable_collisions && !p_freecamera)
    {
        m_collisions = true;
    }
       
    m_planetray = 1000.0 * m_config->m_layers_descr[p_index].ray;

    m_body->Initialize();

    m_description = m_config->m_layers_descr[p_index].description;

    memset(m_alt_grid, 0, sizeof(m_alt_grid));

    _DSDEBUG(planetlayer_logger, std::to_string(m_layer_index) + 
                                    dsstring(" description = ") + m_description + 
                                    dsstring(" ") + std::to_string((int)this) + 
                                    dsstring(" freecamera = ") + std::to_string(p_freecamera)    
    );
}

Layer::~Layer(void)
{
}

Body* Layer::GetBody(void) const
{
    return m_body;
}

bool Layer::GetHotState(void) const
{
    return m_hot;
}

void Layer::RegisterNewCollisionMesheCreationHandler(NewCollisionMesheCreationHandler* p_handler)
{
    m_collision_meshe_creation_handler.push_back(p_handler);
}


int Layer::GetCurrentLOD(void) const
{
    return m_current_lod;
}

void Layer::SetHotState(bool p_hotstate)
{
    m_hot = p_hotstate;
    m_body->SetHotState(m_hot);
}

void Layer::UpdateRelativeAlt(dsreal p_alt)
{
    m_body->UpdateRelativeAlt(p_alt);
}

void Layer::UpdateInvariantViewerPos(const DrawSpace::Utils::Vector& p_pos)
{
    m_body->UpdateInvariantViewerPos(p_pos);
}

void Layer::UpdateHotPoint(const DrawSpace::Utils::Vector& p_vector)
{
    m_body->UpdateHotPoint(p_vector);
}

void Layer::generate_heightmap(Patch* p_patch, HeighmapSubPass::Purpose p_purpose)
{
    // launch hm generation

    std::vector<LOD::Patch*> display_list;
    display_list.push_back(p_patch);

    LOD::HeighmapSubPass* current_hm{ _DRAWSPACE_NEW_(HeighmapSubPass, HeighmapSubPass(m_subpass_creation_handler, m_config, p_patch->GetOrientation(), m_layer_index, p_purpose)) };

    current_hm->RegisterSubpassDoneHandler(&m_subpassDoneCb);
    current_hm->RegisterSubpassAbortedHandler(&m_subpassAbortedCb);

    /*
    const auto node{ static_cast<LOD::FaceDrawingNode*>(current_hm->GetNode()) };
    node->SetDisplayList(display_list);
    */
    const auto nodes_list{ current_hm->GetNodeList() };
    const auto node{ static_cast<LOD::FaceDrawingNode*>(nodes_list.at(0)) };
    node->SetDisplayList(display_list);


    m_heightmap_source_patches[current_hm] = p_patch;

    //p_patch->SetBusySubpass(true, "HeighmapSubPass for purpose : " + std::to_string((int)p_purpose));
    p_patch->AddRelatedSubpasses(current_hm);

    _DSDEBUG(planetlayer_logger, 
        dsstring("layer " ) + std::to_string((int)this) +
        dsstring(" p_patch = ") + std::to_string((int)p_patch) +
        dsstring(" hm subpass = ") + std::to_string((int)current_hm) + 
        dsstring(" p_purpose = ") + std::to_string((int)p_purpose)
    );

    _DSDEBUG(planetlayer_logger, p_patch->DumpInfos());
}

void Layer::Compute(void)
{
    m_body->Compute();


    const auto current_face_index{ m_body->GetCurrentFace() };
    m_current_lod = m_body->GetFace(current_face_index)->GetCurrentPatchLOD();
    const auto curr_patch{ m_body->GetFace(current_face_index)->GetCurrentPatch() };


    if (m_current_patch != curr_patch)
    {
        m_current_patch = curr_patch;
       
        ///// generate new heightmap for foliage
        /*
        if (cst::SurfaceLayer == m_layer_index)
        {
            if (m_current_patch && m_current_lod == 0 && m_current_patch->GetOrientation() == m_body->GetCurrentFace())
            {               

                auto parent_patch{ m_current_patch };

                constexpr auto lod_limit{ cst::FoliageRootLODLevel };

                for (int i = 0; i < lod_limit; i++)
                {
                    if (nullptr == parent_patch->GetParent())
                    {
                        break;
                    }
                    parent_patch = parent_patch->GetParent();
                }

                // recursive exploration
                const std::function<void(LOD::Patch*)> browse_patches
                {
                    [&](LOD::Patch* p_patch) 
                    {
                        const auto owner { p_patch->GetOwner() };
                        for (int i = 0; i < 4; i++)
                        {
                            if (owner->HasChildren())
                            {
                                const auto specialized_child{ static_cast<QuadtreeNode<Patch>*>(owner->GetChild(i)) };

                                const auto sub_patch{ specialized_child->GetContent() };
                                const auto patch_lod{ sub_patch->GetLodLevel() };
                                
                                generate_heightmap(sub_patch, HeighmapSubPass::Purpose::FOR_FOLIAGE);
                                if (patch_lod > 0)
                                {
                                    browse_patches(sub_patch);
                                }
                            }
                        }
                    }
                };

                browse_patches(parent_patch);
            }
        }
        */        
        ///// generate new heightmap for collisions

        if (m_collisions )
        {

            if (m_current_patch && m_current_lod == 0 && m_current_patch->GetOrientation() == m_body->GetCurrentFace())
            {
                
                // collisions : adjust hm source

                auto heightmap_source_patch{ m_current_patch };
                for (int i = 0; i < cst::HeightMapRelativeLOD; i++)
                {
                    if (nullptr == heightmap_source_patch->GetParent())
                    {
                        break;
                    }
                    heightmap_source_patch = heightmap_source_patch->GetParent();
                }

                generate_heightmap(heightmap_source_patch, HeighmapSubPass::Purpose::FOR_COLLISIONS);               
            }
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector view_patch_coords;
    const auto curr_face{ m_body->GetCurrentFace() };

    if (curr_face > -1)
    {
        m_body->GetFace(m_body->GetCurrentFace())->GetCurrentPatchViewCoords(view_patch_coords);
        const auto new_alt{ get_interpolated_height(view_patch_coords[0], view_patch_coords[1]) };

        if (!isnan(new_alt))
        {
            m_currentpatch_current_height = new_alt;
        }
    }    
}

void Layer::build_meshe(float* p_heightmap, DrawSpace::Core::Meshe& p_patchmeshe, LOD::Patch* p_patch, DrawSpace::Core::Meshe& p_outmeshe)
{    
    dsreal max_height{ 0.0 };
    dsreal min_height{ 1000000000.0 };

    for (int y = 0; y < cst::patchResolution; y++)
    {
        for (int x = 0; x < cst::patchResolution; x++)
        {
            Vertex vertex_in, vertex_out;

            const auto index { (cst::patchResolution * y) + x };
            p_patchmeshe.GetVertex(index, vertex_in);

            const auto x_input { (x * (HeighmapSubPass::heightmapTextureSize - 1)) / (cst::patchResolution - 1) };
            const auto y_input { (y * (HeighmapSubPass::heightmapTextureSize - 1)) / (cst::patchResolution - 1) };
            const auto index_hm { (HeighmapSubPass::heightmapTextureSize * (HeighmapSubPass::heightmapTextureSize - 1 - y_input)) + x_input };
           
            const auto alt { p_heightmap[index_hm] };

            m_alt_grid[index] = alt;

            if (alt > max_height)
            {
                max_height = alt;
            }

            if (alt < min_height)
            {
                min_height = alt;
            }

            Vector v_out;
            p_patch->ProjectVertex(Vector(vertex_in.x, vertex_in.y, vertex_in.z, 1.0), v_out);

            //v_out.Scale(m_planetray + alt);

            v_out.Scale(m_planetray);
            v_out.Scale(1.0 + (alt / m_planetray));

            vertex_out.x = v_out[0];
            vertex_out.y = v_out[1];
            vertex_out.z = v_out[2];

            p_outmeshe.AddVertex(vertex_out);
        }
    }

    for (long i = 0; i < p_patchmeshe.GetTrianglesListSize(); i++)
    {
        TrianglePrimitive<unsigned int> t;
        p_patchmeshe.GetTriangles(i, t);
        p_outmeshe.AddTriangle(t);
    }

    m_currentpatch_max_height = max_height;
    m_currentpatch_min_height = min_height;    
}

dsreal Layer::get_interpolated_height(dsreal p_coord_x, dsreal p_coord_y)
{
    int index_hm;

    const auto xcoord{ p_coord_x };
    const auto ycoord{ p_coord_y };

    // trouver les 4 valeurs voisines;

    // xcoord et ycoord sur le range [-0.5, 0.5]
    // trouver les coords discretes de grille patch, c a d [0, cst::patchResolution - 1], encadrant le point coord fourni;

    const auto resol{ cst::patchResolution };

    const auto x1 { Maths::Floor(((xcoord + 0.5)) * resol) };
    auto y1 { Maths::Floor(((ycoord + 0.5)) * resol) };

    const auto x2{ x1 + 1 };
    auto y2{ y1 + 1 };

    y1 = cst::patchResolution - y1;
    y2 = cst::patchResolution - y2;

    index_hm = (cst::patchResolution * y1) + x1;
    const auto h1{ m_alt_grid[index_hm] };

    index_hm = (cst::patchResolution * y1) + x2;
    const auto h2{ m_alt_grid[index_hm] };

    index_hm = (cst::patchResolution * y2) + x2;
    const auto h3{ m_alt_grid[index_hm] };

    index_hm = (cst::patchResolution * y2) + x1;
    const auto h4{ m_alt_grid[index_hm] };

    // calcul des distances du point central vers les 4 coints de bords
    const auto interv { 1.0 / (cst::patchResolution - 1) };

    // passer les coins en range [-0.5, 0.5]

    const auto xg1 { (x1 * interv) - 0.5 };
    const auto yg1 { (y1 * interv) - 0.5 };

    const auto unit_x { (xcoord - xg1) / interv };
    const auto unit_y { (ycoord - yg1) / interv };
    const auto a1 { Maths::Lerp(Maths::Lerp(h1, h4, unit_y), Maths::Lerp(h2, h3, unit_y), unit_x) };

    return Maths::Clamp(m_currentpatch_min_height, m_currentpatch_max_height, a1);
}

void Layer::on_subpassdone(LOD::HeighmapSubPass* p_subpass)
{
    p_subpass->GetHMTexture()->CopyTextureContent();
    const auto heightmap{ (float*)p_subpass->GetHMTextureContent() };

    auto heightmap_source_patch{ m_heightmap_source_patches.at(p_subpass) };

    //heightmap_source_patch->SetBusySubpass(false, "");
    heightmap_source_patch->RemoveRelatedSubpasses(p_subpass);

    _DSDEBUG(planetlayer_logger,
        dsstring("layer ") + std::to_string((int)this) +
        dsstring(" patch = ") + std::to_string((int)heightmap_source_patch) +
        dsstring(" hm subpass = ") + std::to_string((int)p_subpass) +
        dsstring(" hm purpose = ") + std::to_string((int)p_subpass->GetPurpose())
    );

   
    if (heightmap_source_patch->HasHeightMap())
    {
        auto old_buffer{ heightmap_source_patch->GetHeightMap() };
        _DRAWSPACE_DELETE_N_(old_buffer);
    }

    const auto hm_buffer_size{ HeighmapSubPass::heightmapTextureSize * HeighmapSubPass::heightmapTextureSize };
    const auto patch_hm_buffer{ _DRAWSPACE_NEW_EXPLICIT_SIZE_WITH_COMMENT(float, float[hm_buffer_size], hm_buffer_size, "heightmap for patch") };
    memcpy(patch_hm_buffer, heightmap, hm_buffer_size * sizeof(float));
   
    heightmap_source_patch->SetHeightMap(patch_hm_buffer);
    

    Meshe final_meshe;

    build_meshe(heightmap, *(LOD::Body::GetPatcheMeshe()), heightmap_source_patch, final_meshe);
    m_hm_meshe = final_meshe;

    for (auto& e : m_collision_meshe_creation_handler)
    {
        (*e)(m_hm_meshe);
    }

    if (HeighmapSubPass::Purpose::FOR_COLLISIONS == p_subpass->GetPurpose())
    {
        const auto shape_component_name{ "shape_" + std::to_string((long)this) };
        (*m_collision_meshe_update_handler)(shape_component_name, m_meshe_collision_shape, true);
    }

    m_heightmap_source_patches.erase(p_subpass);

    _DRAWSPACE_DELETE_(p_subpass);

    m_collisions_active = true;
}

void Layer::on_subpassaborted(LOD::HeighmapSubPass* p_subpass)
{
    m_heightmap_source_patches.erase(p_subpass);
    _DRAWSPACE_DELETE_(p_subpass);
}


void Layer::ResetBody(void)
{
    m_body->Reset();

    m_current_patch = nullptr;
}

dsreal Layer::GetCurrentPatchMaxHeight(void) const
{
    return m_currentpatch_max_height;
}

dsreal Layer::GetCurrentPatchMinHeight(void) const
{
    return m_currentpatch_min_height;
}

dsreal Layer::GetCurrentPatchCurrentHeight(void) const
{
    return m_currentpatch_current_height;
}

void Layer::RemoveCollider(void)
{
    if (m_collisions_active)
    {
        const auto shape_component_name{ "shape_" + std::to_string((long)this) };
        (*m_collision_meshe_update_handler)(shape_component_name, m_meshe_collision_shape, false);

        m_collisions_active = false;
    }
}

int Layer::GetLayerIndex(void) const
{
    return m_layer_index;
}