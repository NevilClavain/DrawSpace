/* -*-LIC_BEGIN-*- */
/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2021                     
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

#include "renderingnode.h"
#include "resourcesaspect.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
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
            //m_entity_rendering_aspect->AddComponent<ImpostorsRenderingAspectImpl::ImpostorDescriptor>("0", id);

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
        // TODO

    }
    else
    {
        LUA_ERROR("Impostors::cleanup_resources : no rendering aspect");
    }
}
