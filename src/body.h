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

#ifndef _BODY_H_
#define _BODY_H_

#include "drawable.h"

namespace DrawSpace
{
namespace Dynamics
{
class Body
{
protected:
    DrawSpace::Interface::Drawable* m_drawable;

public:
    Body( void );
    Body( DrawSpace::Interface::Drawable* p_drawable );
    virtual ~Body( void );

    void SetDrawable( DrawSpace::Interface::Drawable* p_drawable );
    DrawSpace::Interface::Drawable* GetDrawable( void );

};
}
}
#endif
