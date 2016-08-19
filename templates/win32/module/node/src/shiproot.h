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

#ifndef _SHIPROOT_H_
#define _SHIPROOT_H_

#include "module_root.h"
#include "shipservice.h"


class ShipRoot : public DrawSpace::Interface::Module::Root
{
protected:

    std::map<dsstring, DrawSpace::Interface::Module::Service*> m_services;


public:
    ShipRoot( void );
    virtual ~ShipRoot( void );

    virtual dsstring GetModuleName( void );
    virtual dsstring GetModuleDescr( void );
    virtual std::vector<dsstring> GetServicesList( void );
    virtual DrawSpace::Interface::Module::Service* InstanciateService( const dsstring& p_id );

};

#endif