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

Layer::Layer(DrawSpace::EntityGraph::EntityNodeGraph* p_eg, Config* p_config, Body* p_body, 
                Layer::SubPassCreationHandler* p_subpass_creation_handler, 
                CollisionMesheUpdateHandler* p_collision_meshe_update_handler,
                int p_index, bool p_freecamera) :
m_entitynodegraph(p_eg),
m_config(p_config),
m_body(p_body),
m_subpass_creation_handler(p_subpass_creation_handler),
m_collision_meshe_update_handler(p_collision_meshe_update_handler),
m_hot(false),
m_current_lod(-1),
m_meshe_collision_shape(m_hm_meshe)
{

    if (m_config->m_layers_descr[p_index].enable_collisions)
    {
        if (p_freecamera)
        {
            m_collisions = false;
        }
        else
        {
            m_collisions = true;
        }
    }
    else
    {
        m_collisions = false;
    }


    m_planetray = 1000.0 * m_config->m_layers_descr[p_index].ray;

    if (m_collisions)
    {
        for (int i = 0; i < 6; i++)
        {
            m_collisions_hms[i] = _DRAWSPACE_NEW_(Collisions, Collisions(this, p_config, i, p_index));
            m_collisions_hms[i]->Disable();
        }
    }

    m_body->Initialize();

    m_description = m_config->m_layers_descr[p_index].description;

    memset(m_alt_grid, 0, sizeof(m_alt_grid));
}

Layer::~Layer(void)
{
    if (m_collisions)
    {
        for (int i = 0; i < 6; i++)
        {
            _DRAWSPACE_DELETE_(m_collisions_hms[i]);
        }
    }
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

Layer::SubPassCreationHandler* Layer::GetSubPassCreationHandler(void) const
{
    return m_subpass_creation_handler;
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
    m_body->UpdateInvariantViewerPos( p_pos );
}

void Layer::UpdateHotPoint(const DrawSpace::Utils::Vector& p_vector)
{
    m_body->UpdateHotPoint( p_vector );
}

void Layer::Compute(void)
{
    m_body->Compute();

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////

    m_current_lod = m_body->GetFace(m_body->GetCurrentFace())->GetCurrentPatchLOD();

    auto curr_patch{ m_body->GetFace(m_body->GetCurrentFace())->GetCurrentPatch() };
    
    if (m_current_patch != curr_patch)
    {
        m_current_patch = curr_patch;        
        for (int i = 0; i < 4; i++)
        {
            if (nullptr == curr_patch->GetParent())
            {
                break;
            }
            curr_patch = curr_patch->GetParent();
        }
        
        m_collision_patch = curr_patch;

        if (m_collisions && m_collision_patch && m_current_lod == 0)
        {
            if (curr_patch->GetOrientation() == m_body->GetCurrentFace())
            {
                // ce patch appartient bien a la face "courante"

                // lance la generation de la heightmap

                std::vector<LOD::Patch*> display_list;
                display_list.push_back(m_collision_patch);

                m_draw_collidinghm = true;

                m_current_collisions_hm = m_collisions_hms[curr_patch->GetOrientation()];
                m_current_collisions_hm->Enable();

                const auto node{ static_cast<LOD::FaceDrawingNode*>(m_current_collisions_hm->GetNode()) };
                node->SetDisplayList(display_list);
            }
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////
    
    Vector view_patch_coords;
    const auto curr_face{ m_body->GetCurrentFace() };

    if (curr_face > -1 && !m_draw_collidinghm)
    {
        m_body->GetFace(m_body->GetCurrentFace())->GetCurrentPatchViewCoords(view_patch_coords);
        const auto new_alt{ get_interpolated_height(view_patch_coords[0], view_patch_coords[1]) };

        if (!isnan(new_alt))
        {
            m_currentpatch_current_height = new_alt;
        }
    }   
}

void Layer::build_meshe(DrawSpace::Core::Meshe& p_patchmeshe, LOD::Patch* p_patch, DrawSpace::Core::Meshe& p_outmeshe, float* p_heightmap)
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

            const auto x_input { (x * (Collisions::heightmapTextureSize - 1)) / (cst::patchResolution - 1) };
            const auto y_input { (y * (Collisions::heightmapTextureSize - 1)) / (cst::patchResolution - 1) };
            const auto index_hm { (Collisions::heightmapTextureSize * (Collisions::heightmapTextureSize - 1 - y_input)) + x_input };
           
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

    const auto x2 = x1 + 1;
    auto y2 = y1 + 1;

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

void Layer::SubPassDone(LOD::Collisions* p_collider)
{
    if (m_draw_collidinghm)
    {
        m_current_collisions_hm->GetHMTexture()->CopyTextureContent();

        const auto heightmap { (float*)m_current_collisions_hm->GetHMTextureContent() };

        Meshe final_meshe;
        build_meshe(*(LOD::Body::GetPatcheMeshe()), m_collision_patch, final_meshe, heightmap);
        m_hm_meshe = final_meshe;

        for (auto& e : m_collision_meshe_creation_handler)
        {
            (*e)(m_hm_meshe);
        }

        const auto shape_component_name{ "shape_" + std::to_string((long)this) };
        (*m_collision_meshe_update_handler)(shape_component_name, m_meshe_collision_shape, true);

        m_draw_collidinghm = false;
        m_current_collisions_hm->Disable();

        m_current_collisions_hm = nullptr;    

        m_collisions_active = true;
    }
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
