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

#include "cdlodplanetservice.h"
#include "file.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Utils;
using namespace DrawSpace::Interface::Module;



CDLODPlanetService::CDLODPlanetService( const dsstring& p_id )
{
}

CDLODPlanetService::~CDLODPlanetService( void )
{
}

void CDLODPlanetService::GetKeys( std::vector<DrawSpace::Module::KeySinkBase*>& p_keys )
{
}

void CDLODPlanetService::Init( DrawSpace::Logger::Configuration* p_logconf, 
                            DrawSpace::Core::BaseCallback<void, bool>* p_mousecircularmode_cb, 
                            DrawSpace::Core::BaseCallback<void, bool>* p_mousevisible_cb, 
                            DrawSpace::Core::BaseCallback<void, int>* p_closeapp_cb )
{
}

void CDLODPlanetService::Run( void )
{
}

void CDLODPlanetService::Release( void )
{
}


void CDLODPlanetService::OnKeyPress( long p_key )
{
}

void CDLODPlanetService::OnEndKeyPress( long p_key )
{
}

void CDLODPlanetService::OnKeyPulse( long p_key )
{
}

void CDLODPlanetService::OnMouseMove( long p_xm, long p_ym, long p_dx, long p_dy )
{
}

void CDLODPlanetService::OnMouseLeftButtonDown( long p_xm, long p_ym )
{
}

void CDLODPlanetService::OnMouseLeftButtonUp( long p_xm, long p_ym )
{
}

void CDLODPlanetService::OnMouseRightButtonDown( long p_xm, long p_ym )
{
}

void CDLODPlanetService::OnMouseRightButtonUp( long p_xm, long p_ym )
{
}

void CDLODPlanetService::OnAppEvent( WPARAM p_wParam, LPARAM p_lParam )
{
}
