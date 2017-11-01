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

#ifndef _SYSTEMS_H_
#define _SYSTEMS_H_

#include "entitynodegraph.h"

namespace DrawSpace
{
namespace Interface
{
class System
{
public:
    virtual void Init( void ) = 0;
    virtual void Release( void ) = 0;
    virtual void Run( EntityGraph::EntityNodeGraph* p_entitygraph ) = 0;
    virtual void VisitEntity( Core::Entity* p_parent, Core::Entity* p_entity ) = 0;
};
}
}

#endif