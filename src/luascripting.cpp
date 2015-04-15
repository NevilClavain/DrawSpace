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

#include "luascripting.h"
#include "lua_drawspace.h"
#include "lua_spaceboxnode.h"
#include "lua_transformnode.h"
#include "lua_camerapointnode.h"
#include "lua_fpsmovementnode.h"
#include "lua_keyboard.h"
#include "lua_mouse.h"
#include "lua_matrix.h"
#include "lua_vector.h"
#include "lua_real.h"


LuaScripting::LuaScripting( void )
{
}

bool LuaScripting::Initialize( void )
{
    LuaContext::GetInstance()->Startup();
    Luna2<LuaDrawSpace>::Register( LuaContext::GetInstance()->GetLuaState() );

    Luna2<LuaSpaceboxNode>::Register( LuaContext::GetInstance()->GetLuaState() );
    Luna2<LuaTransformationNode>::Register( LuaContext::GetInstance()->GetLuaState() );
    Luna2<LuaCameraPointNode>::Register( LuaContext::GetInstance()->GetLuaState() );
    Luna2<LuaFpsMovementNode>::Register( LuaContext::GetInstance()->GetLuaState() );

    Luna2<LuaKeyboard>::Register( LuaContext::GetInstance()->GetLuaState() );
    Luna2<LuaMouse>::Register( LuaContext::GetInstance()->GetLuaState() );

    Luna2<LuaMatrix>::Register( LuaContext::GetInstance()->GetLuaState() );
    Luna2<LuaVector>::Register( LuaContext::GetInstance()->GetLuaState() );
    Luna2<LuaReal>::Register( LuaContext::GetInstance()->GetLuaState() );

    return true;
}

void LuaScripting::Shutdown( void )
{
    LuaContext::GetInstance()->Stop();
}

bool LuaScripting::ExecChunk( const char* p_cmd )
{
    return LuaContext::GetInstance()->Exec( p_cmd );
}

bool LuaScripting::ExecFile( const char* p_path )
{
    return LuaContext::GetInstance()->Execfile( p_path );
}

void LuaScripting::GetLastError( dsstring& p_str )
{
    LuaContext::GetInstance()->GetLastError( p_str );
}


void LuaScripting::RegisterScriptErrorHandler( ErrorHandler* p_error_handler )
{
    LuaContext::GetInstance()->RegisterErrorHandler( p_error_handler );
}

void LuaScripting::RegisterScriptCallsHandler( ScriptCallsHandler* p_handler )
{
    LuaContext::GetInstance()->RegisterScriptCallsHandler( p_handler );
}

