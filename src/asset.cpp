/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2015                              
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

#include "asset.h"

using namespace DrawSpace;

Asset::Asset( void )
{

}

Asset::~Asset( void )
{
}

void Asset::GetPropertiesMap( PropertiesMap& p_map )
{
    p_map = m_properties;
}

void Asset::SetName( const dsstring& p_name )
{
    m_assetname = p_name;
}

void Asset::GetName( dsstring& p_name )
{
    p_name = m_assetname;
}
