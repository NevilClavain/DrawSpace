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
#include "file.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Utils;
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

ReflectorNormaleParam::ReflectorNormaleParam( const dsstring& p_id, SkyboxService* p_owner ) : KeySink( p_id ), m_owner( p_owner )
{
}

void ReflectorNormaleParam::OnUpdated( DrawSpace::Utils::Vector p_val )
{
    m_owner->OnReflectorNormaleUpdate( p_val );
}

/////////////////////////////

ScalingParam::ScalingParam( const dsstring& p_id, SkyboxService* p_owner ) : KeySink( p_id ), m_owner( p_owner )
{
}

void ScalingParam::OnUpdated( dsreal p_val )
{
    m_owner->OnScalingUpdate( p_val );
}

/////////////////////////////

TexturesBankPathParam::TexturesBankPathParam( const dsstring& p_id, SkyboxService* p_owner ) : KeySink( p_id ), m_owner( p_owner )
{
}

void TexturesBankPathParam::OnUpdated( dsstring p_val )
{
    Texture::SetRootPath( p_val );
}

/////////////////////////////

TexturesBankVirtualFSPathParam::TexturesBankVirtualFSPathParam( const dsstring& p_id, SkyboxService* p_owner ) : KeySink( p_id ), m_owner( p_owner )
{
}

void TexturesBankVirtualFSPathParam::OnUpdated( dsstring p_val )
{
    m_owner->OnTextureVirtualFSPathUpdate( p_val );
}

/////////////////////////////

TexturesNamesParam::TexturesNamesParam( const dsstring& p_id, SkyboxService* p_owner ) : KeySink( p_id ), m_owner( p_owner )
{
}

void TexturesNamesParam::OnUpdated( std::vector<dsstring> p_val )
{
    m_owner->OnTexturesNamesUpdate( p_val );
}

/////////////////////////////

SkyboxService::SkyboxService( const dsstring& p_id ) :
m_scenenodegraph( NULL ),
m_texturepass( NULL ),
m_texturemirrorpass( NULL ),
m_scaling( 20.0 )
{
    m_scparam = _DRAWSPACE_NEW_( SceneNodeGraphParam, SceneNodeGraphParam( p_id + dsstring( ".SceneNodeGraph" ), this ) );
    m_texturepassparam = _DRAWSPACE_NEW_( TexturePassParam, TexturePassParam( p_id + dsstring( ".TexturePass" ), this ) );
    m_texturemirrorpassparam = _DRAWSPACE_NEW_( TextureMirrorPassParam, TextureMirrorPassParam( p_id + dsstring( ".TextureMirrorPass" ), this ) );
    m_reflectornormaleparam = _DRAWSPACE_NEW_( ReflectorNormaleParam, ReflectorNormaleParam( p_id + dsstring( ".ReflectorNormale" ), this ) );
    m_texturebankpathparam = _DRAWSPACE_NEW_( TexturesBankPathParam, TexturesBankPathParam( p_id + dsstring( ".TexturesBankPath" ), this ) );
    m_texturebankvirtualfspathparam = _DRAWSPACE_NEW_( TexturesBankVirtualFSPathParam, TexturesBankVirtualFSPathParam( p_id + dsstring( ".TexturesBankVirtualFSPath" ), this ) );
    m_texturesnamesparam = _DRAWSPACE_NEW_( TexturesNamesParam, TexturesNamesParam( p_id + dsstring( ".TexturesNames" ), this ) );

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
    p_keys.push_back( m_reflectornormaleparam );
    p_keys.push_back( m_texturebankpathparam );
    p_keys.push_back( m_texturebankvirtualfspathparam );
    p_keys.push_back( m_texturesnamesparam );
}

void SkyboxService::Init( DrawSpace::Logger::Configuration* p_logconf, DrawSpace::Core::BaseCallback<void, bool>* p_mousecircularmode_cb )
{
    m_spacebox_node = _DRAWSPACE_NEW_( SceneNode<DrawSpace::Spacebox>, SceneNode<DrawSpace::Spacebox>( "spacebox" ) );
    m_spacebox_node->SetContent( m_spacebox );

    m_scenenodegraph->RegisterNode( m_spacebox_node );

    m_spacebox_transfo_node = _DRAWSPACE_NEW_( DrawSpace::Core::SceneNode<DrawSpace::Core::Transformation>, DrawSpace::Core::SceneNode<DrawSpace::Core::Transformation>( "spacebox_transfo" ) );
    m_spacebox_transfo_node->SetContent( _DRAWSPACE_NEW_( Transformation, Transformation ) );
    Matrix spacebox_scale;
    spacebox_scale.Scale( m_scaling, m_scaling, m_scaling );
    m_spacebox_transfo_node->GetContent()->PushMatrix( spacebox_scale );

    m_scenenodegraph->AddNode( m_spacebox_transfo_node );
    m_scenenodegraph->RegisterNode( m_spacebox_transfo_node );
    m_spacebox_node->LinkTo( m_spacebox_transfo_node );
}

void SkyboxService::Run( void )
{
    for( int i = 0; i < 6; i++ )
    {
        m_spacebox->GetNodeFromPass( m_texturemirrorpass, i )->SetShaderRealVector( "reflector_normale", m_reflector_normale );
    }
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

    File::MountVirtualFS( "skybox_data.bank" );

    for( long i = 0; i < 6; i++ )
    {
        m_spacebox->GetNodeFromPass( m_texturepass, i )->SetFx( _DRAWSPACE_NEW_( Fx, Fx ) );

        m_spacebox->GetNodeFromPass( m_texturepass, i )->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture.vso", true ) ) );
        m_spacebox->GetNodeFromPass( m_texturepass, i )->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture.pso", true ) ) );

        m_spacebox->GetNodeFromPass( m_texturepass, i )->GetFx()->GetShader( 0 )->LoadFromFile();
        m_spacebox->GetNodeFromPass( m_texturepass, i )->GetFx()->GetShader( 1 )->LoadFromFile();
    }

    File::UnmountVirtualFS();
    File::MountVirtualFS( m_texturevirtualfspath );
    
    m_spacebox->GetNodeFromPass( m_texturepass, Spacebox::FrontQuad )->SetTexture( _DRAWSPACE_NEW_( Texture, Texture( m_textures_names[Spacebox::FrontQuad] ) ), 0 );
    m_spacebox->GetNodeFromPass( m_texturepass, Spacebox::FrontQuad )->GetTexture( 0 )->LoadFromFile();

    m_spacebox->GetNodeFromPass( m_texturepass, Spacebox::RearQuad )->SetTexture( _DRAWSPACE_NEW_( Texture, Texture( m_textures_names[Spacebox::RearQuad] ) ), 0 );
    m_spacebox->GetNodeFromPass( m_texturepass, Spacebox::RearQuad )->GetTexture( 0 )->LoadFromFile();

    m_spacebox->GetNodeFromPass( m_texturepass, Spacebox::TopQuad )->SetTexture( _DRAWSPACE_NEW_( Texture, Texture( m_textures_names[Spacebox::TopQuad] ) ), 0 );
    m_spacebox->GetNodeFromPass( m_texturepass, Spacebox::TopQuad )->GetTexture( 0 )->LoadFromFile();

    m_spacebox->GetNodeFromPass( m_texturepass, Spacebox::BottomQuad )->SetTexture( _DRAWSPACE_NEW_( Texture, Texture( m_textures_names[Spacebox::BottomQuad] ) ), 0 );
    m_spacebox->GetNodeFromPass( m_texturepass, Spacebox::BottomQuad )->GetTexture( 0 )->LoadFromFile();

    m_spacebox->GetNodeFromPass( m_texturepass, Spacebox::LeftQuad )->SetTexture( _DRAWSPACE_NEW_( Texture, Texture( m_textures_names[Spacebox::LeftQuad] ) ), 0 );
    m_spacebox->GetNodeFromPass( m_texturepass, Spacebox::LeftQuad )->GetTexture( 0 )->LoadFromFile();

    m_spacebox->GetNodeFromPass( m_texturepass, Spacebox::RightQuad )->SetTexture( _DRAWSPACE_NEW_( Texture, Texture( m_textures_names[Spacebox::RightQuad] ) ), 0 );
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

    File::UnmountVirtualFS();
    File::MountVirtualFS( "skybox_data.bank" );

    for( long i = 0; i < 6; i++ )
    {

        m_spacebox->GetNodeFromPass( m_texturemirrorpass, i )->SetFx( _DRAWSPACE_NEW_( Fx, Fx ) );

        m_spacebox->GetNodeFromPass( m_texturemirrorpass, i )->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture_mirror.vso", true ) ) );
        m_spacebox->GetNodeFromPass( m_texturemirrorpass, i )->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture_mirror.pso", true ) ) );      

        m_spacebox->GetNodeFromPass( m_texturemirrorpass, i )->GetFx()->GetShader( 0 )->LoadFromFile();
        m_spacebox->GetNodeFromPass( m_texturemirrorpass, i )->GetFx()->GetShader( 1 )->LoadFromFile();

        m_spacebox->GetNodeFromPass( m_texturemirrorpass, i )->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETCULLING, "ccw" ) );
        //m_spacebox->GetNodeFromPass( m_texturemirrorpass, i )->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "false" ) );
        m_spacebox->GetNodeFromPass( m_texturemirrorpass, i )->GetFx()->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETCULLING, "cw" ) );
        //m_spacebox->GetNodeFromPass( m_texturemirrorpass, i )->GetFx()->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "true" ) );
    }   

    File::UnmountVirtualFS();
    File::MountVirtualFS( m_texturevirtualfspath );
    
    m_spacebox->GetNodeFromPass( m_texturemirrorpass, Spacebox::FrontQuad )->SetTexture( _DRAWSPACE_NEW_( Texture, Texture( m_textures_names[Spacebox::FrontQuad] ) ), 0 );
    m_spacebox->GetNodeFromPass( m_texturemirrorpass, Spacebox::FrontQuad )->GetTexture( 0 )->LoadFromFile();

    m_spacebox->GetNodeFromPass( m_texturemirrorpass, Spacebox::RearQuad )->SetTexture( _DRAWSPACE_NEW_( Texture, Texture( m_textures_names[Spacebox::RearQuad] ) ), 0 );
    m_spacebox->GetNodeFromPass( m_texturemirrorpass, Spacebox::RearQuad )->GetTexture( 0 )->LoadFromFile();

    m_spacebox->GetNodeFromPass( m_texturemirrorpass, Spacebox::TopQuad )->SetTexture( _DRAWSPACE_NEW_( Texture, Texture( m_textures_names[Spacebox::TopQuad] ) ), 0 );
    m_spacebox->GetNodeFromPass( m_texturemirrorpass, Spacebox::TopQuad )->GetTexture( 0 )->LoadFromFile();

    m_spacebox->GetNodeFromPass( m_texturemirrorpass, Spacebox::BottomQuad )->SetTexture( _DRAWSPACE_NEW_( Texture, Texture( m_textures_names[Spacebox::BottomQuad] ) ), 0 );
    m_spacebox->GetNodeFromPass( m_texturemirrorpass, Spacebox::BottomQuad )->GetTexture( 0 )->LoadFromFile();

    m_spacebox->GetNodeFromPass( m_texturemirrorpass, Spacebox::LeftQuad )->SetTexture( _DRAWSPACE_NEW_( Texture, Texture( m_textures_names[Spacebox::LeftQuad] ) ), 0 );
    m_spacebox->GetNodeFromPass( m_texturemirrorpass, Spacebox::LeftQuad )->GetTexture( 0 )->LoadFromFile();

    m_spacebox->GetNodeFromPass( m_texturemirrorpass, Spacebox::RightQuad )->SetTexture( _DRAWSPACE_NEW_( Texture, Texture( m_textures_names[Spacebox::RightQuad] ) ), 0 );
    m_spacebox->GetNodeFromPass( m_texturemirrorpass, Spacebox::RightQuad )->GetTexture( 0 )->LoadFromFile();

    m_spacebox->GetNodeFromPass( m_texturemirrorpass, Spacebox::FrontQuad )->SetOrderNumber( 200 );
    m_spacebox->GetNodeFromPass( m_texturemirrorpass, Spacebox::RearQuad )->SetOrderNumber( 200 );
    m_spacebox->GetNodeFromPass( m_texturemirrorpass, Spacebox::TopQuad )->SetOrderNumber( 200 );
    m_spacebox->GetNodeFromPass( m_texturemirrorpass, Spacebox::BottomQuad )->SetOrderNumber( 200 );
    m_spacebox->GetNodeFromPass( m_texturemirrorpass, Spacebox::LeftQuad )->SetOrderNumber( 200 );
    m_spacebox->GetNodeFromPass( m_texturemirrorpass, Spacebox::RightQuad )->SetOrderNumber( 200 );

    for( int i = 0; i < 6; i++ )
    {
        m_spacebox->GetNodeFromPass( m_texturemirrorpass, i )->AddShaderParameter( 0, "reflector_pos", 24 );
        m_spacebox->GetNodeFromPass( m_texturemirrorpass, i )->AddShaderParameter( 0, "reflector_normale", 25 );
    }

}

void SkyboxService::OnReflectorNormaleUpdate( const DrawSpace::Utils::Vector& p_normale )
{
    m_reflector_normale = p_normale;
}

void SkyboxService::OnScalingUpdate( dsreal p_scale )
{
    m_scaling = p_scale;
}

void SkyboxService::OnTextureVirtualFSPathUpdate( const dsstring& p_path )
{
    m_texturevirtualfspath = p_path;
}

void SkyboxService::OnTexturesNamesUpdate( const std::vector<dsstring> p_textures )
{
    m_textures_names = p_textures;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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
