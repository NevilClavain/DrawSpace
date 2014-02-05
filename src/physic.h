/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2014                                
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

#ifndef _PHYSIC_H_
#define _PHYSIC_H_

#include "drawspace_commons.h"

namespace DrawSpace
{
namespace Interface
{
class Physic
{
public:

    virtual void GetDescr( dsstring& p_descr ) = 0;
    virtual void DumpMemoryAllocs( void ) = 0;

    virtual bool Init( void ) = 0;
    virtual void Release( void ) = 0;



};
}
}
#endif

