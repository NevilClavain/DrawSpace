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


FooParam::FooParam( const dsstring& p_id, SkyboxService* p_owner ) : KeySink( p_id ), m_owner( p_owner )
{
}

void FooParam::OnUpdated( int p_val )
{
    m_owner->OnFooUpdated( p_val );
}

SceneNodeGraphParam::SceneNodeGraphParam( const dsstring& p_id, SkyboxService* p_owner ) : KeySink( p_id ), m_owner( p_owner )
{
}

void SceneNodeGraphParam::OnUpdated( DrawSpace::Core::SceneNodeGraph* p_val )
{
    m_owner->OnSceneNodeGraphUpdated( p_val );
}


SkyboxService::SkyboxService( const dsstring& p_id ) :
m_scenenodegraph( NULL )
{
    m_foo = _DRAWSPACE_NEW_( FooParam, FooParam( p_id + dsstring( ".foo" ), this ) );
    m_scparam = _DRAWSPACE_NEW_( SceneNodeGraphParam, SceneNodeGraphParam( p_id + dsstring( ".SceneNodeGraph" ), this ) );
}

SkyboxService::~SkyboxService( void )
{
}

void SkyboxService::GetKeys( std::vector<DrawSpace::Module::KeySinkBase*>& p_keys )
{
    p_keys.push_back( m_foo );
    p_keys.push_back( m_scparam );
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

void SkyboxService::OnFooUpdated( int p_value )
{
    _asm nop
}

void SkyboxService::OnSceneNodeGraphUpdated( DrawSpace::Core::SceneNodeGraph* p_val )
{
    m_scenenodegraph = p_val;
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
