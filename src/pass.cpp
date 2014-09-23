/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2014                                
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

#include "pass.h"
#include "renderer.h"
#include "plugin.h"
#include "misc_utils.h"
#include "configsbase.h"
#include "assetsbase.h"
#include "exceptions.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Utils;

Pass::Pass( const dsstring& p_name ) : 
m_name( p_name ),
m_viewportquad( NULL )
{
    // properties array creation
    m_properties["configname"].AddPropValue<dsstring>( m_configname );

    m_properties["enabledepthclear"].AddPropValue<bool>( false );
    m_properties["enabletargetclear"].AddPropValue<bool>( false );
    m_properties["targetclearcolor"].AddProp<unsigned char>( "r" );
    m_properties["targetclearcolor"].AddProp<unsigned char>( "g" );
    m_properties["targetclearcolor"].AddProp<unsigned char>( "b" );
   
    m_properties["viewportquad"].AddPropValue<bool>( false );
    m_properties["viewportquad_fx"].AddPropValue<dsstring>( "" );

    m_properties["viewportquad_textures"].AddProp<std::vector<std::pair<long, TextureSourceName>>>();

    m_properties["viewportquad_shaderparams"].AddProp<std::map<dsstring, RenderingNode::ShadersParams>>();

}

Pass::~Pass( void )
{
    if( m_viewportquad )
    {
        _DRAWSPACE_DELETE_( m_viewportquad );
    }
}

void Pass::Serialize( Utils::Archive& p_archive  )
{

}

bool Pass::Unserialize( Utils::Archive& p_archive )
{
    return false;
}

void Pass::DumpProperties( dsstring& p_text )
{
    dsstring text_value;

    p_text = "declare_config ";
    p_text += dsstring( PASS_TEXT_KEYWORD );

    p_text += "\n";

    p_text += "configname ";
    p_text += m_properties["configname"].GetPropValue<dsstring>();
    p_text += "\n";


    p_text += "enabledepthclear ";
    p_text += ( m_properties["enabledepthclear"].GetPropValue<bool>() ? "true" : "false" );
    p_text += "\n";

    p_text += "enabletargetclear ";
    p_text += ( m_properties["enabletargetclear"].GetPropValue<bool>() ? "true" : "false" );
    p_text += "\n";

    p_text += "targetclearcolor ";

    IntToString( m_properties["targetclearcolor"].GetPropValue<unsigned char>( "r" ), text_value );
    p_text += text_value;
    p_text += " ";

    IntToString( m_properties["targetclearcolor"].GetPropValue<unsigned char>( "g" ), text_value );
    p_text += text_value;
    p_text += " ";

    IntToString( m_properties["targetclearcolor"].GetPropValue<unsigned char>( "b" ), text_value );
    p_text += text_value;
    p_text += "\n";

    p_text += "viewportquad ";
    p_text += ( true == m_properties["viewportquad"].GetPropValue<bool>() ? "true" : "false" );
    p_text += "\n";

    if( true == m_properties["viewportquad"].GetPropValue<bool>() )
    {
        p_text += "viewportquad_fx ";
        p_text += m_properties["viewportquad_fx"].GetPropValue<dsstring>();
        p_text += "\n";
        
        std::vector<std::pair<long, TextureSourceName>> viewportquad_textures;
        viewportquad_textures = m_properties["viewportquad_textures"].GetPropValue<std::vector<std::pair<long, TextureSourceName>>>();

        for( size_t i = 0; i < viewportquad_textures.size(); i++ )
        {
            p_text += "viewportquad_textures ";

            if( Pass::PASS_NAME == viewportquad_textures[i].second.source )
            {
                p_text += "pass ";
            }
            else
            {
                p_text += "texture ";
            }

            p_text += viewportquad_textures[i].second.name;
            p_text += " ";

            IntToString( viewportquad_textures[i].first, text_value );

            p_text += text_value;
            p_text += "\n";
        }

        std::map<dsstring, RenderingNode::ShadersParams> viewportquad_shaderparams = m_properties["viewportquad_shaderparams"].GetPropValue<std::map<dsstring, RenderingNode::ShadersParams>>();
        for( std::map<dsstring, RenderingNode::ShadersParams>::iterator it = viewportquad_shaderparams.begin(); it != viewportquad_shaderparams.end(); ++ it )
        {
            p_text += "viewportquad_shaderparams ";

            p_text += it->first;
            p_text += " ";

            IntToString( it->second.shader_index, text_value );
            p_text += text_value;
            p_text += " ";


            IntToString( it->second.param_register, text_value );
            p_text += text_value;
            p_text += " ";

            for( long i = 0; i < 4; i++ )
            {
                RealToString( it->second.param_values[i], text_value );
                p_text += text_value;
                p_text += " ";                
            }

            p_text += "\n";
        }
    }

    p_text += "end_config\n";
}

bool Pass::ParseProperties( const dsstring& p_text )
{
    char seps[] = { 0x09, 0x020, 0x00 };

    return RunOnTextChunk( p_text, seps );
}


void Pass::ApplyProperties( void )
{   
    m_configname = m_properties["configname"].GetPropValue<dsstring>();

    bool enabledepthclear = m_properties["enabledepthclear"].GetPropValue<bool>();
    GetRenderingQueue()->EnableDepthClearing( enabledepthclear );

    bool enabletargetclear = m_properties["enabletargetclear"].GetPropValue<bool>();
    GetRenderingQueue()->EnableTargetClearing( enabletargetclear );

    unsigned char r = m_properties["targetclearcolor"].GetPropValue<unsigned char>( "r" );
    unsigned char g = m_properties["targetclearcolor"].GetPropValue<unsigned char>( "g" );
    unsigned char b = m_properties["targetclearcolor"].GetPropValue<unsigned char>( "b" );

    GetRenderingQueue()->SetTargetClearingColor( r, g, b );

    bool viewportquad = m_properties["viewportquad"].GetPropValue<bool>();

    if( viewportquad )
    {
        CreateViewportQuad();

        dsstring viewportquad_fx = m_properties["viewportquad_fx"].GetPropValue<dsstring>();

        if( false == ConfigsBase::GetInstance()->ConfigIdExists( viewportquad_fx ) )
        {
            _DSEXCEPTION( "Config id unknown in ConfigsBase" );
        }

        Configurable* config = ConfigsBase::GetInstance()->GetConfigurable( viewportquad_fx );

        Fx* fx = dynamic_cast<Fx*>( config );
        if( !fx )
        {
            _DSEXCEPTION( "Specified asset is not an Fx" );
        }

        GetViewportQuad()->SetFx( fx );

        std::vector<std::pair<long, TextureSourceName>> viewportquad_textures;

        viewportquad_textures = m_properties["viewportquad_textures"].GetPropValue<std::vector<std::pair<long, TextureSourceName>>>();

        for( size_t i = 0; i < viewportquad_textures.size(); i++ )
        {
            TextureSourceName texture_source_name = viewportquad_textures[i].second;
            long stage = viewportquad_textures[i].first;

            if( PASS_NAME == texture_source_name.source )
            {
                if( false == ConfigsBase::GetInstance()->ConfigIdExists( texture_source_name.name ) )
                {
                    _DSEXCEPTION( "Config id unknown in ConfigsBase" );
                }

                Configurable* pass = ConfigsBase::GetInstance()->GetConfigurable( texture_source_name.name );

                IntermediatePass* ipass = dynamic_cast<IntermediatePass*>( pass );

                if( ipass )
                {
                    GetViewportQuad()->SetTexture( ipass->GetTargetTexture(), stage );
                }
                else
                {
                    _DSEXCEPTION( "Specified pass is not an Intermediate pass" );
                }
            }
            else
            {
                if( false == AssetsBase::GetInstance()->AssetIdExists( texture_source_name.name ) )
                {
                    _DSEXCEPTION( "Asset id unknown in AssetsBase" );
                }

                Asset* asset = AssetsBase::GetInstance()->GetAsset( texture_source_name.name );

                Texture* texture = dynamic_cast<Texture*>( asset);
                if( !texture )
                {
                    _DSEXCEPTION( "Specified asset is not a texture" );
                }

                GetViewportQuad()->SetTexture( static_cast<Texture*>( asset ), stage );
            }
        }

        std::map<dsstring, RenderingNode::ShadersParams> viewportquad_shaderparams = m_properties["viewportquad_shaderparams"].GetPropValue<std::map<dsstring, RenderingNode::ShadersParams>>();
        for( std::map<dsstring, RenderingNode::ShadersParams>::iterator it = viewportquad_shaderparams.begin(); it != viewportquad_shaderparams.end(); ++ it )
        {
            GetViewportQuad()->AddShaderParameter( it->second.shader_index, it->first, it->second.param_register );

            GetViewportQuad()->SetShaderRealVector( it->first, it->second.param_values );
        }
    }
     
    GetRenderingQueue()->SetTargetClearingColor( r, g, b );
}

bool Pass::on_new_line( const dsstring& p_line, long p_line_num, std::vector<dsstring>& p_words )
{
    if( "configname" == p_words[0] )
    {
        if( p_words.size() < 2 )
        {
            _PARSER_MISSING_ARG__
            return false;
        }

        m_properties["configname"].SetPropValue<dsstring>( p_words[1] );
    }
    else if( "enabledepthclear" == p_words[0] )
    {
        if( p_words.size() < 2 )
        {
            _PARSER_MISSING_ARG__
            return false;
        }
        m_properties["enabledepthclear"].SetPropValue<bool>( ( "true" == p_words[1] ? true : false ) );
    }
    else if( "targetclearcolor" == p_words[0] )
    {
        if( p_words.size() < 4 )
        {
            _PARSER_MISSING_ARG__
            return false;
        }

        m_properties["targetclearcolor"].SetPropValue<unsigned char>( "r", (unsigned char)StringToInt( p_words[1] ) );
        m_properties["targetclearcolor"].SetPropValue<unsigned char>( "g", (unsigned char)StringToInt( p_words[2] ) );
        m_properties["targetclearcolor"].SetPropValue<unsigned char>( "b", (unsigned char)StringToInt( p_words[3] ) );
    }
    else if( "viewportquad" == p_words[0] )
    {
        if( p_words.size() < 2 )
        {
            _PARSER_MISSING_ARG__
            return false;
        }
        m_properties["viewportquad"].SetPropValue<bool>( ( "true" == p_words[1] ? true : false ) );
    }
    else if( "viewportquad_fx" == p_words[0] )
    {
        if( p_words.size() < 2 )
        {
            _PARSER_MISSING_ARG__
            return false;
        }
        m_properties["viewportquad_fx"].SetPropValue<dsstring>( p_words[1] );
    }
    else if( "viewportquad_textures" == p_words[0] )
    {
        if( p_words.size() < 4 )
        {
            _PARSER_MISSING_ARG__
            return false;
        }

        std::vector<std::pair<long, TextureSourceName>> viewportquad_textures = m_properties["m_properties"].GetPropValue<std::vector<std::pair<long, TextureSourceName>>>();

        long stage;
        TextureSourceName tsn;

        if( "pass" == p_words[1] )
        {
            tsn.source = Pass::PASS_NAME;
        }
        else if( "texture" == p_words[1] )
        {
            tsn.source = Pass::TEXTURE_NAME;
        }
        else
        {
            _PARSER_UNEXPECTED_KEYWORD_
            return false;
        }

        tsn.name = p_words[2];

        stage = StringToInt( p_words[3] );

        viewportquad_textures.push_back( std::pair<long, TextureSourceName>( stage, tsn ) );

        m_properties["m_properties"].SetPropValue<std::vector<std::pair<long, TextureSourceName>>>( viewportquad_textures );
    }

    else if( "viewportquad_shaderparams" == p_words[0] )
    {
        if( p_words.size() < 8 )
        {
            _PARSER_MISSING_ARG__
            return false;
        }

        std::map<dsstring, RenderingNode::ShadersParams> viewportquad_shaderparams = m_properties["viewportquad_shaderparams"].GetPropValue<std::map<dsstring, RenderingNode::ShadersParams>>();

        RenderingNode::ShadersParams    shader_params;
        dsstring                        param_name;

        param_name = p_words[1];

        shader_params.shader_index = StringToInt( p_words[2] );
        shader_params.param_register = StringToInt( p_words[3] );

        shader_params.param_values[0] = StringToReal( p_words[4] );
        shader_params.param_values[1] = StringToReal( p_words[5] );
        shader_params.param_values[2] = StringToReal( p_words[6] );
        shader_params.param_values[3] = StringToReal( p_words[7] );

        viewportquad_shaderparams[param_name] = shader_params;

        m_properties["viewportquad_shaderparams"].SetPropValue<std::map<dsstring, RenderingNode::ShadersParams>>( viewportquad_shaderparams );
    }
    else
    {
        _PARSER_UNEXPECTED_KEYWORD_
        return false;
    }

    return true;
}

void Pass::GetName( dsstring& p_name )
{
    p_name = m_name;
}


RenderingQueue* Pass::GetRenderingQueue( void )
{
    return m_renderingqueue;
}

void Pass::CreateViewportQuad( void )
{
    DrawSpace::Interface::Renderer* renderer = DrawSpace::Core::SingletonPlugin<DrawSpace::Interface::Renderer>::GetInstance()->m_interface;
    DrawSpace::Interface::Renderer::Characteristics renderer_characteristics;
    renderer->GetRenderCharacteristics( renderer_characteristics );
    m_viewportquad = _DRAWSPACE_NEW_( ViewportQuad, ViewportQuad( m_name + dsstring( "/viewportquad" ), renderer_characteristics.width_viewport, renderer_characteristics.height_viewport ) );

    m_renderingqueue->Add( m_viewportquad );
}

void Pass::CreateViewportQuad( dsreal p_viewport_width, dsreal p_viewport_height )
{
    m_viewportquad = _DRAWSPACE_NEW_( ViewportQuad, ViewportQuad( m_name + dsstring( "/viewportquad" ), p_viewport_width, p_viewport_height ) );
    m_renderingqueue->Add( m_viewportquad );
}

ViewportQuad* Pass::GetViewportQuad( void )
{
    return m_viewportquad;
}

/////////////////////////////////////////////////////////////////////////////////////////////

FinalPass::FinalPass( const dsstring& p_name ) : Pass( p_name )
{
    m_renderingqueue = _DRAWSPACE_NEW_( RenderingQueue, RenderingQueue );
}

FinalPass::~FinalPass( void )
{	
    _DRAWSPACE_DELETE_( m_renderingqueue );
}

/////////////////////////////////////////////////////////////////////////////////////////////

IntermediatePass::IntermediatePass( const dsstring& p_name ) : Pass( p_name )
{
    //////// creation texture target
    DrawSpace::Interface::Renderer* renderer = DrawSpace::Core::SingletonPlugin<DrawSpace::Interface::Renderer>::GetInstance()->m_interface;

    DrawSpace::Interface::Renderer::Characteristics renderer_characteristics;
    renderer->GetRenderCharacteristics( renderer_characteristics );

    m_targettexture = _DRAWSPACE_NEW_( Texture, Texture( m_name + dsstring( "/target" ), true, renderer_characteristics.width_resol, renderer_characteristics.height_resol ) );

    m_renderingqueue = _DRAWSPACE_NEW_( RenderingQueue, RenderingQueue( m_targettexture ) );
}

IntermediatePass::IntermediatePass( const dsstring& p_name, long p_target_width, long p_target_height ) : Pass( p_name )
{
    //////// creation texture target
    m_targettexture = _DRAWSPACE_NEW_( Texture, Texture( m_name + dsstring( "/target" ), true, p_target_width, p_target_height ) );
    m_renderingqueue = _DRAWSPACE_NEW_( RenderingQueue, RenderingQueue( m_targettexture ) );
}

IntermediatePass::~IntermediatePass( void )
{
    _DRAWSPACE_DELETE_( m_renderingqueue );
    _DRAWSPACE_DELETE_( m_targettexture );

}

Core::Texture* IntermediatePass::GetTargetTexture( void )
{
    return m_targettexture;
}