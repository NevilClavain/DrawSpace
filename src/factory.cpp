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

#include "factory.h"
#include "exceptions.h"

DrawSpace::Core::Factory* DrawSpace::Core::Factory::m_instance = NULL;

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Utils;

Factory::Factory( void )
{


}

Factory::~Factory( void )
{

}

DrawSpace::Core::Configurable* Factory::BuildConfigurableFromText( const dsstring& p_keyword, const dsstring& p_text )
{
    // instanciate config object

    if( m_configs_instanciationfuncs_bytext.count( p_keyword ) > 0 )
    {
        Configurable* config = ( *m_configs_instanciationfuncs_bytext[p_keyword] ) ();

        // call ParseProperties() method
        if( false == config->ParseProperties( p_text ) )
        {
            dsstring config_parse_error;
            config->GetLastError( config_parse_error );

            m_lasterror = dsstring( "Local parsing error for " ) + p_keyword + dsstring( " : " ) + config_parse_error;
            return NULL;
        }

        config->ApplyProperties();
        return config;
    }
    else
    {
        _DSEXCEPTION( dsstring( "no registered instanciation function for the keyword : " ) +  m_config_keyword );
    }

    return NULL;
}

DrawSpace::Asset* Factory::BuildAssetFromText( const dsstring& p_keyword, const dsstring& p_text )
{
    if( m_assets_instanciationfuncs_bytext.count( p_keyword ) > 0 )
    {
        Asset* asset = ( *m_assets_instanciationfuncs_bytext[p_keyword] ) ();

        // call ParseProperties() method
        if( false == asset->ParseProperties( p_text ) )
        {
            dsstring asset_parse_error;
            asset->GetLastError( asset_parse_error );

            m_lasterror = dsstring( "Local parsing error for " ) + p_keyword + dsstring( " : " ) + asset_parse_error;
            return NULL;
        }

        asset->ApplyProperties();
        return asset;
    }
    else
    {
        _DSEXCEPTION( dsstring( "no registered instanciation function for the keyword : " ) +  m_asset_keyword );
    }

    return NULL;
}

bool Factory::on_new_line( const dsstring& p_line, long p_line_num, std::vector<dsstring>& p_words )
{
    return 0;
}

bool Factory::ExecuteFromTextFile( const dsstring& p_path )
{
    m_capture_config_props = m_capture_asset_props = false;
    char seps[] = { 0x09, 0x020, 0x00 };
    // run parser
    bool status = Run( p_path, seps );
    return status;
}

bool Factory::ExecuteFromBinaryFile( const dsstring& p_path )
{
    // TODO
    return false;
}

bool Factory::ExecuteFromArchiveChunk( const DrawSpace::Utils::Archive& p_arc )
{
    // TODO
    return false;
}

bool Factory::ExecuteFromTextChunk( const dsstring& p_text )
{
    m_capture_config_props = m_capture_asset_props = false;
    char seps[] = { 0x09, 0x020, 0x00 };

    return RunOnTextChunk( p_text, seps );
}

void Factory::RegisterAssetInstanciationFuncByText( const dsstring& p_keyword, Asset::InstanciateFunc p_func )
{
    m_assets_instanciationfuncs_bytext[p_keyword] = p_func;
}

void Factory::RegisterAssetInstanciationFuncByArc( short p_magic, Asset::InstanciateFunc p_func )
{
    m_assets_instanciationfuncs_byarc[p_magic] = p_func;
}

void Factory::RegisterConfigInstanciationFuncByText( const dsstring& p_keyword, Configurable::InstanciateFunc p_func )
{
    m_configs_instanciationfuncs_bytext[p_keyword] = p_func;
}

void Factory::RegisterConfigInstanciationFuncByArc( short p_magic, Configurable::InstanciateFunc p_func )
{
    m_configs_instanciationfuncs_byarc[p_magic] = p_func;
}

void Factory::GetAssetKeywordsList( std::vector<dsstring>& p_outlist )
{
    for( std::map<dsstring, Asset::InstanciateFunc>::iterator it = m_assets_instanciationfuncs_bytext.begin(); it != m_assets_instanciationfuncs_bytext.end(); ++it )
    {
        p_outlist.push_back( it->first );
    }
}

void Factory::GetConfigKeywordsList( std::vector<dsstring>& p_outlist )
{
    for( std::map<dsstring, Configurable::InstanciateFunc>::iterator it = m_configs_instanciationfuncs_bytext.begin(); it != m_configs_instanciationfuncs_bytext.end(); ++it )
    {
        p_outlist.push_back( it->first );
    }
}

Asset* Factory::InstanciateAssetFromKeyword( const dsstring& p_keyword )
{
    return ( *m_assets_instanciationfuncs_bytext[p_keyword] ) ();
}

Configurable* Factory::InstanciateConfigFromKeyword( const dsstring& p_keyword )
{
    return ( *m_configs_instanciationfuncs_bytext[p_keyword] ) ();
}
