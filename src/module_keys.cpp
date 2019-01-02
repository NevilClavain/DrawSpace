/* -*-LIC_BEGIN-*- */
/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2019                     
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

#include "module_keys.h"

using namespace DrawSpace::Module;


KeysLinkTable::KeysLinkTable( void )
{
}

KeysLinkTable::~KeysLinkTable( void )
{
}

void KeysLinkTable::RegisterClientKey( KeySourceBase* p_key )
{
    m_clientsKeys.push_back( p_key );
    updateLinks();
}

void KeysLinkTable::RegisterModuleKey( KeySinkBase* p_key )
{
    m_moduleKeys.push_back( p_key );
    updateLinks();
}

void KeysLinkTable::RegisterClientKeysVector( const std::vector<DrawSpace::Module::KeySourceBase*>& p_keys )
{
    for( size_t i = 0; i < p_keys.size(); i++ )
    {
        m_clientsKeys.push_back( p_keys[i] );
    }
    updateLinks();
}

void KeysLinkTable::RegisterModuleKeysVector( const std::vector<DrawSpace::Module::KeySinkBase*>& p_keys )
{
    for( size_t i = 0; i < p_keys.size(); i++ )
    {
        m_moduleKeys.push_back( p_keys[i] );
    }
    updateLinks();
}

void KeysLinkTable::updateLinks( void )
{
    for( size_t i = 0; i < m_clientsKeys.size(); i++ )
    {
        for( size_t j = 0; j < m_moduleKeys.size(); j++ )
        {
            dsstring clientId = m_clientsKeys[i]->GetId();
            dsstring modId = m_moduleKeys[j]->GetId();

            if( clientId == modId )
            {
                m_clientsKeys[i]->AddSink( m_moduleKeys[j] );
            }
        }
    }
}
