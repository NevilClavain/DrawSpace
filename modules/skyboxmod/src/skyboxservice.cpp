/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2017                        
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
m_texturemirrorpass( NULL )
{    
    m_texturepassparam = _DRAWSPACE_NEW_( TexturePassParam, TexturePassParam( p_id + dsstring( ".TexturePass" ), this ) );
    m_texturemirrorpassparam = _DRAWSPACE_NEW_( TextureMirrorPassParam, TextureMirrorPassParam( p_id + dsstring( ".TextureMirrorPass" ), this ) );
    m_reflectornormaleparam = _DRAWSPACE_NEW_( ReflectorNormaleParam, ReflectorNormaleParam( p_id + dsstring( ".ReflectorNormale" ), this ) );
    m_texturebankpathparam = _DRAWSPACE_NEW_( TexturesBankPathParam, TexturesBankPathParam( p_id + dsstring( ".TexturesBankPath" ), this ) );
    m_texturebankvirtualfspathparam = _DRAWSPACE_NEW_( TexturesBankVirtualFSPathParam, TexturesBankVirtualFSPathParam( p_id + dsstring( ".TexturesBankVirtualFSPath" ), this ) );
    m_texturesnamesparam = _DRAWSPACE_NEW_( TexturesNamesParam, TexturesNamesParam( p_id + dsstring( ".TexturesNames" ), this ) );    
}

SkyboxService::~SkyboxService( void )
{
}

void SkyboxService::GetKeys( std::vector<DrawSpace::Module::KeySinkBase*>& p_keys )
{
    p_keys.push_back( m_texturepassparam );
    p_keys.push_back( m_texturemirrorpassparam );
    p_keys.push_back( m_reflectornormaleparam );
    p_keys.push_back( m_texturebankpathparam );
    p_keys.push_back( m_texturebankvirtualfspathparam );
    p_keys.push_back( m_texturesnamesparam );
}

void SkyboxService::Init( DrawSpace::Logger::Configuration* p_logconf, 
                            DrawSpace::Core::BaseCallback<void, bool>* p_mousecircularmode_cb, 
                            DrawSpace::Core::BaseCallback<void, bool>* p_mousevisible_cb, 
                            DrawSpace::Core::BaseCallback<void, int>* p_closeapp_cb )
{
}

void SkyboxService::Run( void )
{
    for( auto it = m_nodes.begin(); it != m_nodes.end(); ++it )
    {    
        for( int i = 0; i < 6; i++ )
        {
            it->second.skybox->GetNodeFromPass( m_texturemirrorpass, i )->SetShaderRealVector( "reflector_normale", m_reflector_normale );
        }
    }
}

void SkyboxService::Release( void )
{
}

DrawSpace::Core::BaseSceneNode* SkyboxService::InstanciateSceneNode( const dsstring& p_sceneNodeName )
{
    SkyboxNodeEntry entry;

    if( !m_texturepass || !m_texturemirrorpass || m_textures_names.size() != 6 )
    {
        return NULL;
    }

    entry.skybox = _DRAWSPACE_NEW_( DrawSpace::Spacebox, DrawSpace::Spacebox );

    entry.skybox->RegisterPassSlot( m_texturepass );
    

    //File::MountVirtualFS( "skybox_data.bank" );

    for( long i = 0; i < 6; i++ )
    {
        entry.skybox->GetNodeFromPass( m_texturepass, i )->SetFx( _DRAWSPACE_NEW_( Fx, Fx ) );

        entry.skybox->GetNodeFromPass( m_texturepass, i )->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture.vso", true ) ) );
        entry.skybox->GetNodeFromPass( m_texturepass, i )->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture.pso", true ) ) );

        entry.skybox->GetNodeFromPass( m_texturepass, i )->GetFx()->GetShader( 0 )->LoadFromFile();
        entry.skybox->GetNodeFromPass( m_texturepass, i )->GetFx()->GetShader( 1 )->LoadFromFile();
    }

    //File::UnmountVirtualFS();
    //File::MountVirtualFS( m_texturevirtualfspath );
    
    entry.skybox->GetNodeFromPass( m_texturepass, Spacebox::FrontQuad )->SetTexture( _DRAWSPACE_NEW_( Texture, Texture( m_textures_names[Spacebox::FrontQuad] ) ), 0 );
    entry.skybox->GetNodeFromPass( m_texturepass, Spacebox::FrontQuad )->GetTexture( 0 )->LoadFromFile();

    entry.skybox->GetNodeFromPass( m_texturepass, Spacebox::RearQuad )->SetTexture( _DRAWSPACE_NEW_( Texture, Texture( m_textures_names[Spacebox::RearQuad] ) ), 0 );
    entry.skybox->GetNodeFromPass( m_texturepass, Spacebox::RearQuad )->GetTexture( 0 )->LoadFromFile();

    entry.skybox->GetNodeFromPass( m_texturepass, Spacebox::TopQuad )->SetTexture( _DRAWSPACE_NEW_( Texture, Texture( m_textures_names[Spacebox::TopQuad] ) ), 0 );
    entry.skybox->GetNodeFromPass( m_texturepass, Spacebox::TopQuad )->GetTexture( 0 )->LoadFromFile();

    entry.skybox->GetNodeFromPass( m_texturepass, Spacebox::BottomQuad )->SetTexture( _DRAWSPACE_NEW_( Texture, Texture( m_textures_names[Spacebox::BottomQuad] ) ), 0 );
    entry.skybox->GetNodeFromPass( m_texturepass, Spacebox::BottomQuad )->GetTexture( 0 )->LoadFromFile();

    entry.skybox->GetNodeFromPass( m_texturepass, Spacebox::LeftQuad )->SetTexture( _DRAWSPACE_NEW_( Texture, Texture( m_textures_names[Spacebox::LeftQuad] ) ), 0 );
    entry.skybox->GetNodeFromPass( m_texturepass, Spacebox::LeftQuad )->GetTexture( 0 )->LoadFromFile();

    entry.skybox->GetNodeFromPass( m_texturepass, Spacebox::RightQuad )->SetTexture( _DRAWSPACE_NEW_( Texture, Texture( m_textures_names[Spacebox::RightQuad] ) ), 0 );
    entry.skybox->GetNodeFromPass( m_texturepass, Spacebox::RightQuad )->GetTexture( 0 )->LoadFromFile();


    entry.skybox->GetNodeFromPass( m_texturepass, Spacebox::FrontQuad )->SetOrderNumber( 200 );
    entry.skybox->GetNodeFromPass( m_texturepass, Spacebox::RearQuad )->SetOrderNumber( 200 );
    entry.skybox->GetNodeFromPass( m_texturepass, Spacebox::TopQuad )->SetOrderNumber( 200 );
    entry.skybox->GetNodeFromPass( m_texturepass, Spacebox::BottomQuad )->SetOrderNumber( 200 );
    entry.skybox->GetNodeFromPass( m_texturepass, Spacebox::LeftQuad )->SetOrderNumber( 200 );
    entry.skybox->GetNodeFromPass( m_texturepass, Spacebox::RightQuad )->SetOrderNumber( 200 );


    entry.skybox->RegisterPassSlot( m_texturemirrorpass );


    //File::UnmountVirtualFS();
    //File::MountVirtualFS( "skybox_data.bank" );

    for( long i = 0; i < 6; i++ )
    {

        entry.skybox->GetNodeFromPass( m_texturemirrorpass, i )->SetFx( _DRAWSPACE_NEW_( Fx, Fx ) );

        entry.skybox->GetNodeFromPass( m_texturemirrorpass, i )->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture_mirror.vso", true ) ) );
        entry.skybox->GetNodeFromPass( m_texturemirrorpass, i )->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture_mirror.pso", true ) ) );      

        entry.skybox->GetNodeFromPass( m_texturemirrorpass, i )->GetFx()->GetShader( 0 )->LoadFromFile();
        entry.skybox->GetNodeFromPass( m_texturemirrorpass, i )->GetFx()->GetShader( 1 )->LoadFromFile();

        entry.skybox->GetNodeFromPass( m_texturemirrorpass, i )->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETCULLING, "ccw" ) );
        //entry.skybox->GetNodeFromPass( m_texturemirrorpass, i )->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "false" ) );
        entry.skybox->GetNodeFromPass( m_texturemirrorpass, i )->GetFx()->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETCULLING, "cw" ) );
        //entry.skybox->GetNodeFromPass( m_texturemirrorpass, i )->GetFx()->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "true" ) );
    }   

    File::UnmountVirtualFS();
    File::MountVirtualFS( m_texturevirtualfspath );
    
    entry.skybox->GetNodeFromPass( m_texturemirrorpass, Spacebox::FrontQuad )->SetTexture( _DRAWSPACE_NEW_( Texture, Texture( m_textures_names[Spacebox::FrontQuad] ) ), 0 );
    entry.skybox->GetNodeFromPass( m_texturemirrorpass, Spacebox::FrontQuad )->GetTexture( 0 )->LoadFromFile();

    entry.skybox->GetNodeFromPass( m_texturemirrorpass, Spacebox::RearQuad )->SetTexture( _DRAWSPACE_NEW_( Texture, Texture( m_textures_names[Spacebox::RearQuad] ) ), 0 );
    entry.skybox->GetNodeFromPass( m_texturemirrorpass, Spacebox::RearQuad )->GetTexture( 0 )->LoadFromFile();

    entry.skybox->GetNodeFromPass( m_texturemirrorpass, Spacebox::TopQuad )->SetTexture( _DRAWSPACE_NEW_( Texture, Texture( m_textures_names[Spacebox::TopQuad] ) ), 0 );
    entry.skybox->GetNodeFromPass( m_texturemirrorpass, Spacebox::TopQuad )->GetTexture( 0 )->LoadFromFile();

    entry.skybox->GetNodeFromPass( m_texturemirrorpass, Spacebox::BottomQuad )->SetTexture( _DRAWSPACE_NEW_( Texture, Texture( m_textures_names[Spacebox::BottomQuad] ) ), 0 );
    entry.skybox->GetNodeFromPass( m_texturemirrorpass, Spacebox::BottomQuad )->GetTexture( 0 )->LoadFromFile();

    entry.skybox->GetNodeFromPass( m_texturemirrorpass, Spacebox::LeftQuad )->SetTexture( _DRAWSPACE_NEW_( Texture, Texture( m_textures_names[Spacebox::LeftQuad] ) ), 0 );
    entry.skybox->GetNodeFromPass( m_texturemirrorpass, Spacebox::LeftQuad )->GetTexture( 0 )->LoadFromFile();

    entry.skybox->GetNodeFromPass( m_texturemirrorpass, Spacebox::RightQuad )->SetTexture( _DRAWSPACE_NEW_( Texture, Texture( m_textures_names[Spacebox::RightQuad] ) ), 0 );
    entry.skybox->GetNodeFromPass( m_texturemirrorpass, Spacebox::RightQuad )->GetTexture( 0 )->LoadFromFile();

    entry.skybox->GetNodeFromPass( m_texturemirrorpass, Spacebox::FrontQuad )->SetOrderNumber( 200 );
    entry.skybox->GetNodeFromPass( m_texturemirrorpass, Spacebox::RearQuad )->SetOrderNumber( 200 );
    entry.skybox->GetNodeFromPass( m_texturemirrorpass, Spacebox::TopQuad )->SetOrderNumber( 200 );
    entry.skybox->GetNodeFromPass( m_texturemirrorpass, Spacebox::BottomQuad )->SetOrderNumber( 200 );
    entry.skybox->GetNodeFromPass( m_texturemirrorpass, Spacebox::LeftQuad )->SetOrderNumber( 200 );
    entry.skybox->GetNodeFromPass( m_texturemirrorpass, Spacebox::RightQuad )->SetOrderNumber( 200 );

    for( int i = 0; i < 6; i++ )
    {
        entry.skybox->GetNodeFromPass( m_texturemirrorpass, i )->AddShaderParameter( 0, "reflector_pos", 24 );
        entry.skybox->GetNodeFromPass( m_texturemirrorpass, i )->AddShaderParameter( 0, "reflector_normale", 25 );
    }

    /////////////////////////////////////

    entry.skybox_node = _DRAWSPACE_NEW_( SceneNode<DrawSpace::Spacebox>, SceneNode<DrawSpace::Spacebox>( p_sceneNodeName ) );
    entry.skybox_node->SetContent( entry.skybox );

    m_nodes[p_sceneNodeName] = entry;

    return entry.skybox_node;
}

void SkyboxService::RegisterScenegraphCallbacks( DrawSpace::Core::SceneNodeGraph& p_scenegraph )
{
}

void SkyboxService::UnregisterScenegraphCallbacks( DrawSpace::Core::SceneNodeGraph& p_scenegraph )
{
}

void SkyboxService::ReleaseSceneNode( const dsstring& p_sceneNodeName )
{
    //TODO
}

void SkyboxService::OnSceneNodeGraphUpdated( DrawSpace::Core::SceneNodeGraph* p_val )
{
    m_scenenodegraph = p_val;
}

void SkyboxService::OnTexturePassUpdate( DrawSpace::IntermediatePass* p_val )
{
    m_texturepass = p_val;
}

void SkyboxService::OnTextureMirrorPassUpdate( DrawSpace::IntermediatePass* p_val )
{
    m_texturemirrorpass = p_val;
}

void SkyboxService::OnReflectorNormaleUpdate( const DrawSpace::Utils::Vector& p_normale )
{
    m_reflector_normale = p_normale;
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
