/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2015                              
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

#ifndef _SPHERELOD_SUBPASS_H_
#define _SPHERELOD_SUBPASS_H_

#include "pass.h"
#include "renderingnode.h"

namespace DrawSpace
{
namespace SphericalLOD
{
class SubPass
{
protected:

    DrawSpace::Core::RenderingNode*             m_subpass_node;
    DrawSpace::IntermediatePass*                m_subpass;

public:

    SubPass( void );
    virtual ~SubPass( void );

    virtual void                                        Draw( void );
    virtual void                                        SubPassDone( void ) = 0;
};
}
}
#endif