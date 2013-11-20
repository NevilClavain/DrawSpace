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

#include "factory.h"
#include "archive.h"

namespace DrawSpace
{

class Asset
{
protected:
    dsstring    m_assetname;
    bool        m_render_ready; // si true, chargé dans le render plugin

public:

    Asset( void );
    Asset( const dsstring& p_name );
    virtual ~Asset( void );

    virtual void Serialize( Core::Factory& p_factory, Utils::Archive& p_archive  ) = 0;
    virtual void Unserialize( Core::Factory& p_factory, Utils::Archive& p_archive ) = 0;

    virtual void GetName( dsstring& p_name );
    virtual void SetRenderReady( void );
    virtual void UnsetRenderReady( void );
};
}
#endif