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
}

Pass::~Pass( void )
{
    if( m_viewportquad )
    {
        _DRAWSPACE_DELETE_( m_viewportquad );
    }
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

    m_renderingqueue = _DRAWSPACE_NEW_( RenderingQueue, RenderingQueue );
}

FinalPass::~FinalPass( void )
{	
    _DRAWSPACE_DELETE_( m_renderingqueue );
}

void FinalPass::Serialize( Utils::Archive& p_archive  )
{

}

bool FinalPass::Unserialize( Utils::Archive& p_archive )
{
    return false;
}

void FinalPass::DumpProperties( dsstring& p_text )
{
}

bool FinalPass::ParseProperties( const dsstring& p_text )
{
    char seps[] = { 0x09, 0x020, 0x00 };

    return RunOnTextChunk( p_text, seps );
}


void FinalPass::ApplyProperties( void )
{   
    m_configname = m_properties["configname"].GetPropValue<dsstring>();

    bool enabledepthclear = m_properties["enabledepthclear"].GetPropValue<bool>();
    GetRenderingQueue()->EnableDepthClearing( enabledepthclear );

    bool enabletargetclear = m_properties["enabletargetclear"].GetPropValue<bool>();
    GetRenderingQueue()->EnableTargetClearing( enabletargetclear );

    unsigned char r = m_properties["targetclearcolor"].GetPropValue<unsigned char>( "r" );
    unsigned char g = m_properties["targetclearcolor"].GetPropValue<unsigned char>( "g" );
    unsigned char b = m_properties["targetclearcolor"].GetPropValue<unsigned char>( "b" );

    bool viewportquad = m_properties["viewportquad"].GetPropValue<bool>();

    if( viewportquad )
    {
        CreateViewportQuad();

        dsstring viewportquad_fx = m_properties["viewportquad_fx"].GetPropValue<dsstring>();

        if( false == ConfigsBase::GetInstance()->ConfigIdExists( viewportquad_fx ) )
        {
            _DSEXCEPTION( "Config id unknown in ConfigsBase" );
        }

        Configurable* fx = ConfigsBase::GetInstance()->GetConfigurable( viewportquad_fx );

        GetViewportQuad()->SetFx( static_cast<Fx*>( fx ) );

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

            }
        }
    }

    
    GetRenderingQueue()->SetTargetClearingColor( r, g, b );
}

bool FinalPass::on_new_line( const dsstring& p_line, long p_line_num, std::vector<dsstring>& p_words )
{
    return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////

IntermediatePass::IntermediatePass( const dsstring& p_name ) : Pass( p_name )
{
    // properties array creation
    m_properties["configname"].AddPropValue<dsstring>( m_configname );


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

void IntermediatePass::Serialize( Utils::Archive& p_archive  )
{

}

bool IntermediatePass::Unserialize( Utils::Archive& p_archive )
{
    return false;
}

void IntermediatePass::DumpProperties( dsstring& p_text )
{
}

bool IntermediatePass::ParseProperties( const dsstring& p_text )
{
    return false;
}

void IntermediatePass::ApplyProperties( void )
{
}

bool IntermediatePass::on_new_line( const dsstring& p_line, long p_line_num, std::vector<dsstring>& p_words )
{
    return true;
}
