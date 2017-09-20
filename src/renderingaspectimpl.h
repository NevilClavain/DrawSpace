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

#ifndef _RENDERINGASPECTIMPL_H_
#define _RENDERINGASPECTIMPL_H_

#include "renderingqueue.h"

namespace DrawSpace
{
namespace Core
{
class RenderingAspect;

class RenderingAspectImpl abstract
{
protected:
    RenderingAspect* m_owner;

    virtual void run( RenderingAspect* p_renderingaspect ) = 0;
    
public:
    RenderingAspectImpl( void ) : m_owner( NULL )
    {    
    }

    virtual bool VisitRenderPassDescr( const dsstring& p_name, RenderingQueue* p_passqueue ) = 0;

    friend class RenderingAspect;
};
}
}


#endif