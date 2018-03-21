/* -*-LIC_BEGIN-*- */
/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2018                        
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

#include "luaclass_entity.h"
#include "luaclass_renderpassnodegraph.h"
#include "renderingaspect.h"
#include "timeaspect.h"

#include "bodyaspect.h"
#include "cameraaspect.h"
#include "physicsaspect.h"
#include "renderingaspect.h"
#include "serviceaspect.h"
#include "timeaspect.h"
#include "transformaspect.h"
#include "textrenderingaspectimpl.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Utils;
using namespace DrawSpace::Aspect;
using namespace DrawSpace::AspectImplementations;

const char LuaClass_Entity::className[] = "Entity";
const Luna<LuaClass_Entity>::RegType LuaClass_Entity::methods[] =
{
    { "add_renderingaspect", &LuaClass_Entity::LUA_addrenderingaspect },
    { "add_timeaspect", &LuaClass_Entity::LUA_addtimeaspect },
    { "connect_renderingaspect_rendergraph", &LuaClass_Entity::LUA_connect_renderingaspect_rendergraph },
    { "add_component", &LuaClass_Entity::LUA_addcomponent },
    { "read_component_fromid", &LuaClass_Entity::LUA_readcomponentfromid },
    { "write_component_fromid", &LuaClass_Entity::LUA_writecomponentfromid },
	{ 0, 0 }
};

LuaClass_Entity::LuaClass_Entity( lua_State* p_L )
{
}

LuaClass_Entity::~LuaClass_Entity( void )
{
}

Core::Aspect* LuaClass_Entity::get_aspect( AspectType p_type )
{
    static std::map<AspectType, std::function<Core::Aspect*( DrawSpace::Core::Entity& )>> aspect_type_aig = 
    {
        { BODY_ASPECT, []( DrawSpace::Core::Entity& p_entity )->Core::Aspect* { return p_entity.GetAspect<BodyAspect>(); } },
        { CAMERA_ASPECT, []( DrawSpace::Core::Entity& p_entity )->Core::Aspect* { return p_entity.GetAspect<CameraAspect>(); } },
        { PHYSICS_ASPECT, []( DrawSpace::Core::Entity& p_entity )->Core::Aspect* { return p_entity.GetAspect<PhysicsAspect>(); } },
        { RENDERING_ASPECT, []( DrawSpace::Core::Entity& p_entity )->Core::Aspect* { return p_entity.GetAspect<RenderingAspect>(); } },
        { SERVICE_ASPECT, []( DrawSpace::Core::Entity& p_entity )->Core::Aspect* { return p_entity.GetAspect<ServiceAspect>(); } },
        { TIME_ASPECT, []( DrawSpace::Core::Entity& p_entity )->Core::Aspect* { return p_entity.GetAspect<TimeAspect>(); } },
        { TRANSFORM_ASPECT, []( DrawSpace::Core::Entity& p_entity )->Core::Aspect* { return p_entity.GetAspect<TransformAspect>(); } },
    };
    
    return aspect_type_aig[p_type]( m_entity );
}

DrawSpace::Core::Entity& LuaClass_Entity::GetEntity( void )
{
    return m_entity;
}

int LuaClass_Entity::LUA_addrenderingaspect( lua_State* p_L )
{
    m_entity.AddAspect<RenderingAspect>();
    return 0;
}


int LuaClass_Entity::LUA_addtimeaspect( lua_State* p_L )
{
    TimeAspect* time_aspect = m_entity.AddAspect<TimeAspect>();

    time_aspect->AddComponent<TimeManager>( "time_manager" );
    time_aspect->AddComponent<TimeAspect::TimeScale>( "time_scale", TimeAspect::NORMAL_TIME );
    time_aspect->AddComponent<dsstring>( "output_formated_datetime", "..." );
    time_aspect->AddComponent<dstime>( "time", 0 );
    time_aspect->AddComponent<int>( "output_fps" );
    time_aspect->AddComponent<int>( "output_world_nbsteps" );

    time_aspect->AddComponent<dsreal>( "output_time_factor" );
    return 0;
}

int LuaClass_Entity::LUA_connect_renderingaspect_rendergraph( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc < 1 )
	{
		lua_pushstring( p_L, "Entity::connect_renderingaspect_rendergraph : argument(s) missing" );
		lua_error( p_L );		
	}

    LuaClass_RenderPassNodeGraph* lua_rg = Luna<LuaClass_RenderPassNodeGraph>::check( p_L, 1 );

    RenderingAspect* rendering_aspect = m_entity.GetAspect<RenderingAspect>();
    if( rendering_aspect )
    {
        rendering_aspect->AddImplementation( &lua_rg->GetPassesRenderAspectImpl() );
    }
    else
    {
		lua_pushstring( p_L, "Entity::connect_renderingaspect_rendergraph : entity has no rendering aspect. Call add_renderingaspect() method first" );
		lua_error( p_L );    
    }

    return 0;
}

int LuaClass_Entity::LUA_addcomponent( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc < 2 )
	{
		lua_pushstring( p_L, "Entity::add_component : argument(s) missing" );
		lua_error( p_L );		
	}

    AspectType aspect_type = static_cast<AspectType>( luaL_checkint( p_L, 1 ) );
    ComponentType comp_type = static_cast<ComponentType>( luaL_checkint( p_L, 2 ) );

    Core::Aspect* aspect = get_aspect( aspect_type );
    if( NULL == aspect )
    {
		lua_pushstring( p_L, "Entity::add_component : aspect doesnt exists in this entity!" );
		lua_error( p_L );		    
    }

    int argc_compl = argc - 2; // nbre d'args apres le aspect_type et le comp_type

    try
    {
        switch( comp_type )
        {
            case COMP_INT:
            {
                if( argc_compl < 1 )
                {
		            lua_pushstring( p_L, "Entity::add_component : argument(s) missing : component id" );
		            lua_error( p_L );	            
                }

                dsstring id = luaL_checkstring( p_L, 3 );
                if( argc_compl < 2 )
                {
                    aspect->AddComponent<int>( id );
                }
                else
                {
                    int i = luaL_checkint( p_L, 4 );
                    aspect->AddComponent<int>( id, i );                    
                }
            }
            break;
    
            case COMP_LONG:
            {
                if( argc_compl < 1 )
                {
		            lua_pushstring( p_L, "Entity::add_component : argument(s) missing : component id" );
		            lua_error( p_L );	            
                }

                dsstring id = luaL_checkstring( p_L, 3 );
                if( argc_compl < 2 )
                {
                    aspect->AddComponent<long>( id );
                }
                else
                {
                    int i = luaL_checkint( p_L, 4 );  
                    aspect->AddComponent<long>( id, i );
                }
            }
            break;

            case COMP_DSREAL:
            {
                if( argc_compl < 1 )
                {
		            lua_pushstring( p_L, "Entity::add_component : argument(s) missing : component id" );
		            lua_error( p_L );	            
                }

                dsstring id = luaL_checkstring( p_L, 3 );
                if( argc_compl < 2 )
                {
                    aspect->AddComponent<dsreal>( id );
                }
                else
                {
                    dsreal r = luaL_checknumber( p_L, 4 );  
                    aspect->AddComponent<dsreal>( id, r );                    
                }
            }
            break;

            case COMP_FLOAT:
            {
                if( argc_compl < 1 )
                {
		            lua_pushstring( p_L, "Entity::add_component : argument(s) missing : component id" );
		            lua_error( p_L );	            
                }

                dsstring id = luaL_checkstring( p_L, 3 );
                if( argc_compl < 2 )
                {
                    aspect->AddComponent<float>( id );
                }
                else
                {
                    float f = luaL_checknumber( p_L, 4 );  
                    aspect->AddComponent<float>( id, f );
                }
            }
            break;

            case COMP_DSSTRING:
            {
                if( argc_compl < 1 )
                {
		            lua_pushstring( p_L, "Entity::add_component : argument(s) missing : component id" );
		            lua_error( p_L );	            
                }

                dsstring id = luaL_checkstring( p_L, 3 );
                if( argc_compl < 2 )
                {
                    aspect->AddComponent<dsstring>( id );
                }
                else
                {
                    dsstring s = luaL_checkstring( p_L, 4 );  
                    aspect->AddComponent<dsstring>( id, s );                    
                }
            }
            break;

            case COMP_BOOL:
            {
                if( argc_compl < 1 )
                {
		            lua_pushstring( p_L, "Entity::add_component : argument(s) missing : component id" );
		            lua_error( p_L );	            
                }

                dsstring id = luaL_checkstring( p_L, 3 );
                if( argc_compl < 2 )
                {
                    aspect->AddComponent<bool>( id );
                }
                else
                {
                    bool b = luaL_checkint( p_L, 4 );  
                    aspect->AddComponent<bool>( id, b );                    
                }
            }
            break;

            case COMP_TEXTDISPLAY:
            {
                if( argc_compl < 7 )
                {
		            lua_pushstring( p_L, "Entity::add_component : argument(s) missing for text display" );
		            lua_error( p_L );	            
                }

                dsstring id = luaL_checkstring( p_L, 3 );
                int posx = luaL_checkint( p_L, 4 );
                int posy = luaL_checkint( p_L, 5 );
                int r = luaL_checkint( p_L, 6 );
                int g = luaL_checkint( p_L, 7 );
                int b = luaL_checkint( p_L, 8 );
                dsstring text = luaL_checkstring( p_L, 9 );

                aspect->AddComponent<TextRenderingAspectImpl::TextDisplay>( id, posx, posy, r, g, b, text );
            }
            break;
        }
    }
    catch( dsexception& p_e )
    {
        const char* what = p_e.what();
        char msgexcp[1024];
        sprintf( msgexcp, "exception catch : %s", what ); 
		lua_pushstring( p_L, msgexcp );
		lua_error( p_L );	
    }

    return 0;
}

int LuaClass_Entity::LUA_readcomponentfromid( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc < 2 )
	{
		lua_pushstring( p_L, "Entity::read_component_fromid : argument(s) missing" );
		lua_error( p_L );		
	}

    AspectType aspect_type = static_cast<AspectType>( luaL_checkint( p_L, 1 ) );
    ComponentType comp_type = static_cast<ComponentType>( luaL_checkint( p_L, 2 ) );

    Core::Aspect* aspect = get_aspect( aspect_type );
    if( NULL == aspect )
    {
		lua_pushstring( p_L, "Entity::read_component_fromid : aspect doesnt exists in this entity!" );
		lua_error( p_L );		    
    }

    int argc_compl = argc - 2; // nbre d'args apres le aspect_type et le comp_type

    switch( comp_type )
    {
        case COMP_INT:
        {
            if( argc_compl < 1 )
            {
		        lua_pushstring( p_L, "Entity::read_component_fromid : argument(s) missing : component id" );
		        lua_error( p_L );	            
            }

            try
            {
                dsstring id = luaL_checkstring( p_L, 3 );
                int i = aspect->GetComponent<int>( id )->getPurpose();

                lua_pushinteger( p_L, i );
                return 1;
            }
            catch( dsexception& p_e )
            {
                const char* what = p_e.what();
                char msgexcp[1024];
                sprintf( msgexcp, "exception catch : %s", what ); 
		        lua_pushstring( p_L, msgexcp );
		        lua_error( p_L );	
            }
        }
        break;
    
        case COMP_LONG:
        {

        }
        break;

        case COMP_DSREAL:
        {

        }
        break;

        case COMP_FLOAT:
        {

        }
        break;

        case COMP_DSSTRING:
        {

        }
        break;

        case COMP_BOOL:
        {
               
        }
        break;

        case COMP_TEXTDISPLAY:
        {

        }
        break;
    }
    return 0;
}

int LuaClass_Entity::LUA_writecomponentfromid( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc < 2 )
	{
		lua_pushstring( p_L, "Entity::write_component_fromid : argument(s) missing" );
		lua_error( p_L );		
	}

    AspectType aspect_type = static_cast<AspectType>( luaL_checkint( p_L, 1 ) );
    ComponentType comp_type = static_cast<ComponentType>( luaL_checkint( p_L, 2 ) );

    Core::Aspect* aspect = get_aspect( aspect_type );
    if( NULL == aspect )
    {
		lua_pushstring( p_L, "Entity::write_component_fromid : aspect doesnt exists in this entity!" );
		lua_error( p_L );		    
    }

    int argc_compl = argc - 2; // nbre d'args apres le aspect_type et le comp_type

   switch( comp_type )
    {
        case COMP_INT:
        {
            if( argc_compl < 2 )
            {
		        lua_pushstring( p_L, "Entity::write_component_fromid : argument(s) missing" );
		        lua_error( p_L );	            
            }

            try
            {
                dsstring id = luaL_checkstring( p_L, 3 );
                int i = luaL_checkint( p_L, 4 );
                aspect->GetComponent<int>( id )->getPurpose() = i;
            }
            catch( dsexception& p_e )
            {
                const char* what = p_e.what();
                char msgexcp[1024];
                sprintf( msgexcp, "exception catch : %s", what ); 
		        lua_pushstring( p_L, msgexcp );
		        lua_error( p_L );	
            }
        }
        break;
    
        case COMP_LONG:
        {

        }
        break;

        case COMP_DSREAL:
        {

        }
        break;

        case COMP_FLOAT:
        {

        }
        break;

        case COMP_DSSTRING:
        {

        }
        break;

        case COMP_BOOL:
        {
               
        }
        break;

        case COMP_TEXTDISPLAY:
        {
            if( argc_compl < 7 )
            {
		        lua_pushstring( p_L, "Entity::write_component_fromid : argument(s) missing for text display" );
		        lua_error( p_L );
            }
            try
            {
                dsstring id = luaL_checkstring( p_L, 3 );
                int posx = luaL_checkint( p_L, 4 );
                int posy = luaL_checkint( p_L, 5 );
                int r = luaL_checkint( p_L, 6 );
                int g = luaL_checkint( p_L, 7 );
                int b = luaL_checkint( p_L, 8 );
                dsstring text = luaL_checkstring( p_L, 9 );

                TextRenderingAspectImpl::TextDisplay td( posx, posy, r, g, b, text );
                aspect->GetComponent<TextRenderingAspectImpl::TextDisplay>( id )->getPurpose() = td;

                
            }
            catch( dsexception& p_e )
            {
                const char* what = p_e.what();
                char msgexcp[1024];
                sprintf( msgexcp, "exception catch : %s", what ); 
		        lua_pushstring( p_L, msgexcp );
		        lua_error( p_L );	
            }          
        }
        break;
    }

    return 0;
}