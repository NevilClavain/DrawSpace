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
    std::map<dsstring, DrawSpace::Utils::BaseQuadtreeNode*>     m_patches;
    int                                                         m_orientation;

    dsreal                                                      m_planet_diameter;
    DrawSpace::Utils::Vector                                    m_relative_hotpoint;
    DrawSpace::Utils::Vector                                    m_prev_relative_hotpoint;
    DrawSpace::Utils::QuadtreeNode<Patch>*                      m_currentleaf;

    dsreal                                                      m_ratio_split_threshold;
    dsreal                                                      m_ratio_merge_threshold;

    DrawSpace::Utils::Vector                                    m_movement;

    dsreal                                                      m_alignment_factor;

    std::map<dsstring, Patch*>                                  m_patchesleafs;

    dsreal                                                      m_currentLOD;

    //Maps                                                        m_maps_factory;

    void on_nodeinstanciation( DrawSpace::Utils::BaseQuadtreeNode* p_node );
    void on_nodedeletion( DrawSpace::Utils::BaseQuadtreeNode* p_node );
    void on_nodesplit( DrawSpace::Utils::BaseQuadtreeNode* p_node );
    void on_nodemerge( DrawSpace::Utils::BaseQuadtreeNode* p_node );

    void set_border_neighbours( DrawSpace::Utils::QuadtreeNode<Patch>* p_node );
    void unset_border_neighbours( DrawSpace::Utils::QuadtreeNode<Patch>* p_node );

    bool is_hotpoint_bound_in_node( DrawSpace::Utils::BaseQuadtreeNode* p_node, const DrawSpace::Utils::Vector& p_hotpoint );
    dsreal alt_ratio( dsreal p_altitud );

    void split_group( DrawSpace::Utils::BaseQuadtreeNode* p_node );
    void merge_group( DrawSpace::Utils::BaseQuadtreeNode* p_node );

    
    bool check_split( DrawSpace::Utils::Vector& p_hotpoint );
    bool check_merge( DrawSpace::Utils::Vector& p_hotpoint );

    DrawSpace::Utils::QuadtreeNode<Patch>* find_leaf_under( DrawSpace::Utils::QuadtreeNode<Patch>* p_current, DrawSpace::Utils::Vector& p_point );
    
    virtual void recursive_split( DrawSpace::Utils::BaseQuadtreeNode* p_node );

public:

    Face( void );
    virtual ~Face( void );

    bool Init( int p_orientation );
    virtual Patch* GetPatch( const dsstring& p_name );
    virtual void Split( const dsstring& p_name );
    virtual void Merge( const dsstring& p_name );
    virtual void SetPlanetDiameter( dsreal p_diameter );
    virtual void UpdateRelativeHotpoint( const DrawSpace::Utils::Vector& p_point );
    virtual void ComputeLOD( void );
    virtual bool ComputeAlignmentFactor( void );

    virtual DrawSpace::Utils::QuadtreeNode<Patch>* GetCurrentLeaf( void );
    virtual dsreal GetAlignmentFactor( void );

    virtual void ResetMeshe( void );

    virtual void GetLeafs( std::map<dsstring, Patch*>& p_list );

    virtual dsreal GetCurrentLOD( void );

    virtual void RecursiveSplitFromRoot( void );

    //virtual Maps* GetMapsFactory( void );
};
}
}

#endif
