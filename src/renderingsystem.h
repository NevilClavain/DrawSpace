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

#ifndef _RENDERINGSYSTEM_H_
#define _RENDERINGSYSTEM_H_

#include "renderpassnodegraph.h"
#include "entitynodegraph.h"

#include "plugin.h"
#include "renderer.h"

namespace DrawSpace
{
namespace Core
{

class RenderingSystem
{
protected:

    DrawSpace::Interface::Renderer* m_renderer;

public:
    RenderingSystem(void);
    ~RenderingSystem(void);

    void Run( RenderPassNodeGraph* p_rendergraph, EntityNodeGraph* p_entitygraph );

    void VisitRenderPassDescr( RenderPassNode::PassDescr* p_passdescr ) const;
    void VisitEntity( Entity* p_entity ) const;

};

}
}

#endif