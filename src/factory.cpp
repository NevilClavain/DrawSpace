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
#include "assetsbase.h"
#include "configsbase.h"
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
    if( m_capture_asset_props )
    {   
        if( "#" == p_words[0] )
        {
            return true;
        }
        else if( DECLARE_END_ASSET_KEYWORD == p_words[0] )
        {
            m_capture_asset_props = false;

            Asset* asset = BuildAssetFromText( m_asset_keyword, m_asset_properties );
            if( asset )
            {
                // register in Assetsbase

                // check out if an name has been specified for asset
                dsstring assetname;
                asset->GetName( assetname );

                if( "" == assetname )
                {
                    _DSEXCEPTION( "asset has no name, and so cannot be registered in AssetsBase" );
                }

                if( AssetsBase::GetInstance()->AssetIdExists( assetname ) )
                {
                    _DSEXCEPTION( "asset with same id " + assetname + " already registered in AssetsBase" );
                }

                AssetsBase::GetInstance()->RegisterAsset( assetname, asset );
            }
            else
            {
                // m_last_error deja positionne dans BuildAssetFromText()
                return false;
            }           
        }
        else
        {
            m_asset_properties += p_line;
            m_asset_properties += "\n";
        }
    }
    else if( m_capture_config_props )
    {
        if( "#" == p_words[0] )
        {
            return true;
        }
        else if( DECLARE_END_CONFIG_KEYWORD == p_words[0] )
        {
            m_capture_config_props = false;

            Configurable* config = BuildConfigurableFromText( m_config_keyword, m_config_properties );
            if( config )
            {
                // register instance in Configsbase

                // check out if an name has been specified for config instance

                if( ConfigsBase::GetInstance()->ConfigurableInstanceExists( m_config_name ) )
                {
                    _DSEXCEPTION( "config instance with same id already registered in ConfigsBase" );
                }

                ConfigsBase::GetInstance()->RegisterConfigurableInstance( m_config_name, config );

                // instance Configurable cr��e et stock�e par fichier config resource : lui attribuer le nom specifique
                config->SetSpecificName( m_config_name );
            }
            else
            {
                // m_last_error deja positionne dans BuildConfigurableFromText()
                return false;
            }

        }
        else
        {
            m_config_properties += p_line;
            m_config_properties += "\n";
        }
    }
    else
    {
        if( "#" == p_words[0] )
        {
            return true;
        }
        else if( DECLARE_ASSET_INSTANCE_KEYWORD == p_words[0] )
        {
            if( p_words.size() < 2 )
            {
                _PARSER_MISSING_ARG__
                return false;
            }
            m_capture_asset_props = true;

            m_asset_properties = "";
            m_asset_keyword = p_words[1];

        }
        else if( DECLARE_CONFIG_INSTANCE_KEYWORD == p_words[0] )
        {
            if( p_words.size() < 3 )
            {
                _PARSER_MISSING_ARG__
                return false;
            }
            m_capture_config_props = true;

            m_config_properties = "";
            m_config_keyword = p_words[1];
            m_config_name = p_words[2];
        }
        else
        {
            _PARSER_UNEXPECTED_KEYWORD_
            return false;
        }
    }
    return true;
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
