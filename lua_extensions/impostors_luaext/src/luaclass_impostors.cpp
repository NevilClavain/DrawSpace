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
#include "luaclass_impostors.h"
#include "luaclass_entity.h"
#include "renderingaspect.h"
#include "impostorsrenderingaspectimpl.h"
#include "luaclass_renderpassnodegraph.h"
#include "luaclass_renderconfig.h"
#include "luaclass_impostorsdescriptionsarray.h"
#include "luaclass_matrix.h"

#include "renderingnode.h"
#include "resourcesaspect.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Utils;
using namespace DrawSpace::Aspect;

const char LuaClass_Impostors::className[] = "Impostors";
const Luna<LuaClass_Impostors>::RegType LuaClass_Impostors::methods[] =
{
	{ "attach_toentity", &LuaClass_Impostors::LUA_attachtoentity },
    { "detach_fromentity", &LuaClass_Impostors::LUA_detachfromentity },
    { "set_passforrenderid", &LuaClass_Impostors::LUA_setPassForRenderId },
    { "register_to_rendering", &LuaClass_Impostors::LUA_registertorendering },
    { "unregister_from_rendering", &LuaClass_Impostors::LUA_unregisterfromrendering },
    { "configure", &LuaClass_Impostors::LUA_configure },
    { "release", &LuaClass_Impostors::LUA_release },
    { "set_shaderreal", &LuaClass_Impostors::LUA_setshaderreal },
    { "set_shaderrealvector", &LuaClass_Impostors::LUA_setshaderrealvector },
    { "set_shaderrealinvector", &LuaClass_Impostors::LUA_setshaderrealinvector },
    { "set_shaderrealmatrix", &LuaClass_Impostors::LUA_setshaderrealmatrix },
    { "set_shaderbool", &LuaClass_Impostors::LUA_setshaderbool },
	{ 0, 0 }
};

LuaClass_Impostors::LuaClass_Impostors( lua_State* p_L )
{
}

LuaClass_Impostors::~LuaClass_Impostors( void )
{
}

int LuaClass_Impostors::LUA_attachtoentity(lua_State* p_L)
{
    int argc{ lua_gettop(p_L) };
    
    if (argc < 1)
    {
        LUA_ERROR("Impostors::attach_toentity : argument(s) missing");
    }
    
    LuaClass_Entity* lua_ent{ Luna<LuaClass_Entity>::check(p_L, 1) };
    
    DrawSpace::Core::Entity& entity{ lua_ent->GetEntity() };
    RenderingAspect* rendering_aspect{ entity.GetAspect<RenderingAspect>() };

    if (NULL == rendering_aspect)
    {
        LUA_ERROR("Impostors::attach_toentity : entity has no rendering aspect!");
    }
    
    m_entity_rendering_aspect = rendering_aspect;
    m_entity = &entity;
    
    m_impostors_render = _DRAWSPACE_NEW_(ImpostorsRenderingAspectImpl, ImpostorsRenderingAspectImpl);
    m_entity_rendering_aspect->AddImplementation(m_impostors_render, nullptr);
    
    return 0;
}

int LuaClass_Impostors::LUA_detachfromentity(lua_State* p_L)
{
    if (!m_entity)
    {
        LUA_ERROR("Impostors::detach_fromentity : argument(s) missing");
    }
    if (m_impostors_render)
    {
        _DRAWSPACE_DELETE_(m_impostors_render);
    }
    LUA_TRY
    {
        m_entity_rendering_aspect->RemoveImplementation(m_impostors_render);

    } LUA_CATCH;

    m_entity_rendering_aspect = nullptr;
    m_entity = nullptr;
    return 0;
}

int LuaClass_Impostors::LUA_setPassForRenderId(lua_State* p_L)
{
    int argc{ lua_gettop(p_L) };
    if (argc < 2)
    {
        LUA_ERROR("Impostors::set_passforrenderid : argument(s) missing");
    }

    dsstring rc_id{ luaL_checkstring(p_L, 1) };
    dsstring pass_id{ luaL_checkstring(p_L, 2) };

    m_rcname_to_passes[rc_id].push_back(pass_id);

    return 0;
}

int LuaClass_Impostors::LUA_registertorendering(lua_State* p_L)
{
    if (NULL == m_impostors_render)
    {
        LUA_ERROR("Impostors::register_to_rendering : no impostors rendering aspect impl created");
    }

    int argc{ lua_gettop(p_L) };
    if (argc < 1)
    {
        LUA_ERROR("Impostors::register_to_rendering : argument(s) missing");
    }

    LuaClass_RenderPassNodeGraph* lua_rg{ Luna<LuaClass_RenderPassNodeGraph>::check(p_L, 1) };

    m_impostors_render->RegisterToRendering(lua_rg->GetRenderGraph());
    return 0;
}

int LuaClass_Impostors::LUA_unregisterfromrendering(lua_State* p_L)
{
    if (NULL == m_impostors_render)
    {
        LUA_ERROR("Impostors::unregister_from_rendering : no impostors rendering aspect impl created");
    }

    int argc{ lua_gettop(p_L) };
    if (argc < 1)
    {
        LUA_ERROR("Impostors::unregister_from_rendering : argument(s) missing");
    }

    LuaClass_RenderPassNodeGraph* lua_rg{ Luna<LuaClass_RenderPassNodeGraph>::check(p_L, 1) };

    m_impostors_render->UnregisterFromRendering(lua_rg->GetRenderGraph());
    return 0;
}

int LuaClass_Impostors::LUA_configure(lua_State* p_L)
{
    if (NULL == m_entity)
    {
        LUA_ERROR("Impostors::configure : no attached entity");
    }
    ResourcesAspect* resources_aspect{ m_entity->GetAspect<ResourcesAspect>() };
    if (!resources_aspect)
    {
        LUA_ERROR("Impostors::configure : attached entity has no resources aspect !");
    }

    int argc{ lua_gettop(p_L) };
    if (argc < 2)
    {
        LUA_ERROR("Impostors::configure : argument(s) missing");
    }

    LuaClass_RenderConfig* rcfg{ Luna<LuaClass_RenderConfig>::check(p_L, 1) };
    LuaClass_ImpostorsDescriptionsArray* descriptions_arrays{ Luna<LuaClass_ImpostorsDescriptionsArray>::check(p_L, 2) };

    // recupere l'aspect rendu s'il existe pour cette entitee
    if (m_entity_rendering_aspect)
    {
        LUA_TRY
        {
            // retrieve descriptor arrays content
            auto descriptors_array{ descriptions_arrays->GetData() };
            int d_count{ 0 };

            for (auto& e : descriptors_array)
            {
                m_entity_rendering_aspect->AddComponent<ImpostorsRenderingAspectImpl::ImpostorDescriptor>(std::to_string(d_count++), e);
            }

            LuaClass_RenderConfig::Data rcfg_data{ rcfg->GetData() };
            for (size_t i = 0; i < rcfg_data.render_contexts.size(); i++)
            {
                LuaClass_RenderContext::Data render_context{ rcfg_data.render_contexts[i] };
                if (m_rcname_to_passes.end() != m_rcname_to_passes.find(render_context.rendercontextname))
                {
                    for (auto& pass_id : m_rcname_to_passes.at(render_context.rendercontextname))
                    {
                        m_entity_rendering_aspect->AddComponent<ImpostorsRenderingAspectImpl::PassSlot>(pass_id, pass_id);

                        RenderingNode* rnode{ m_entity_rendering_aspect->GetComponent<ImpostorsRenderingAspectImpl::PassSlot>(pass_id)->getPurpose().GetRenderingNode() };
                        m_renderingnodes[pass_id] = rnode;

                        if (render_context.fxparams.size() < 1)
                        {
                            cleanup_resources(p_L);
                            LUA_ERROR("Impostors::configure : missing fx parameters description");
                        }

                        LuaClass_FxParams::Data fx_params = render_context.fxparams[0];

                        DrawSpace::Core::Fx* fx = _DRAWSPACE_NEW_(Fx, Fx);
                        rnode->SetFx(fx);

                        ///////////////////////// les shaders
                        size_t nb_shaders = fx_params.shaders.size();
                        for (size_t j = 0; j < nb_shaders; j++)
                        {
                            std::pair<dsstring, bool> shader_infos = fx_params.shaders[j];

                            dsstring shader_path = shader_infos.first;
                            bool is_compiled = shader_infos.second;

                            Shader* shader = _DRAWSPACE_NEW_(Shader, Shader(shader_path, is_compiled));

                            dsstring res_id = dsstring("shader_") + std::to_string((int)shader);
                            resources_aspect->AddComponent<std::tuple<Shader*, bool, int>>(res_id, std::make_tuple(shader, false, j));
                            fx->AddShader(shader);
                        }

                        ///////////////////////// les rendestates

                        DrawSpace::Core::RenderStatesSet& rss = fx_params.rss;
                        fx->SetRenderStates(rss);

                        ///////////////////////// les textures

                        size_t nb_textures_set = render_context.textures_sets.size();
                        if (nb_textures_set > 0)
                        {
                            LuaClass_TexturesSet::Data textures = render_context.textures_sets[0];

                            for (int j = 0; j < DrawSpace::Core::RenderingNode::NbMaxTextures; j++)
                            {
                                dsstring texture_path = textures.textures[j];
                                if (texture_path != "")
                                {
                                    Texture* texture = _DRAWSPACE_NEW_(Texture, Texture(texture_path));

                                    dsstring res_id = dsstring("texture_") + std::to_string((int)texture);
                                    resources_aspect->AddComponent<std::tuple<Texture*, bool>>(res_id, std::make_tuple(texture, false));
                                    rnode->SetTexture(texture, j);
                                }
                            }
                        }

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
        LUA_ERROR("Impostors::configure : not attached to an entity");
    }
    return 0;
}

int LuaClass_Impostors::LUA_release(lua_State* p_L)
{
    cleanup_resources(p_L);
    return 0;
}

void LuaClass_Impostors::cleanup_resources(lua_State* p_L)
{
    if (NULL == m_entity)
    {
        LUA_ERROR("Impostors::cleanup_resources : no attached entity");
    }
    ResourcesAspect* resources_aspect = m_entity->GetAspect<ResourcesAspect>();
    if (!resources_aspect)
    {
        LUA_ERROR("Impostors::cleanup_resources : attached entity has no resources aspect !");
    }

    if (m_entity_rendering_aspect)
    {
        for (auto& e : m_renderingnodes)
        {
            e.second->CleanupShaderParams();
            dsstring id{ e.first };
            RenderingNode* rnode{ m_entity_rendering_aspect->GetComponent<ImpostorsRenderingAspectImpl::PassSlot>(id)->getPurpose().GetRenderingNode() };
            Fx* fx{ rnode->GetFx() };
            if (fx)
            {
                for (long i = 0; i < fx->GetShadersListSize(); i++)
                {
                    Shader* shader = fx->GetShader(i);
                    dsstring res_id = dsstring("shader_") + std::to_string((int)shader);
                    resources_aspect->RemoveComponent<std::tuple<Shader*, bool, int>>(res_id);
                    _DRAWSPACE_DELETE_(shader);
                }
                fx->ClearShaders();
            }

            _DRAWSPACE_DELETE_(fx);

            for (int i = 0; i < rnode->GetTextureListSize(); i++)
            {
                Texture* texture = rnode->GetTexture(i);
                if (texture)
                {
                    dsstring res_id = dsstring("texture_") + std::to_string((int)texture);
                    resources_aspect->RemoveComponent<std::tuple<Texture*, bool>>(res_id);

                    _DRAWSPACE_DELETE_(texture);
                    rnode->SetTexture(NULL, i);
                }
            }

            LUA_TRY
            {
                m_entity_rendering_aspect->RemoveComponent<ImpostorsRenderingAspectImpl::PassSlot>(id);

            } LUA_CATCH;

        }
        m_renderingnodes.clear();
        m_entity_rendering_aspect->RemoveAllComponentsOfType<ImpostorsRenderingAspectImpl::ImpostorDescriptor>();
    }
    else
    {
        LUA_ERROR("Impostors::cleanup_resources : no rendering aspect");
    }
}

int LuaClass_Impostors::LUA_setshaderreal(lua_State* p_L)
{
    int argc = lua_gettop(p_L);
    if (argc < 3)
    {
        LUA_ERROR("MesheRendering::set_shaderreal : argument(s) missing");
    }

    dsstring pass_id = luaL_checkstring(p_L, 1);
    dsstring param_id = luaL_checkstring(p_L, 2);
    dsreal val = luaL_checknumber(p_L, 3);

    if (0 == m_renderingnodes.count(pass_id))
    {
        LUA_ERROR("MesheRendering::set_shaderreal : unknown pass");
    }
    else
    {
        LUA_TRY
        {
            m_renderingnodes.at(pass_id)->SetShaderReal(param_id, val);

        } LUA_CATCH;
    }

    return 0;
}

int LuaClass_Impostors::LUA_setshaderrealvector(lua_State* p_L)
{
    int argc = lua_gettop(p_L);
    if (argc < 6)
    {
        LUA_ERROR("MesheRendering::set_shaderrealvector : argument(s) missing");
    }

    dsstring pass_id = luaL_checkstring(p_L, 1);
    dsstring param_id = luaL_checkstring(p_L, 2);
    dsreal valx = luaL_checknumber(p_L, 3);
    dsreal valy = luaL_checknumber(p_L, 4);
    dsreal valz = luaL_checknumber(p_L, 5);
    dsreal valw = luaL_checknumber(p_L, 6);

    if (0 == m_renderingnodes.count(pass_id))
    {
        LUA_ERROR(dsstring("MesheRendering::set_shaderrealvector : unknown pass ") + pass_id);
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

int LuaClass_Impostors::LUA_setshaderrealinvector(lua_State* p_L)
{
    int argc = lua_gettop(p_L);
    if (argc < 4)
    {
        LUA_ERROR("MesheRendering::set_shaderrealinvector : argument(s) missing");
    }

    dsstring pass_id = luaL_checkstring(p_L, 1);
    dsstring param_id = luaL_checkstring(p_L, 2);
    int param_index_in_vector = luaL_checkinteger(p_L, 3);
    dsreal val = luaL_checknumber(p_L, 4);

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

int LuaClass_Impostors::LUA_setshaderrealmatrix(lua_State* p_L)
{
    int argc = lua_gettop(p_L);
    if (argc < 3)
    {
        LUA_ERROR("MesheRendering::set_shaderrealmatrix : argument(s) missing");
    }

    dsstring pass_id = luaL_checkstring(p_L, 1);
    dsstring param_id = luaL_checkstring(p_L, 2);
    LuaClass_Matrix* lua_mat = Luna<LuaClass_Matrix>::check(p_L, 3);

    if (0 == m_renderingnodes.count(pass_id))
    {
        LUA_ERROR("MesheRendering::set_shaderrealmatrix : unknown pass");
    }
    else
    {
        LUA_TRY
        {
            m_renderingnodes.at(pass_id)->SetShaderRealMatrix(param_id, lua_mat->GetMatrix());

        } LUA_CATCH;
    }
    return 0;
}

int LuaClass_Impostors::LUA_setshaderbool(lua_State* p_L)
{
    int argc = lua_gettop(p_L);
    if (argc < 3)
    {
        LUA_ERROR("MesheRendering::set_shaderreal : argument(s) missing");
    }

    dsstring pass_id = luaL_checkstring(p_L, 1);
    dsstring param_id = luaL_checkstring(p_L, 2);
    bool val = luaL_checkint(p_L, 3);

    if (0 == m_renderingnodes.count(pass_id))
    {
        LUA_ERROR("MesheRendering::set_shaderreal : unknown pass");
    }
    else
    {
        LUA_TRY
        {
            m_renderingnodes.at(pass_id)->SetShaderBool(param_id, val);

        } LUA_CATCH;
    }
    return 0;
}
