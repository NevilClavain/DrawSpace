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
#include "luaclass_planetrendering.h"
#include "luaclass_entity.h"
#include "luaclass_timemanagerref.h"
#include "planetsrenderingaspectimpl.h"
#include "planetsluaext.h"
#include "luaclass_renderlayer.h"
#include "luaclass_renderpassnodegraph.h"
#include "planetscentraladmin.h"


using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Utils;
using namespace DrawSpace::Aspect;


const char LuaClass_PlanetRendering::className[] = "PlanetRendering";
const Luna<LuaClass_PlanetRendering>::RegType LuaClass_PlanetRendering::methods[] =
{
	{ "attach_toentity", &LuaClass_PlanetRendering::LUA_attachtoentity },
    { "detach_fromentity", &LuaClass_PlanetRendering::LUA_detachfromentity },
    { "set_passforplanetlayerrenderid", &LuaClass_PlanetRendering::LUA_setPassForPlanetLayerRenderId },
    { "set_passforfoliagelayerrenderid", &LuaClass_PlanetRendering::LUA_setPassForFoliageLayerRenderId },
    { "configure", &LuaClass_PlanetRendering::LUA_configure },
    { "release", &LuaClass_PlanetRendering::LUA_release },
    { "register_to_rendering", &LuaClass_PlanetRendering::LUA_registertorendering },
    { "unregister_from_rendering", &LuaClass_PlanetRendering::LUA_unregisterfromrendering },
    { "declare_foliageparameters", &LuaClass_PlanetRendering::LUA_declarefoliageparams },

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

    PlanetsCentralAdmin::GetInstance()->Register(m_planet_render, PlanetsLuaExtension::GetInstance()->GetHub());

    return 0;
}

int LuaClass_PlanetRendering::LUA_detachfromentity(lua_State* p_L)
{
    if (!m_entity)
    {
        LUA_ERROR("PlanetRendering::detach_fromentity : argument(s) missing");
    }

    LUA_TRY
    {
        m_entity_rendering_aspect->RemoveImplementation(m_planet_render);

    } LUA_CATCH;

    if (m_planet_render)
    {
        PlanetsCentralAdmin::GetInstance()->Unregister(m_planet_render);
        _DRAWSPACE_DELETE_(m_planet_render);
    }

    m_entity_rendering_aspect = nullptr;
    m_entity = nullptr;
    return 0;
}

int LuaClass_PlanetRendering::LUA_setPassForPlanetLayerRenderId(lua_State* p_L)
{
    int argc{ lua_gettop(p_L) };
    if (argc < 2)
    {
        LUA_ERROR("PlanetRendering::set_passforplanetlayerrenderid : argument(s) missing");
    }

    const auto rc_id{ luaL_checkstring(p_L, 1) };
    const auto pass_id{ luaL_checkstring(p_L, 2) };

    m_planetlayers_rcname_to_passes[rc_id].push_back(pass_id);

    return 0;
}

int LuaClass_PlanetRendering::LUA_setPassForFoliageLayerRenderId(lua_State* p_L)
{
    int argc{ lua_gettop(p_L) };
    if (argc < 2)
    {
        LUA_ERROR("PlanetRendering::set_passforfoliagelayerrenderid : argument(s) missing");
    }

    const auto rc_id{ luaL_checkstring(p_L, 1) };
    const auto pass_id{ luaL_checkstring(p_L, 2) };

    m_foliagelayers_rcname_to_passes[rc_id].push_back(pass_id);

    return 0;
}

void LuaClass_PlanetRendering::configure_from_renderlayer(lua_State* p_L, LuaClass_RenderLayer* p_lua_renderlayer, DrawSpace::Aspect::ResourcesAspect* p_resources_aspect, const dsstring& p_comp_prefix )
{
    std::vector<std::map<dsstring, std::vector<std::array<Texture*, RenderingNode::NbMaxTextures>>>>    layers_textures;
    std::vector<std::map<dsstring, Fx*>>                                                                layers_fx;
    std::vector<std::map<dsstring, std::vector<std::pair<dsstring, RenderingNode::ShadersParams>>>>     layers_shaders_params;
    std::vector<std::map<dsstring, int>>                                                                layers_ro;
    std::map<dsstring, int>                                                                             rcname_to_layer_index;

    const auto rcfg_list_size{ p_lua_renderlayer->GetRenderConfigListSize() };

    layers_ro.resize(rcfg_list_size);
    layers_shaders_params.resize(rcfg_list_size);
    layers_fx.resize(rcfg_list_size);
    layers_textures.resize(rcfg_list_size);

    for (int k = 0; k < rcfg_list_size; k++)
    {
        const auto render_config{ p_lua_renderlayer->GetRenderConfig(k) };
        const auto rc_list_size{ render_config.second.render_contexts.size() };
        const auto cfg_index{ render_config.first };

        std::map<dsstring, std::vector<std::array<Texture*, RenderingNode::NbMaxTextures>>> config_textures;
        std::map<dsstring, Fx*> config_fxs;
        std::map<dsstring, int> config_ros;
        std::map<dsstring, std::vector<std::pair<dsstring, RenderingNode::ShadersParams>>> config_shadersparams;

        for (size_t i = 0; i < rc_list_size; i++)
        {
            const LuaClass_RenderContext::Data render_context{ render_config.second.render_contexts[i] };

            rcname_to_layer_index[render_context.rendercontextname] = cfg_index;  // NB : 'layer' or 'config' -> the same thing

            ///////////////// textures

            const auto textures_set_size = render_context.textures_sets.size();
            // les N jeux de 32 textures stages
            std::vector<std::array<Texture*, RenderingNode::NbMaxTextures>> textures;
            for (int texture_face_index = 0; texture_face_index < textures_set_size; texture_face_index++)
            {
                std::array<Texture*, RenderingNode::NbMaxTextures> textures_set = { nullptr };

                const auto txts_set{ render_context.textures_sets[texture_face_index] };

                for (int texture_stage_index = 0; texture_stage_index < RenderingNode::NbMaxTextures; texture_stage_index++)
                {
                    const auto texture_name{ txts_set.textures[texture_stage_index] };
                    if (texture_name != "")
                    {
                        auto texture{ _DRAWSPACE_NEW_(Texture, Texture(texture_name)) };
                        const auto res_id{ dsstring("texture_") + std::to_string((int)texture) };
                        p_resources_aspect->AddComponent<std::tuple<Texture*, bool>>(res_id, std::make_tuple(texture, false));

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

            const LuaClass_FxParams::Data fx_params{ render_context.fxparams[0] };
            const auto fx{ _DRAWSPACE_NEW_(Fx, Fx) };

            fx->SetRenderStates(fx_params.rss);

            for (size_t j = 0; j < fx_params.shaders.size(); j++)
            {
                const auto shader_file_infos{ fx_params.shaders[j] };
                const auto shader{ _DRAWSPACE_NEW_(Shader, Shader(shader_file_infos.first, shader_file_infos.second)) };
                const auto res_id{ dsstring("shader_") + std::to_string((int)shader) };
                p_resources_aspect->AddComponent<std::tuple<Shader*, bool, int>>(res_id, std::make_tuple(shader, false, j));
                fx->AddShader(shader);
            }

            config_fxs[render_context.rendercontextname] = fx;

            //////////////// parametres de shaders

            std::vector<std::pair<dsstring, RenderingNode::ShadersParams>> texturepass_shaders_params;
            for (size_t j = 0; j < render_context.shaders_params.size(); j++)
            {
                const auto param{ render_context.shaders_params[j] };
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

    m_entity_rendering_aspect->AddComponent<std::map<dsstring, int>>(p_comp_prefix + "_rcname_to_layer_index", rcname_to_layer_index);
    m_entity_rendering_aspect->AddComponent<std::vector<std::map<dsstring, std::vector<std::array<Texture*, RenderingNode::NbMaxTextures>>>>>(p_comp_prefix + "_textures", layers_textures);
    m_entity_rendering_aspect->AddComponent<std::vector<std::map<dsstring, Fx*>>>(p_comp_prefix + "_fx", layers_fx);
    m_entity_rendering_aspect->AddComponent<std::vector<std::map<dsstring, std::vector<std::pair<dsstring, RenderingNode::ShadersParams>>>>>(p_comp_prefix + "_shaders_params", layers_shaders_params);
    m_entity_rendering_aspect->AddComponent<std::vector<std::map<dsstring, int>>>(p_comp_prefix + "_ro", layers_ro);
}

int LuaClass_PlanetRendering::LUA_configure(lua_State* p_L)
{
    if (nullptr == m_entity)
    {
        LUA_ERROR("PlanetRendering::configure : no attached entity");
    }
    const auto resources_aspect{ m_entity->GetAspect<ResourcesAspect>() };
    if (!resources_aspect)
    {
        LUA_ERROR("PlanetRendering::configure : attached entity has no resources aspect !");
    }

    const auto argc{ lua_gettop(p_L) };
    if (argc < 2)
    {
        LUA_ERROR("PlanetRendering::configure : argument(s) missing");
    }

    const auto lua_planetrenderlayer{ Luna<LuaClass_RenderLayer>::check(p_L, 1) };
    const auto lua_foliagerenderlayer{ Luna<LuaClass_RenderLayer>::check(p_L, 2) };

    if (m_entity_rendering_aspect)
    {
        LUA_TRY
        {
            configure_from_renderlayer(p_L, lua_planetrenderlayer, resources_aspect, "planetlayers");
            m_entity_rendering_aspect->AddComponent<std::map<dsstring, std::vector<dsstring>>>("planetlayers_rcname_to_passes", m_planetlayers_rcname_to_passes);

            configure_from_renderlayer(p_L, lua_foliagerenderlayer, resources_aspect, "foliagelayers");
            m_entity_rendering_aspect->AddComponent<std::map<dsstring, std::vector<dsstring>>>("foliagelayers_rcname_to_passes", m_foliagelayers_rcname_to_passes);

            m_entity_rendering_aspect->AddComponent<std::map<size_t, DrawSpace::Core::Meshe*>>("foliages_meshes", m_foliages_meshes);

            m_entity_rendering_aspect->AddComponent<std::map<size_t, bool>>("foliages_global_lits", m_foliages_global_lits);
            m_entity_rendering_aspect->AddComponent<std::map<size_t, bool>>("foliages_detailed_lits", m_foliages_detailed_lits);
            m_entity_rendering_aspect->AddComponent<std::map<size_t, int>>("foliages_local_seeds", m_foliages_local_seeds);
            

            // declare foliage meshes to resources manager
            for (auto e : m_foliages_meshes_paths) {

                const auto meshe_path{ e.second };

                const auto meshe_res_id{ std::to_string(e.first) + dsstring("_") + e.second };

                resources_aspect->AddComponent<std::tuple<Meshe*, dsstring, dsstring, bool>>(meshe_res_id,
                    std::make_tuple(m_foliages_meshes.at(e.first), meshe_path, m_foliages_meshes_ids.at(e.first), false));
            }

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
    cleanup_resources(p_L);
    return 0;
}

int LuaClass_PlanetRendering::LUA_declarefoliageparams(lua_State* p_L)
{
    const auto argc{ lua_gettop(p_L) };
    if (argc < 16)
    {
        LUA_ERROR("PlanetRendering::declare_foliageparams : argument(s) missing");
    }
    const auto meshe_key{ luaL_checkinteger(p_L, 1) };
    const auto meshe_path{ luaL_checkstring(p_L, 2) };
    const auto meshe_id{ luaL_checkstring(p_L, 3) };

    const auto global_lit{ (bool)luaL_checkinteger(p_L, 4) };
    const auto detailed_lit{ (bool)luaL_checkinteger(p_L, 5) };

    const auto local_seed{ luaL_checkinteger(p_L, 6) };

    const auto temperature_range_min{ luaL_checknumber( p_L, 7 ) };
    const auto temperature_range_max{ luaL_checknumber( p_L, 8 ) };

    const auto humidity_range_min{ luaL_checknumber( p_L, 9 ) };
    const auto humidity_range_max{ luaL_checknumber( p_L, 10 ) };

    const auto nb_poles_min{ luaL_checkinteger(p_L, 11) };
    const auto nb_poles_max{ luaL_checkinteger(p_L, 12) };

    const auto pole_ray_min{ luaL_checkinteger(p_L, 13) };
    const auto pole_ray_max{ luaL_checkinteger(p_L, 14) };

    const auto nbpoints_per_pole_min{ luaL_checkinteger(p_L, 15) };
    const auto nbpoints_per_pole_max{ luaL_checkinteger(p_L, 16) };
    // to be continued...

    
    m_foliages_meshes_paths[meshe_key] = meshe_path;
    m_foliages_meshes_ids[meshe_key] = meshe_id;
    m_foliages_global_lits[meshe_key] = global_lit;
    m_foliages_detailed_lits[meshe_key] = detailed_lit;
    m_foliages_local_seeds[meshe_key] = local_seed;


    m_foliages_meshes[meshe_key] = _DRAWSPACE_NEW_(Meshe, Meshe);
    m_foliages_meshes.at(meshe_key)->SetPath(meshe_path);

    return 0;
}

int LuaClass_PlanetRendering::LUA_registertorendering(lua_State* p_L)
{
    if (NULL == m_planet_render)
    {
        LUA_ERROR("PlanetRendering::register_to_rendering : no impostors rendering aspect impl created");
    }

    const auto argc{ lua_gettop(p_L) };
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
    if (NULL == m_planet_render)
    {
        LUA_ERROR("PlanetRendering::unregister_from_rendering : no rendering aspect impl created");
    }

    const auto argc{ lua_gettop(p_L) };
    if (argc < 1)
    {
        LUA_ERROR("PlanetRendering::unregister_from_rendering : argument(s) missing");
    }

    LuaClass_RenderPassNodeGraph* lua_rg{ Luna<LuaClass_RenderPassNodeGraph>::check(p_L, 1) };

    m_planet_render->UnregisterFromRendering(lua_rg->GetRenderGraph());

    return 0;
}


void LuaClass_PlanetRendering::cleanup_resources(lua_State* p_L)
{
    if (NULL == m_entity)
    {
        LUA_ERROR("PlanetRendering::cleanup_resources : no attached entity");
    }
    ResourcesAspect* resources_aspect{ m_entity->GetAspect<ResourcesAspect>() };
    if (!resources_aspect)
    {
        LUA_ERROR("PlanetRendering::cleanup_resources : attached entity has no resources aspect !");
    }

    if (m_entity_rendering_aspect)
    {

        /////////////////// meshes

        for(auto e : m_foliages_meshes)
        {
            _DRAWSPACE_DELETE_(e.second);
        }
        
        m_foliages_meshes_ids.clear();
        m_foliages_meshes.clear();
        m_foliages_global_lits.clear();
        m_foliages_detailed_lits.clear();
        m_foliages_local_seeds.clear();


        /////////////////// textures
       
        const auto layers_planet_textures_comp{ m_entity_rendering_aspect->GetComponent<std::vector<std::map<dsstring, std::vector<std::array<Texture*, RenderingNode::NbMaxTextures>>>>>("planetlayers_textures") };
        if (layers_planet_textures_comp)
        {
            const auto layers_textures{ layers_planet_textures_comp->getPurpose() };
            for (auto& e1 : layers_textures)
            {
                for (auto& e2 : e1)
                {
                    for (auto& e3 : e2.second)
                    {
                        const auto texture_set{ e3 };

                        for (size_t texture_stage_index = 0; texture_stage_index < texture_set.size(); texture_stage_index++)
                        {
                            auto texture{ texture_set[texture_stage_index] };
                            if (texture)
                            {
                                dsstring id;
                                dsstring res_id = dsstring("texture_") + std::to_string((int)texture);
                                resources_aspect->RemoveComponent<std::tuple<Texture*, bool>>(res_id);

                                _DRAWSPACE_DELETE_(texture);
                            }
                        }
                    }
                }
            }

            m_entity_rendering_aspect->RemoveComponent<std::vector<std::map<dsstring, std::vector<std::array<Texture*, RenderingNode::NbMaxTextures>>>>>("planetlayers_textures");
        }


        const auto layers_foliage_textures_comp{ m_entity_rendering_aspect->GetComponent<std::vector<std::map<dsstring, std::vector<std::array<Texture*, RenderingNode::NbMaxTextures>>>>>("foliagelayers_textures") };
        if (layers_foliage_textures_comp)
        {
            const auto layers_textures{ layers_foliage_textures_comp->getPurpose() };
            for (auto& e1 : layers_textures)
            {
                for (auto& e2 : e1)
                {
                    for (auto& e3 : e2.second)
                    {
                        const auto texture_set{ e3 };

                        for (size_t texture_stage_index = 0; texture_stage_index < texture_set.size(); texture_stage_index++)
                        {
                            auto texture{ texture_set[texture_stage_index] };
                            if (texture)
                            {
                                dsstring id;
                                dsstring res_id = dsstring("texture_") + std::to_string((int)texture);
                                resources_aspect->RemoveComponent<std::tuple<Texture*, bool>>(res_id);

                                _DRAWSPACE_DELETE_(texture);
                            }
                        }
                    }
                }
            }

            m_entity_rendering_aspect->RemoveComponent<std::vector<std::map<dsstring, std::vector<std::array<Texture*, RenderingNode::NbMaxTextures>>>>>("foliagelayers_textures");
        }


        /////////////////// fx

        const auto planet_layers_fx_comp{ m_entity_rendering_aspect->GetComponent<std::vector<std::map<dsstring, Fx*>>>("planetlayers_fx") };
        if (planet_layers_fx_comp)
        {
            std::vector<std::map<dsstring, Fx*>> layers_fx = planet_layers_fx_comp->getPurpose();
            for (auto& e1 : layers_fx)
            {
                for (auto& e2 : e1)
                {
                    auto fx{ e2.second };
                    for (int j = 0; j < fx->GetShadersListSize(); j++)
                    {
                        auto shader{ fx->GetShader(j) };

                        dsstring id;
                        dsstring res_id = dsstring("shader_") + std::to_string((int)shader);
                        resources_aspect->RemoveComponent<std::tuple<Shader*, bool, int>>(res_id);

                        _DRAWSPACE_DELETE_(shader);
                    }
                    _DRAWSPACE_DELETE_(fx);
                }
            }
            m_entity_rendering_aspect->RemoveComponent<std::vector<std::map<dsstring, Fx*>>>("planetlayers_fx");
        }

        const auto foliage_layers_fx_comp{ m_entity_rendering_aspect->GetComponent<std::vector<std::map<dsstring, Fx*>>>("foliagelayers_fx") };
        if (foliage_layers_fx_comp)
        {
            std::vector<std::map<dsstring, Fx*>> layers_fx = foliage_layers_fx_comp->getPurpose();
            for (auto& e1 : layers_fx)
            {
                for (auto& e2 : e1)
                {
                    auto fx{ e2.second };
                    for (int j = 0; j < fx->GetShadersListSize(); j++)
                    {
                        auto shader{ fx->GetShader(j) };

                        dsstring id;
                        dsstring res_id = dsstring("shader_") + std::to_string((int)shader);
                        resources_aspect->RemoveComponent<std::tuple<Shader*, bool, int>>(res_id);

                        _DRAWSPACE_DELETE_(shader);
                    }
                    _DRAWSPACE_DELETE_(fx);
                }
            }
            m_entity_rendering_aspect->RemoveComponent<std::vector<std::map<dsstring, Fx*>>>("foliagelayers_fx");
        }

        //////////////// args shaders

        if (m_entity_rendering_aspect->GetComponent<std::vector<std::map<dsstring, std::vector<std::pair<dsstring, RenderingNode::ShadersParams>>>>>("planetlayers_shaders_params"))
        {
            m_entity_rendering_aspect->RemoveComponent<std::vector<std::map<dsstring, std::vector<std::pair<dsstring, RenderingNode::ShadersParams>>>>>("planetlayers_shaders_params");
        }

        if (m_entity_rendering_aspect->GetComponent<std::vector<std::map<dsstring, std::vector<std::pair<dsstring, RenderingNode::ShadersParams>>>>>("foliagelayers_shaders_params"))
        {
            m_entity_rendering_aspect->RemoveComponent<std::vector<std::map<dsstring, std::vector<std::pair<dsstring, RenderingNode::ShadersParams>>>>>("foliagelayers_shaders_params");
        }

        //////////////// rendering orders

        if (m_entity_rendering_aspect->GetComponent<std::vector<std::map<dsstring, int>>>("planetlayers_ro"))
        {
            m_entity_rendering_aspect->RemoveComponent<std::vector<std::map<dsstring, int>>>("planetlayers_ro");
        }

        if (m_entity_rendering_aspect->GetComponent<std::vector<std::map<dsstring, int>>>("foliagelayers_ro"))
        {
            m_entity_rendering_aspect->RemoveComponent<std::vector<std::map<dsstring, int>>>("foliagelayers_ro");
        }


        //////////////// rcname_to_passes

        if (m_entity_rendering_aspect->GetComponent<std::map<dsstring, std::vector<dsstring>>>("planetlayers_rcname_to_passes"))
        {
            m_entity_rendering_aspect->RemoveComponent<std::map<dsstring, std::vector<dsstring>>>("planetlayers_rcname_to_passes");
        }

        if (m_entity_rendering_aspect->GetComponent<std::map<dsstring, std::vector<dsstring>>>("foliagelayers_rcname_to_passes"))
        {
            m_entity_rendering_aspect->RemoveComponent<std::map<dsstring, std::vector<dsstring>>>("foliagelayers_rcname_to_passes");
        }


        //////////////// rcname_to_layer_index

        if (m_entity_rendering_aspect->GetComponent<std::map<dsstring, int>>("planetlayers_rcname_to_layer_index"))
        {
            m_entity_rendering_aspect->RemoveComponent<std::map<dsstring, int>>("planetlayers_rcname_to_layer_index");
        }

        if (m_entity_rendering_aspect->GetComponent<std::map<dsstring, int>>("foliagelayers_rcname_to_layer_index"))
        {
            m_entity_rendering_aspect->RemoveComponent<std::map<dsstring, int>>("foliagelayers_rcname_to_layer_index");
        }

        if (m_entity_rendering_aspect->GetComponent<std::map<size_t, DrawSpace::Core::Meshe*>>("foliages_meshes"))
        {
            m_entity_rendering_aspect->RemoveComponent<std::map<size_t, DrawSpace::Core::Meshe*>>("foliages_meshes");
        }

        for (auto& e : m_foliages_meshes_paths)
        {
            const auto meshe_path{ e.second };
            const auto meshe_res_id{ std::to_string(e.first) + dsstring("_") + e.second };

            resources_aspect->RemoveComponent<std::tuple<Meshe*, dsstring, dsstring, bool>>(meshe_res_id);
        }
        m_foliages_meshes_paths.clear();

    }
}

DrawSpace::Aspect::RenderingAspect* LuaClass_PlanetRendering::GetRenderingAspect(void) const
{
    return m_entity_rendering_aspect;
}
