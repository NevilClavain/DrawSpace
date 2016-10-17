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

#include "shipservice.h"

using namespace DrawSpace::Core;
using namespace DrawSpace::Interface::Module;

ShipService::ShipService( void )
{
}

ShipService::~ShipService( void )
{
}


void ShipService::GetKeys( std::vector<DrawSpace::Module::KeySinkBase*>& p_keys )
{

}

void ShipService::Init( DrawSpace::Logger::Configuration* p_logconf, DrawSpace::Core::BaseCallback<void, bool>* p_mousecircularmode_cb )
{
}

void ShipService::Run( void )
{
}

void ShipService::Release( void )
{
}

DrawSpace::Core::BaseSceneNode* ShipService::GetSceneNode( void )
{
    return NULL;
}

void ShipService::OnKeyPress( long p_key )
{
}

void ShipService::OnEndKeyPress( long p_key )
{
}

void ShipService::OnKeyPulse( long p_key )
{
}

void ShipService::OnMouseMove( long p_xm, long p_ym, long p_dx, long p_dy )
{
}

void ShipService::OnMouseLeftButtonDown( long p_xm, long p_ym )
{
}

void ShipService::OnMouseLeftButtonUp( long p_xm, long p_ym )
{
}

void ShipService::OnMouseRightButtonDown( long p_xm, long p_ym )
{
}

void ShipService::OnMouseRightButtonUp( long p_xm, long p_ym )
{
}

void ShipService::OnAppEvent( WPARAM p_wParam, LPARAM p_lParam )
{
}
