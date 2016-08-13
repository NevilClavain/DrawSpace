/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2016                              
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

#ifndef _MODULE_SERVICE_
#define _MODULE_SERVICE_

#include "scenenode.h"

namespace DrawSpace
{
namespace Interface
{
namespace Module
{
class Service
{
public:
    
    virtual void                            Init( void ) = 0;
    virtual void                            Run( void ) = 0;
    virtual void                            Release( void ) = 0;
    virtual DrawSpace::Core::BaseSceneNode* GetSceneNode( void ) = 0;

};
}
}
}
#endif