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


#ifndef _TRANSFORMSYSTEM_H_
#define _TRANSFORMSYSTEM_H_

#include "systems.h"
#include "entitynodegraph.h"
#include "matrix.h"
#include "callback.h"

namespace DrawSpace
{
namespace Systems
{
class TransformSystem : public Interface::System
{
protected:

    int                                 m_step;   

    DrawSpace::Utils::Matrix            m_viewtransform_todispatch;
    DrawSpace::Utils::Matrix            m_projtransform_todispatch;

public:
    TransformSystem( void );
    ~TransformSystem( void );

    bool Init( EntityGraph::EntityNodeGraph* p_entitygraph ) { return true; };
    void Release( EntityGraph::EntityNodeGraph* p_entitygraph ) {};

    void Run( EntityGraph::EntityNodeGraph* p_entitygraph );
    void VisitEntity( Core::Entity* p_parent, Core::Entity* p_entity );
};
}
}

#endif
