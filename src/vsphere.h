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

#ifndef _VSPHERE_H_
#define _VSPHERE_H_

#include "drawspace_commons.h"
#include "asset.h"
#include "vector.h"
#include "matrix.h"
#include "mutex.h"

namespace DrawSpace
{
namespace Core
{
class VSphere
{
protected:

    Utils::Vector       m_point;
    Utils::Vector       m_transformed_point;
    dsreal              m_ray;

    dsstring            m_name;
    Utils::Mutex        m_mutex;

public:
    VSphere( const dsstring& p_name );
    VSphere( const dsstring& p_name, const Utils::Vector& p_point, dsreal p_ray );

    virtual ~VSphere( void );

    void    Transform( Utils::Matrix& p_mat );
    bool    Collide( const Utils::Vector& p_testpoint );

    void    SetRay( dsreal p_ray );
    dsreal  GetRay( void );
    void    SetPoint( const Utils::Vector& p_point );
    void    GetTransformedPoint( Utils::Vector& p_point );
};
}
}

#endif
