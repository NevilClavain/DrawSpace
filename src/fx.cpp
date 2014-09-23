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

#include "fx.h"
#include "renderer.h"
#include "plugin.h"
#include "md5.h"
#include "assetsbase.h"
#include "exceptions.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Utils;
using namespace DrawSpace::Interface;

Fx::Fx( void )
{
    // properties array creation
    m_properties["shaders"].AddProp<std::vector<dsstring>>();
    m_properties["renderstates_in"].AddProp<std::vector<RenderState>>();
    m_properties["renderstates_out"].AddProp<std::vector<RenderState>>();
    m_properties["configname"].AddPropValue<dsstring>( m_configname );
}

Fx::~Fx( void )
{


}

Shader* Fx::GetShader( long p_index )
{
    return m_shaders[p_index];
}

RenderState Fx::GetRenderStateIn( long p_index )
{
    return m_renderstates_in[p_index];
}

RenderState Fx::GetRenderStateOut( long p_index )
{
    return m_renderstates_out[p_index];
}

long Fx::GetShadersListSize( void )
{
    return (long)m_shaders.size();
}

long Fx::GetRenderStatesInListSize( void )
{
    return (long)m_renderstates_in.size();
}

long Fx::GetRenderStatesOutListSize( void )
{
    return (long)m_renderstates_out.size();
}

void Fx::AddShader( Shader* p_shader )
{
    m_shaders.push_back( p_shader );
}

void Fx::AddRenderStateIn( const RenderState& p_renderstate )
{
    m_renderstates_in.push_back( p_renderstate );
}

void Fx::AddRenderStateOut( const RenderState& p_renderstate )
{
    m_renderstates_out.push_back( p_renderstate );
}

void Fx::Serialize( Archive& p_archive )
{

}
bool Fx::Unserialize( Archive& p_archive )
{
    return true;
}

void Fx::GetMD5( dsstring& p_md5 )
{
    MD5 md5;

    unsigned char* shaders;

    dsstring hash_shaders = "";
    dsstring hash_rs = "";
    dsstring hash_rsargs = "";

    size_t total_shaders_data_size = 0;
    for( size_t i = 0; i < m_shaders.size(); i++ )
    {
        total_shaders_data_size += m_shaders[i]->GetDataSize();
    }

    shaders = new unsigned char[total_shaders_data_size];

    unsigned char* curr = shaders;
    for( size_t i = 0; i < m_shaders.size(); i++ )
    {
        memcpy( curr, m_shaders[i]->GetData(), m_shaders[i]->GetDataSize() );

        curr += m_shaders[i]->GetDataSize();
    }

    if( m_shaders.size() > 0 )
    {
        hash_shaders = md5.digestMemory( (BYTE*)shaders, (int)( total_shaders_data_size ) );
    }

    delete[] shaders;

    unsigned long* renderstates;
    renderstates = new unsigned long[m_renderstates_in.size() + m_renderstates_out.size()];

    int count = 0;
    for( size_t i = 0; i < m_renderstates_in.size(); i++ )
    {
        renderstates[count++] = (unsigned long)m_renderstates_in[i].GetOperation();
    }
    for( size_t i = 0; i < m_renderstates_out.size(); i++ )
    {
        renderstates[count++] = (unsigned long)m_renderstates_out[i].GetOperation();
    }

    if( count > 0 )
    {
        hash_rs = md5.digestMemory( (BYTE*)renderstates, count * sizeof( unsigned long ) );
    }

    delete[] renderstates;

    dsstring renderstates_arg = "";
    for( size_t i = 0; i < m_renderstates_in.size(); i++ )
    {
        dsstring arg;
        m_renderstates_in[i].GetArg( arg );

        renderstates_arg += arg;
    }

    if( m_renderstates_in.size() > 0 )
    {
        hash_rsargs = md5.digestMemory( (BYTE*)renderstates_arg.c_str(), (int)renderstates_arg.size() );
    }

    p_md5 = hash_shaders + hash_rs + hash_rsargs;
}

bool Fx::on_new_line( const dsstring& p_line, long p_line_num, std::vector<dsstring>& p_words )
{
    if( "shaders" == p_words[0] )
    {
        if( p_words.size() < 2 )
        {
            _PARSER_MISSING_ARG__
            return false;
        }

        std::vector<dsstring> shaders_name;
        for( size_t i = 1; i < p_words.size(); i++ )
        {
            shaders_name.push_back( p_words[i] );
        }
        m_properties["shaders"].SetPropValue<std::vector<dsstring>>( shaders_name );
    }
    else if( "configname" == p_words[0] )
    {
        if( p_words.size() < 2 )
        {
            _PARSER_MISSING_ARG__
            return false;
        }

        m_properties["configname"].SetPropValue<dsstring>( p_words[1] );
    }
    else if( "renderstates_in" == p_words[0] )
    {
        /*
        std::vector<RenderState> rs_list;

        // le nombre de word doit etre impair (mot cle + n paire (operation et argument)
        if( 0 == p_words.size() % 2 )
        {
            _PARSER_MISSING_ARG__
            return false;            
        }

        for( size_t i = 0; i < ( p_words.size() - 1 ) / 2; i++ )
        {
            RenderState rs;
            rs.SetOperationFromString( p_words[ ( 2 * i ) + 1 ] ); 
            rs.SetArg( p_words[ ( 2 * i ) + 2 ] );

            rs_list.push_back( rs );
        }

        m_properties["renderstates_in"].SetPropValue<std::vector<RenderState>>( rs_list );
        */

        std::vector<RenderState> renderstates_in = m_properties["renderstates_in"].GetPropValue<std::vector<RenderState>>();

        if( 0 == p_words.size() < 3 )
        {
            _PARSER_MISSING_ARG__
            return false;            
        }

        RenderState rs;
        rs.SetOperationFromString( p_words[1] ); 
        rs.SetArg( p_words[2] );

        renderstates_in.push_back( rs );

        m_properties["renderstates_in"].SetPropValue<std::vector<RenderState>>( renderstates_in );
    }
    else if( "renderstates_out" == p_words[0] )
    {
        /*
        std::vector<RenderState> rs_list;

        // le nombre de word doit etre impair (mot cle + n paire (operation et argument)
        if( 0 == p_words.size() % 2 )
        {
            _PARSER_MISSING_ARG__
            return false;            
        }

        for( size_t i = 0; i < ( p_words.size() - 1 ) / 2; i++ )
        {
            RenderState rs;
            rs.SetOperationFromString( p_words[ ( 2 * i ) + 1 ] ); 
            rs.SetArg( p_words[ ( 2 * i ) + 2 ] );

            rs_list.push_back( rs );
        }

        m_properties["renderstates_out"].SetPropValue<std::vector<RenderState>>( rs_list );

        */

        std::vector<RenderState> renderstates_out = m_properties["renderstates_out"].GetPropValue<std::vector<RenderState>>();

        if( 0 == p_words.size() < 3 )
        {
            _PARSER_MISSING_ARG__
            return false;            
        }

        RenderState rs;
        rs.SetOperationFromString( p_words[1] ); 
        rs.SetArg( p_words[2] );

        renderstates_out.push_back( rs );

        m_properties["renderstates_out"].SetPropValue<std::vector<RenderState>>( renderstates_out );

    }
    else
    {
        _PARSER_UNEXPECTED_KEYWORD_
        return false;
    }

    return true;
}

void Fx::DumpProperties( dsstring& p_text )
{
    dsstring text_value;

    p_text = "declare_config ";
    p_text += dsstring( FX_TEXT_KEYWORD );

    p_text += "\n";

    p_text += "configname ";
    p_text += m_properties["configname"].GetPropValue<dsstring>();
    p_text += "\n";


    p_text += "shaders ";

    std::vector<dsstring> shaders_list = m_properties["shaders"].GetPropValue<std::vector<dsstring>>();
    for( size_t i = 0; i < shaders_list.size(); i++ )
    {
        dsstring shader_name = shaders_list[i];

        p_text += shader_name;
        p_text += " ";
    }
    
    p_text += "\n";


    p_text += "renderstates_in";
    std::vector<RenderState> rsin_list = m_properties["renderstates_in"].GetPropValue<std::vector<RenderState>>();
    for( size_t i = 0; i < rsin_list.size(); i++ )
    {
        RenderState rs = rsin_list[i];

        rs.GetOperationToString( text_value );
        p_text += text_value;
        p_text += " ";

        rs.GetArg( text_value );
        p_text += text_value;
        p_text += " ";

    }
    p_text += "\n";


    p_text += "renderstates_out";
    std::vector<RenderState> rsout_list = m_properties["renderstates_out"].GetPropValue<std::vector<RenderState>>();
    for( size_t i = 0; i < rsout_list.size(); i++ )
    {
        RenderState rs = rsout_list[i];

        rs.GetOperationToString( text_value );
        p_text += text_value;
        p_text += " ";

        rs.GetArg( text_value );
        p_text += text_value;
        p_text += " ";

    }
    p_text += "\n";


    p_text += "end_config\n";

}

bool Fx::ParseProperties( const dsstring& p_text )
{
    char seps[] = { 0x09, 0x020, 0x00 };

    return RunOnTextChunk( p_text, seps );
}

void Fx::ApplyProperties( void )
{
    std::vector<dsstring> shaders_list = m_properties["shaders"].GetPropValue<std::vector<dsstring>>();
    for( size_t i = 0; i < shaders_list.size(); i++ )
    {
        dsstring shader_name = shaders_list[i];

        if( false == AssetsBase::GetInstance()->AssetIdExists( shader_name ) )
        {
            _DSEXCEPTION( "Asset id unknown in AssetsBase" );
        }

        Asset* asset = AssetsBase::GetInstance()->GetAsset( shader_name );

        Shader* shader = dynamic_cast<Shader*>( asset );
        if( !shader )
        {
            _DSEXCEPTION( "Specified asset is not a shader" );
        }

        AddShader( static_cast<Shader*>( shader ) );
    }

    std::vector<RenderState> rsin_list = m_properties["renderstates_in"].GetPropValue<std::vector<RenderState>>();
    for( size_t i = 0; i < rsin_list.size(); i++ )
    {
        RenderState rs = rsin_list[i];
        AddRenderStateIn( rs );
    }

    std::vector<RenderState> rsout_list = m_properties["renderstates_out"].GetPropValue<std::vector<RenderState>>();
    for( size_t i = 0; i < rsout_list.size(); i++ )
    {
        RenderState rs = rsout_list[i];
        AddRenderStateOut( rs );
    }

    m_configname = m_properties["configname"].GetPropValue<dsstring>();
}

