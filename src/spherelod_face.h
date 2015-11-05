/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2015                              
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

#define NB_LOD_RANGES       14


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

    DrawSpace::Utils::QuadtreeNode<Patch>*                      m_currentleaf;
    std::vector<Patch*>                                         m_displaylist;

    Patch*                                                      m_currentPatch;
    int                                                         m_currentPatchLOD;

    int                                                         m_orientation;
    dsreal                                                      m_planet_diameter;
    DrawSpace::Utils::Vector                                    m_relative_hotpoint;
    DrawSpace::Utils::Vector                                    m_cubeface_hotpoint;
    dsreal                                                      m_alignment_factor;
    bool                                                        m_hot;
    dsreal                                                      m_relative_alt;

    int                                                         m_lod_slipping_sup;
    int                                                         m_lod_slipping_inf;    

    dsreal                                                      m_lodranges[NB_LOD_RANGES];

    //Maps                                                        m_maps_factory;

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

public:

    Face( dsreal p_diameter );
    virtual ~Face( void );

    bool Init( int p_orientation );
    
    void UpdateRelativeHotpoint( const DrawSpace::Utils::Vector& p_point );

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
};
}
}

#endif
