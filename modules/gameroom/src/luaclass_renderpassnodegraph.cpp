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
#include "luaclass_renderpassnodegraph.h"
#include "luaclass_renderstatesset.h"
#include "luaclass_renderassembly.h"
#include "mainservice.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;

const char LuaClass_RenderPassNodeGraph::className[] = "RenderPassNodeGraph";
const Luna<LuaClass_RenderPassNodeGraph>::RegType LuaClass_RenderPassNodeGraph::methods[] =
{
    { "create_root", &LuaClass_RenderPassNodeGraph::LUA_createroot },
    { "create_child", &LuaClass_RenderPassNodeGraph::LUA_createchild },
    { "remove_pass", &LuaClass_RenderPassNodeGraph::LUA_removepass },
    { "set_pass_targetclearcolor", &LuaClass_RenderPassNodeGraph::LUA_setpasstargetclearcolor },
    { "set_pass_targetclearstate", &LuaClass_RenderPassNodeGraph::LUA_setpasstargetclearstate },
    { "set_pass_depthclearstate", &LuaClass_RenderPassNodeGraph::LUA_setpassdepthclearstate },
    { "create_pass_viewportquad", &LuaClass_RenderPassNodeGraph::LUA_createpassviewportquad },
    { "remove_pass_viewportquad", &LuaClass_RenderPassNodeGraph::LUA_removepassviewportquad },
    { "load_pass_viewportquad_resources", &LuaClass_RenderPassNodeGraph::LUA_loadpassviewportquadresources },
    { "unload_pass_viewportquad_resources", &LuaClass_RenderPassNodeGraph::LUA_unloadpassviewportquadresources },
    { "update_renderingqueues", &LuaClass_RenderPassNodeGraph::LUA_updaterenderingqueues },
	{ 0, 0 }
};

LuaClass_RenderPassNodeGraph::LuaClass_RenderPassNodeGraph( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc < 1 )
	{	
        LUA_ERROR( "RenderPassNodeGraph::RenderPassNodeGraph : argument(s) missing" );
	}

    dsstring id = luaL_checkstring( p_L, 1 );
    m_passes_render.SetRendergraph( &m_rendergraph );
    MainService::GetInstance()->RegisterRenderGraph( id, this );
    m_id = id;
}

LuaClass_RenderPassNodeGraph::~LuaClass_RenderPassNodeGraph( void )
{
    MainService::GetInstance()->UnregisterRenderGraph( m_id );
}

DrawSpace::AspectImplementations::PassesRenderingAspectImpl& LuaClass_RenderPassNodeGraph::GetPassesRenderAspectImpl( void )
{
    return m_passes_render;
}

DrawSpace::RenderGraph::RenderPassNodeGraph& LuaClass_RenderPassNodeGraph::GetRenderGraph( void )
{
    return m_rendergraph;
}

int LuaClass_RenderPassNodeGraph::LUA_createroot( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc < 1 )
	{	
        LUA_ERROR( "RenderPassNodeGraph::create_rootpass : argument(s) missing" );
	}

    dsstring pass_id = luaL_checkstring( p_L, 1 );
    
    m_passes[pass_id].m_renderpassnode = m_rendergraph.CreateRoot( pass_id );

    // reglages par defaut
    m_passes[pass_id].m_renderpassnode.GetRenderingQueue()->EnableDepthClearing( false );
    m_passes[pass_id].m_renderpassnode.GetRenderingQueue()->EnableTargetClearing( true );
    m_passes[pass_id].m_renderpassnode.GetRenderingQueue()->SetTargetClearingColor( 26, 27, 180, 255 );   
    return 0;
}

int LuaClass_RenderPassNodeGraph::LUA_createchild( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc < 3 )
	{	
        LUA_ERROR( "RenderPassNodeGraph::create_childpass : argument(s) missing" );
	}

    dsstring pass_id = luaL_checkstring( p_L, 1 );
    dsstring child_pass_id = luaL_checkstring( p_L, 2 );
    int target_stage = luaL_checkint( p_L, 3 );

    if( m_passes.count( pass_id ) )
    {
        m_passes[child_pass_id].m_renderpassnode = m_passes[pass_id].m_renderpassnode.CreateChild( child_pass_id, target_stage );

        // reglages par defaut
        m_passes[child_pass_id].m_renderpassnode.GetRenderingQueue()->EnableDepthClearing( true );
        m_passes[child_pass_id].m_renderpassnode.GetRenderingQueue()->EnableTargetClearing( true );
        m_passes[child_pass_id].m_renderpassnode.GetRenderingQueue()->SetTargetClearingColor( 0, 50, 0, 255 );   
    }
    else
    {       
        LUA_ERROR( "RenderPassNodeGraph::set_pass_targetclearcolor : unknown pass id" );
    }

    return 0;
}

int LuaClass_RenderPassNodeGraph::LUA_removepass( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc < 1 )
	{		
        LUA_ERROR( "RenderPassNodeGraph::remove_pass : argument(s) missing" );
	}

    dsstring pass_id = luaL_checkstring( p_L, 1 );

    if( m_passes.count( pass_id ) )
    {
        m_passes[pass_id].m_renderpassnode.Erase();
        m_passes.erase( pass_id );
    }
    else
    {        
        LUA_ERROR( "RenderPassNodeGraph::remove_pass : unknown pass id" );
    }
    return 0;
}

int LuaClass_RenderPassNodeGraph::LUA_setpasstargetclearcolor( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc < 4 )
	{		
        LUA_ERROR( "RenderPassNodeGraph::set_pass_targetclearcolor : argument(s) missing" );
	}

	dsstring pass_id = luaL_checkstring( p_L, 1 );
    int r = luaL_checkint( p_L, 2 );
    int g = luaL_checkint( p_L, 3 );
    int b = luaL_checkint( p_L, 4 );

    if( m_passes.count( pass_id ) )
    {
        m_passes[pass_id].m_renderpassnode.GetRenderingQueue()->SetTargetClearingColor( r, g, b, 255 );
    }
    else
    {       
        LUA_ERROR( "RenderPassNodeGraph::set_pass_targetclearcolor : unknown pass id" );
    }

    return 0;
}

int LuaClass_RenderPassNodeGraph::LUA_setpasstargetclearstate( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc < 2 )
	{		
        LUA_ERROR( "RenderPassNodeGraph::set_pass_targetclearstate : argument(s) missing" );
	}

	dsstring pass_id = luaL_checkstring( p_L, 1 );
    int state = luaL_checkint( p_L, 2 );

    if( m_passes.count( pass_id ) )
    {
        m_passes[pass_id].m_renderpassnode.GetRenderingQueue()->EnableTargetClearing( state );
    }
    else
    {        
        LUA_ERROR( "RenderPassNodeGraph::set_pass_targetclearstate : unknown pass id" );
    }

    return 0;
}

int LuaClass_RenderPassNodeGraph::LUA_setpassdepthclearstate( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc < 2 )
	{		
        LUA_ERROR( "RenderPassNodeGraph::set_pass_depthclearstate : argument(s) missing" );
	}

	dsstring pass_id = luaL_checkstring( p_L, 1 );
    int state = luaL_checkint( p_L, 2 );

    if( m_passes.count( pass_id ) )
    {
        m_passes[pass_id].m_renderpassnode.GetRenderingQueue()->EnableDepthClearing( state );
    }
    else
    {        
        LUA_ERROR( "RenderPassNodeGraph::set_pass_depthclearstate : unknown pass id" );
    }
    return 0;
}

int LuaClass_RenderPassNodeGraph::LUA_createpassviewportquad( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc < 1 )
	{		
        LUA_ERROR( "RenderPassNodeGraph::create_pass_viewportquad : argument(s) missing" );
	}

	dsstring pass_id = luaL_checkstring( p_L, 1 );

    dsreal zdepth = 0.0;
    if( argc > 1 )
    {
        zdepth = luaL_checknumber( p_L, 2 );
    }

    if( m_passes.count( pass_id ) )
    {
        m_passes[pass_id].m_renderpassnode.CreateViewportQuad( zdepth );
        m_passes[pass_id].m_renderpassnode.GetViewportQuad()->SetFx( &( m_passes[pass_id].m_fx ) );
    }
    else
    {        
        LUA_ERROR( "RenderPassNodeGraph::create_pass_viewportquad : unknown pass id" );
    }

    return 0;
}

int LuaClass_RenderPassNodeGraph::LUA_removepassviewportquad( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc < 1 )
	{		
        LUA_ERROR( "RenderPassNodeGraph::remove_pass_viewportquad : argument(s) missing" );
	}

	dsstring pass_id = luaL_checkstring( p_L, 1 );

    if( m_passes.count( pass_id ) )
    {
        m_passes[pass_id].m_renderpassnode.RemoveViewportQuad();
    }
    else
    {        
        LUA_ERROR( "RenderPassNodeGraph::remove_pass_viewportquad : unknown pass id" );
    }

    return 0;
}

int LuaClass_RenderPassNodeGraph::LUA_loadpassviewportquadresources( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc < 2 )
	{		
        LUA_ERROR( "RenderPassNodeGraph::load_pass_viewportquad_resources : argument(s) missing" );
	}

    dsstring pass_id = luaL_checkstring( p_L, 1 );
    LuaClass_RenderAssembly* lua_renderassembly = Luna<LuaClass_RenderAssembly>::check( p_L, 2 );
    if( NULL == lua_renderassembly )
    {
        LUA_ERROR( "RenderPassNodeGraph::load_pass_viewportquad_resources : argument 1 must be of type LuaClass_RenderAssembly" );
    }

    if( m_passes.count( pass_id ) )
    {
        ViewportQuad* vpq = m_passes[pass_id].m_renderpassnode.GetViewportQuad();
        if( vpq )
        {
            ///////////////////////// les shaders
            size_t nb_shaders = lua_renderassembly->GetNbShaderFiles();

            for( size_t i = 0; i < nb_shaders; i++ )
            {
                std::pair<dsstring,bool> shader_infos = lua_renderassembly->GetShaderFile( i );

                dsstring shader_path = shader_infos.first;
                bool is_compiled = shader_infos.second;

                bool status;

                Shader* shader = _DRAWSPACE_NEW_( Shader, Shader );
                status = shader->LoadFromFile( shader_path, is_compiled );

                if( !status )
                {
                    // clean tout ce qui a deja ete charge...
                    unload_resources( p_L, pass_id );

                    LUA_ERROR( "RenderPassNodeGraph::load_pass_viewportquad_resources : shader loading operation failed" );
                }
                else
                {
                    m_passes[pass_id].m_fx.AddShader( shader );
                    m_passes[pass_id].m_renderpassnode.SetRenderingQueueUpdateFlag();
                }
            }
            ///////////////////////// les rendestates

            DrawSpace::Core::RenderStatesSet& rss = lua_renderassembly->GetRenderStatesSet();
            m_passes[pass_id].m_fx.SetRenderStates( rss );

            ///////////////////////// les textures

            for( size_t i = 0; i < DrawSpace::Core::RenderingNode::NbMaxTextures; i++ )
            {
                dsstring texture_path = lua_renderassembly->GetTextureFile( i );
                if( texture_path != "" )
                {
                    bool status;
                    Texture* texture = _DRAWSPACE_NEW_( Texture, Texture( texture_path ) );
                    status = texture->LoadFromFile();
                    if( !status )
                    {
                        // clean tout ce qui a deja ete charge...
                        unload_resources( p_L, pass_id );

                        LUA_ERROR( "RenderPassNodeGraph::load_pass_viewportquad_resources : texture loading operation failed" );
                    }
                    else
                    {
                        m_passes[pass_id].m_renderpassnode.GetViewportQuad()->SetTexture( texture, i );
                        m_passes[pass_id].m_renderpassnode.SetRenderingQueueUpdateFlag();
                    }
                }
            }
        }
        else
        {
            LUA_ERROR( "RenderPassNodeGraph::load_pass_viewportquad_resources : no viewportquad created for this pass" );
        }
    }
    else
    {
        LUA_ERROR( "RenderPassNodeGraph::load_pass_viewportquad_resources : unknown pass id" );
    }
    return 0;
}

void LuaClass_RenderPassNodeGraph::unload_resources( lua_State* p_L, const dsstring& p_passid )
{
    dsstring pass_id = p_passid;

    if( m_passes.count( pass_id ) )
    {
        ViewportQuad* vpq = m_passes[pass_id].m_renderpassnode.GetViewportQuad();
        if( vpq )
        {
            bool update_queue = false;
            for( long i = 0; i < m_passes[pass_id].m_fx.GetShadersListSize(); i++ )
            {
                Shader* shader = m_passes[pass_id].m_fx.GetShader( i );
                _DRAWSPACE_DELETE_( shader );

                update_queue = true;
            }
            m_passes[pass_id].m_fx.ClearShaders();

            for( long i = 0; i < vpq->GetTextureListSize(); i++ )
            {
                Texture* texture = vpq->GetTexture( i );
                if( texture )
                {
                    _DRAWSPACE_DELETE_( texture );
                    vpq->SetTexture( NULL, i );
                    update_queue = true;
                }
            }

            if( update_queue )
            {
                m_passes[pass_id].m_renderpassnode.SetRenderingQueueUpdateFlag();
            }
        }
        else
        {
            LUA_ERROR( "RenderPassNodeGraph::unload_pass_viewportquad_resources : no viewportquad created for this pass" );
        }
    }
    else
    {
        LUA_ERROR( "RenderPassNodeGraph::unload_pass_viewportquad_resources : unknown pass id" );
    }
}

int LuaClass_RenderPassNodeGraph::LUA_unloadpassviewportquadresources( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc < 1 )
	{		
        LUA_ERROR( "RenderPassNodeGraph::unload_pass_viewportquad_resources : argument(s) missing" );
	}

    dsstring pass_id = luaL_checkstring( p_L, 1 );
    unload_resources( p_L, pass_id );

    return 0;
}

int LuaClass_RenderPassNodeGraph::LUA_updaterenderingqueues( lua_State* p_L )
{
    m_rendergraph.PushSignal_UpdatedRenderingQueues();
    return 0;
}