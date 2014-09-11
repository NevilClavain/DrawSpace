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

#ifndef _CONFIGURABLE_H_
#define _CONFIGURABLE_H_


#include "archive.h"

namespace DrawSpace
{
namespace Core
{
class Configurable
{
protected:

    std::map<dsstring, std::vector<dsstring>> m_properties;

public:
    Configurable( void );
    virtual ~Configurable( void );

    //PropertyPool& GetPropertyPool( void );

    virtual void Serialize( Utils::Archive& p_archive  ) = 0;
    virtual void Unserialize( Utils::Archive& p_archive ) = 0;

    virtual void ApplyConfiguration( void ) = 0;

};

}
}

#endif