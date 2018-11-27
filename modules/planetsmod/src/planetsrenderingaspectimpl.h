#pragma once

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

#include "renderingaspectimpl.h"
#include "renderer.h"
#include "plugin.h"
#include "renderingnode.h"
#include "renderpassnodegraph.h"

namespace DrawSpace
{
namespace AspectImplementations
{
class PlanetsRenderingAspectImpl : public DrawSpace::Interface::AspectImplementations::RenderingAspectImpl
{
protected:

public:
    
protected:

    bool        m_add_in_rendergraph;

    void        init_rendering_objects(void);
    void        release_rendering_objects(void);

    void        update_shader_params(void); // for all passes

   
public:
    PlanetsRenderingAspectImpl( void );

    bool VisitRenderPassDescr( const dsstring& p_name, DrawSpace::Core::RenderingQueue* p_passqueue );

    void RegisterToRendering( DrawSpace::RenderGraph::RenderPassNodeGraph& p_rendergraph );
    void UnregisterFromRendering( DrawSpace::RenderGraph::RenderPassNodeGraph& p_rendergraph );

    virtual bool Init( DrawSpace::Core::Entity* p_entity ) { return true; };
    virtual void Run( DrawSpace::Core::Entity* p_entity );
};
}
}

