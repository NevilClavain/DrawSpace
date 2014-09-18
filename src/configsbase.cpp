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

#include "configsbase.h"

DrawSpace::Core::ConfigsBase* DrawSpace::Core::ConfigsBase::m_instance = NULL;

using namespace DrawSpace;
using namespace DrawSpace::Core;

ConfigsBase::ConfigsBase( void ) : m_configreg_handler( NULL )
{
}

ConfigsBase::~ConfigsBase( void )
{
}

void ConfigsBase::RegisterConfigurable( const dsstring& p_id, DrawSpace::Core::Configurable* p_conf )
{
    m_configurables[p_id] = p_conf;
    p_conf->SetName( p_id );
    if( m_configreg_handler )
    {
        (*m_configreg_handler)( p_conf );
    }
}

DrawSpace::Core::Configurable* ConfigsBase::GetConfigurable( const dsstring& p_id )
{
    if( m_configurables.count( p_id ) > 0 )
    {
        return m_configurables[p_id];
    }
    return NULL;
}

void ConfigsBase::RegisterConfigurableRegistrationHandler( ConfigurableRegistrationHandler* p_handler )
{
    m_configreg_handler = p_handler;
}
