
planetmod = {}
planetmod.view = {}

planetmod.module = Module("planetsmod", "planets")
planetmod.module:load()
g:print(planetmod.module:get_descr().. ' loaded')

-- stockage des instances modeles : paire {entity, renderer, specific_config}
planetmod.models = {}

planetmod.wavepass_name = 'default'


planetmod.setup_specific_config=function(config_description, planet_specific_configuration)

	if config_description['resources_path'] ~= nil then
		planet_specific_configuration:set_resourcespath(config_description['resources_path'])
	end

	if config_description['planet_ray'] ~= nil then
		planet_specific_configuration:set_planetray(config_description['planet_ray'])
	end

	if config_description['gravity_acc'] ~= nil then
		planet_specific_configuration:set_gravityacc(config_description['gravity_acc'])
	end

    if config_description['atmo_thickness'] ~= nil then
		planet_specific_configuration:set_atmothickness(config_description['atmo_thickness'])
	end

    if config_description['flatclouds_altitude'] ~= nil then
		planet_specific_configuration:set_flatcloudsaltitude(config_description['flatclouds_altitude'])
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

	if config_description['collision_vshader'] ~= nil and 
	   config_description['collision_pshader'] ~= nil then
		planet_specific_configuration:set_collisionshaders(config_description['collision_vshader'], config_description['collision_pshader'])
	end

	if config_description['collision_vshader_compiled'] ~= nil and 
	   config_description['collision_pshader_compiled'] ~= nil then
		planet_specific_configuration:set_collisionshaderscompiled(config_description['collision_vshader_compiled'], config_description['collision_pshader_compiled'])
	end

	if config_description['enable_collisionmeshe_display'] ~= nil and 
	   config_description['enable_collisionmeshe_display'] ~= nil then
		planet_specific_configuration:enable_collisiondisplay(config_description['enable_collisionmeshe_display'])
	end

	if config_description['collisionmeshe_display_vshader'] ~= nil and 
	   config_description['collisionmeshe_display_pshader'] ~= nil then
		planet_specific_configuration:set_collisiondisplayshaders(config_description['collisionmeshe_display_vshader'], config_description['collisionmeshe_display_pshader'])
	end

	if config_description['collisionmeshe_display_vshader_compiled'] ~= nil and 
	   config_description['collisionmeshe_display_pshader_compiled'] ~= nil then
		planet_specific_configuration:set_collisiondisplayshaderscompiled(config_description['collisionmeshe_display_vshader_compiled'], config_description['collisionmeshe_display_pshader_compiled'])
	end

	if config_description['bump_pass'] ~= nil then
		planet_specific_configuration:set_bumppass(config_description['bump_pass'])
	end

	if config_description['reflection_pass'] ~= nil then
		planet_specific_configuration:set_reflectionpass(config_description['reflection_pass'])
	end

	if config_description['main_pass'] ~= nil then
		planet_specific_configuration:set_mainpass(config_description['main_pass'])
	end

	if config_description['oceanmask_pass'] ~= nil then
		planet_specific_configuration:set_oceanmaskpass(config_description['oceanmask_pass'])
	end

	if config_description['wave_pass_resol'] ~= nil then
		planet_specific_configuration:set_wavepassresol(config_description['wave_pass_resol'])
	end

	if config_description['ocean_bump_factor'] ~= nil then
		planet_specific_configuration:set_oceanbumpfactor(config_description['ocean_bump_factor'])
	end

end

planetmod.layers =
{ 
	--surface_layer = 
	[0] = 
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
					{ ope=RENDERSTATE_OPE_SETTEXTUREFILTERTYPE, value="linear" },
					{ ope=RENDERSTATE_OPE_SETVERTEXTEXTUREFILTERTYPE, value="linear" }
												
				},
				rs_out =
				{
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
					{ ope=RENDERSTATE_OPE_SETFILLMODE, value="line" }
						
				},
				rs_out =
				{
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

	--atmosphere_layer
	[1] = 
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
		},

		atmo_mirror_rendering =	
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
				},
				rs_out =
				{
					{ ope=RENDERSTATE_OPE_ENABLEZBUFFER, value="false" },
					{ ope=RENDERSTATE_OPE_ALPHABLENDENABLE, value="false" },
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
		},
	},

	--flatclouds_layer
	[2] = 
	{
		flatclouds_rendering =	
		{
			fx =
			{
				shaders = 
				{
					{ path='planet_flatclouds_vs.hlsl', mode=SHADER_NOT_COMPILED },
					{ path='planet_flatclouds_ps.hlsl', mode=SHADER_NOT_COMPILED }
				},
				
				rs_in = 
				{
					{ ope=RENDERSTATE_OPE_SETTEXTUREFILTERTYPE, value="linear" },
					{ ope=RENDERSTATE_OPE_ALPHABLENDENABLE, value="true" },
					{ ope=RENDERSTATE_OPE_ALPHABLENDOP, value="add" },
					{ ope=RENDERSTATE_OPE_ALPHABLENDFUNC, value="always" },
					{ ope=RENDERSTATE_OPE_ALPHABLENDDEST, value="invsrcalpha" },
					{ ope=RENDERSTATE_OPE_ALPHABLENDSRC, value="srcalpha" },

						
				},
				rs_out =
				{
					{ ope=RENDERSTATE_OPE_SETTEXTUREFILTERTYPE, value="none" },
					{ ope=RENDERSTATE_OPE_ALPHABLENDENABLE, value="false" },
				}
			},
			textures =
			{
				[1] = 
				{
					{ path='se_asia_clouds_8k.jpg', stage=0 }
				}
			},
			vertex_textures =
			{
			},
			shaders_params = 
			{
			},
			rendering_order = 5500		
		},

		flatclouds_mirror_rendering =	
		{
			fx =
			{
				shaders = 
				{
					{ path='planet_flatclouds_vs.hlsl', mode=SHADER_NOT_COMPILED },
					{ path='planet_flatclouds_ps.hlsl', mode=SHADER_NOT_COMPILED }
				},
				
				rs_in = 
				{
					{ ope=RENDERSTATE_OPE_SETTEXTUREFILTERTYPE, value="linear" },
					{ ope=RENDERSTATE_OPE_ALPHABLENDENABLE, value="true" },
					{ ope=RENDERSTATE_OPE_ALPHABLENDOP, value="add" },
					{ ope=RENDERSTATE_OPE_ALPHABLENDFUNC, value="always" },
					{ ope=RENDERSTATE_OPE_ALPHABLENDDEST, value="invsrcalpha" },
					{ ope=RENDERSTATE_OPE_ALPHABLENDSRC, value="srcalpha" },						
				},
				rs_out =
				{
					{ ope=RENDERSTATE_OPE_SETTEXTUREFILTERTYPE, value="none" },
					{ ope=RENDERSTATE_OPE_ALPHABLENDENABLE, value="false" },
				}
			},
			textures =
			{
				[1] = 
				{
					{ path='se_asia_clouds_8k.jpg', stage=0 }
				}
			},
			vertex_textures =
			{
			},
			shaders_params = 
			{
			},
			rendering_order = 5500		
		}

	},
	
	--ocean_layer
	[3] = 
	{
		oceans_rendering = 
		{
			fx =
			{
				shaders = 
				{
					{ path='planet_ocean_vs.hlsl', mode=SHADER_NOT_COMPILED },
					{ path='planet_ocean_ps.hlsl', mode=SHADER_NOT_COMPILED }
				},
				
				rs_in = 
				{
					{ ope=RENDERSTATE_OPE_ENABLEZBUFFER, value="false" },
					--{ ope=RENDERSTATE_OPE_SETFILLMODE, value="line" }
						
				},
				rs_out =
				{
					{ ope=RENDERSTATE_OPE_ENABLEZBUFFER, value="false"},
					--{ ope=RENDERSTATE_OPE_SETFILLMODE, value="solid" }
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
			rendering_order = 4900
			--rendering_order = 5100
		},
		oceans_bump_rendering = 
		{
			fx = 
			{
				shaders = 
				{
					{ path='planet_ocean_bump_vs.hlsl',mode=SHADER_NOT_COMPILED },
					{ path='planet_ocean_bump_ps.hlsl',mode=SHADER_NOT_COMPILED }
				},
				rs_in = 
				{					
					{ ope=RENDERSTATE_OPE_SETTEXTUREFILTERTYPE, value="linear" },
					{ ope=RENDERSTATE_OPE_ENABLEZBUFFER, value="true"}
				},
				rs_out =
				{
					{ ope=RENDERSTATE_OPE_ENABLEZBUFFER, value="false" },
					{ ope=RENDERSTATE_OPE_SETTEXTUREFILTERTYPE, value="none" }
				}
			},
			textures =
			{
			},
			vertex_textures =
			{
			},
			rendering_order = 10000,
			shaders_params = 
			{			
			}
		}
	}
}

planetmod.createmodelview = function(p_rendergraph, p_entity_id, p_passes_bindings, p_planet_specific_config_descr)

  local entity
  local renderer

  entity,renderer=commons.create_rendering_from_module(planetmod.layers, planetmod.module, "planetsRender", p_passes_bindings)
  local specific_config = PlanetSpecificConfig()

  planetmod.setup_specific_config(p_planet_specific_config_descr, specific_config)

  specific_config:connect_wavepass(p_rendergraph, planetmod.wavepass_name, renderer)

  specific_config:apply(renderer)

  renderer:register_to_rendering(rg)

  
  entity:add_aspect(INFOS_ASPECT)
  entity:setup_info( "entity_name", p_entity_id )

  entity:add_aspect(PHYSICS_ASPECT)
  
  
  local pair = { ['entity'] = entity, ['renderer'] = renderer, ['specific_config'] = specific_config }
  planetmod.models[p_entity_id] = pair

  return entity

end

planetmod.trashmodelview = function(p_rendergraph, p_entitygraph, p_entity_id)

  local entity = planetmod.models[p_entity_id]['entity']
  local renderer = planetmod.models[p_entity_id]['renderer']

  entity:release_info("entity_name")
  entity:remove_aspect(INFOS_ASPECT)
  

  commons.trash.rendering(p_rendergraph, planetmod.module, entity, renderer)
  entity:remove_aspect(PHYSICS_ASPECT)

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

planetmod.view.load = function(p_entity_id, p_passes_bindings, p_planet_specific_config_descr, wavepass_name)

  local found_id = FALSE
  for k, v in pairs(spaceboxmod.models) do

    if k == p_entity_id then
	  found_id = TRUE
	end
  end

  planetmod.wavepass_name = wavepass_name

  if found_id == TRUE then
    g:print('Entity '..p_entity_id..' already exists')
  else
    model.view.load('planet model', planetmod.createmodelview, p_passes_bindings, nil, p_entity_id, p_planet_specific_config_descr)
  end  
end

planetmod.compute_ocean_sub_lights_level = function(global_camera_local_pos)

  local underwater_lightfactor = 0.0

  local viewwerPosVector = global_camera_local_pos

  viewwerPosVector:normalize();


  -- ambient light 
  underwater_lightfactor = underwater_lightfactor + (0.299 * environment.ambient_light.r + 0.587 * environment.ambient_light.g + 0.114 * environment.ambient_light.b)

  -- light0
  if environment.lights_enabled.x == TRUE then

    local light1Vector = Vector(-environment.light0.direction.x,
                                    -environment.light0.direction.y,
                                    -environment.light0.direction.z,
                                    1.0)

    
    light1Vector:normalize();

    local prodsca = light1Vector:dotproduct_with(viewwerPosVector)
    if prodsca < 0.0 then
      prodsca = 0.0
    end

    underwater_lightfactor = underwater_lightfactor + prodsca
  end

  -- light1
  if environment.lights_enabled.y == TRUE then

    local light2Vector = Vector(-environment.light1.direction.x,
                                    -environment.light1.direction.y,
                                    -environment.light1.direction.z,
                                    1.0)

    
    light2Vector:normalize();

    local prodsca = light2Vector:dotproduct_with(viewwerPosVector)
    if prodsca < 0.0 then
      prodsca = 0.0
    end

    underwater_lightfactor = underwater_lightfactor + prodsca
  end

  -- light2
  if environment.lights_enabled.z == TRUE then

    local light3Vector = Vector(-environment.light2.direction.x,
                                    -environment.light2.direction.y,
                                    -environment.light2.direction.z,
                                    1.0)

    
    light3Vector:normalize();

    local prodsca = light3Vector:dotproduct_with(viewwerPosVector)
    if prodsca < 0.0 then
      prodsca = 0.0
    end

    underwater_lightfactor = underwater_lightfactor + prodsca
  end
  
  return underwater_lightfactor
end