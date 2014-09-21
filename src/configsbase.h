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

#ifndef _CONFIGSBASE_H_
#define _CONFIGSBASE_H_

#include "configurable.h"
#include "callback.h"

namespace DrawSpace
{
namespace Core
{
class ConfigsBase
{
public:

    typedef BaseCallback<void, DrawSpace::Core::Configurable*> ConfigurableRegistrationHandler;

protected:

    std::map<dsstring, DrawSpace::Core::Configurable*>      m_configurables;
    ConfigurableRegistrationHandler*                        m_configreg_handler;

    static ConfigsBase*                                     m_instance;

    ConfigsBase( void );

public:

    static ConfigsBase* GetInstance( void )
    {
        if( NULL == m_instance )
        {
            m_instance = new ConfigsBase();
        }

        return m_instance;
    }
    
    virtual ~ConfigsBase( void );

    void RegisterConfigurable( const dsstring& p_id, DrawSpace::Core::Configurable* p_conf );
    DrawSpace::Core::Configurable* GetConfigurable( const dsstring& p_id );

    void RegisterConfigurableRegistrationHandler( ConfigurableRegistrationHandler* p_handler );

    bool ConfigIdExists( const dsstring& p_id );
};
}
}
#endif