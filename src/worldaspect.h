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

#ifndef _WORLDASPECT_H_
#define _WORLDASPECT_H_

#include "worldaspectimpl.h"
#include "entity.h"


namespace DrawSpace
{
namespace Core
{
class WorldAspect : public Aspect
{
protected:
    DrawSpace::Utils::Matrix                                                        m_worldtransform;
    std::vector<DrawSpace::Interface::AspectImplementations::WorldAspectImpl*>      m_impls;

public:
    WorldAspect( void );

    void AddImplementation( DrawSpace::Interface::AspectImplementations::WorldAspectImpl* p_impl );
    
    void GetWorldTransform( DrawSpace::Utils::Matrix& p_worldtransform );

    void ComputeTransforms( Entity* p_parent, Entity* p_entity );
};
}
}

#endif