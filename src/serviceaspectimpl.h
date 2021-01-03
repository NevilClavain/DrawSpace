/* -*-LIC_BEGIN-*- */
/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2021                     
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

#ifndef _SERVICEASPECTIMPL_H_
#define _SERVICEASPECTIMPL_H_

#include "drawspace_commons.h"

namespace DrawSpace
{
namespace Aspect
{
class ServiceAspect;
}
namespace Interface
{
namespace AspectImplementations
{
class ServiceAspectImpl abstract
{
protected:
    DrawSpace::Aspect::ServiceAspect* m_owner;

public:
    virtual void SetOwner( DrawSpace::Aspect::ServiceAspect* p_owner ) { m_owner = p_owner; };

    virtual bool Init( void ) = 0;
    virtual void Run( void ) = 0;
    virtual void Release( void ) = 0;

    virtual void OnKeyPress( long p_key ) {};
    virtual void OnEndKeyPress( long p_key ) {};
    virtual void OnKeyPulse( long p_key ) {};
    virtual void OnChar( long p_char, long p_scan ) {};
    virtual void OnMouseMove( long p_xm, long p_ym, long p_dx, long p_dy ) {};
    virtual void OnMouseWheel( long p_delta ) {};
    virtual void OnMouseLeftButtonDown( long p_xm, long p_ym ) {};
    virtual void OnMouseLeftButtonUp( long p_xm, long p_ym ) {};
    virtual void OnMouseRightButtonDown( long p_xm, long p_ym ) {};
    virtual void OnMouseRightButtonUp( long p_xm, long p_ym ) {};
    virtual void OnAppEvent( WPARAM p_wParam, LPARAM p_lParam ) {};
};
}
}
}

#endif
