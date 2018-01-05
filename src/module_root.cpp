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

#include "module_root.h"

using namespace DrawSpace;
using namespace DrawSpace::Interface::Module;


void Root::UpdateRenderer( DrawSpace::Interface::Renderer* p_renderer )
{
    DrawSpace::Core::SingletonPlugin<DrawSpace::Interface::Renderer>::GetInstance()->m_interface = p_renderer;
}

std::vector<dsstring> Root::GetServicesList( void )
{
    std::vector<dsstring> list;
    for( auto it = m_services.begin(); it != m_services.end(); ++it )
    {
        list.push_back( it->first );
    }
    return list;
}

Service* Root::InstanciateService( const dsstring& p_id )
{
    if( m_services.count( p_id ) > 0 )
    {
        return m_services[p_id];
    }
    return NULL;
}


DrawSpace::Interface::AspectImplementations::RenderingAspectImpl* Root::InstanciateRenderingAspectImpls( const dsstring& p_id )
{
    return NULL;
}

DrawSpace::Interface::AspectImplementations::TransformAspectImpl* Root::InstanciateTransformAspectImpls( const dsstring& p_id )
{
    return NULL;
}

DrawSpace::Interface::AspectImplementations::ServiceAspectImpl* Root::InstanciateServiceAspectImpl( const dsstring& p_id )
{
    return NULL;
}

void Root::DumpMemoryAllocs( void )
{
    DrawSpace::Utils::MemAlloc::GetInstance()->DumpContent();
}
