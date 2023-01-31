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
#include "luaclass_meshrendering.h"
#include "luaclass_texturesset.h"
#include "luaclass_fxparams.h"
#include "luaclass_rendercontext.h"
#include "luaclass_renderconfig.h"
#include "luaclass_renderpassnodegraph.h"
#include "luaclass_entity.h"
#include "luaclass_matrix.h"
#include "meshrenderingaspectimpl.h"
#include "renderingaspect.h"
#include "resourcesaspect.h"
#include "passslot.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Utils;
using namespace DrawSpace::Aspect;
using namespace DrawSpace::AspectImplementations;


const char LuaClass_MeshRendering::className[] = "MeshRendering";
const Luna<LuaClass_MeshRendering>::RegType LuaClass_MeshRendering::methods[] =
{
    { "attach_toentity", &LuaClass_MeshRendering::LUA_attachtoentity },
    { "detach_fromentity", &LuaClass_MeshRendering::LUA_detachfromentity },
    { "configure", &LuaClass_MeshRendering::LUA_configure },
    { "release", &LuaClass_MeshRendering::LUA_release },
    { "register_to_rendering", &LuaClass_MeshRendering::LUA_registertorendering },
    { "unregister_from_rendering", &LuaClass_MeshRendering::LUA_unregisterfromrendering },
    { "set_shaderreal", &LuaClass_MeshRendering::LUA_setshaderreal },
    { "set_shaderrealvector", &LuaClass_MeshRendering::LUA_setshaderrealvector },
	{ "set_shaderrealinvector", &LuaClass_MeshRendering::LUA_setshaderrealinvector },
    { "set_shaderrealmatrix", &LuaClass_MeshRendering::LUA_setshaderrealmatrix },
    { "set_shaderbool", &LuaClass_MeshRendering::LUA_setshaderbool },
    { "set_passnodetexturefrompass", &LuaClass_MeshRendering::LUA_setpassnodetexturefrompass },
    { "set_normalegenerationmode", &LuaClass_MeshRendering::LUA_setNormaleGenerationMode },
    { "set_tbgenerationmode", &LuaClass_MeshRendering::LUA_setTBGenerationMode },
    { "set_setnormaletransformation", &LuaClass_MeshRendering::LUA_setNormaleTransformation },
	{ "set_passforrenderid", & LuaClass_MeshRendering::LUA_setPassForRenderId },
	{ 0, 0 }
};

LuaClass_MeshRendering::LuaClass_MeshRendering( lua_State* p_L )
{
}

LuaClass_MeshRendering::~LuaClass_MeshRendering( void )
{
}

int LuaClass_MeshRendering::LUA_attachtoentity( lua_State* p_L )
{
    const auto argc{ lua_gettop(p_L) };
	if( argc < 1 )
	{		
        LUA_ERROR( "MesheRendering::attach_toentity : argument(s) missing" );
	}

    LuaClass_Entity* lua_ent = Luna<LuaClass_Entity>::check( p_L, 1 );

    DrawSpace::Core::Entity& entity{ lua_ent->GetEntity() };
    RenderingAspect* rendering_aspect = entity.GetAspect<RenderingAspect>();

    if( NULL == rendering_aspect )
    {
        LUA_ERROR( "MesheRendering::attach_toentity : entity has no rendering aspect!" );
    }

    m_entity_rendering_aspect = rendering_aspect;
    m_entity = &entity;

    m_entity_rendering_aspect->AddImplementation( &m_meshe_render, NULL );

    return 0;
}

int LuaClass_MeshRendering::LUA_detachfromentity( lua_State* p_L )
{
    if( NULL == m_entity )
    {
        LUA_ERROR( "MesheRendering::detach_fromentity : argument(s) missing" );
    }

    LUA_TRY
    {
        m_entity_rendering_aspect->RemoveImplementation( &m_meshe_render );

    } LUA_CATCH; 

    m_entity_rendering_aspect = nullptr;
    m_entity = nullptr;

    return 0;
}

int LuaClass_MeshRendering::LUA_configure( lua_State* p_L )
{
    if (NULL == m_entity)
    {
        LUA_ERROR("MesheRendering::configure : no attached entity");
    }

    const auto resources_aspect = m_entity->GetAspect<ResourcesAspect>();
    if (!resources_aspect)
    {
        LUA_ERROR("MesheRendering::configure : attached entity has no resources aspect !");
    }

    const auto argc{ lua_gettop(p_L) };
    if( argc < 3 )
	{		
        LUA_ERROR( "MesheRendering::configure : argument(s) missing" );
	}

    const auto rcfg{ Luna<LuaClass_RenderConfig>::check(p_L, 1) };
    const auto meshe_path{ luaL_checkstring(p_L, 2) };
    const auto meshe_name{ luaL_checkstring(p_L, 3) };

    // recupere l'aspect rendu s'il existe pour cette entitee
    if( m_entity_rendering_aspect )
    {
        LUA_TRY
        {
            const auto rcfg_data{ rcfg->GetData() };

            for(size_t i = 0; i < rcfg_data.render_contexts.size(); i++)
            {
                const auto render_context{ rcfg_data.render_contexts[i] };
				
				if (m_rcname_to_passes.end() != m_rcname_to_passes.find(render_context.rendercontextname))
				{
                    for (auto& pass_id : m_rcname_to_passes.at(render_context.rendercontextname))
                    {
                        m_entity_rendering_aspect->AddComponent<PassSlot>(pass_id, pass_id);
                        auto rnode{ m_entity_rendering_aspect->GetComponent<PassSlot>(pass_id)->getPurpose().GetRenderingNode() };
                        m_renderingnodes[pass_id] = rnode;

                        //  on a besoin que d'un seul fx....
                        if (render_context.fxparams.size() < 1)
                        {
                            cleanup_resources(p_L);
                            LUA_ERROR("MesheRendering::configure : missing fx parameters description");
                        }

                        auto fx_params{ render_context.fxparams[0] };

                        auto fx { _DRAWSPACE_NEW_(Fx, Fx) };
                        rnode->SetFx(fx);

                        ///////////////////////// les shaders
                        const auto nb_shaders{ fx_params.shaders.size() };
                        for (size_t j = 0; j < nb_shaders; j++)
                        {
                            const auto shader_infos{ fx_params.shaders[j] };

                            const auto shader_path{ shader_infos.first };
                            const auto is_compiled{ shader_infos.second };

                            auto shader{ _DRAWSPACE_NEW_(Shader, Shader(shader_path, is_compiled)) };

                            const auto res_id{ dsstring("shader_") + std::to_string((int)shader) };

                            resources_aspect->AddComponent<std::tuple<Shader*, bool, int>>(res_id, std::make_tuple(shader, false, j));
                            fx->AddShader(shader);
                        }

                        ///////////////////////// les rendestates

                        const auto rss{ fx_params.rss };
                        fx->SetRenderStates(rss);

                        ///////////////////////// les textures

                        const auto nb_textures_set{ render_context.textures_sets.size() };
                        if (nb_textures_set > 0)
                        {
                            const auto textures{ render_context.textures_sets[0] };

                            for (int j = 0; j < DrawSpace::Core::RenderingNode::NbMaxTextures; j++)
                            {
                                const auto texture_path{ textures.textures[j] };
                                if (texture_path != "")
                                {
                                    const auto texture{ _DRAWSPACE_NEW_(Texture, Texture(texture_path)) };

                                    const auto res_id{ dsstring("texture_") + std::to_string((int)texture) };
                                    resources_aspect->AddComponent<std::tuple<Texture*, bool>>(res_id, std::make_tuple(texture, false));
                                    rnode->SetTexture(texture, j);
                                }
                            }
                        }

                        ///////////////////////// les vertex textures

                        const auto nb_vtextures_set{ render_context.vertex_textures_sets.size() };
                        if (nb_vtextures_set > 0)
                        {
                            const auto vtextures{ render_context.vertex_textures_sets[0] };

                            for (int j = 0; j < DrawSpace::Core::RenderingNode::NbMaxTextures; j++)
                            {
                                const auto texture_path{ vtextures.textures[j] };
                                if (texture_path != "")
                                {
                                    //bool status;
                                    auto texture{ _DRAWSPACE_NEW_(Texture, Texture(texture_path)) };

                                    const auto res_id{ dsstring("vtexture_") + std::to_string((int)texture) };
                                    resources_aspect->AddComponent<std::tuple<Texture*, bool>>(res_id, std::make_tuple(texture, false));
                                    rnode->SetVertexTexture(texture, j);
                                }
                            }
                        }

                        const auto meshe_res_id{ dsstring("meshe_") + pass_id };

                        resources_aspect->AddComponent<std::tuple<Meshe*, dsstring, dsstring, bool>>(meshe_res_id,
                            std::make_tuple(&m_meshe, meshe_path, meshe_name, false));

                        m_meshe.SetPath(meshe_path);

                        rnode->SetMeshe(&m_meshe);

                        /// params de shaders

                        for (size_t j = 0; j < render_context.shaders_params.size(); j++)
                        {
                            LuaClass_RenderContext::NamedShaderParam param = render_context.shaders_params[j];

                            dsstring param_id = param.first;

                            RenderingNode::ShadersParams indexes = param.second;
                            rnode->AddShaderParameter(indexes.shader_index, param_id, indexes.param_register);
                        }

                        /// rendering order

                        rnode->SetOrderNumber(render_context.rendering_order);
                    }
				}
            } // for

        } LUA_CATCH;
    }
    else
    {
        LUA_ERROR( "MesheRendering::configure : not attached to an entity" );
    }

    return 0;
}

int LuaClass_MeshRendering::LUA_setshaderreal( lua_State* p_L )
{
    const auto argc{ lua_gettop(p_L) };
	if( argc < 3 )
	{		
        LUA_ERROR( "MesheRendering::set_shaderreal : argument(s) missing" );
	}

    const auto pass_id{ luaL_checkstring(p_L, 1) };
    const auto param_id{ luaL_checkstring(p_L, 2) };
    const auto val{ luaL_checknumber(p_L, 3) };

    if( 0 == m_renderingnodes.count( pass_id ) )
    {
        LUA_ERROR( "MesheRendering::set_shaderreal : unknown pass" ) ;
    }
    else
    {
        LUA_TRY
        {
            m_renderingnodes.at(pass_id)->SetShaderReal( param_id, val );

        } LUA_CATCH;    
    }

    return 0;
}

int LuaClass_MeshRendering::LUA_setshaderrealvector( lua_State* p_L )
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
    dsreal valw = luaL_checknumber( p_L, 6 );

    if( 0 == m_renderingnodes.count( pass_id ) )
    {
        LUA_ERROR( dsstring( "MesheRendering::set_shaderrealvector : unknown pass " ) + pass_id) ;
    }
    else
    {
        LUA_TRY
        {
            m_renderingnodes.at(pass_id)->SetShaderRealVector( param_id, Vector( valx, valy, valz, valw ) );

        } LUA_CATCH;    
    }
    return 0;
}

int LuaClass_MeshRendering::LUA_setshaderrealinvector(lua_State* p_L)
{
    const auto argc{ lua_gettop(p_L) };
	if (argc < 4)
	{
		LUA_ERROR("MesheRendering::set_shaderrealinvector : argument(s) missing");
	}

    const auto pass_id{ luaL_checkstring(p_L, 1) };
    const auto param_id{ luaL_checkstring(p_L, 2) };
    const auto param_index_in_vector{ luaL_checkinteger(p_L, 3) };
    const auto val{ luaL_checknumber(p_L, 4) };

	if (0 == m_renderingnodes.count(pass_id))
	{
		LUA_ERROR("MesheRendering::set_shaderrealinvector : unknown pass");
	}
	else
	{
		LUA_TRY
		{
            m_renderingnodes.at(pass_id)->SetShaderRealInVector(param_id, param_index_in_vector, val);

		} LUA_CATCH;
	}
	return 0;
}

int LuaClass_MeshRendering::LUA_setshaderrealmatrix( lua_State* p_L )
{
    const auto argc{ lua_gettop(p_L) };
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
            m_renderingnodes.at(pass_id)->SetShaderRealMatrix( param_id, lua_mat->GetMatrix() );

        } LUA_CATCH;    
    }
    return 0;
}

int LuaClass_MeshRendering::LUA_setshaderbool( lua_State* p_L )
{
    const auto argc{ lua_gettop(p_L) };
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
            m_renderingnodes.at(pass_id)->SetShaderBool( param_id, val );

        } LUA_CATCH;    
    }
    return 0;
}


void LuaClass_MeshRendering::cleanup_resources( lua_State* p_L )
{
    if (NULL == m_entity)
    {
        LUA_ERROR("MesheRendering::cleanup_resources : no attached entity");
    }
    auto resources_aspect{ m_entity->GetAspect<ResourcesAspect>() };
    if (!resources_aspect)
    {
        LUA_ERROR("MesheRendering::cleanup_resources : attached entity has no resources aspect !");
    }

    if( m_entity_rendering_aspect )
    {
        for( auto it = m_renderingnodes.begin(); it != m_renderingnodes.end(); ++it )
        {
            it->second->CleanupShaderParams();
            const auto id{ it->first };
            const auto rnode{ it->second };
           
            auto fx{ rnode->GetFx() };
			if (fx)
			{
				for (long i = 0; i < fx->GetShadersListSize(); i++)
				{
                    auto shader{ fx->GetShader(i) };
                    const auto res_id{ dsstring("shader_") + std::to_string((int)shader) };
					resources_aspect->RemoveComponent<std::tuple<Shader*, bool, int>>(res_id);

					_DRAWSPACE_DELETE_(shader);
				}
				fx->ClearShaders();

                _DRAWSPACE_DELETE_(fx);
			}
            
            for( int i = 0; i < rnode->GetTextureListSize(); i++ )
            {
                auto texture{ rnode->GetTexture(i) };
                if( texture )
                {
                    if( 0 == m_external_textures.count( texture ) )
                    {
                        // pas une texture target d'une pass quelconque (car sinon dans ce cas, ce n'est pas a cette classe de 
                        // rdessallouer)

                        const auto res_id{ dsstring("texture_") + std::to_string((int)texture) };
                        resources_aspect->RemoveComponent<std::tuple<Texture*, bool>>(res_id);

                        _DRAWSPACE_DELETE_( texture );
                        rnode->SetTexture( NULL, i );
                    }
                    else
                    {
                        m_external_textures.erase( texture );
                    }
                }
            }

            for( int i = 0; i < rnode->GetTextureListSize(); i++ )
            {
                auto vtexture{ rnode->GetVertexTexture(i) };
                if( vtexture )
                {
                    if( 0 == m_external_textures.count( vtexture ) )
                    {
                        // pas une texture target d'une pass quelconque (car sinon dans ce cas, ce n'est pas a cette classe de 
                        // rdessallouer)

                        const auto res_id{ dsstring("vtexture_") + std::to_string((int)vtexture) };
                        resources_aspect->RemoveComponent<std::tuple<Texture*, bool>>(res_id);

                        _DRAWSPACE_DELETE_( vtexture );
                        rnode->SetVertexTexture( NULL, i );
                    }
                    else
                    {
                        m_external_textures.erase( vtexture );
                    }
                }
            }

            m_entity_rendering_aspect->RemoveComponent<PassSlot>( id );

            const auto meshe_res_id{ dsstring("meshe_") + id };
			if (resources_aspect->GetComponent<std::tuple<Meshe*, dsstring, dsstring, bool>>(meshe_res_id))
			{
				resources_aspect->RemoveComponent<std::tuple<Meshe*, dsstring, dsstring, bool>>(meshe_res_id);
			}
        }
        m_renderingnodes.clear();

        m_meshe.ClearTriangles();
        m_meshe.ClearVertices();
    }
    else
    {
        LUA_ERROR( "MesheRendering::cleanup_resources : no rendering aspect" );
    }
}

int LuaClass_MeshRendering::LUA_release( lua_State* p_L )
{
    cleanup_resources( p_L );
    return 0;
}

int LuaClass_MeshRendering::LUA_registertorendering( lua_State* p_L )
{
    const auto argc{ lua_gettop(p_L) };
	if( argc < 1 )
	{		
        LUA_ERROR( "MesheRendering::register_to_rendering : argument(s) missing" );
	}

    const auto lua_rg{ Luna<LuaClass_RenderPassNodeGraph>::check(p_L, 1) };

    m_meshe_render.RegisterToRendering( lua_rg->GetRenderGraph() );
    return 0;
}

int LuaClass_MeshRendering::LUA_unregisterfromrendering( lua_State* p_L )
{
    const auto argc{ lua_gettop(p_L) };
	if( argc < 1 )
	{		
        LUA_ERROR( "MesheRendering::unregister_from_rendering : argument(s) missing" );
	}

    const auto lua_rg{ Luna<LuaClass_RenderPassNodeGraph>::check(p_L, 1) };

    m_meshe_render.UnregisterFromRendering( lua_rg->GetRenderGraph() );
    return 0;
}

int LuaClass_MeshRendering::LUA_setpassnodetexturefrompass( lua_State* p_L )
{
    const auto argc{ lua_gettop(p_L) };
	if( argc < 4 )
	{		
        LUA_ERROR( "MesheRendering::set_passnodetexturefrompass : argument(s) missing" );
	}

    const auto rg{ Luna<LuaClass_RenderPassNodeGraph>::check(p_L, 1) };
    const auto pass_src_id{ luaL_checkstring(p_L, 2) };
    const auto pass_dest_id{ luaL_checkstring(p_L, 3) };
    const auto stage{ luaL_checkint(p_L, 4) };

    if( 0 == m_renderingnodes.count( pass_dest_id ) )
    {
        LUA_ERROR( "MesheRendering::set_passnodetexturefrompass : unknown pass" ) ;
    }
    else
    {
        auto target_texture{ rg->GetNode(pass_src_id).GetTargetTexture() };
        m_renderingnodes[pass_dest_id]->SetTexture( target_texture, stage );
        m_external_textures.insert( target_texture );
    }

    return 0;
}

int LuaClass_MeshRendering::LUA_setNormaleGenerationMode(lua_State* p_L)
{
    const auto argc{ lua_gettop(p_L) };
    if (argc < 1)
    {
        LUA_ERROR("MesheRendering::set_normalegenerationmode : argument(s) missing");
    }

    const auto mode{ luaL_checkint(p_L, 1) };

    m_meshe.SetNGenerationMode( static_cast<Meshe::NormalesGenerationMode>(mode));
    return 0;
}

int LuaClass_MeshRendering::LUA_setTBGenerationMode(lua_State* p_L)
{
    const auto argc{ lua_gettop(p_L) };
    if (argc < 1)
    {
        LUA_ERROR("MesheRendering::set_tbgenerationmode : argument(s) missing");
    }

    const auto mode{ luaL_checkint(p_L, 1) };

    m_meshe.SetTBGenerationMode(static_cast<Meshe::TangentBinormalesGenerationMode>(mode));
    return 0;
}

int LuaClass_MeshRendering::LUA_setNormaleTransformation(lua_State* p_L)
{
    const auto argc{ lua_gettop(p_L) };
    if (argc < 1)
    {
        LUA_ERROR("MesheRendering::set_setnormaletransformation : argument(s) missing");
    }

    const auto lua_ent{ Luna<LuaClass_Matrix>::check(p_L, 1) };

    m_meshe.SetNormalesTransf(lua_ent->GetMatrix());
    return 0;
}

int LuaClass_MeshRendering::LUA_setPassForRenderId(lua_State* p_L)
{
    const auto argc{ lua_gettop(p_L) };
	if (argc < 2)
	{
		LUA_ERROR("MesheRendering::set_passforrenderid : argument(s) missing");
	}

    const auto rc_id{ luaL_checkstring(p_L, 1) };
    const auto pass_id{ luaL_checkstring(p_L, 2) };

    m_rcname_to_passes[rc_id].push_back(pass_id);

	return 0;
}
