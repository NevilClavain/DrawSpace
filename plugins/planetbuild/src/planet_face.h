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

class Face : public DrawSpace::Core::RenderingNode
{
protected:

    typedef DrawSpace::Core::CallBack<Face, void, DrawSpace::Utils::BaseQuadtreeNode*>   InstanciationCallback;

    DrawSpace::Utils::QuadtreeNode<Patch>*                      m_rootpatch;
    std::map<dsstring, DrawSpace::Utils::BaseQuadtreeNode*>     m_patchesleafs;
    std::map<dsstring, Patch*>                                  m_patches;
    int                                                         m_orientation;
    DrawSpace::Interface::Renderer*                             m_renderer;

    void on_nodeinstanciation( DrawSpace::Utils::BaseQuadtreeNode* p_node );

public:
    Face( DrawSpace::Interface::Renderer* p_renderer );
    virtual ~Face( void );

    bool Init( int p_orientation );
    void Draw( const DrawSpace::Utils::Matrix& p_world, DrawSpace::Utils::Matrix& p_view );
    void Split( const dsstring& p_patchname );

    DrawSpace::Core::Meshe* GetPatchFromName( const dsstring& p_name );

};

#endif