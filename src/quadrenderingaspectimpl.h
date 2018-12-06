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

#ifndef _QUADRENDERINGASPECTIMPL_H_
#define _QUADRENDERINGASPECTIMPL_H_

#include "renderingaspectimpl.h"
#include "renderer.h"
#include "plugin.h"
#include "renderingnode.h"
#include "renderpassnodegraph.h"

namespace DrawSpace
{
namespace AspectImplementations
{
class QuadRenderingAspectImpl : public DrawSpace::Interface::AspectImplementations::RenderingAspectImpl
{

private:
    void*                                   m_sh_data;
    void*                                   m_rs_data;
    void*                                   m_tx_data;
    void*                                   m_meshe_data;

    Utils::Matrix                           m_proj;

public:
    QuadRenderingAspectImpl( void );

    bool IsText( void ) override { return true; };

    bool Init( DrawSpace::Core::Entity* p_entity );
    void Release(void) {};
    void Run( DrawSpace::Core::Entity* p_entity );
};
}
}

#endif