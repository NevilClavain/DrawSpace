/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2015                              
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

#include "luautils.h"


bool push_namedluatable( lua_State* p_L, const dsstring& p_tablename )
{
    lua_pushstring( p_L,  p_tablename.c_str() );
    lua_gettable( p_L, -2 );  

    if( !lua_istable( p_L, -1 ) )
    {
        return false;
    }
    return true;
}

dsreal get_namedrealfromtable( lua_State* p_L, const dsstring& p_key )
{
    lua_pushstring( p_L,  p_key.c_str() );
    lua_gettable( p_L, -2 );  

    dsreal val = luaL_checknumber( p_L, -1 );

    lua_pop( p_L, 1 ); // pop extracted value
    return val;
}