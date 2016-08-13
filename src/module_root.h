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

#ifndef _MODULE_ROOT_
#define _MODULE_ROOT_

#include "module_service.h"

namespace DrawSpace
{
namespace Interface
{
namespace Module
{
class Root
{
public:
    
    virtual dsstring                GetModuleName( void ) = 0;
    virtual dsstring                GetModuleDescr( void ) = 0;
    virtual std::vector<dsstring>   GetServicesList( void ) = 0;
    virtual Service*                InstanciateService( const dsstring& p_id ) = 0;


};
}
}
}
#endif