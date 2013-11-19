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

#include "renderingnode.h"
#include "quadtree.h"
#include "renderer.h"
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


    DrawSpace::Utils::QuadtreeNode<Patch>*                      m_rootpatch;    
    std::map<dsstring, DrawSpace::Utils::BaseQuadtreeNode*>     m_patches;
    int                                                         m_orientation;
    PatchInstanciationHandler*                                  m_inst_handler;
    PatchSplitHandler*                                          m_split_handler;
    PatchDeletionHandler*                                       m_del_handler;
    PatchMergeHandler*                                          m_merge_handler;

    void on_nodeinstanciation( DrawSpace::Utils::BaseQuadtreeNode* p_node );
    void on_nodedeletion( DrawSpace::Utils::BaseQuadtreeNode* p_node );
    void on_nodesplit( DrawSpace::Utils::BaseQuadtreeNode* p_node );
    void on_nodemerge( DrawSpace::Utils::BaseQuadtreeNode* p_node );

public:
    Face( PatchInstanciationHandler* p_inst_handler, PatchSplitHandler* p_split_handler );
    virtual ~Face( void );

    bool Init( int p_orientation );
    virtual Patch* GetPatch( const dsstring& p_name );
    virtual void Split( const dsstring& p_name );
};

#endif