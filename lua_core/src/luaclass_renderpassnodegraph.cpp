/* -*-LIC_BEGIN-*- */
/*
*
* DrawSpace Rendering engine
* Emmanuel Chaumont Copyright (c) 2013-2023
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

#include "luaclass_texturesset.h"
#include "luaclass_fxparams.h"
#include "luaclass_rendercontext.h"
#include "luaclass_renderconfig.h"
#include "luaclass_matrix.h"
#include "mainservice.h"
#include "renderingaspectimpl.h"
#include "vector.h"

#include "mainservice.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Utils;
using namespace DrawSpace::Maths;

const char LuaClass_RenderPassNodeGraph::className[] = "RenderPassNodeGraph";
const Luna<LuaClass_RenderPassNodeGraph>::RegType LuaClass_RenderPassNodeGraph::methods[] =
{
    { "create_root", &LuaClass_RenderPassNodeGraph::LUA_createroot },
    { "create_child", &LuaClass_RenderPassNodeGraph::LUA_createchild },
    { "remove_pass", &LuaClass_RenderPassNodeGraph::LUA_removepass },
    { "set_pass_targetslice", &LuaClass_RenderPassNodeGraph::LUA_setpasstargetslice },
    { "set_pass_targetclearcolor", &LuaClass_RenderPassNodeGraph::LUA_setpasstargetclearcolor },
    { "set_pass_targetclearstate", &LuaClass_RenderPassNodeGraph::LUA_setpasstargetclearstate },
    { "set_pass_depthclearstate", &LuaClass_RenderPassNodeGraph::LUA_setpassdepthclearstate },
    { "create_pass_viewportquad", &LuaClass_RenderPassNodeGraph::LUA_createpassviewportquad },
    { "remove_pass_viewportquad", &LuaClass_RenderPassNodeGraph::LUA_removepassviewportquad },
    { "configure_pass_viewportquad_resources", &LuaClass_RenderPassNodeGraph::LUA_configurepassviewportquadresources },
    { "release_pass_viewportquad_resources", &LuaClass_RenderPassNodeGraph::LUA_releasepassviewportquadresources },
    { "update_renderingqueues", &LuaClass_RenderPassNodeGraph::LUA_updaterenderingqueues },    
    { "enable_pass", &LuaClass_RenderPassNodeGraph::LUA_enablepass },
    { "disable_pass", &LuaClass_RenderPassNodeGraph::LUA_disablepass },
    { "cleanup_renderingqueues", &LuaClass_RenderPassNodeGraph::LUA_cleanuprenderingqueues },
    { "set_viewportquadshaderrealvector", &LuaClass_RenderPassNodeGraph::LUA_setviewportquadshaderrealvector },
	{ "set_viewportquadshaderrealmatrix", &LuaClass_RenderPassNodeGraph::LUA_setviewportquadshaderrealmatrix },
    { "set_targettexturedepth", &LuaClass_RenderPassNodeGraph::LUA_settargettexturedepth },
	{ "add_renderpasseventcb", &LuaClass_RenderPassNodeGraph::LUA_addrenderpasseventcb },
	{ "remove_renderpasseventcb", &LuaClass_RenderPassNodeGraph::LUA_removerenderpasseventcb },
	{ 0, 0 }
};

LuaClass_RenderPassNodeGraph::LuaClass_RenderPassNodeGraph( lua_State* p_L ) :
m_renderpass_event_cb( this, &LuaClass_RenderPassNodeGraph::on_renderpass_event)
{
	int argc = lua_gettop( p_L );
	if( argc < 1 )
	{	
        LUA_ERROR( "RenderPassNodeGraph::RenderPassNodeGraph : argument(s) missing" );
	}

    dsstring id = luaL_checkstring( p_L, 1 );
    m_passes_render.SetRendergraph( &m_rendergraph );
    MainService::getInstance()->RegisterRenderGraph( id, this );
    m_rendergraph.SetSystemsHub(MainService::getInstance()->GetHub());

    m_id = id;

	m_rendergraph.RegisterRenderPassEvtHandler(&m_renderpass_event_cb);
}

LuaClass_RenderPassNodeGraph::~LuaClass_RenderPassNodeGraph( void )
{
    MainService::getInstance()->UnregisterRenderGraph( m_id );
	m_rendergraph.UnregisterRenderPassEvtHandler(&m_renderpass_event_cb);
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
    bool targetdims_fromrenderer = true;
    long targetdims_width = 255; 
    long targetdims_height = 255;

    Core::Texture::RenderPurpose renderpurpose = Core::Texture::RENDERPURPOSE_COLOR;
    Core::Texture::RenderTarget rendertarget = Core::Texture::RENDERTARGET_GPU;

    if( argc >= 4 )
    {
        renderpurpose = static_cast<Core::Texture::RenderPurpose>( luaL_checkint( p_L, 4 ) );
    }

    if( argc >= 5 )
    {
        rendertarget = static_cast<Core::Texture::RenderTarget>( luaL_checkint( p_L, 5 ) );
    }

    if( argc >= 6 )
    {
        targetdims_fromrenderer = luaL_checkint( p_L, 6 );
    }

    if( argc  == 8 )
    {
        targetdims_width = luaL_checkint( p_L, 7 );
        targetdims_height = luaL_checkint( p_L, 8 );
    }

    if( m_passes.count( pass_id ) )
    {
        m_passes[child_pass_id].m_renderpassnode = m_passes[pass_id].m_renderpassnode.CreateChild( child_pass_id, target_stage, 
                                                                                                    renderpurpose,
                                                                                                    rendertarget,
                                                                                                    targetdims_fromrenderer,
                                                                                                    targetdims_width,
                                                                                                    targetdims_height );

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

int LuaClass_RenderPassNodeGraph::LUA_setpasstargetslice(lua_State* p_L)
{
    int argc = lua_gettop(p_L);
    if (argc < 2)
    {
        LUA_ERROR("RenderPassNodeGraph::set_pass_targetslice : argument(s) missing");
    }

    dsstring pass_id = luaL_checkstring(p_L, 1);
    int slice = luaL_checkint(p_L, 2);

    if (m_passes.count(pass_id))
    {
        m_passes[pass_id].m_renderpassnode.GetRenderingQueue()->SetTargetSlice(slice);
    }
    else
    {
        LUA_ERROR("RenderPassNodeGraph::set_pass_targetclearcolor : unknown pass id");
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

int LuaClass_RenderPassNodeGraph::LUA_configurepassviewportquadresources( lua_State* p_L )
{
    DrawSpace::Systems::Hub* hub = MainService::getInstance()->GetHub();
	Systems::ResourcesSystem& resources_system = hub->GetSystem<Systems::ResourcesSystem>("ResourcesSystem");

	int argc = lua_gettop( p_L );
	if( argc < 2 )
	{		
        LUA_ERROR( "RenderPassNodeGraph::configure_pass_viewportquad_resources : argument(s) missing" );
	}

    dsstring pass_id = luaL_checkstring( p_L, 1 );

    LuaClass_RenderConfig* rcfg = Luna<LuaClass_RenderConfig>::check( p_L, 2 );

    if( m_passes.count( pass_id ) )
    {
        LuaClass_RenderConfig::Data rcfg_data = rcfg->GetData();

        for (size_t i = 0; i < rcfg_data.render_contexts.size(); i++)
        {
            LuaClass_RenderContext::Data render_context = rcfg_data.render_contexts[i];
            if( render_context.rendercontextname == pass_id )
            {        
                ViewportQuad* vpq = m_passes[pass_id].m_renderpassnode.GetViewportQuad();
                if( vpq )
                {
                    //  on a besoin que d'un seul fx....
                    if( render_context.fxparams.size() < 1 )
                    {
                        cleanup_resources( p_L, pass_id );
                        LUA_ERROR( "RenderPassNodeGraph::configure_pass_viewportquad_resources : missing fx parameters description" );                
                    }            

                    LuaClass_FxParams::Data fx_params = render_context.fxparams[0];

                    ///////////////////////// les shaders
                    size_t nb_shaders = fx_params.shaders.size();

                    for( size_t i = 0; i < nb_shaders; i++ )
                    {
                        std::pair<dsstring,bool> shader_infos = fx_params.shaders[i];

                        dsstring shader_path = shader_infos.first;
                        bool is_compiled = shader_infos.second;
                        Shader* shader = _DRAWSPACE_NEW_(Shader, Shader(shader_path, is_compiled));

                        resources_system.LoadShader( shader, i );

                        m_passes[pass_id].m_fx.AddShader(shader);
                        m_passes[pass_id].m_renderpassnode.SetRenderingQueueUpdateFlag();

                    }
                    ///////////////////////// les rendestates

                    DrawSpace::Core::RenderStatesSet& rss = fx_params.rss;
                    m_passes[pass_id].m_fx.SetRenderStates( rss );

                    ///////////////////////// les textures

                    size_t nb_textures_set = render_context.textures_sets.size();
                    //  on a besoin que d'un seul jeu de textures...
                    if( nb_textures_set != 1 )
                    {
                        cleanup_resources( p_L, pass_id );
                        LUA_ERROR( "RenderPassNodeGraph::configure_pass_viewportquad_resources : no textures set provided !" );
                    }

                    LuaClass_TexturesSet::Data textures = render_context.textures_sets[0];

                    for( size_t i = 0; i < DrawSpace::Core::RenderingNode::NbMaxTextures; i++ )
                    {
                        dsstring texture_path = textures.textures[i];
                        if( texture_path != "" )
                        {
                            //bool status;
                            Texture* texture = _DRAWSPACE_NEW_( Texture, Texture( texture_path ) );
                            resources_system.LoadTexture(texture);
                            m_passes[pass_id].m_renderpassnode.GetViewportQuad()->SetTexture(texture, i);
                            m_passes[pass_id].m_renderpassnode.SetRenderingQueueUpdateFlag();
                        }
                    }

                    /// params de shaders

                    for( size_t j = 0; j < render_context.shaders_params.size(); j++ )
                    {
                        LuaClass_RenderContext::NamedShaderParam param = render_context.shaders_params[j];
                    
                        dsstring param_id = param.first;

                        RenderingNode::ShadersParams indexes = param.second;
                        m_passes[pass_id].m_renderpassnode.GetViewportQuad()->AddShaderParameter( indexes.shader_index, param_id, indexes.param_register );                    
                    }
                }
                else 
                {
                    LUA_ERROR( "RenderPassNodeGraph::configure_pass_viewportquad_resources : no viewportquad created for this pass" );
                }

                break;
            }
        }
    }
    else
    {
        LUA_ERROR( "RenderPassNodeGraph::configure_pass_viewportquad_resources : unknown pass id" );
    }

    return 0;
}

void LuaClass_RenderPassNodeGraph::cleanup_resources( lua_State* p_L, const dsstring& p_passid )
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
            LUA_ERROR( "RenderPassNodeGraph::cleanup_resources : no viewportquad created for this pass" );
        }
    }
    else
    {
        LUA_ERROR( "RenderPassNodeGraph::cleanup_resources : unknown pass id" );
    }
}

void LuaClass_RenderPassNodeGraph::on_renderpass_event(DrawSpace::RenderGraph::RenderPassNodeGraph::RenderPassEvent p_event, const dsstring& p_passname)
{
	for (auto& it = m_renderpassevent_lua_callbacks.begin(); it != m_renderpassevent_lua_callbacks.end(); ++it)
	{
		LuaContext::getInstance()->CallLuaFunc(it->second, static_cast<int>(p_event), p_passname);
	}
}

int LuaClass_RenderPassNodeGraph::LUA_releasepassviewportquadresources( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc < 1 )
	{		
        LUA_ERROR( "RenderPassNodeGraph::release_pass_viewportquad_resources : argument(s) missing" );
	}

    dsstring pass_id = luaL_checkstring( p_L, 1 );
    cleanup_resources( p_L, pass_id );

    return 0;
}

int LuaClass_RenderPassNodeGraph::LUA_updaterenderingqueues( lua_State* p_L )
{
    m_rendergraph.PushSignal_UpdatedRenderingQueues();
    return 0;
}

int LuaClass_RenderPassNodeGraph::LUA_disablepass(lua_State* p_L)
{
    int argc = lua_gettop(p_L);
    if (argc < 1)
    {
        LUA_ERROR("RenderPassNodeGraph::disable_pass : argument(s) missing");
    }

    dsstring pass_id = luaL_checkstring(p_L, 1);
    m_rendergraph.PushSignal_DisablePass(pass_id);

    return 0;
}

int LuaClass_RenderPassNodeGraph::LUA_enablepass(lua_State* p_L)
{
    int argc = lua_gettop(p_L);
    if (argc < 1)
    {
        LUA_ERROR("RenderPassNodeGraph::enable_pass : argument(s) missing");
    }

    dsstring pass_id = luaL_checkstring(p_L, 1);
    m_rendergraph.PushSignal_EnablePass(pass_id);

    return 0;
}

int LuaClass_RenderPassNodeGraph::LUA_cleanuprenderingqueues(lua_State* p_L)
{
    m_rendergraph.PushSignal_CleanupRenderingQueues();
    return 0;
}

int LuaClass_RenderPassNodeGraph::LUA_setviewportquadshaderrealvector( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc < 6 )
	{		
        LUA_ERROR( "RenderPassNodeGraph::set_viewportquadshaderrealvector : argument(s) missing" );
	}

    dsstring pass_id = luaL_checkstring( p_L, 1 );
    dsstring param_id = luaL_checkstring( p_L, 2 );
    dsreal valx = luaL_checknumber( p_L, 3 );
    dsreal valy = luaL_checknumber( p_L, 4 );
    dsreal valz = luaL_checknumber( p_L, 5 );
    dsreal valw = luaL_checknumber( p_L, 6 );

    if( m_passes.count( pass_id ) )
    {                        
        ViewportQuad* vpq = m_passes[pass_id].m_renderpassnode.GetViewportQuad();
        if( !vpq )
        {
            LUA_ERROR( "RenderPassNodeGraph::set_viewportquadshaderrealvector : no viewportquad created for this pass" );
        }
        else
        {
            LUA_TRY
            {
                vpq->SetShaderRealVector( param_id, Vector( valx, valy, valz, valw ) );

            } LUA_CATCH;    
        }
    }
    else
    {
        LUA_ERROR( "RenderPassNodeGraph::set_viewportquadshaderrealvector : unknown pass id" );
    }
    return 0;
}

int LuaClass_RenderPassNodeGraph::LUA_setviewportquadshaderrealmatrix(lua_State* p_L)
{
	int argc = lua_gettop(p_L);
	if (argc < 3)
	{
		LUA_ERROR("RenderPassNodeGraph::set_viewportquadshaderrealmatrix : argument(s) missing");
	}

	dsstring pass_id = luaL_checkstring(p_L, 1);
	dsstring param_id = luaL_checkstring(p_L, 2);
	LuaClass_Matrix* lua_mat = Luna<LuaClass_Matrix>::check(p_L, 3);

	if (m_passes.count(pass_id))
	{
		ViewportQuad* vpq = m_passes[pass_id].m_renderpassnode.GetViewportQuad();
		if (!vpq)
		{
			LUA_ERROR("RenderPassNodeGraph::set_viewportquadshaderrealmatrix : no viewportquad created for this pass");
		}
		else
		{
			LUA_TRY
			{
				Matrix mat = lua_mat->GetMatrix();
				mat.transpose();
				vpq->SetShaderRealMatrix(param_id, mat);

			} LUA_CATCH;
		}
	}
	else
	{
		LUA_ERROR("RenderPassNodeGraph::set_viewportquadshaderrealvector : unknown pass id");
	}
	return 0;


	return 0;
}

int LuaClass_RenderPassNodeGraph::LUA_addrenderpasseventcb(lua_State* p_L)
{
	LuaContext::AddCallback(p_L, [this](const std::string& p_cbid, int p_reffunc) { RegisterPassEventCallback(p_cbid, p_reffunc); });
	return 0;
}

int LuaClass_RenderPassNodeGraph::LUA_removerenderpasseventcb(lua_State* p_L)
{
	LuaContext::RemoveCallback(p_L, [this](const std::string& p_cbid)->int { return UnregisterPassEventCallback(p_cbid); });
	return 0;
}

int LuaClass_RenderPassNodeGraph::LUA_settargettexturedepth(lua_State* p_L)
{
    int argc = lua_gettop(p_L);
    if (argc < 2)
    {
        LUA_ERROR("RenderPassNodeGraph::set_targettexturedepth : argument(s) missing");
    }

    dsstring pass_id = luaL_checkstring(p_L, 1);
    int depth = luaL_checkint(p_L, 2);

    if (m_passes.count(pass_id))
    {
        Texture* target_texture = m_passes[pass_id].m_renderpassnode.GetTargetTexture();
        if (target_texture)
        {
            // declare this target as a 3D-Texture
            m_passes[pass_id].m_renderpassnode.GetTargetTexture()->SetDepth(depth);
        }
        else
        {
            LUA_ERROR("RenderPassNodeGraph::set_targettexturedepth : not an intermediate pass : " << pass_id);
        }
    }
    else
    {
        LUA_ERROR("RenderPassNodeGraph::set_targettexturedepth : unknown pass id");
    }
    return 0;
}

DrawSpace::RenderGraph::RenderPassNode& LuaClass_RenderPassNodeGraph::GetNode( const dsstring& p_pass_id )
{
    if( 0 == m_passes.count( p_pass_id ) )
    {
        _DSEXCEPTION( "unknown pass id" )
    }
    else
    {
        return m_passes[p_pass_id].m_renderpassnode;
    }
}



void LuaClass_RenderPassNodeGraph::RegisterPassEventCallback(const dsstring& p_id, int p_regindex)
{
	m_renderpassevent_lua_callbacks[p_id] = p_regindex;
}

int LuaClass_RenderPassNodeGraph::UnregisterPassEventCallback(const dsstring& p_id)
{
	int index = -1;
	if (m_renderpassevent_lua_callbacks.count(p_id))
	{
		index = m_renderpassevent_lua_callbacks[p_id];
		m_renderpassevent_lua_callbacks.erase(p_id);
	}
	return index;
}
