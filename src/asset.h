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

#ifndef _ASSET_H_
#define _ASSET_H_

#include "archive.h"

namespace DrawSpace
{

class Asset
{
protected:
    dsstring    m_assetname;

    std::map<dsstring, std::vector<dsstring>>   m_properties;

public:

    Asset( void );
    virtual ~Asset( void );

    virtual void Serialize( Utils::Archive& p_archive  ) = 0;
    virtual void Unserialize( Utils::Archive& p_archive ) = 0;

    virtual bool Load( void ) = 0;

    virtual void SetName( const dsstring& p_name );
};
}
#endif