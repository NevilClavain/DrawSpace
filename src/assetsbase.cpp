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

#include "assetsbase.h"

DrawSpace::Core::AssetsBase* DrawSpace::Core::AssetsBase::m_instance = NULL;

using namespace DrawSpace;
using namespace DrawSpace::Core;


AssetsBase::AssetsBase( void ) : m_assetreg_handler( NULL )
{
}

AssetsBase::~AssetsBase( void )
{
}

void AssetsBase::RegisterAsset( const dsstring& p_id, DrawSpace::Asset* p_asset )
{
    m_assets[p_id] = p_asset;    
    if( m_assetreg_handler )
    {
        (*m_assetreg_handler)( p_asset );
    }
}

DrawSpace::Asset* AssetsBase::GetAsset( const dsstring& p_id )
{
    if( m_assets.count( p_id ) > 0 )
    {
        return m_assets[p_id];
    }
    return NULL;
}

void AssetsBase::RegisterAssetRegistrationHandler( AssetRegistrationHandler* p_handler )
{
    m_assetreg_handler = p_handler;
}

bool AssetsBase::AssetIdExists( const dsstring& p_id )
{
    if( m_assets.count( p_id ) > 0 )
    {
        return true;
    }
    return false;
}

void AssetsBase::GetAssetsList( std::map<dsstring, DrawSpace::Asset*>& p_list )
{
    p_list = m_assets;
}