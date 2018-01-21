/* -*-LIC_BEGIN-*- */
/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2018                        
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
/* -*-LIC_END-*- */

#ifndef _COMPONENT_H_
#define _COMPONENT_H_

#include "drawspace_commons.h"

namespace DrawSpace
{
namespace Core
{
class BaseComponent abstract {};

template<typename T>
using ComponentPurpose = std::unique_ptr<T>;

template<typename T>
class Component sealed : public BaseComponent
{
private:
    ComponentPurpose<T>       m_purpose;

public:
    template<class... Args>
    void MakePurpose(Args&&... p_args)
    {
        m_purpose = std::make_unique<T>((std::forward<Args>(p_args))...);
    }

    virtual T& getPurpose(void) const
    {
        return *( m_purpose.get() );
    }
};
}
}

#endif
