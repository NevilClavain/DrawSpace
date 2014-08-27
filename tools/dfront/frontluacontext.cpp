

#include "frontluacontext.h"
#include "renderframe.h"


FrontLuaContext::FrontLuaContext( void )
{
}

FrontLuaContext::~FrontLuaContext( void )
{
}

void FrontLuaContext::Startup( void )
{
    LuaContext::Startup();

    REGISTER_FUNC( "print", lua_print );
}

int FrontLuaContext::lua_print( lua_State* p_L )
{
    std::string text;

    int argc = lua_gettop( p_L );
    if( argc != 1 )
    {
		lua_pushstring( p_L, "print : bad number of args" );
		lua_error( p_L );
    }

    const char* arg;

	arg = luaL_checkstring( p_L, 1 );
	text = arg;

    RenderFrame::GetInstance()->PrintConsole( text );

    return 0;
}
