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

#pragma once

#include <set>

#include "vector.h"
#include "quadtree.h"
#include "lod_subpass.h"
#include "pass.h"

namespace LOD
{
struct Config;

// fwd decl
class HeighmapSubPass;

class Patch : public SubPass
{
public:

    static constexpr int    NorthNeighbour      = 0;
    static constexpr int    SouthNeighbour      = 1;
    static constexpr int    WestNeighbour       = 2;
    static constexpr int    EastNeighbour       = 3;
    static constexpr int    NorthWestNeighbour  = 4;
    static constexpr int    NorthEastNeighbour  = 5;
    static constexpr int    SouthWestNeighbour  = 6;
    static constexpr int    SouthEastNeighbour  = 7;

    static constexpr int    FrontPlanetFace    = 0;
    static constexpr int    RearPlanetFace     = 1;
    static constexpr int    LeftPlanetFace     = 2;
    static constexpr int    RightPlanetFace    = 3;
    static constexpr int    TopPlanetFace      = 4;
    static constexpr int    BottomPlanetFace   = 5;
    
    struct FoliagesCoordinates
    {
        FoliagesCoordinates(dsreal p_x, dsreal p_y, dsreal p_orientation, dsreal p_appearance_threshold) :
            x(p_x),
            y(p_y),
            orientation(p_orientation),
            appearance_threshold(p_appearance_threshold)
        {
        };

        dsreal x;
        dsreal y;
        dsreal orientation;
        dsreal appearance_threshold;
    };
    
    Patch( dsreal p_ray, int p_orientation, Patch* p_parent, int p_nodeid, DrawSpace::BaseQuadtreeNode* p_owner, 
            SubPass::SubPassCreationHandler* p_handler, Config* p_config, int p_layer_index, int p_nbLODRanges );

    virtual ~Patch( void );
   
    DrawSpace::BaseQuadtreeNode*                        GetNeighbour( int p_id ) const;
    dsreal                                              GetSideLength( void ) const ;
    dsreal                                              GetUnitSideLenght( void ) const ;
    int                                                 GetOrientation( void ) const ;
    dsreal                                              GetTriangleSideLength( void ) const;
    void                                                GetGlobalRelUVCoords(DrawSpace::Maths::Vector& p_uv) const;
    void                                                GetGlobalUVCoords(DrawSpace::Maths::Vector& p_uv) const ;
    void                                                GetPos( dsreal& p_xpos, dsreal& p_ypos ) const;
    void                                                GetUnitPos( dsreal& p_xpos, dsreal& p_ypos ) const;
    DrawSpace::BaseQuadtreeNode*                        GetOwner( void ) const;
    int                                                 GetLodLevel( void ) const;
    DrawSpace::Core::Texture*                           GetDataTexture( void ) const;
    Patch*                                              GetTextureReferent( void ) const;
  
    void                                                SetNeighbour(DrawSpace::BaseQuadtreeNode* p_patch, int p_id);
    void                                                ProjectVertex( const DrawSpace::Maths::Vector& p_in, DrawSpace::Maths::Vector& p_out );
    bool                                                IsCircleIntersection( dsreal p_centerx, dsreal p_centery, dsreal p_ray );

    void                                                CleanupSubpasses(void);

    void                                                AddRelatedSubpasses(HeighmapSubPass* p_subpass);
    void                                                RemoveRelatedSubpasses(HeighmapSubPass* p_subpass);


    Patch*                                              GetParent(void) const;

    void                                                SubPassDone( void );
    void                                                SubPassAborted(void);

    void                                                SetHeightMap(float* p_hm);
    bool                                                HasHeightMap(void);
    float*                                              GetHeightMap(void) const;

    int                                                 GetLayerIndex(void) const;

    std::map<int, std::vector<FoliagesCoordinates>>     GetFoliageCoordsList(void) const;

    dsreal                                              GetTemperature(void) const;
    dsreal                                              GetHumidity(void) const;

    dsstring                                            DumpInfos(void) const;

    static void                                         GetNormalVector(int p_orientation, DrawSpace::Maths::Vector& p_vector);

    static void                                         CubeToSphere( const DrawSpace::Maths::Vector& p_in, DrawSpace::Maths::Vector& p_out );
    static void                                         SphereToCube( const DrawSpace::Maths::Vector& p_in, DrawSpace::Maths::Vector& p_out );
    static void                                         XYToXYZ( int p_orientation, dsreal p_x, dsreal p_y, DrawSpace::Maths::Vector& p_out );
    
    // change les coords d'un vecteur pour obtenir son equivalent dans le repere face front, ou x et y du repere 2D correspondent
    // bien au x et y du repere 3D
    static void                                         ConvertVectorToFrontFaceCoords( int p_orientation, const DrawSpace::Maths::Vector& p_in, DrawSpace::Maths::Vector& p_out );

private:

    using SubpassDoneCb = DrawSpace::Core::CallBack<Patch, void, HeighmapSubPass*>;
    using SubpassAbortedCb = DrawSpace::Core::CallBack<Patch, void, HeighmapSubPass*>;

    Patch*                                          m_parent;
    Config*                                         m_config;

    dsreal                                          m_sidelength;
    dsreal                                          m_xpos;
    dsreal                                          m_ypos;
    dsreal                                          m_ray;

    dsreal                                          m_global_ref_u1;
    dsreal                                          m_global_ref_v1;
    dsreal                                          m_global_ref_u2;
    dsreal                                          m_global_ref_v2;

    dsreal                                          m_global_u1;
    dsreal                                          m_global_v1;
    dsreal                                          m_global_u2;
    dsreal                                          m_global_v2;


    int                                             m_orientation;
    int                                             m_nodeid;
    DrawSpace::BaseQuadtreeNode*                    m_owner;
    DrawSpace::BaseQuadtreeNode*                    m_neighbours[8];
    int                                             m_lod_level;

    DrawSpace::IntermediatePass*                    m_datatexture_pass{ nullptr };
    Patch*                                          m_texture_referent;

    int                                             m_datatexture_current_resol{ 0 };

    DrawSpace::IntermediatePass*                    m_heightmap_pass{ nullptr };

    SubPass::SubPassCreationHandler*                m_subpasscreation_handler{ nullptr };


    std::vector<EntryInfos>                         m_subpass_entry_infos_list;

    bool                                            m_enable_datatexture;
    bool                                            m_enable_foliage;

    int                                             m_nbLODRanges;

    float*                                          m_heightmap{ nullptr };

    int                                             m_layer_index;


    std::set<HeighmapSubPass*>                      m_related_subpasses; // list of subpasses working with this patch

    //std::vector<FoliagesCoordinates>              m_foliagesCoordinates;

    std::map<int,std::vector<FoliagesCoordinates>>  m_foliagesCoordinates;

    dsreal                                          m_current_temperature{ -1.0 };
    dsreal                                          m_current_humidity{ -1.0 };


    /////////////////////////////////////////////////////////////////////////////////////

    DrawSpace::IntermediatePass*                    create_data_texture_pass(int p_resol);
    void                                            prepare_data_texture(int p_layer_index, int p_resol);
    void                                            recurs_update_texture_referent(Patch* p_texture_referent);
    static float                                    half_to_float(unsigned short p_val);

};
}

