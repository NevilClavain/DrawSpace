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

#ifndef _ASSETSBASE_H_
#define _ASSETSBASE_H_


#include "asset.h"
#include "callback.h"
#include "mesheimport.h"
#include "fontimport.h"

namespace DrawSpace
{
namespace Core
{
class AssetsBase
{
public:

    typedef BaseCallback<void, DrawSpace::Asset*> AssetRegistrationHandler;

protected:

    std::map<dsstring, DrawSpace::Asset*>   m_assets;
    AssetRegistrationHandler*               m_assetreg_handler;
    DrawSpace::Interface::MesheImport*      m_ac3dimporter;
    DrawSpace::Interface::FontImport*       m_cbfgimporter;

    static AssetsBase*                      m_instance;

    AssetsBase( void );

public:

    static AssetsBase* GetInstance( void )
    {
        if( NULL == m_instance )
        {
            m_instance = new AssetsBase();
        }

        return m_instance;
    }
    
    virtual ~AssetsBase( void );

    void RegisterAsset( const dsstring& p_id, DrawSpace::Asset* p_asset );
    DrawSpace::Asset* GetAsset( const dsstring& p_id );
    void RegisterAssetRegistrationHandler( AssetRegistrationHandler* p_handler );
    bool AssetIdExists( const dsstring& p_id );
    void GetAssetsList( std::map<dsstring, DrawSpace::Asset*>& p_list );

    DrawSpace::Interface::MesheImport* GetAC3CMesheImporter( void );
    DrawSpace::Interface::FontImport* GetCBFGFontImporter( void );
};
}
}
#endif
