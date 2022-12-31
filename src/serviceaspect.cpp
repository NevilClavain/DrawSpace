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

#include "serviceaspect.h"
#include "entity.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Aspect;
using namespace DrawSpace::Interface;

ServiceAspect::ServiceAspect( void )
{
}

void ServiceAspect::AddImplementation( AspectImplementations::ServiceAspectImpl* p_impl )
{
    m_impls.push_back( p_impl );
    p_impl->SetOwner( this );

    p_impl->Init();
}

void ServiceAspect::RemoveImplementation(DrawSpace::Interface::AspectImplementations::ServiceAspectImpl* p_impl)
{
    for (auto it = m_impls.begin(); it != m_impls.end(); ++it)
    {
        if (*it == p_impl)
        {
            p_impl->Release();
            m_impls.erase(it);
            break;
        }
    }
}

void ServiceAspect::Run( void )
{
    for( size_t i = 0; i < m_impls.size(); i++ )
    {
        m_impls[i]->Run();
    }
}

void ServiceAspect::OnKeyPress( long p_key )
{
    for( size_t i = 0; i < m_impls.size(); i++ )
    {
        m_impls[i]->OnKeyPress( p_key );
    }
}

void ServiceAspect::OnEndKeyPress( long p_key )
{
    for( size_t i = 0; i < m_impls.size(); i++ )
    {
        m_impls[i]->OnEndKeyPress( p_key );
    }
}

void ServiceAspect::OnKeyPulse( long p_key )
{
    for( size_t i = 0; i < m_impls.size(); i++ )
    {
        m_impls[i]->OnKeyPulse( p_key );
    }
}

void ServiceAspect::OnChar( long p_char, long p_scan )
{
    for( size_t i = 0; i < m_impls.size(); i++ )
    {
        m_impls[i]->OnChar( p_char, p_scan );
    }
}

void ServiceAspect::OnMouseMove( long p_xm, long p_ym, long p_dx, long p_dy )
{
    for( size_t i = 0; i < m_impls.size(); i++ )
    {
        m_impls[i]->OnMouseMove( p_xm, p_ym, p_dx, p_dy );
    }
}

void ServiceAspect::OnMouseWheel( long p_delta )
{
    for( size_t i = 0; i < m_impls.size(); i++ )
    {
        m_impls[i]->OnMouseWheel( p_delta );
    }
}

void ServiceAspect::OnMouseLeftButtonDown( long p_xm, long p_ym )
{
    for( size_t i = 0; i < m_impls.size(); i++ )
    {
        m_impls[i]->OnMouseLeftButtonDown( p_xm, p_ym );
    }
}

void ServiceAspect::OnMouseLeftButtonUp( long p_xm, long p_ym )
{
    for( size_t i = 0; i < m_impls.size(); i++ )
    {
        m_impls[i]->OnMouseLeftButtonUp( p_xm, p_ym );
    }
}

void ServiceAspect::OnMouseRightButtonDown( long p_xm, long p_ym )
{
    for( size_t i = 0; i < m_impls.size(); i++ )
    {
        m_impls[i]->OnMouseRightButtonDown( p_xm, p_ym );
    }
}

void ServiceAspect::OnMouseRightButtonUp( long p_xm, long p_ym )
{
    for( size_t i = 0; i < m_impls.size(); i++ )
    {
        m_impls[i]->OnMouseRightButtonUp( p_xm, p_ym );
    }
}

void ServiceAspect::OnAppEvent( WPARAM p_wParam, LPARAM p_lParam )
{
    for( size_t i = 0; i < m_impls.size(); i++ )
    {
        m_impls[i]->OnAppEvent( p_wParam, p_lParam );
    }
}
