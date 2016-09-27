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

#include "skyboxservice.h"

using namespace DrawSpace::Core;
using namespace DrawSpace::Interface::Module;

SkyboxService::SkyboxService( void )
{
}

SkyboxService::~SkyboxService( void )
{
}

void SkyboxService::GetLocalKeys( const dsstring p_instanceName, std::vector<DrawSpace::Module::KeySinkBase*>& p_keys )
{

}

void SkyboxService::GetGlobalKeys( std::vector<DrawSpace::Module::KeySinkBase*>& p_keys )
{

}

void SkyboxService::Init( DrawSpace::Logger::Configuration* p_logconf, DrawSpace::Core::BaseCallback<void, bool>* p_mousecircularmode_cb )
{
}

void SkyboxService::Run( void )
{
}

void SkyboxService::Release( void )
{
}

DrawSpace::Core::BaseSceneNode* SkyboxService::GetSceneNode( void )
{
    return NULL;
}

void SkyboxService::OnKeyPress( long p_key )
{
}

void SkyboxService::OnEndKeyPress( long p_key )
{
}

void SkyboxService::OnKeyPulse( long p_key )
{
}

void SkyboxService::OnMouseMove( long p_xm, long p_ym, long p_dx, long p_dy )
{
}

void SkyboxService::OnMouseLeftButtonDown( long p_xm, long p_ym )
{
}

void SkyboxService::OnMouseLeftButtonUp( long p_xm, long p_ym )
{
}

void SkyboxService::OnMouseRightButtonDown( long p_xm, long p_ym )
{
}

void SkyboxService::OnMouseRightButtonUp( long p_xm, long p_ym )
{
}

void SkyboxService::OnAppEvent( WPARAM p_wParam, LPARAM p_lParam )
{
}
