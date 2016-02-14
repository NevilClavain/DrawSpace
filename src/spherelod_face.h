/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2016                              
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

#ifndef _SPHERELOD_FACE_H_
#define _SPHERELOD_FACE_H_

#include "renderingnode.h"
#include "quadtree.h"
#include "renderer.h"
#include "spherelod_patch.h"

namespace DrawSpace
{
namespace SphericalLOD
{

class Face
{
protected:
    typedef DrawSpace::Core::CallBack<Face, void, DrawSpace::Utils::BaseQuadtreeNode*>   InstanciationCallback;
    typedef DrawSpace::Core::CallBack<Face, void, DrawSpace::Utils::BaseQuadtreeNode*>   DeletionCallback;
    typedef DrawSpace::Core::CallBack<Face, void, DrawSpace::Utils::BaseQuadtreeNode*>   SplitCallback;
    typedef DrawSpace::Core::CallBack<Face, void, DrawSpace::Utils::BaseQuadtreeNode*>   MergeCallback;

    DrawSpace::Utils::QuadtreeNode<Patch>*                      m_rootpatch; 

    DrawSpace::SphericalLOD::Config*                            m_config;

    DrawSpace::Utils::QuadtreeNode<Patch>*                      m_currentleaf;
    std::vector<Patch*>                                         m_displaylist;
    std::vector<Patch*>                                         m_work_displaylist;

    Patch*                                                      m_currentPatch;
    int                                                         m_currentPatchLOD;

    Patch*                                                      m_work_currentPatch;
    int                                                         m_work_currentPatchLOD;

    DrawSpace::Core::Texture*                                   m_perlinnoise_texture;


    int                                                         m_orientation;
    dsreal                                                      m_planet_diameter;
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

    dsreal                                                      m_lodranges[NB_LOD_RANGES];

    Patch::SubPassCreationHandler*                              m_subpasscreation_handler;

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

    Face( dsreal p_diameter, DrawSpace::SphericalLOD::Config* p_config, Patch::SubPassCreationHandler* p_handler );
    virtual ~Face( void );

    bool Init( int p_orientation );
    
    void UpdateRelativeHotpoint( const DrawSpace::Utils::Vector& p_point );

    void UpdateLODComputationParams( void );
    void UpdateLODComputationResults( void );

    void Compute( void );    
    bool ComputeAlignmentFactor( void );

    void ResetDisplayList( void );
    
    DrawSpace::Utils::QuadtreeNode<Patch>* GetCurrentLeaf( void );
    dsreal GetAlignmentFactor( void );
    
    void GetDisplayList( std::vector<Patch*>& p_displaylist );

    void SetHotState( bool p_hotstate );
    void UpdateRelativeAlt( dsreal p_alt );   

    Patch* GetCurrentPatch( void );
    int GetCurrentPatchLOD( void );

    Patch* GetRootPatch( void );

    dsreal GetHotpointGroundAltitud( void );

    void Split( int p_depth );

    void Reset( void ); // merge all

    dsreal GetRelativeAlt( void );
    dsreal GetRelativeAltSphere( void );
};
}
}

#endif
