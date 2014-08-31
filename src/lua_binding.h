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

#ifndef _LUA_BINDING_H_
#define _LUA_BINDING_H_

#include "luna.h"
#include "drawspace_commons.h"

namespace DrawSpace
{

class LuaBinding;

class LuaBindingsDirectory
{
protected:

    std::map<dsstring, LuaBinding*> m_bindings;
    static LuaBindingsDirectory*    m_instance;

    LuaBindingsDirectory( void );

public:

    virtual ~LuaBindingsDirectory( void );

    static LuaBindingsDirectory* GetInstance( void )
    {
        if( NULL == m_instance )
        {
            m_instance = new LuaBindingsDirectory();
        }

        return m_instance;
    }

    void Register( const dsstring& p_id, LuaBinding* p_binding );
    LuaBinding* Get(  const dsstring& p_id );

};

class LuaBinding
{
public:
    LuaBinding( void );
    ~LuaBinding( void );
};


}

#endif