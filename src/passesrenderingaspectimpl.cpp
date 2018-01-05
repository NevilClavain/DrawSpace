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

#include "passesrenderingaspectimpl.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::RenderGraph;
using namespace DrawSpace::AspectImplementations;

PassesRenderingAspectImpl::PassesRenderingAspectImpl( void ) :
m_rendergraph( NULL )
{
}

void PassesRenderingAspectImpl::Run( Entity* p_entity )
{
    if( m_rendergraph )
    {
        m_rendergraph->Accept( this );
    }
    else
    {
        _DSEXCEPTION( "no rendergraph associated with PassesRenderingAspectImpl : please use PassesRenderingAspectImpl::SetRendergraph()" )
    }
}

bool PassesRenderingAspectImpl::VisitRenderPassDescr( const dsstring& p_name, RenderingQueue* p_passqueue )
{
    p_passqueue->Draw();
    return false;
}

void PassesRenderingAspectImpl::SetRendergraph( RenderPassNodeGraph* p_rendergraph )
{
    m_rendergraph = p_rendergraph;
}
