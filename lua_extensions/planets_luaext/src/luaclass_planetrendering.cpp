/* -*-LIC_BEGIN-*- */
/*
*
* DrawSpace Rendering engine
* Emmanuel Chaumont Copyright (c) 2013-2022
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
#include "luaclass_planetrendering.h"
#include "luaclass_entity.h"
#include "luaclass_timemanagerref.h"
#include "planetsrenderingaspectimpl.h"
#include "planetsluaext.h"
#include "luaclass_renderlayer.h"
#include "luaclass_renderpassnodegraph.h"


using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Utils;
using namespace DrawSpace::Aspect;


const char LuaClass_PlanetRendering::className[] = "PlanetRendering";
const Luna<LuaClass_PlanetRendering>::RegType LuaClass_PlanetRendering::methods[] =
{
	{ "attach_toentity", &LuaClass_PlanetRendering::LUA_attachtoentity },
    { "detach_fromentity", &LuaClass_PlanetRendering::LUA_detachfromentity },
    { "set_passforrenderid", &LuaClass_PlanetRendering::LUA_setPassForRenderId },
    { "configure", &LuaClass_PlanetRendering::LUA_configure },
    { "release", &LuaClass_PlanetRendering::LUA_release },
    { "register_to_rendering", &LuaClass_PlanetRendering::LUA_registertorendering },
    { "unregister_from_rendering", &LuaClass_PlanetRendering::LUA_unregisterfromrendering },

	{ 0, 0 }
};

LuaClass_PlanetRendering::LuaClass_PlanetRendering( lua_State* p_L )
{
    int argc = lua_gettop(p_L);
    if (argc < 1)
    {
        LUA_ERROR("Rendering::Rendering : argument(s) missing");
    }
    LuaClass_TimeManagerRef* lua_tmref = Luna<LuaClass_TimeManagerRef>::check(p_L, 1);
    m_tm = lua_tmref->GetTimeManager();
}

LuaClass_PlanetRendering::~LuaClass_PlanetRendering( void )
{
}

int LuaClass_PlanetRendering::LUA_attachtoentity(lua_State* p_L)
{
    int argc{ lua_gettop(p_L) };
    if (argc < 1)
    {
        LUA_ERROR("PlanetRendering::attach_toentity : argument(s) missing");
    }

    LuaClass_Entity* lua_ent{ Luna<LuaClass_Entity>::check(p_L, 1) };
    DrawSpace::Core::Entity& entity{ lua_ent->GetEntity() };

    RenderingAspect* rendering_aspect{ entity.GetAspect<RenderingAspect>() };
    if (NULL == rendering_aspect)
    {
        LUA_ERROR("PlanetRendering::attach_toentity : entity has no rendering aspect!");
    }

    m_entity_rendering_aspect = rendering_aspect;
    m_entity = &entity;

    m_planet_render = _DRAWSPACE_NEW_(PlanetsRenderingAspectImpl, PlanetsRenderingAspectImpl);
    m_planet_render->SetHub(PlanetsLuaExtension::GetInstance()->GetHub());
    m_entity_rendering_aspect->AddImplementation(m_planet_render, m_tm);

    return 0;
}

int LuaClass_PlanetRendering::LUA_detachfromentity(lua_State* p_L)
{
    if (!m_entity)
    {
        LUA_ERROR("PlanetRendering::detach_fromentity : argument(s) missing");
    }
    if (m_planet_render)
    {
        _DRAWSPACE_DELETE_(m_planet_render);
    }
    LUA_TRY
    {
        m_entity_rendering_aspect->RemoveImplementation(m_planet_render);

    } LUA_CATCH;

    m_entity_rendering_aspect = nullptr;
    m_entity = nullptr;
    return 0;
}

int LuaClass_PlanetRendering::LUA_setPassForRenderId(lua_State* p_L)
{
    int argc{ lua_gettop(p_L) };
    if (argc < 2)
    {
        LUA_ERROR("PlanetRendering::set_passforrenderid : argument(s) missing");
    }

    dsstring rc_id{ luaL_checkstring(p_L, 1) };
    dsstring pass_id{ luaL_checkstring(p_L, 2) };
    m_rcname_to_passes[rc_id].push_back(pass_id);

    return 0;
}

int LuaClass_PlanetRendering::LUA_configure(lua_State* p_L)
{
    if (NULL == m_entity)
    {
        LUA_ERROR("PlanetRendering::configure : no attached entity");
    }
    ResourcesAspect* resources_aspect{ m_entity->GetAspect<ResourcesAspect>() };
    if (!resources_aspect)
    {
        LUA_ERROR("PlanetRendering::configure : attached entity has no resources aspect !");
    }

    int argc{ lua_gettop(p_L) };
    if (argc < 1)
    {
        LUA_ERROR("PlanetRendering::configure : argument(s) missing");
    }

    LuaClass_RenderLayer* lua_renderlayer{ Luna<LuaClass_RenderLayer>::check(p_L, 1) };

    if (m_entity_rendering_aspect)
    {
        LUA_TRY
        {

            std::vector<std::map<dsstring, std::vector<std::array<Texture*, RenderingNode::NbMaxTextures>>>>    layers_textures;
            std::vector<std::map<dsstring, Fx*>>                                                                layers_fx;
            std::vector<std::map<dsstring,std::vector<std::pair<dsstring, RenderingNode::ShadersParams>>>>      layers_shaders_params;
            std::vector<std::map<dsstring, int>>                                                                layers_ro;
            std::map<dsstring, int>                                                                             rcname_to_layer_index;

            int rcfg_list_size = lua_renderlayer->GetRenderConfigListSize();

            layers_ro.resize(rcfg_list_size);
            layers_shaders_params.resize(rcfg_list_size);
            layers_fx.resize(rcfg_list_size);
            layers_textures.resize(rcfg_list_size);

            for (int k = 0; k < rcfg_list_size; k++)
            {
                auto render_config{ lua_renderlayer->GetRenderConfig(k) };
                size_t rc_list_size{ render_config.second.render_contexts.size() };
                int cfg_index{ render_config.first };


                std::map<dsstring, std::vector<std::array<Texture*, RenderingNode::NbMaxTextures>>> config_textures;
                std::map<dsstring, Fx*> config_fxs;
                std::map<dsstring, int> config_ros;
                std::map<dsstring, std::vector<std::pair<dsstring, RenderingNode::ShadersParams>>> config_shadersparams;

                for (size_t i = 0; i < rc_list_size; i++)
                {
                    LuaClass_RenderContext::Data render_context{ render_config.second.render_contexts[i] };

                    rcname_to_layer_index[render_context.rendercontextname] = cfg_index;  // NB : 'layer' or 'config' -> the same thing

                    ///////////////// textures

                    int textures_set_size = render_context.textures_sets.size();
                    // les N jeux de 32 textures stages
                    std::vector<std::array<Texture*, RenderingNode::NbMaxTextures>> textures;
                    for (int texture_face_index = 0; texture_face_index < textures_set_size; texture_face_index++)
                    {
                        std::array<Texture*, RenderingNode::NbMaxTextures> textures_set = { NULL };

                        LuaClass_TexturesSet::Data txts_set = render_context.textures_sets[texture_face_index];

                        for (int texture_stage_index = 0; texture_stage_index < RenderingNode::NbMaxTextures; texture_stage_index++)
                        {
                            dsstring texture_name = txts_set.textures[texture_stage_index];
                            if (texture_name != "")
                            {
                                Texture* texture = _DRAWSPACE_NEW_(Texture, Texture(texture_name));
                                dsstring res_id = dsstring("texture_") + std::to_string((int)texture);
                                resources_aspect->AddComponent<std::tuple<Texture*, bool>>(res_id, std::make_tuple(texture, false));

                                textures_set[texture_stage_index] = texture;
                            }
                        }
                        textures.push_back(textures_set);
                    }
                    config_textures[render_context.rendercontextname] = textures;

                    ////////////////// fx pour chaque passes
                    if (render_context.fxparams.size() < 1)
                    {
                        cleanup_resources(p_L);
                        LUA_ERROR("Rendering::configure : missing fx parameters description");
                    }
                    LuaClass_FxParams::Data fx_params = render_context.fxparams[0];

                    Fx* fx = _DRAWSPACE_NEW_(Fx, Fx);

                    fx->SetRenderStates(fx_params.rss);

                    for (size_t j = 0; j < fx_params.shaders.size(); j++)
                    {
                        std::pair<dsstring, bool> shader_file_infos = fx_params.shaders[j];
                        Shader* shader = _DRAWSPACE_NEW_(Shader, Shader(shader_file_infos.first, shader_file_infos.second));
                        dsstring res_id = dsstring("shader_") + std::to_string((int)shader);
                        resources_aspect->AddComponent<std::tuple<Shader*, bool, int>>(res_id, std::make_tuple(shader, false, j));
                        fx->AddShader(shader);
                    }

                    config_fxs[render_context.rendercontextname] = fx;

                    //////////////// parametres de shaders

                    std::vector<std::pair<dsstring, RenderingNode::ShadersParams>> texturepass_shaders_params;
                    for (size_t j = 0; j < render_context.shaders_params.size(); j++)
                    {
                        LuaClass_RenderContext::NamedShaderParam param = render_context.shaders_params[j];
                        texturepass_shaders_params.push_back(param);
                    }

                    config_shadersparams[render_context.rendercontextname] = texturepass_shaders_params;

                    ///////////////// rendering order
                    config_ros[render_context.rendercontextname] = render_context.rendering_order;
                }

                layers_textures[cfg_index] = config_textures;
                layers_fx[cfg_index] = config_fxs;
                layers_ro[cfg_index] = config_ros;
                layers_shaders_params[cfg_index] = config_shadersparams;
            }

            m_entity_rendering_aspect->AddComponent<std::map<dsstring, int>>("rcname_to_layer_index", rcname_to_layer_index);
            m_entity_rendering_aspect->AddComponent<std::map<dsstring, std::vector<dsstring>>>("rcname_to_passes", m_rcname_to_passes);
            m_entity_rendering_aspect->AddComponent<std::vector<std::map<dsstring, std::vector<std::array<Texture*, RenderingNode::NbMaxTextures>>>>>("layers_textures", layers_textures);
            m_entity_rendering_aspect->AddComponent<std::vector<std::map<dsstring, Fx*>>>("layers_fx", layers_fx);
            m_entity_rendering_aspect->AddComponent<std::vector<std::map<dsstring, std::vector<std::pair<dsstring, RenderingNode::ShadersParams>>>>>("layers_shaders_params", layers_shaders_params);
            m_entity_rendering_aspect->AddComponent<std::vector<std::map<dsstring, int>>>("layers_ro", layers_ro);

        } LUA_CATCH;
    }
    else
    {
        LUA_ERROR("PlanetRendering::configure : not attached to an entity");
    }

    return 0;
}

int LuaClass_PlanetRendering::LUA_release(lua_State* p_L)
{
    return 0;
}

int LuaClass_PlanetRendering::LUA_registertorendering(lua_State* p_L)
{
    if (NULL == m_planet_render)
    {
        LUA_ERROR("PlanetRendering::register_to_rendering : no impostors rendering aspect impl created");
    }

    int argc{ lua_gettop(p_L) };
    if (argc < 1)
    {
        LUA_ERROR("PlanetRendering::register_to_rendering : argument(s) missing");
    }

    LuaClass_RenderPassNodeGraph* lua_rg{ Luna<LuaClass_RenderPassNodeGraph>::check(p_L, 1) };
    m_planet_render->RegisterToRendering(lua_rg->GetRenderGraph());

    return 0;
}

int LuaClass_PlanetRendering::LUA_unregisterfromrendering(lua_State* p_L)
{
    return 0;
}


void LuaClass_PlanetRendering::cleanup_resources(lua_State* p_L)
{

}

DrawSpace::Aspect::RenderingAspect* LuaClass_PlanetRendering::GetRenderingAspect(void) const
{
    return m_entity_rendering_aspect;
}