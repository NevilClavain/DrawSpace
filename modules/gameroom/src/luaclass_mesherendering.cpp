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
#include "luaclass_mesherendering.h"
#include "luaclass_texturesset.h"
#include "luaclass_fxparams.h"
#include "luaclass_rendercontext.h"
#include "luaclass_renderconfig.h"
#include "luaclass_renderpassnodegraph.h"
#include "luaclass_entity.h"
#include "luaclass_matrix.h"

#include "mainservice.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Utils;
using namespace DrawSpace::Aspect;
using namespace DrawSpace::AspectImplementations;


const char LuaClass_MesheRendering::className[] = "MesheRendering";
const Luna<LuaClass_MesheRendering>::RegType LuaClass_MesheRendering::methods[] =
{
    { "attach_toentity", &LuaClass_MesheRendering::LUA_attachtoentity },
    { "detach_fromentity", &LuaClass_MesheRendering::LUA_detachfromentity },
    { "configure", &LuaClass_MesheRendering::LUA_configure },
    { "release", &LuaClass_MesheRendering::LUA_release },
    { "register_to_rendering", &LuaClass_MesheRendering::LUA_registertorendering },
    { "unregister_from_rendering", &LuaClass_MesheRendering::LUA_unregisterfromrendering },
    { "set_shaderreal", &LuaClass_MesheRendering::LUA_setshaderreal },
    { "set_shaderrealvector", &LuaClass_MesheRendering::LUA_setshaderrealvector },
    { "set_shaderrealmatrix", &LuaClass_MesheRendering::LUA_setshaderrealmatrix },
    { "set_shaderbool", &LuaClass_MesheRendering::LUA_setshaderbool },
	{ 0, 0 }
};

LuaClass_MesheRendering::LuaClass_MesheRendering( lua_State* p_L ) :
m_meshe_render( NULL ),
m_entity_rendering_aspect( NULL ),
m_entity( NULL )
{
    m_meshe.SetImporter( MainService::GetInstance()->GetMesheImport() );
}

LuaClass_MesheRendering::~LuaClass_MesheRendering( void )
{
}

int LuaClass_MesheRendering::LUA_attachtoentity( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc < 1 )
	{		
        LUA_ERROR( "MesheRendering::attach_toentity : argument(s) missing" );
	}

    LuaClass_Entity* lua_ent = Luna<LuaClass_Entity>::check( p_L, 1 );

    DrawSpace::Core::Entity& entity = lua_ent->GetEntity();
    RenderingAspect* rendering_aspect = entity.GetAspect<RenderingAspect>();

    if( NULL == rendering_aspect )
    {
        LUA_ERROR( "MesheRendering::attach_toentity : entity has no rendering aspect!" );
    }

    m_entity_rendering_aspect = rendering_aspect;
    m_entity = &entity;

    m_meshe_render = _DRAWSPACE_NEW_( DrawSpace::AspectImplementations::MesheRenderingAspectImpl, DrawSpace::AspectImplementations::MesheRenderingAspectImpl );
    m_entity_rendering_aspect->AddImplementation( m_meshe_render );

    return 0;
}

int LuaClass_MesheRendering::LUA_detachfromentity( lua_State* p_L )
{
    if( NULL == m_entity )
    {
        LUA_ERROR( "MesheRendering::detach_fromentity : argument(s) missing" );
    }

    if( m_meshe_render )
    {
        _DRAWSPACE_DELETE_( m_meshe_render );
    }

    LUA_TRY
    {
        m_entity_rendering_aspect->RemoveImplementation( m_meshe_render );

    } LUA_CATCH; 

    m_entity_rendering_aspect = NULL;
    m_entity = NULL;

    return 0;
}


int LuaClass_MesheRendering::LUA_configure( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc < 4 )
	{		
        LUA_ERROR( "MesheRendering::configure : argument(s) missing" );
	}

    dsstring pass_id = luaL_checkstring( p_L, 1 );
    //LuaClass_RenderAssembly* lua_renderassembly = Luna<LuaClass_RenderAssembly>::check( p_L, 2 );
    LuaClass_RenderConfig* rcfg = Luna<LuaClass_RenderConfig>::check( p_L, 2 );

    dsstring meshe_path = luaL_checkstring( p_L, 3 );
    int meshe_index = luaL_checkint( p_L, 4 );

    // recupere l'aspect rendu s'il existe pour cette entitee
    if( m_entity_rendering_aspect )
    {               
        LUA_TRY
        {
            m_entity_rendering_aspect->AddComponent<MesheRenderingAspectImpl::PassSlot>( pass_id, pass_id );
            RenderingNode* rnode = m_entity_rendering_aspect->GetComponent<MesheRenderingAspectImpl::PassSlot>( pass_id )->getPurpose().GetRenderingNode();

            m_renderingnodes[pass_id] = rnode;

            for( int i = 0; i < rcfg->GetRenderContextListSize(); i++ )
            {
                LuaClass_RenderContext* render_context = rcfg->GetRenderContext( i );
                if( render_context->GetPassName() == pass_id )
                {
                    //  on a besoin que d'un seul fx....
                    if( render_context->GetFxParamsListSize() < 1 )
                    {
                        cleanup_resources( p_L, pass_id );
                        LUA_ERROR( "MesheRendering::configure : missing fx parameters description" );                
                    }  

                    LuaClass_FxParams* fx_params = render_context->GetFxParams( 0 );

                    ///////////////////////// les shaders
                    size_t nb_shaders = fx_params->GetNbShaderFiles();
                    for( size_t i = 0; i < nb_shaders; i++ )
                    {
                        std::pair<dsstring,bool> shader_infos = fx_params->GetShaderFile( i );

                        dsstring shader_path = shader_infos.first;
                        bool is_compiled = shader_infos.second;

                        bool status;

                        Shader* shader = _DRAWSPACE_NEW_( Shader, Shader );
                        status = shader->LoadFromFile( shader_path, is_compiled );

                        if( !status )
                        {
                            // clean tout ce qui a deja ete charge...
                            cleanup_resources( p_L, pass_id );
                            LUA_ERROR( "MesheRendering::configure : shader loading operation failed" );
                        }
                        else
                        {
                            m_fx.AddShader( shader );                    
                        }
                    }

                    ///////////////////////// les rendestates

                    DrawSpace::Core::RenderStatesSet& rss = fx_params->GetRenderStatesSet();
                    m_fx.SetRenderStates( rss );


                    ///////////////////////// les textures

                    size_t nb_textures_set = render_context->GetTexturesSetListSize();
                    //  on a besoin que d'un seul jeu de textures...
                    if( nb_textures_set != 1 )
                    {
                        cleanup_resources( p_L, pass_id );
                        LUA_ERROR( "MesheRendering::configure : no textures set provided !" );
                    }

                    
                    LuaClass_TexturesSet* textures = render_context->GetTexturesSet( 0 );

                    for( size_t i = 0; i < DrawSpace::Core::RenderingNode::NbMaxTextures; i++ )
                    {
                        dsstring texture_path = textures->GetTextureFile( i );
                        if( texture_path != "" )
                        {
                            bool status;
                            Texture* texture = _DRAWSPACE_NEW_( Texture, Texture( texture_path ) );
                            status = texture->LoadFromFile();
                            if( !status )
                            {
                                // clean tout ce qui a deja ete charge...
                                cleanup_resources( p_L, pass_id );
                                LUA_ERROR( "MesheRendering::configure : texture loading operation failed" );
                            }
                            else
                            {
                                rnode->SetTexture( texture, i );
                            }
                        }
                    }

                    // et pour finir, le meshe
                    bool status = m_meshe.LoadFromFile( meshe_path, meshe_index );
                    if( !status )
                    {
                        cleanup_resources( p_L, pass_id );
                        LUA_ERROR( "MesheRendering::configure : meshe loading operation failed" );
                    }
                    rnode->SetMeshe( &m_meshe );
                    rnode->SetFx( &m_fx );

                    /// params de shaders

                    for( int j = 0; j < render_context->GetShadersParamsListSize(); j++ )
                    {
                        LuaClass_RenderContext::NamedShaderParam param = render_context->GetNamedShaderParam( j );
                    
                        dsstring param_id = param.first;

                        RenderingNode::ShadersParams indexes = param.second;
                        rnode->AddShaderParameter( indexes.shader_index, param_id, indexes.param_register );                    
                    }

                    break;
                }
            }

        } LUA_CATCH;
    }
    else
    {
        LUA_ERROR( "MesheRendering::configure : not attached to an entity" );
    }

    return 0;
}

int LuaClass_MesheRendering::LUA_setshaderreal( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc < 3 )
	{		
        LUA_ERROR( "MesheRendering::set_shaderreal : argument(s) missing" );
	}

    dsstring pass_id = luaL_checkstring( p_L, 1 );
    dsstring param_id = luaL_checkstring( p_L, 2 );
    dsreal val = luaL_checknumber( p_L, 3 );

    if( 0 == m_renderingnodes.count( pass_id ) )
    {
        LUA_ERROR( "MesheRendering::set_shaderreal : unknown pass" ) ;
    }
    else
    {
        LUA_TRY
        {
            m_renderingnodes[pass_id]->SetShaderReal( param_id, val );

        } LUA_CATCH;    
    }

    return 0;
}

int LuaClass_MesheRendering::LUA_setshaderrealvector( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc < 6 )
	{		
        LUA_ERROR( "MesheRendering::set_shaderrealvector : argument(s) missing" );
	}

    dsstring pass_id = luaL_checkstring( p_L, 1 );
    dsstring param_id = luaL_checkstring( p_L, 2 );
    dsreal valx = luaL_checknumber( p_L, 3 );
    dsreal valy = luaL_checknumber( p_L, 4 );
    dsreal valz = luaL_checknumber( p_L, 5 );
    dsreal valw = luaL_checknumber( p_L, 5 );

    if( 0 == m_renderingnodes.count( pass_id ) )
    {
        LUA_ERROR( "MesheRendering::set_shaderrealvector : unknown pass" ) ;
    }
    else
    {
        LUA_TRY
        {
            m_renderingnodes[pass_id]->SetShaderRealVector( param_id, Vector( valx, valy, valz, valw ) );

        } LUA_CATCH;    
    }
    return 0;
}

int LuaClass_MesheRendering::LUA_setshaderrealmatrix( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc < 3 )
	{		
        LUA_ERROR( "MesheRendering::set_shaderrealmatrix : argument(s) missing" );
	}

    dsstring pass_id = luaL_checkstring( p_L, 1 );
    dsstring param_id = luaL_checkstring( p_L, 2 );
    LuaClass_Matrix* lua_mat = Luna<LuaClass_Matrix>::check( p_L, 3 );

    if( 0 == m_renderingnodes.count( pass_id ) )
    {
        LUA_ERROR( "MesheRendering::set_shaderrealmatrix : unknown pass" ) ;
    }
    else
    {
        LUA_TRY
        {
            m_renderingnodes[pass_id]->SetShaderRealMatrix( param_id, lua_mat->GetMatrix() );

        } LUA_CATCH;    
    }
    return 0;
}

int LuaClass_MesheRendering::LUA_setshaderbool( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc < 3 )
	{
        LUA_ERROR( "MesheRendering::set_shaderreal : argument(s) missing" );
	}

    dsstring pass_id = luaL_checkstring( p_L, 1 );
    dsstring param_id = luaL_checkstring( p_L, 2 );
    bool val = luaL_checkint( p_L, 3 );

    if( 0 == m_renderingnodes.count( pass_id ) )
    {
        LUA_ERROR( "MesheRendering::set_shaderreal : unknown pass" ) ;
    }
    else
    {
        LUA_TRY
        {
            m_renderingnodes[pass_id]->SetShaderBool( param_id, val );

        } LUA_CATCH;    
    }
    return 0;
}

void LuaClass_MesheRendering::cleanup_resources( lua_State* p_L, const dsstring& p_id )
{
    if( m_entity_rendering_aspect )
    {
        for( auto it = m_renderingnodes.begin(); it != m_renderingnodes.end(); ++it )
        {
            it->second->CleanupShaderParams();
        }
        m_renderingnodes.clear();

        RenderingNode* rnode = m_entity_rendering_aspect->GetComponent<MesheRenderingAspectImpl::PassSlot>( p_id )->getPurpose().GetRenderingNode();

        for( long i = 0; i < m_fx.GetShadersListSize(); i++ )
        {
            Shader* shader = m_fx.GetShader( i );
            _DRAWSPACE_DELETE_( shader );
        }
        m_fx.ClearShaders();

        for( long i = 0; i < rnode->GetTextureListSize(); i++ )
        {
            Texture* texture = rnode->GetTexture( i );
            if( texture )
            {
                _DRAWSPACE_DELETE_( texture );
                rnode->SetTexture( NULL, i );
            }
        }

        m_meshe.ClearTriangles();
        m_meshe.ClearVertices();

        LUA_TRY
        {
            m_entity_rendering_aspect->RemoveComponent<MesheRenderingAspectImpl::PassSlot>( p_id );

        } LUA_CATCH; 
    }
    else
    {
        LUA_ERROR( "MesheRendering::cleanup_resources : no rendering aspect" );
    }
}

int LuaClass_MesheRendering::LUA_release( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc < 1 )
	{		
        LUA_ERROR( "MesheRendering::release : argument(s) missing" );
	}

    dsstring pass_id = luaL_checkstring( p_L, 1 );

    cleanup_resources( p_L, pass_id );
    return 0;
}

int LuaClass_MesheRendering::LUA_registertorendering( lua_State* p_L )
{
    if( NULL == m_meshe_render )
    {
        LUA_ERROR( "MesheRendering::register_to_rendering : no meshe rendering aspect impl created" );
    }

	int argc = lua_gettop( p_L );
	if( argc < 1 )
	{		
        LUA_ERROR( "MesheRendering::register_to_rendering : argument(s) missing" );
	}

    LuaClass_RenderPassNodeGraph* lua_rg = Luna<LuaClass_RenderPassNodeGraph>::check( p_L, 1 );

    m_meshe_render->RegisterToRendering( lua_rg->GetRenderGraph() );
    return 0;
}

int LuaClass_MesheRendering::LUA_unregisterfromrendering( lua_State* p_L )
{
    if( NULL == m_meshe_render )
    {
        LUA_ERROR( "MesheRendering::unregister_from_rendering : no meshe rendering aspect impl created" );
    }

	int argc = lua_gettop( p_L );
	if( argc < 1 )
	{		
        LUA_ERROR( "MesheRendering::unregister_from_rendering : argument(s) missing" );
	}

    LuaClass_RenderPassNodeGraph* lua_rg = Luna<LuaClass_RenderPassNodeGraph>::check( p_L, 1 );

    m_meshe_render->UnregisterFromRendering( lua_rg->GetRenderGraph() );
    return 0;
}