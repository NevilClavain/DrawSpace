
planetmod = {}
planetmod.view = {}

planetmod.module = Module("planetsmod", "planets")
planetmod.module:load()
g:print(planetmod.module:get_descr().. ' loaded')

-- stockage des instances modeles : paire {entity, renderer, specific_config}
planetmod.models = {}


planetmod.setup_specific_config=function(config_description, planet_specific_configuration)

	if config_description['resources_path'] ~= nil then
		planet_specific_configuration:set_resourcespath(config_description['resources_path'])
	end

	if config_description['planet_ray'] ~= nil then
		planet_specific_configuration:set_planetray(config_description['planet_ray'])
	end

    if config_description['atmo_thickness'] ~= nil then
		planet_specific_configuration:set_atmothickness(config_description['atmo_thickness'])
	end


	if config_description['plains_amplitude'] ~= nil and config_description['mountains_amplitude'] ~= nil then
		planet_specific_configuration:set_amplitudes(config_description['plains_amplitude'], config_description['mountains_amplitude'])
	end

	if config_description['vertical_offset'] ~= nil and config_description['mountains_offset'] ~= nil then
		planet_specific_configuration:set_offsets(config_description['vertical_offset'], config_description['mountains_offset'])
	end

	if config_description['plains_seed1'] ~= nil and 
	   config_description['plains_seed2'] ~= nil and 
	   config_description['mix_seed1'] ~= nil and 
	   config_description['mix_seed2'] ~= nil then

	   planet_specific_configuration:set_seeds(config_description['plains_seed1'],
												config_description['plains_seed2'],
												config_description['mix_seed1'],
												config_description['mix_seed2'])

	end

	if config_description['terrainbump_factor'] ~= nil then
		planet_specific_configuration:set_terrainbumpfactor(config_description['terrainbump_factor'])
	end

	if config_description['splat_transition_up_relative_alt'] ~= nil and 
	   config_description['splat_transition_down_relative_alt'] ~= nil and
	   config_description['splat_texture_resol'] ~= nil then

	   planet_specific_configuration:set_splattingparams(config_description['splat_transition_up_relative_alt'],
												config_description['splat_transition_down_relative_alt'],
												config_description['splat_texture_resol'])

	end

	if config_description['zbuffer_activation_relative_alt'] ~= nil then
		planet_specific_configuration:set_zbufferactivationrelalt(config_description['zbuffer_activation_relative_alt'])
	end


	if config_description['atmo_kr'] ~= nil and 
	   config_description['fog_alt_limit'] ~= nil and
	   config_description['fog_density'] ~= nil then

	   planet_specific_configuration:set_fogandatmoparams(config_description['atmo_kr'],
												config_description['fog_alt_limit'],
												config_description['fog_density'])

	end

	if config_description['beach_limit'] ~= nil then
		planet_specific_configuration:set_beachlimit(config_description['beach_limit'])
	end

	if config_description['landplace_patch'] ~= nil then
		planet_specific_configuration:enable_landplacepatch(config_description['landplace_patch'])
	end

	if config_description['enable_atmosphere'] ~= nil then
		planet_specific_configuration:enable_atmosphere(config_description['enable_atmosphere'])
	end

	if config_description['climate_vshader'] ~= nil and 
	   config_description['climate_pshader'] ~= nil then
		planet_specific_configuration:set_climateshaders(config_description['climate_vshader'], config_description['climate_pshader'])
	end

	if config_description['climate_vshader_compiled'] ~= nil and 
	   config_description['climate_pshader_compiled'] ~= nil then
		planet_specific_configuration:set_climateshaderscompiled(config_description['climate_vshader_compiled'], config_description['climate_pshader_compiled'])
	end
end

planetmod.layers =
{ 
	surface_layer = 
	{
		surface_rendering =	
		{
			fx =
			{
				shaders = 
				{
					{ path='planet_surface_vs.hlsl',mode=SHADER_NOT_COMPILED },
					{ path='planet_surface_ps.hlsl',mode=SHADER_NOT_COMPILED }
				},
				rs_in = 
				{
					{ ope=RENDERSTATE_OPE_ENABLEZBUFFER, value="false" },
					{ ope=RENDERSTATE_OPE_SETTEXTUREFILTERTYPE, value="linear" },
					{ ope=RENDERSTATE_OPE_SETVERTEXTEXTUREFILTERTYPE, value="linear" }
												
				},
				rs_out =
				{
					{ ope=RENDERSTATE_OPE_ENABLEZBUFFER, value="false"},
					{ ope=RENDERSTATE_OPE_SETTEXTUREFILTERTYPE, value="none" },
					{ ope=RENDERSTATE_OPE_SETVERTEXTEXTUREFILTERTYPE, value="none" }						
				}		
			},
			textures =
			{
				[1] = 
				{
					{ path='earth_th_pixels_16.jpg', stage=0 },
					{ path='earth_th_splatting_16.jpg', stage=1 },
				}
			},
			vertex_textures =
			{
			},
			shaders_params = 
			{
			},
			rendering_order = 5000
		},

		surface_wireframe_rendering =	
		{
			fx =
			{
				shaders = 
				{
					{ path='planet_surface_vs.hlsl',mode=SHADER_NOT_COMPILED },
					{ path='planet_surface_ps.hlsl',mode=SHADER_NOT_COMPILED }
				},
				rs_in = 
				{
					{ ope=RENDERSTATE_OPE_ENABLEZBUFFER, value="false" },						
					{ ope=RENDERSTATE_OPE_SETFILLMODE, value="line" }
						
				},
				rs_out =
				{
					{ ope=RENDERSTATE_OPE_ENABLEZBUFFER, value="false"},						
					{ ope=RENDERSTATE_OPE_SETFILLMODE, value="solid" }
				}		
			},
			textures =
			{
				[1] = 
				{
					{ path='earth_th_pixels_16.jpg', stage=0 },
					{ path='earth_th_splatting_16.jpg', stage=1 },
				}
			},
			vertex_textures =
			{
			},
			shaders_params = 
			{
			},
			rendering_order = 5000
		}
	},
	atmosphere_layer =
	{
		atmo_rendering =	
		{
			fx =
			{
				shaders = 
				{
					{ path='planet_atmosphere_vs.hlsl',mode=SHADER_NOT_COMPILED },
					{ path='planet_atmosphere_ps.hlsl',mode=SHADER_NOT_COMPILED }
				},
				rs_in = 
				{
					{ ope=RENDERSTATE_OPE_ENABLEZBUFFER, value="false" },
					{ ope=RENDERSTATE_OPE_ALPHABLENDENABLE, value="true" },
					{ ope=RENDERSTATE_OPE_ALPHABLENDOP, value="add" },
					{ ope=RENDERSTATE_OPE_ALPHABLENDFUNC, value="always" },
					{ ope=RENDERSTATE_OPE_ALPHABLENDDEST, value="invsrcalpha" },
					{ ope=RENDERSTATE_OPE_ALPHABLENDSRC, value="srcalpha" },
					{ ope=RENDERSTATE_OPE_SETCULLING, value="ccw" }
				},
				rs_out =
				{
					{ ope=RENDERSTATE_OPE_ENABLEZBUFFER, value="false" },
					{ ope=RENDERSTATE_OPE_ALPHABLENDENABLE, value="false" },
					{ ope=RENDERSTATE_OPE_SETCULLING, value="cw" }
				}
			},
			textures =
			{
			},
			vertex_textures =
			{
			},
			shaders_params = 
			{
			},
			rendering_order = 4500		
		}
	}
}

planetmod.createmodelview = function(p_rendergraph, p_entity_id, p_passes_bindings, p_planet_specific_config_descr)

  local entity
  local renderer

  entity,renderer=commons.create_rendering_from_module(planetmod.layers, planetmod.module, "planetsRender", p_passes_bindings)
  local specific_config = PlanetSpecificConfig()

  planetmod.setup_specific_config(p_planet_specific_config_descr, specific_config)
  specific_config:apply(renderer)

  renderer:register_to_rendering(rg)

  entity:add_aspect(BODY_ASPECT)
  entity:add_aspect(INFOS_ASPECT)
  entity:setup_info( "entity_name", p_entity_id )
  
  --p_entitygraph:add_child(p_parent_entity_id,p_entity_id,entity)

  local pair = { ['entity'] = entity, ['renderer'] = renderer, ['specific_config'] = specific_config }
  planetmod.models[p_entity_id] = pair

  return entity

end

planetmod.trashmodelview = function(p_rendergraph, p_entitygraph, p_entity_id)

  local entity = planetmod.models[p_entity_id]['entity']
  local renderer = planetmod.models[p_entity_id]['renderer']

  entity:release_info("entity_name")
  entity:remove_aspect(INFOS_ASPECT)
  entity:remove_aspect(BODY_ASPECT)

  commons.trash.rendering(p_rendergraph, planetmod.module, entity, renderer)
  p_entitygraph:remove(p_entity_id)

  local entry = planetmod.models[p_entity_id]
  entry['entity'] = nil
  entry['renderer'] = nil
  entry['specific_config'] = nil

  planetmod.models[p_entity_id] = nil

end

planetmod.view.unload = function(p_entity_id)

  local found_id = FALSE
  for k, v in pairs(planetmod.models) do

    if k == p_entity_id then
	  found_id = TRUE
	end
  end

  if found_id == TRUE then
     model.view.unload(planetmod.trashmodelview, p_entity_id)
  else
    g:print('Unknown entity '..p_entity_id)
  end

end

planetmod.view.load = function(p_entity_id, p_passes_bindings, p_planet_specific_config_descr)

  local found_id = FALSE
  for k, v in pairs(spaceboxmod.models) do

    if k == p_entity_id then
	  found_id = TRUE
	end
  end

  if found_id == TRUE then
    g:print('Entity '..p_entity_id..' already exists')
  else
    model.view.load('planet model', planetmod.createmodelview, p_passes_bindings, nil, nil, p_entity_id, p_planet_specific_config_descr)
  end  

end