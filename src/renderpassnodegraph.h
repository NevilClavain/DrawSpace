/* -*-LIC_BEGIN-*- */
/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2018                        
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

#ifndef _RENDERPASSNODEGRAPH_H_
#define _RENDERPASSNODEGRAPH_H_

#include "renderpassnode.h"
#include "renderingaspectimpl.h"
#include <queue>

namespace DrawSpace
{
namespace RenderGraph
{
class RenderPassNodeGraph
{
public:
    using PassDescrTree = st_tree::tree<RenderPassNode::PassDescr*>;

private:

    typedef enum
    {
        SIGNAL_UPDATED_RENDERINGQUEUES

    } Signals;

    mutable PassDescrTree   m_tree;
    std::queue<Signals>     m_signals;


    void cleanup_treenodes( void );

public:
    RenderPassNodeGraph( void );
    ~RenderPassNodeGraph( void );

    RenderPassNode CreateRoot( const dsstring& p_name );
    void Erase( void );
    void Accept( DrawSpace::Interface::AspectImplementations::RenderingAspectImpl* p_renderingaspectimpl );

    void ProcessSignals( void );

    void PushSignal_UpdatedRenderingQueues( void );
};

}
}

#endif
