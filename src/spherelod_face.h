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
    std::map<dsstring, Patch*>                                  m_patchesleafs;
    DrawSpace::Utils::QuadtreeNode<Patch>*                      m_currentleaf;
    std::vector<Patch*>                                         m_displaylist;

    int                                                         m_orientation;
    dsreal                                                      m_planet_diameter;
    DrawSpace::Utils::Vector                                    m_relative_hotpoint;
    DrawSpace::Utils::Vector                                    m_cubeface_hotpoint;
    DrawSpace::Utils::Vector                                    m_prev_relative_hotpoint;
    
    DrawSpace::Utils::Vector                                    m_movement;

    dsreal                                                      m_alignment_factor;

    dsreal                                                      m_currentLOD;

    bool                                                        m_hot;

    //Maps                                                        m_maps_factory;

    void on_nodeinstanciation( DrawSpace::Utils::BaseQuadtreeNode* p_node );
    void on_nodedeletion( DrawSpace::Utils::BaseQuadtreeNode* p_node );
    void on_nodesplit( DrawSpace::Utils::BaseQuadtreeNode* p_node );
    void on_nodemerge( DrawSpace::Utils::BaseQuadtreeNode* p_node );

    void set_border_neighbours( DrawSpace::Utils::QuadtreeNode<Patch>* p_node );
    void unset_border_neighbours( DrawSpace::Utils::QuadtreeNode<Patch>* p_node );

    bool is_hotpoint_bound_in_node( DrawSpace::Utils::BaseQuadtreeNode* p_node, const DrawSpace::Utils::Vector& p_hotpoint );
    DrawSpace::Utils::QuadtreeNode<Patch>* find_leaf_under( DrawSpace::Utils::QuadtreeNode<Patch>* p_current, DrawSpace::Utils::Vector& p_point );
    
    virtual void recursive_split( DrawSpace::Utils::BaseQuadtreeNode* p_node );

    void compute_cubeface_hotpoint( void );

public:

    Face( void );
    virtual ~Face( void );

    bool Init( int p_orientation );
    Patch* GetPatch( const dsstring& p_name );
    void SetPlanetDiameter( dsreal p_diameter );
    void UpdateRelativeHotpoint( const DrawSpace::Utils::Vector& p_point );
    void ComputeLOD( void );
    bool ComputeAlignmentFactor( void );
    
    DrawSpace::Utils::QuadtreeNode<Patch>* GetCurrentLeaf( void );
    dsreal GetAlignmentFactor( void );
    

    void GetLeafs( std::map<dsstring, Patch*>& p_list );
    void GetDisplayList( std::vector<Patch*>& p_displaylist );

    dsreal GetCurrentLOD( void );

    void RecursiveSplitFromRoot( void );

    void SetHotState( bool p_hotstate );

    //virtual Maps* GetMapsFactory( void );
};
}
}

#endif
