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
#include "luaclass_linemeshrendering.h"
#include "luaclass_renderconfig.h"
#include "luaclass_renderpassnodegraph.h"

#include "renderingaspect.h"
#include "resourcesaspect.h"

#include "luaclass_entity.h"
#include "passslot.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Utils;
using namespace DrawSpace::Aspect;
//using namespace DrawSpace::AspectImplementations;


const char LuaClass_LineMeshRendering::className[] = "LineMeshRendering";
const Luna<LuaClass_LineMeshRendering>::RegType LuaClass_LineMeshRendering::methods[] =
{
    { "attach_toentity", &LuaClass_LineMeshRendering::LUA_attachtoentity },
    { "detach_fromentity", &LuaClass_LineMeshRendering::LUA_detachfromentity },
    { "configure", &LuaClass_LineMeshRendering::LUA_configure },
    { "release", &LuaClass_LineMeshRendering::LUA_release },
    { "register_to_rendering", &LuaClass_LineMeshRendering::LUA_registertorendering },
    { "unregister_from_rendering", &LuaClass_LineMeshRendering::LUA_unregisterfromrendering },
    { "set_shaderrealvector", &LuaClass_LineMeshRendering::LUA_setshaderrealvector },
    { "set_passforrenderid", &LuaClass_LineMeshRendering::LUA_setPassForRenderId },
    { "add_vertex", &LuaClass_LineMeshRendering::LUA_AddVertex },
    { "add_line", &LuaClass_LineMeshRendering::LUA_AddLine },
	{ 0, 0 }
};


LuaClass_LineMeshRendering::LuaClass_LineMeshRendering(lua_State* p_L)
{
}

LuaClass_LineMeshRendering::~LuaClass_LineMeshRendering(void)
{
}

void LuaClass_LineMeshRendering::cleanup_resources(lua_State* p_L)
{
    if (NULL == m_entity)
    {
        LUA_ERROR("LineMeshRendering::cleanup_resources : no attached entity");
    }
    auto resources_aspect{ m_entity->GetAspect<ResourcesAspect>() };
    if (!resources_aspect)
    {
        LUA_ERROR("LineMeshRendering::cleanup_resources : attached entity has no resources aspect !");
    }

    if (m_entity_rendering_aspect)
    {
        for (auto it = m_renderingnodes.begin(); it != m_renderingnodes.end(); ++it)
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

            m_entity_rendering_aspect->RemoveComponent<PassSlot>(id);
        }

        m_renderingnodes.clear();

        m_linemeshe.ClearLines();
        m_linemeshe.ClearVertices();
    }
    else
    {
        LUA_ERROR("LineMeshRendering::cleanup_resources : no rendering aspect");
    }
}

int LuaClass_LineMeshRendering::LUA_attachtoentity(lua_State* p_L)
{
    const auto argc{ lua_gettop(p_L) };
    if (argc < 1)
    {
        LUA_ERROR("LineMeshRendering::attach_toentity : argument(s) missing");
    }

    auto lua_ent{ Luna<LuaClass_Entity>::check(p_L, 1) };

    DrawSpace::Core::Entity& entity{ lua_ent->GetEntity() };
    const auto rendering_aspect{ entity.GetAspect<RenderingAspect>() };

    if (NULL == rendering_aspect)
    {
        LUA_ERROR("LineMeshRendering::attach_toentity : entity has no rendering aspect!");
    }

    m_entity_rendering_aspect = rendering_aspect;
    m_entity = &entity;

    m_entity_rendering_aspect->AddImplementation(&m_meshe_render, NULL);


    return 0;
}

int LuaClass_LineMeshRendering::LUA_detachfromentity(lua_State* p_L)
{
    if (NULL == m_entity)
    {
        LUA_ERROR("LineMeshRendering::detach_fromentity : argument(s) missing");
    }

    LUA_TRY
    {
        m_entity_rendering_aspect->RemoveImplementation(&m_meshe_render);

    } LUA_CATCH;

    m_entity_rendering_aspect = nullptr;
    m_entity = nullptr;
    return 0;
}

int LuaClass_LineMeshRendering::LUA_configure(lua_State* p_L)
{
    if (nullptr == m_entity)
    {
        LUA_ERROR("LineMeshRendering::configure : no attached entity");
    }

    const auto resources_aspect{ m_entity->GetAspect<ResourcesAspect>() };
    if (!resources_aspect)
    {
        LUA_ERROR("LineMeshRendering::configure : attached entity has no resources aspect !");
    }

    const auto argc{ lua_gettop(p_L) };
    if (argc < 2)
    {
        LUA_ERROR("LineMeshRendering::configure : argument(s) missing");
    }

    const auto rcfg{ Luna<LuaClass_RenderConfig>::check(p_L, 1) };
    const auto meshe_name{ luaL_checkstring(p_L, 2) };

    if (m_entity_rendering_aspect)
    {
        LUA_TRY
        {
            const auto rcfg_data{ rcfg->GetData() };
            for (size_t i = 0; i < rcfg_data.render_contexts.size(); i++)
            {
                const auto render_context{ rcfg_data.render_contexts[i] };
                if (m_rcname_to_passes.end() != m_rcname_to_passes.find(render_context.rendercontextname))
                {
                    for (auto& pass_id : m_rcname_to_passes.at(render_context.rendercontextname))
                    {
                        m_entity_rendering_aspect->AddComponent<PassSlot>(pass_id, pass_id, PassSlot::PrimitiveType::LINE);
                        const auto rnode{ m_entity_rendering_aspect->GetComponent<PassSlot>(pass_id)->getPurpose().GetRenderingNode() };

                        m_renderingnodes[pass_id] = rnode;

                        //  on a besoin que d'un seul fx....
                        if (render_context.fxparams.size() < 1)
                        {
                            cleanup_resources(p_L);
                            LUA_ERROR("MesheRendering::configure : missing fx parameters description");
                        }

                        const auto fx_params{ render_context.fxparams[0] };

                        auto fx{ _DRAWSPACE_NEW_(Fx, Fx) };
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

                        ///////////////////////// le lines meshe

                        const auto meshe_res_id{ dsstring("meshe_") + pass_id };
                        
                        m_linemeshe.SetName(meshe_name);
                        rnode->SetLineMeshe(&m_linemeshe);

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
            }

        } LUA_CATCH;
    }
    else
    {
        LUA_ERROR("LineMeshRendering::configure : not attached to an entity");
    }

    return 0;
}

int LuaClass_LineMeshRendering::LUA_release(lua_State* p_L)
{
    cleanup_resources(p_L);
    return 0;
}


int LuaClass_LineMeshRendering::LUA_setshaderrealvector(lua_State* p_L)
{
    int argc = lua_gettop(p_L);
    if (argc < 6)
    {
        LUA_ERROR("LineMeshRendering::set_shaderrealvector : argument(s) missing");
    }

    const auto pass_id{ luaL_checkstring(p_L, 1) };
    const auto param_id{ luaL_checkstring(p_L, 2) };
    const auto valx{ luaL_checknumber(p_L, 3) };
    const auto valy{ luaL_checknumber(p_L, 4) };
    const auto valz{ luaL_checknumber(p_L, 5) };
    const auto valw{ luaL_checknumber(p_L, 6) };

    if (0 == m_renderingnodes.count(pass_id))
    {
        LUA_ERROR(dsstring("LineMeshRendering::set_shaderrealvector : unknown pass ") + pass_id);
    }
    else
    {
        LUA_TRY
        {
            m_renderingnodes.at(pass_id)->SetShaderRealVector(param_id, Vector(valx, valy, valz, valw));

        } LUA_CATCH;
    }
    return 0;
}

int LuaClass_LineMeshRendering::LUA_registertorendering(lua_State* p_L)
{
    const auto argc{ lua_gettop(p_L) };
    if (argc < 1)
    {
        LUA_ERROR("LineMeshRendering::register_to_rendering : argument(s) missing");
    }

    const auto lua_rg{ Luna<LuaClass_RenderPassNodeGraph>::check(p_L, 1) };

    m_meshe_render.RegisterToRendering(lua_rg->GetRenderGraph());
    return 0;
}

int LuaClass_LineMeshRendering::LUA_unregisterfromrendering(lua_State* p_L)
{
    const auto argc{ lua_gettop(p_L) };
    if (argc < 1)
    {
        LUA_ERROR("LineMeshRendering::unregister_from_rendering : argument(s) missing");
    }

    const auto lua_rg{ Luna<LuaClass_RenderPassNodeGraph>::check(p_L, 1) };

    m_meshe_render.UnregisterFromRendering(lua_rg->GetRenderGraph());
    return 0;
}


int LuaClass_LineMeshRendering::LUA_setPassForRenderId(lua_State* p_L)
{
    const auto argc{ lua_gettop(p_L) };
    if (argc < 2)
    {
        LUA_ERROR("LineMeshRendering::set_passforrenderid : argument(s) missing");
    }

    const auto rc_id{ luaL_checkstring(p_L, 1) };
    const auto pass_id{ luaL_checkstring(p_L, 2) };

    m_rcname_to_passes[rc_id].push_back(pass_id);

    return 0;
}

int LuaClass_LineMeshRendering::LUA_AddVertex(lua_State* p_L)
{
    const auto argc{ lua_gettop(p_L) };
    if (argc < 3)
    {
        LUA_ERROR("LineMeshRendering::add_vertex : argument(s) missing");
    }

    const auto valx{ luaL_checknumber(p_L, 1) };
    const auto valy{ luaL_checknumber(p_L, 2) };
    const auto valz{ luaL_checknumber(p_L, 3) };
    m_linemeshe.AddVertex({ valx, valy, valz });

    return 0;
}

int LuaClass_LineMeshRendering::LUA_AddLine(lua_State* p_L)
{
    const auto argc{ lua_gettop(p_L) };
    if (argc < 2)
    {
        LUA_ERROR("LineMeshRendering::add_line : argument(s) missing");
    }

    const auto id0{ luaL_checkint(p_L, 1) };
    const auto id1{ luaL_checkint(p_L, 2) };
    m_linemeshe.AddLine({ static_cast<unsigned int>(id0), static_cast<unsigned int>(id1) });

    return 0;
}

