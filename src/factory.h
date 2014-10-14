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

#ifndef _FACTORY_H_
#define _FACTORY_H_

#include "drawspace_commons.h"
#include "archive.h"
#include "parser.h"
#include "asset.h"
#include "configurable.h"

#define DECLARE_ASSET_INSTANCE_KEYWORD              "declare_asset_instance"
#define DECLARE_CONFIG_INSTANCE_KEYWORD             "declare_config_instance"
#define DECLARE_CONFIG_KEYWORD                      "declare_config"

#define DECLARE_END_ASSET_KEYWORD                   "end_asset"
#define DECLARE_END_CONFIG_KEYWORD                  "end_config"

namespace DrawSpace
{
namespace Core
{
class Factory : public DrawSpace::Utils::Parser
{
protected:

    std::map<dsstring, Asset::InstanciateFunc>              m_assets_instanciationfuncs_bytext;
    std::map<short, Asset::InstanciateFunc>                 m_assets_instanciationfuncs_byarc;

    std::map<dsstring, Configurable::InstanciateFunc>       m_configs_instanciationfuncs_bytext;
    std::map<short, Configurable::InstanciateFunc>          m_configs_instanciationfuncs_byarc;


    dsstring                                                m_asset_properties;
    dsstring                                                m_asset_keyword;

    dsstring                                                m_config_properties;
    dsstring                                                m_config_keyword;
    dsstring                                                m_config_name;

    bool                                                    m_capture_asset_props;
    bool                                                    m_capture_config_props;

    bool                                                    m_store_config_text_only;
    
    

    static Factory* m_instance;

    Factory( void );

    virtual bool on_new_line( const dsstring& p_line, long p_line_num, std::vector<dsstring>& p_words );

public:

    static Factory* GetInstance( void )
    {
        if( NULL == m_instance )
        {
            m_instance = new Factory();
        }

        return m_instance;
    }

    ~Factory( void );

    DrawSpace::Core::Configurable* BuildConfigurableFromText( const dsstring& p_keyword, const dsstring& p_text );
    DrawSpace::Asset* BuildAssetFromText( const dsstring& p_keyword, const dsstring& p_text );

    bool ExecuteFromTextFile( const dsstring& p_path );
    bool ExecuteFromBinaryFile( const dsstring& p_path );

    bool ExecuteFromArchiveChunk( const DrawSpace::Utils::Archive& p_arc );
    bool ExecuteFromTextChunk( const dsstring& p_text );

    void RegisterAssetInstanciationFuncByText( const dsstring& p_keyword, Asset::InstanciateFunc p_func );
    void RegisterAssetInstanciationFuncByArc( short p_magic, Asset::InstanciateFunc p_func );

    void RegisterConfigInstanciationFuncByText( const dsstring& p_keyword, Configurable::InstanciateFunc p_func );
    void RegisterConfigInstanciationFuncByArc( short p_magic, Configurable::InstanciateFunc p_func );

    void GetAssetKeywordsList( std::vector<dsstring>& p_outlist );
    void GetConfigKeywordsList( std::vector<dsstring>& p_outlist );

    DrawSpace::Asset* InstanciateAssetFromKeyword( const dsstring& p_keyword );
    DrawSpace::Core::Configurable* InstanciateConfigFromKeyword( const dsstring& p_keyword );

};
}
}
#endif