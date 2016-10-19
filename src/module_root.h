/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2016                              
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

#ifndef _MODULE_ROOT_
#define _MODULE_ROOT_

#include "module_service.h"
#include "plugin.h"

namespace DrawSpace
{
namespace Interface
{
namespace Module
{
class Root
{
protected:

    // table des differents services
    std::map<dsstring, DrawSpace::Interface::Module::Service*>  m_services;
    dsstring                                                    m_id;           //identifiant instance de module

public:

    virtual void                    UpdateRenderer( DrawSpace::Interface::Renderer* p_renderer )
    {
        DrawSpace::Core::SingletonPlugin<DrawSpace::Interface::Renderer>::GetInstance()->m_interface = p_renderer;
    }

    virtual void                    SetInstanceId( const dsstring& p_id ) { m_id = p_id; };

    virtual dsstring                GetModuleName( void ) = 0;
    virtual dsstring                GetModuleDescr( void ) = 0;
    virtual void                    ServicesInit( void ) = 0;


    virtual std::vector<dsstring>   GetServicesList( void )
    {
        std::vector<dsstring> list;
        for( auto it = m_services.begin(); it != m_services.end(); ++it )
        {
            list.push_back( it->first );
        }
        return list;
    }

    virtual Service*                InstanciateService( const dsstring& p_id )
    {
        if( m_services.count( p_id ) > 0 )
        {
            return m_services[p_id];
        }
        return NULL;
    }


};
}
}
}
#endif