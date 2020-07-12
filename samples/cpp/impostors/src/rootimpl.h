/* -*-LIC_BEGIN-*- */
/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2020                     
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

#ifndef _ROOTIMPL_H_
#define _ROOTIMPL_H_

#include "module_root.h"

class RootImpl : public DrawSpace::Interface::Module::Root
{
public:
    RootImpl( void );
    virtual ~RootImpl( void );

    virtual dsstring GetModuleName( void ) const;
    virtual dsstring GetModuleDescr( void ) const;
    virtual void Init( void );
    virtual void Release(void);
    DrawSpace::Interface::AspectImplementations::ServiceAspectImpl* InstanciateServiceAspectImpl( const dsstring& p_id );
};

#endif
