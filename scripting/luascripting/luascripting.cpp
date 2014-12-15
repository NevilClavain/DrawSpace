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

#include "luascripting.h"
#include "lua_drawspace.h"


LuaScripting::LuaScripting( void ) :
m_handler( NULL )
{
}

bool LuaScripting::Initialize( void )
{
    LuaContext::GetInstance()->Startup();
    Luna<LuaDrawSpace>::Register( LuaContext::GetInstance()->GetLuaState() );

    return true;
}

void LuaScripting::Shutdown( void )
{
    LuaContext::GetInstance()->Stop();
}

void LuaScripting::ExecChunk( const char* p_cmd )
{
    LuaContext::GetInstance()->Exec( p_cmd );
}

void LuaScripting::ExecFile( const char* p_path )
{
    LuaContext::GetInstance()->Execfile( p_path );
}

void LuaScripting::RegisterScriptErrorHandler( ErrorHandler* p_error_handler )
{
    LuaContext::GetInstance()->RegisterErrorHandler( p_error_handler );
}

void LuaScripting::RegisterScriptGlobalPrintHandler( GlobalPrintHandler* p_handler )
{
    LuaContext::GetInstance()->RegisterPrintHandler( p_handler );
}

void LuaScripting::RegisterDrawspaceDisplayFramerateHandler( DrawspaceDisplayFramerateHandler* p_handler )
{
    LuaContext::GetInstance()->RegisterDisplayFrameRateHandler( p_handler );
}

void LuaScripting::RegisterCB( DrawSpace::Core::BaseCallback<void, int>* p_handler )
{
    m_handler = p_handler;
}

void LuaScripting::TestCBCall( void )
{
    if( m_handler )
    {
        (*m_handler)( 666 );
    }
}