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

#include "test02root.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Utils;
using namespace DrawSpace::Interface::Module;

Test02Root::Test02Root( void )
{
    m_services["mainloop"] = new MainLoopService();

    DrawSpace::Core::Shader::EnableShadersDescrInFinalPath( true );
    DrawSpace::Core::Shader::SetRootPath( ".\\..\\..\\DrawSpace\\shaders_bank" );
}

Test02Root::~Test02Root( void )
{
}

dsstring Test02Root::GetModuleName( void )
{
    return "Test02Root";
}

dsstring Test02Root::GetModuleDescr( void )
{
    return "test02 module";
}

std::vector<dsstring> Test02Root::GetServicesList( void )
{
    std::vector<dsstring> list;

    for( auto it = m_services.begin(); it != m_services.end(); ++it )
    {
        list.push_back( it->first );
    }
    return list;
}

Service* Test02Root::InstanciateService( const dsstring& p_id )
{
    if( m_services.count( p_id ) > 0 )
    {
        return m_services[p_id];
    }
    return NULL;
}