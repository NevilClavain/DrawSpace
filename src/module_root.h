/* -*-LIC_BEGIN-*- */
/*
*
* DrawSpace Rendering engine
* Emmanuel Chaumont Copyright (c) 2013-2023
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

#pragma once

#include "plugin.h"

#include "renderingaspectimpl.h"
#include "transformaspectimpl.h"
#include "serviceaspectimpl.h"


namespace DrawSpace
{
namespace Utils
{
class MemAlloc;
}


namespace Systems
{
class Hub;
}

namespace Interface
{
class Renderer;

namespace Module
{
class Root
{
protected:

    dsstring                                                    m_id;           //identifiant instance de module
    Systems::Hub*                                               m_hub;

public:
    Root( void );

    virtual void                                                                UpdateRenderer( DrawSpace::Interface::Renderer* p_renderer );

    virtual void                                                                SetInstanceId( const dsstring& p_id ) { m_id = p_id; };

    virtual void                                                                SetHub(Systems::Hub* p_hub);

    virtual dsstring                                                            GetModuleName( void ) const = 0;
    virtual dsstring                                                            GetModuleDescr( void ) const = 0;
    
    virtual void                                                                Init( void ) = 0;
    virtual void                                                                Release(void) = 0;

    virtual void                                                                DumpMemoryAllocs( void );

    virtual DrawSpace::Interface::AspectImplementations::ServiceAspectImpl*     InstanciateServiceAspectImpl( const dsstring& p_id );
    virtual void                                                                TrashServiceAspectImpls( DrawSpace::Interface::AspectImplementations::ServiceAspectImpl* p_impl ) {};

    virtual Utils::MemAlloc*                                                    GetMemAllocInstance( void ) const;
};
}
}
}

