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

#include "lua_binding.h"

using namespace DrawSpace;

LuaBindingsDirectory* LuaBindingsDirectory::m_instance = NULL;

LuaBindingsDirectory::LuaBindingsDirectory( void )
{
}

LuaBindingsDirectory::~LuaBindingsDirectory( void )
{
}

void LuaBindingsDirectory::Register( const dsstring& p_id, LuaBinding* p_binding )
{
    m_bindings[p_id] = p_binding;
}

LuaBinding* LuaBindingsDirectory::Get(  const dsstring& p_id )
{
    if( m_bindings.count( p_id ) > 0 )
    {
        return m_bindings[p_id];
    }
    return NULL;
}

LuaBinding::LuaBinding( void )
{
}

LuaBinding::~LuaBinding( void )
{

}

