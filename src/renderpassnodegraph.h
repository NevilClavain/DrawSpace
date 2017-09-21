/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2017                        
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

#ifndef _RENDERPASSNODEGRAPH_H_
#define _RENDERPASSNODEGRAPH_H_

#include "renderpassnode.h"
#include "renderingaspectimpl.h"

namespace DrawSpace
{
namespace Core
{
class PassesRenderingAspectImpl;
class RenderPassNodeGraph sealed
{
public:
    using PassDescrTree = st_tree::tree<RenderPassNode::PassDescr*>;

private:

    mutable PassDescrTree  m_tree;
    void cleanup_treenodes( void );

public:

    RenderPassNodeGraph( void );
    ~RenderPassNodeGraph( void );

    RenderPassNode CreateRoot( const dsstring& p_name );
    void Erase( void );
    void Accept( RenderingAspectImpl* p_renderingaspectimpl );


    void RenderingQueueModSignal( void );
};

}
}

#endif