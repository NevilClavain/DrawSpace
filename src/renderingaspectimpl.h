/* -*-LIC_BEGIN-*- */
/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2019                     
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

#ifndef _RENDERINGASPECTIMPL_H_
#define _RENDERINGASPECTIMPL_H_

#include "entity.h"
#include "renderingqueue.h"

namespace DrawSpace
{
namespace Utils
{
class TimeManager;
}

namespace Aspect
{
class RenderingAspect;
}

namespace RenderGraph
{
class RenderPassNodeGraph;
}

namespace EntityGraph
{
class EntityNodeGraph;
}

namespace Interface
{
namespace AspectImplementations
{
class RenderingAspectImpl abstract
{
protected:
    DrawSpace::Aspect::RenderingAspect* m_owner;
    
public:
    RenderingAspectImpl( void ) : 
    m_owner( NULL )
    {    
    }
    virtual inline void SetOwner( DrawSpace::Aspect::RenderingAspect* p_owner ) { m_owner = p_owner; };
    virtual inline bool IsText( void ) { return false; };
   
    ///////////// API ///////////////////////////////////////////

    virtual bool VisitRenderPassDescr( const dsstring& p_name, DrawSpace::Core::RenderingQueue* p_passqueue ) { return false; };
    virtual void RegisterToRendering( DrawSpace::RenderGraph::RenderPassNodeGraph& p_rendergraph ) {};
    virtual void UnregisterFromRendering( DrawSpace::RenderGraph::RenderPassNodeGraph& p_rendergraph ) {};

    virtual void ComponentsUpdated( void ) {};

    virtual bool Init( DrawSpace::Core::Entity* p_entity, DrawSpace::Utils::TimeManager* p_timemanager ) = 0;
    virtual void Release( void ) = 0;
    virtual void Run( DrawSpace::Core::Entity* p_entity ) = 0;

    virtual void SetEntityNodeGraph(EntityGraph::EntityNodeGraph* p_entitynodegraph) = 0;
};
}
}
}


#endif
