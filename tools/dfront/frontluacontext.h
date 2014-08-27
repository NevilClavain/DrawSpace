
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

#ifndef _FRONTLUACONTEXT_H_
#define _FRONTLUACONTEXT_H_

#include "lua_context.h"

#define REGISTER_FUNC( __alias__, __func__ ) lua_register( m_L, __alias__, __func__ ); m_func_list.push_back( __alias__ );

class FrontLuaContext : public DrawSpace::LuaContext
{
protected:
    std::vector<std::string> m_func_list;

    static int lua_print( lua_State* p_L );

public:
    FrontLuaContext( void );
    virtual ~FrontLuaContext( void );

    void Startup( void );


};


#endif