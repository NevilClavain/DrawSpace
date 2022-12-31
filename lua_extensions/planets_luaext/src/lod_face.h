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

#include "csts.h"
#include "renderingnode.h"
#include "quadtree.h"
#include "renderer.h"
#include "lod_patch.h"

namespace LOD
{
struct Config;

class Face
{
protected:
    using InstanciationCallback = DrawSpace::Core::CallBack<Face, void, DrawSpace::Utils::BaseQuadtreeNode*>;
    using DeletionCallback      = DrawSpace::Core::CallBack<Face, void, DrawSpace::Utils::BaseQuadtreeNode*>;
    using SplitCallback         = DrawSpace::Core::CallBack<Face, void, DrawSpace::Utils::BaseQuadtreeNode*>;
    using MergeCallback         = DrawSpace::Core::CallBack<Face, void, DrawSpace::Utils::BaseQuadtreeNode*>;

    InstanciationCallback*                                      m_cb_inst;
    DeletionCallback*                                           m_cb_del;
    InstanciationCallback*                                      m_cb_merge;
    DeletionCallback*                                           m_cb_split;

    DrawSpace::Utils::QuadtreeNode<Patch>*                      m_rootpatch; 

    Config*                                                     m_config;

    DrawSpace::Utils::QuadtreeNode<Patch>*                      m_currentleaf;
    std::vector<Patch*>                                         m_displaylist;
    std::vector<Patch*>                                         m_work_displaylist;

    Patch*                                                      m_currentPatch;
    DrawSpace::Utils::Vector                                    m_currentPatchViewCoords;
    int                                                         m_currentPatchLOD;

    Patch*                                                      m_work_currentPatch;
    int                                                         m_work_currentPatchLOD;

    int                                                         m_orientation;
    dsreal                                                      m_diameter;
    DrawSpace::Utils::Vector                                    m_relative_hotpoint;
    DrawSpace::Utils::Vector                                    m_cubeface_hotpoint;
    DrawSpace::Utils::Vector                                    m_work_cubeface_hotpoint;
    dsreal                                                      m_alignment_factor;
    bool                                                        m_hot;
    dsreal                                                      m_relative_alt;
    dsreal                                                      m_relative_alt_sphere;

    int                                                         m_lod_slipping_sup;
    int                                                         m_lod_slipping_inf;    

    int                                                         m_work_lod_slipping_sup;
    int                                                         m_work_lod_slipping_inf;    

    dsreal                                                      m_lodranges[cst::nbLODRangesMax];

    Patch::SubPassCreationHandler*                              m_subpasscreation_handler;

    int                                                         m_min_lodlevel;

    int                                                         m_layer_index;

    int                                                         m_nbLODRanges;

    void on_nodeinstanciation( DrawSpace::Utils::BaseQuadtreeNode* p_node );
    void on_nodedeletion( DrawSpace::Utils::BaseQuadtreeNode* p_node );
    void on_nodesplit( DrawSpace::Utils::BaseQuadtreeNode* p_node );
    void on_nodemerge( DrawSpace::Utils::BaseQuadtreeNode* p_node );

    void set_border_neighbours( DrawSpace::Utils::QuadtreeNode<Patch>* p_node );
    void unset_border_neighbours( DrawSpace::Utils::QuadtreeNode<Patch>* p_node );

    bool is_hotpoint_bound_in_node( DrawSpace::Utils::BaseQuadtreeNode* p_node );
    DrawSpace::Utils::QuadtreeNode<Patch>* find_leaf_under( DrawSpace::Utils::QuadtreeNode<Patch>* p_current );

    void compute_cubeface_hotpoint( void );
    
    bool recursive_build_displaylist( DrawSpace::Utils::BaseQuadtreeNode* p_current_node, int p_lodlevel );
   
    void init_lodranges( void );

    void recursive_split( DrawSpace::Utils::BaseQuadtreeNode* p_currpatch, int p_dest_depth, int p_current_depth );

    void recursive_merge( DrawSpace::Utils::BaseQuadtreeNode* p_currpatch );

public:

    Face( Config* p_config, int p_layer_index, Patch::SubPassCreationHandler* p_handler, int p_nbLODRanges );
    virtual ~Face( void );

    bool Init( int p_orientation );

    DrawSpace::Utils::QuadtreeNode<Patch>*  GetCurrentLeaf(void) const;
    dsreal                                  GetAlignmentFactor(void) const;
    void                                    GetDisplayList(std::vector<Patch*>& p_displaylist) const;
    Patch*                                  GetCurrentPatch(void) const;
    void                                    GetCurrentPatchViewCoords(DrawSpace::Utils::Vector& p_outcoords) const;
    int                                     GetCurrentPatchLOD(void) const;
    Patch*                                  GetRootPatch(void) const;
    dsreal                                  GetRelativeAlt(void) const;
    dsreal                                  GetRelativeAltSphere(void) const;
    void                                    GetCurrentCubeFaceHotPoint(DrawSpace::Utils::Vector& p_cubeface_hotpoint) const;
    void                                    GetRelativeHotPoint(DrawSpace::Utils::Vector& p_rhotpoint) const;
    
    // appele par Body::UpdateHotPoint()
    void                                    UpdateRelativeHotpoint( const DrawSpace::Utils::Vector& p_point );

    // appel� par Body::UpdateRelativeAlt()
    void                                    UpdateRelativeAlt(dsreal p_alt);

    // appel� par Body::Compute()
    void                                    UpdateLODComputationParams(void);
    void                                    UpdateLODComputationResults(void);
    void                                    Compute( void );
    bool                                    ComputeAlignmentFactor(void);



    void                                    ResetDisplayList( void );    
    void                                    SetHotState( bool p_hotstate );

    // 3/2/2019 : pas sur que ca serve... 
    /*
    void                                    Split( int p_depth );
    */
    void                                    Reset( void ); // merge all
    

};
}
