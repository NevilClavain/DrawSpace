/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2014                                
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

#ifndef _PLANET_FACE_H_
#define _PLANET_FACE_H_

#include <renderingnode.h>
#include <quadtree.h>
#include <renderer.h>
#include <mutex.h>
#include "planet_patch.h"

class Face
{
public:
    typedef DrawSpace::Core::BaseCallback2<void, int, Patch*>                            PatchInstanciationHandler;
    typedef DrawSpace::Core::BaseCallback2<void, int, Patch*>                            PatchSplitHandler;
    typedef DrawSpace::Core::BaseCallback2<void, int, Patch*>                            PatchDeletionHandler;
    typedef DrawSpace::Core::BaseCallback2<void, int, Patch*>                            PatchMergeHandler;

protected:
    typedef DrawSpace::Core::CallBack<Face, void, DrawSpace::Utils::BaseQuadtreeNode*>   InstanciationCallback;
    typedef DrawSpace::Core::CallBack<Face, void, DrawSpace::Utils::BaseQuadtreeNode*>   DeletionCallback;
    typedef DrawSpace::Core::CallBack<Face, void, DrawSpace::Utils::BaseQuadtreeNode*>   SplitCallback;
    typedef DrawSpace::Core::CallBack<Face, void, DrawSpace::Utils::BaseQuadtreeNode*>   MergeCallback;

    //static const int patchresol = 11;


    DrawSpace::Utils::QuadtreeNode<Patch>*                      m_rootpatch;    
    std::map<dsstring, DrawSpace::Utils::BaseQuadtreeNode*>     m_patches;
    int                                                         m_orientation;

    std::vector<PatchInstanciationHandler*>                     m_inst_handlers;
    std::vector<PatchSplitHandler*>                             m_split_handlers;
    std::vector<PatchDeletionHandler*>                          m_del_handlers;
    std::vector<PatchMergeHandler*>                             m_merge_handlers;


    dsreal                                                      m_planet_diameter;
    DrawSpace::Utils::Vector                                    m_relative_hotpoint;
    DrawSpace::Utils::Vector                                    m_prev_relative_hotpoint;
    DrawSpace::Utils::QuadtreeNode<Patch>*                      m_currentleaf;

    dsreal                                                      m_ratio_split_threshold;
    dsreal                                                      m_ratio_merge_threshold;

    DrawSpace::Utils::Vector                                    m_movement;

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

public:

    Face( void );
    virtual ~Face( void );

    bool Init( int p_orientation );
    virtual Patch* GetPatch( const dsstring& p_name );
    virtual void Split( const dsstring& p_name );
    virtual void Merge( const dsstring& p_name );
    virtual void SetPlanetDiameter( dsreal p_diameter );
    virtual void UpdateRelativeHotpoint( const DrawSpace::Utils::Vector& p_point );
    //virtual bool Compute( void );

    virtual void AddInstHandler( PatchInstanciationHandler* p_handler );
    virtual void AddSplitHandler( PatchSplitHandler* p_handler );
    virtual void AddDelHandler( PatchDeletionHandler* p_handler );
    virtual void AddMergeHandler( PatchMergeHandler* p_handler );
    virtual DrawSpace::Utils::QuadtreeNode<Patch>* GetCurrentLeaf( void );
};

#endif