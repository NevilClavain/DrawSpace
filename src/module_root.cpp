/* -*-LIC_BEGIN-*- */
/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2020                     
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

#include "module_root.h"
#include "hub.h"
#include "memalloc.h"
#include <Physfs.h>

using namespace DrawSpace;
using namespace DrawSpace::Interface::Module;
using namespace DrawSpace::Systems;
using namespace DrawSpace::Utils;

Root::Root(void) : 
m_hub( NULL )
{
	if (PHYSFS_isInit() == 0)
	{
		if (PHYSFS_init(NULL) == 0)
		{
			_DSEXCEPTION("PHYSFS init failure" + dsstring(PHYSFS_getLastError()));
		}
	}
}

void Root::UpdateRenderer( DrawSpace::Interface::Renderer* p_renderer )
{
    DrawSpace::Core::SingletonPlugin<DrawSpace::Interface::Renderer>::GetInstance()->m_interface = p_renderer;
}

std::vector<dsstring> Root::GetServicesList( void ) const
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

void Root::SetHub(Systems::Hub* p_hub)
{
    m_hub = p_hub;
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

MemAlloc* Root::GetMemAllocInstance(void) const
{
    return MemAlloc::GetInstance();
}
