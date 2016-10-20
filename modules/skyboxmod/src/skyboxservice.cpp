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

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Interface::Module;

/////////////////////////////

SceneNodeGraphParam::SceneNodeGraphParam( const dsstring& p_id, SkyboxService* p_owner ) : KeySink( p_id ), m_owner( p_owner )
{
}

void SceneNodeGraphParam::OnUpdated( DrawSpace::Core::SceneNodeGraph* p_val )
{
    m_owner->OnSceneNodeGraphUpdated( p_val );
}

/////////////////////////////

TexturePassParam::TexturePassParam( const dsstring& p_id, SkyboxService* p_owner ) : KeySink( p_id ), m_owner( p_owner )
{
}

void TexturePassParam::OnUpdated( DrawSpace::IntermediatePass* p_val )
{
    m_owner->OnTexturePassUpdate( p_val );
}

/////////////////////////////

TextureMirrorPassParam::TextureMirrorPassParam( const dsstring& p_id, SkyboxService* p_owner ) : KeySink( p_id ), m_owner( p_owner )
{
}

void TextureMirrorPassParam::OnUpdated( DrawSpace::IntermediatePass* p_val )
{
    m_owner->OnTextureMirrorPassUpdate( p_val );
}

/////////////////////////////

SkyboxService::SkyboxService( const dsstring& p_id ) :
m_scenenodegraph( NULL ),
m_texturepass( NULL ),
m_texturemirrorpass( NULL )
{
    m_scparam = _DRAWSPACE_NEW_( SceneNodeGraphParam, SceneNodeGraphParam( p_id + dsstring( ".SceneNodeGraph" ), this ) );
    m_texturepassparam = _DRAWSPACE_NEW_( TexturePassParam, TexturePassParam( p_id + dsstring( ".TexturePass" ), this ) );
    m_texturemirrorpassparam = _DRAWSPACE_NEW_( TextureMirrorPassParam, TextureMirrorPassParam( p_id + dsstring( ".TextureMirrorPass" ), this ) );

    m_spacebox = _DRAWSPACE_NEW_( DrawSpace::Spacebox, DrawSpace::Spacebox );
}

SkyboxService::~SkyboxService( void )
{
}

void SkyboxService::GetKeys( std::vector<DrawSpace::Module::KeySinkBase*>& p_keys )
{
    p_keys.push_back( m_scparam );
    p_keys.push_back( m_texturepassparam );
    p_keys.push_back( m_texturemirrorpassparam );
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

void SkyboxService::OnSceneNodeGraphUpdated( DrawSpace::Core::SceneNodeGraph* p_val )
{
    m_scenenodegraph = p_val;
}

void SkyboxService::OnTexturePassUpdate( DrawSpace::IntermediatePass* p_val )
{
    m_spacebox->RegisterPassSlot( p_val );
    m_texturepass = p_val;

    for( long i = 0; i < 6; i++ )
    {
        m_spacebox->GetNodeFromPass( m_texturepass, i )->SetFx( _DRAWSPACE_NEW_( Fx, Fx ) );

        m_spacebox->GetNodeFromPass( m_texturepass, i )->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture.vso", true ) ) );
        m_spacebox->GetNodeFromPass( m_texturepass, i )->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture.pso", true ) ) );

        m_spacebox->GetNodeFromPass( m_texturepass, i )->GetFx()->GetShader( 0 )->LoadFromFile();
        m_spacebox->GetNodeFromPass( m_texturepass, i )->GetFx()->GetShader( 1 )->LoadFromFile();
    }  
    
    m_spacebox->GetNodeFromPass( m_texturepass, Spacebox::FrontQuad )->SetTexture( _DRAWSPACE_NEW_( Texture, Texture( "sb0.bmp" ) ), 0 );
    m_spacebox->GetNodeFromPass( m_texturepass, Spacebox::FrontQuad )->GetTexture( 0 )->LoadFromFile();

    m_spacebox->GetNodeFromPass( m_texturepass, Spacebox::RearQuad )->SetTexture( _DRAWSPACE_NEW_( Texture, Texture( "sb2.bmp" ) ), 0 );
    m_spacebox->GetNodeFromPass( m_texturepass, Spacebox::RearQuad )->GetTexture( 0 )->LoadFromFile();

    m_spacebox->GetNodeFromPass( m_texturepass, Spacebox::TopQuad )->SetTexture( _DRAWSPACE_NEW_( Texture, Texture( "sb4.bmp" ) ), 0 );
    m_spacebox->GetNodeFromPass( m_texturepass, Spacebox::TopQuad )->GetTexture( 0 )->LoadFromFile();

    m_spacebox->GetNodeFromPass( m_texturepass, Spacebox::BottomQuad )->SetTexture( _DRAWSPACE_NEW_( Texture, Texture( "sb4.bmp" ) ), 0 );
    m_spacebox->GetNodeFromPass( m_texturepass, Spacebox::BottomQuad )->GetTexture( 0 )->LoadFromFile();

    m_spacebox->GetNodeFromPass( m_texturepass, Spacebox::LeftQuad )->SetTexture( _DRAWSPACE_NEW_( Texture, Texture( "sb3.bmp" ) ), 0 );
    m_spacebox->GetNodeFromPass( m_texturepass, Spacebox::LeftQuad )->GetTexture( 0 )->LoadFromFile();

    m_spacebox->GetNodeFromPass( m_texturepass, Spacebox::RightQuad )->SetTexture( _DRAWSPACE_NEW_( Texture, Texture( "sb1.bmp" ) ), 0 );
    m_spacebox->GetNodeFromPass( m_texturepass, Spacebox::RightQuad )->GetTexture( 0 )->LoadFromFile();


    m_spacebox->GetNodeFromPass( m_texturepass, Spacebox::FrontQuad )->SetOrderNumber( 200 );
    m_spacebox->GetNodeFromPass( m_texturepass, Spacebox::RearQuad )->SetOrderNumber( 200 );
    m_spacebox->GetNodeFromPass( m_texturepass, Spacebox::TopQuad )->SetOrderNumber( 200 );
    m_spacebox->GetNodeFromPass( m_texturepass, Spacebox::BottomQuad )->SetOrderNumber( 200 );
    m_spacebox->GetNodeFromPass( m_texturepass, Spacebox::LeftQuad )->SetOrderNumber( 200 );
    m_spacebox->GetNodeFromPass( m_texturepass, Spacebox::RightQuad )->SetOrderNumber( 200 );
}

void SkyboxService::OnTextureMirrorPassUpdate( DrawSpace::IntermediatePass* p_val )
{
    m_spacebox->RegisterPassSlot( p_val );
    m_texturemirrorpass = p_val;
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
