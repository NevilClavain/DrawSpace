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

#include "luacontext.h"
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
    { "add_aspect", &LuaClass_Entity::LUA_addaspect },
    { "connect_renderingaspect_rendergraph", &LuaClass_Entity::LUA_connect_renderingaspect_rendergraph },
    { "configure_timemanager", &LuaClass_Entity::LUA_configuretimemmanager },
    { "read_timemanager", &LuaClass_Entity::LUA_readtimemmanager },
    { "add_component", &LuaClass_Entity::LUA_addcomponent },
    { "remove_component", &LuaClass_Entity::LUA_removecomponent },
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

int LuaClass_Entity::LUA_addaspect( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc < 1 )
	{
        LUA_ERROR( "Entity::add_aspect : argument(s) missing" );
	}

    AspectType aspect_type = static_cast<AspectType>( luaL_checkint( p_L, 1 ) );

    static std::map<AspectType, std::function<void( DrawSpace::Core::Entity& )>> aspect_add_aig = 
    {
        { BODY_ASPECT, []( DrawSpace::Core::Entity& p_entity ) { p_entity.AddAspect<BodyAspect>(); } },
        { CAMERA_ASPECT, []( DrawSpace::Core::Entity& p_entity ) { p_entity.AddAspect<CameraAspect>(); } },
        { PHYSICS_ASPECT, []( DrawSpace::Core::Entity& p_entity ) { p_entity.AddAspect<PhysicsAspect>(); } },
        { RENDERING_ASPECT, []( DrawSpace::Core::Entity& p_entity ) { p_entity.AddAspect<RenderingAspect>(); } },
        { SERVICE_ASPECT, []( DrawSpace::Core::Entity& p_entity ) { p_entity.AddAspect<ServiceAspect>(); } },
        { TIME_ASPECT, []( DrawSpace::Core::Entity& p_entity ) { p_entity.AddAspect<TimeAspect>(); } },
        { TRANSFORM_ASPECT, []( DrawSpace::Core::Entity& p_entity ) { p_entity.AddAspect<TransformAspect>(); } },
    };

    aspect_add_aig[aspect_type]( m_entity );

    return 0;
}

int LuaClass_Entity::LUA_configuretimemmanager( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc < 1 )
	{
        LUA_ERROR( "Entity::configure_timemanager : argument(s) missing" );
	}

    TimeAspect* time_aspect = m_entity.GetAspect<TimeAspect>();
    if( NULL == time_aspect )
    {
        LUA_ERROR( "Entity::configure_timemanager : time aspect doesnt exists in this entity!" );
    }

    int time_scale = luaL_checkint( p_L, 1 );


    time_aspect->AddComponent<TimeManager>( "time_manager" );
    time_aspect->AddComponent<TimeAspect::TimeScale>( "time_scale", static_cast<TimeAspect::TimeScale>( time_scale ) );
    time_aspect->AddComponent<dsstring>( "output_formated_datetime", "..." );
    time_aspect->AddComponent<dstime>( "time", 0 );
    time_aspect->AddComponent<int>( "output_fps" );
    time_aspect->AddComponent<int>( "output_world_nbsteps" );

    time_aspect->AddComponent<dsreal>( "output_time_factor" );

    return 0;
}

int LuaClass_Entity::LUA_readtimemmanager( lua_State* p_L )
{
    TimeAspect* time_aspect = m_entity.GetAspect<TimeAspect>();
    if( NULL == time_aspect )
    {
        LUA_ERROR( "Entity::configure_timemanager : time aspect doesnt exists in this entity!" );
    }

    TimeAspect::TimeScale time_scale = time_aspect->GetComponent<TimeAspect::TimeScale>( "time_scale" )->getPurpose();
    dsstring datetime = time_aspect->GetComponent<dsstring>( "output_formated_datetime" )->getPurpose();

    lua_pushinteger( p_L, time_scale );
    lua_pushstring( p_L, datetime.c_str() );
    // to be continued...

    return 0;
}

int LuaClass_Entity::LUA_connect_renderingaspect_rendergraph( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc < 1 )
	{
        LUA_ERROR( "Entity::connect_renderingaspect_rendergraph : argument(s) missing" );
	}

    LuaClass_RenderPassNodeGraph* lua_rg = Luna<LuaClass_RenderPassNodeGraph>::check( p_L, 1 );

    RenderingAspect* rendering_aspect = m_entity.GetAspect<RenderingAspect>();
    if( rendering_aspect )
    {
        rendering_aspect->AddImplementation( &lua_rg->GetPassesRenderAspectImpl() );
    }
    else
    {
        LUA_ERROR( "Entity::connect_renderingaspect_rendergraph : entity has no rendering aspect. Call add_renderingaspect() method first" );
    }

    return 0;
}

int LuaClass_Entity::LUA_addcomponent( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc < 2 )
	{
        LUA_ERROR( "Entity::add_component : argument(s) missing" );
	}

    AspectType aspect_type = static_cast<AspectType>( luaL_checkint( p_L, 1 ) );
    ComponentType comp_type = static_cast<ComponentType>( luaL_checkint( p_L, 2 ) );

    Core::Aspect* aspect = get_aspect( aspect_type );
    if( NULL == aspect )
    {
        LUA_ERROR( "Entity::add_component : aspect doesnt exists in this entity!" );
    }

    int argc_compl = argc - 2; // nbre d'args apres le aspect_type et le comp_type

    LUA_TRY
    {
        switch( comp_type )
        {
            case COMP_INT:
            {
                if( argc_compl < 1 )
                {
                    LUA_ERROR( "Entity::add_component : argument(s) missing : component id" );
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
                    LUA_ERROR( "Entity::add_component : argument(s) missing : component id" );
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
                    LUA_ERROR( "Entity::add_component : argument(s) missing : component id" );
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
                    LUA_ERROR( "Entity::add_component : argument(s) missing : component id" );
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
                    LUA_ERROR( "Entity::add_component : argument(s) missing : component id" );
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
                    LUA_ERROR( "Entity::add_component : argument(s) missing : component id" );
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
                    LUA_ERROR( "Entity::add_component : argument(s) missing for text display" );
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
    } LUA_CATCH

    return 0;
}

int LuaClass_Entity::LUA_removecomponent( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc < 2 )
	{
        LUA_ERROR( "Entity::remove_component : argument(s) missing" );
	}

    AspectType aspect_type = static_cast<AspectType>( luaL_checkint( p_L, 1 ) );
    ComponentType comp_type = static_cast<ComponentType>( luaL_checkint( p_L, 2 ) );

    Core::Aspect* aspect = get_aspect( aspect_type );
    if( NULL == aspect )
    {
        LUA_ERROR( "Entity::remove_component : aspect doesnt exists in this entity!" );
    }

    int argc_compl = argc - 2; // nbre d'args apres le aspect_type et le comp_type

    if( argc_compl < 1 )
    {
        LUA_ERROR( "Entity::remove_component : argument(s) missing : component id" );
    }

    dsstring id = luaL_checkstring( p_L, 3 );

    static std::map<ComponentType, std::function<void( DrawSpace::Core::Aspect*, const dsstring& )>> comp_remove_aig = 
    {
        { COMP_INT, []( DrawSpace::Core::Aspect* p_aspect, const dsstring& p_id ) { p_aspect->RemoveComponent<int>( p_id ); } },
        { COMP_LONG, []( DrawSpace::Core::Aspect* p_aspect, const dsstring& p_id ) { p_aspect->RemoveComponent<long>( p_id ); } },
        { COMP_DSREAL, []( DrawSpace::Core::Aspect* p_aspect, const dsstring& p_id ) { p_aspect->RemoveComponent<dsstring>( p_id ); } },
        { COMP_DSREAL, []( DrawSpace::Core::Aspect* p_aspect, const dsstring& p_id ) { p_aspect->RemoveComponent<dsstring>( p_id ); } },
        { COMP_FLOAT, []( DrawSpace::Core::Aspect* p_aspect, const dsstring& p_id ) { p_aspect->RemoveComponent<float>( p_id ); } },
        { COMP_DSSTRING, []( DrawSpace::Core::Aspect* p_aspect, const dsstring& p_id ) { p_aspect->RemoveComponent<dsstring>( p_id ); } },
        { COMP_BOOL, []( DrawSpace::Core::Aspect* p_aspect, const dsstring& p_id ) { p_aspect->RemoveComponent<bool>( p_id ); } },
        { COMP_TEXTDISPLAY, []( DrawSpace::Core::Aspect* p_aspect, const dsstring& p_id ) { p_aspect->RemoveComponent<TextRenderingAspectImpl::TextDisplay>( p_id ); } },
    };

    LUA_TRY
    {
        comp_remove_aig[comp_type]( aspect, id );

    } LUA_CATCH

    return 0;
}

int LuaClass_Entity::LUA_readcomponentfromid( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc < 2 )
	{
        LUA_ERROR( "Entity::read_component_fromid : argument(s) missing" );
	}

    AspectType aspect_type = static_cast<AspectType>( luaL_checkint( p_L, 1 ) );
    ComponentType comp_type = static_cast<ComponentType>( luaL_checkint( p_L, 2 ) );

    Core::Aspect* aspect = get_aspect( aspect_type );
    if( NULL == aspect )
    {
        LUA_ERROR( "Entity::read_component_fromid : aspect doesnt exists in this entity!" );
    }

    int argc_compl = argc - 2; // nbre d'args apres le aspect_type et le comp_type

    if( argc_compl < 1 )
    {
        LUA_ERROR( "Entity::read_component_fromid : argument(s) missing : component id" );
    }

    switch( comp_type )
    {
        case COMP_INT:
        {
            LUA_TRY
            {
                dsstring id = luaL_checkstring( p_L, 3 );
                int i = aspect->GetComponent<int>( id )->getPurpose();

                lua_pushinteger( p_L, i );
                return 1;

            } LUA_CATCH
        }
        break;
    
        case COMP_LONG:
        {
            LUA_TRY
            {
                dsstring id = luaL_checkstring( p_L, 3 );
                long l = aspect->GetComponent<long>( id )->getPurpose();

                lua_pushinteger( p_L, l );
                return 1;

            } LUA_CATCH
        }
        break;

        case COMP_DSREAL:
        {
            LUA_TRY
            {
                dsstring id = luaL_checkstring( p_L, 3 );
                dsreal r = aspect->GetComponent<dsreal>( id )->getPurpose();

                lua_pushnumber( p_L, r );
                return 1;

            } LUA_CATCH
        }
        break;

        case COMP_FLOAT:
        {
            LUA_TRY
            {
                dsstring id = luaL_checkstring( p_L, 3 );
                float f = aspect->GetComponent<float>( id )->getPurpose();

                lua_pushnumber( p_L, f );
                return 1;

            } LUA_CATCH
        }
        break;

        case COMP_DSSTRING:
        {
            LUA_TRY
            {
                dsstring id = luaL_checkstring( p_L, 3 );
                dsstring s = aspect->GetComponent<dsstring>( id )->getPurpose();

                lua_pushstring( p_L, s.c_str() );
                return 1;

            } LUA_CATCH
        }
        break;

        case COMP_BOOL:
        {
            LUA_TRY
            {
                dsstring id = luaL_checkstring( p_L, 3 );
                bool b = aspect->GetComponent<bool>( id )->getPurpose();

                lua_pushinteger( p_L, b );
                return 1;

            } LUA_CATCH               
        }
        break;

        case COMP_TEXTDISPLAY:
        {
            LUA_TRY
            {
                dsstring id = luaL_checkstring( p_L, 3 );
                TextRenderingAspectImpl::TextDisplay t = aspect->GetComponent<TextRenderingAspectImpl::TextDisplay>( id )->getPurpose();

                lua_pushinteger( p_L, t.m_posx );
                lua_pushinteger( p_L, t.m_posy );
                lua_pushinteger( p_L, t.m_r );
                lua_pushinteger( p_L, t.m_g );
                lua_pushinteger( p_L, t.m_b );
                lua_pushstring( p_L, t.m_text.c_str() );
                return 6;

            } LUA_CATCH    
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
        LUA_ERROR( "Entity::write_component_fromid : argument(s) missing" );
	}

    AspectType aspect_type = static_cast<AspectType>( luaL_checkint( p_L, 1 ) );
    ComponentType comp_type = static_cast<ComponentType>( luaL_checkint( p_L, 2 ) );

    Core::Aspect* aspect = get_aspect( aspect_type );
    if( NULL == aspect )
    {
        LUA_ERROR( "Entity::write_component_fromid : aspect doesnt exists in this entity!" );
    }

    int argc_compl = argc - 2; // nbre d'args apres le aspect_type et le comp_type

   switch( comp_type )
    {
        case COMP_INT:
        {
            if( argc_compl < 2 )
            {
                LUA_ERROR( "Entity::write_component_fromid : argument(s) missing" );
            }

            LUA_TRY
            {
                dsstring id = luaL_checkstring( p_L, 3 );
                int i = luaL_checkint( p_L, 4 );
                aspect->GetComponent<int>( id )->getPurpose() = i;

            } LUA_CATCH
        }
        break;
    
        case COMP_LONG:
        {
            if( argc_compl < 2 )
            {
                LUA_ERROR( "Entity::write_component_fromid : argument(s) missing" );
            }

            LUA_TRY
            {
                dsstring id = luaL_checkstring( p_L, 3 );
                long l = luaL_checkint( p_L, 4 );
                aspect->GetComponent<long>( id )->getPurpose() = l;

            } LUA_CATCH
        }
        break;

        case COMP_DSREAL:
        {
            if( argc_compl < 2 )
            {
                LUA_ERROR( "Entity::write_component_fromid : argument(s) missing" );
            }

            LUA_TRY
            {
                dsstring id = luaL_checkstring( p_L, 3 );
                dsreal r = luaL_checknumber( p_L, 4 );
                aspect->GetComponent<dsreal>( id )->getPurpose() = r;

            } LUA_CATCH
        }
        break;

        case COMP_FLOAT:
        {
            if( argc_compl < 2 )
            {
                LUA_ERROR( "Entity::write_component_fromid : argument(s) missing" );
            }

            LUA_TRY
            {
                dsstring id = luaL_checkstring( p_L, 3 );
                float f = luaL_checknumber( p_L, 4 );
                aspect->GetComponent<float>( id )->getPurpose() = f;

            } LUA_CATCH
        }
        break;

        case COMP_DSSTRING:
        {
            if( argc_compl < 2 )
            {
                LUA_ERROR( "Entity::write_component_fromid : argument(s) missing" );
            }

            LUA_TRY
            {
                dsstring id = luaL_checkstring( p_L, 3 );
                dsstring s = luaL_checkstring( p_L, 4 );
                aspect->GetComponent<dsstring>( id )->getPurpose() = s;

            } LUA_CATCH
        }
        break;

        case COMP_BOOL:
        {
            if( argc_compl < 2 )
            {
                LUA_ERROR( "Entity::write_component_fromid : argument(s) missing" );
            }

            LUA_TRY
            {
                dsstring id = luaL_checkstring( p_L, 3 );
                bool b = luaL_checkint( p_L, 4 );
                aspect->GetComponent<bool>( id )->getPurpose() = b;

            } LUA_CATCH               
        }
        break;

        case COMP_TEXTDISPLAY:
        {
            if( argc_compl < 7 )
            {
                LUA_ERROR( "Entity::write_component_fromid : argument(s) missing for text display" );
            }
            LUA_TRY
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
                
            } LUA_CATCH
        }
        break;
    }

    return 0;
}