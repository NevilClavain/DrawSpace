
planetmod = {}
planetmod.view = {}

-- stockage des instances modeles : paire {entity, renderer, specific_config}
planetmod.models = {}

planetmod.requested_rendering_layers = nil

planetmod.requested_naturaldrawing_layers = nil
planetmod.requested_naturaldrawing_bindings = nil

planetmod.wavepass_name = 'default'


planetmod.read_infos=function(planet_specific_configuration)

	local infos_description = {}

	infos_description['delayedSingleSubPassQueueSize'] = planet_specific_configuration:get_outparam("OUT_delayedSingleSubPassQueueSize")

	local views_infos = { planet_specific_configuration:get_outparam("OUT_viewsInfos") }

	local nb_views = views_infos[1]

	local formatted_views_infos = {}

	for i = 0, nb_views-1, 1 do
		
		local views_infos_entry = {}
		local offset = (16 * i) + 2
		local camera_name = views_infos[offset]
		local current_lod = views_infos[offset + 1]
		local relative = views_infos[offset + 2]
		local rel_alt = views_infos[offset + 3]
		local alt = views_infos[offset + 4]

		local current_patch_max_height = views_infos[offset + 5]
		local current_patch_min_height = views_infos[offset + 6]
		local current_patch_current_height = views_infos[offset + 7]

		local local_camera_pos_x = views_infos[offset + 8]
		local local_camera_pos_y = views_infos[offset + 9]
		local local_camera_pos_z = views_infos[offset + 10]

		local longitud = views_infos[offset + 11]
		local latitud = views_infos[offset + 12]

		local global_camera_pos_x = views_infos[offset + 13]
		local global_camera_pos_y = views_infos[offset + 14]
		local global_camera_pos_z = views_infos[offset + 15]

		views_infos_entry['currentLOD'] = current_lod
		views_infos_entry['relative'] = relative
		views_infos_entry['relative_altitude'] = rel_alt
		views_infos_entry['altitude'] = alt

		views_infos_entry['current_patch_max_height'] = current_patch_max_height
		views_infos_entry['current_patch_min_height'] = current_patch_min_height
		views_infos_entry['current_patch_current_height'] = current_patch_current_height

		views_infos_entry['local_camera_pos_x'] = local_camera_pos_x
		views_infos_entry['local_camera_pos_y'] = local_camera_pos_y
		views_infos_entry['local_camera_pos_z'] = local_camera_pos_z

		views_infos_entry['longitud'] = longitud
		views_infos_entry['latitud'] = latitud

		views_infos_entry['global_camera_pos_x'] = global_camera_pos_x
		views_infos_entry['global_camera_pos_y'] = global_camera_pos_y
		views_infos_entry['global_camera_pos_z'] = global_camera_pos_z

		formatted_views_infos[camera_name] = views_infos_entry

	end

	infos_description['viewsInfos'] = formatted_views_infos

	
	return infos_description
end


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

	if config_description['temp_scale'] ~= nil then
		planet_specific_configuration:set_tempscale(config_description['temp_scale'])
	end


	if config_description['lim_polar'] ~= nil then
		planet_specific_configuration:set_limpolar(config_description['lim_polar'])
	end

	if config_description['lim_tropical'] ~= nil then
		planet_specific_configuration:set_limtropical(config_description['lim_tropical'])
	end

	if config_description['k_polar'] ~= nil then
		planet_specific_configuration:set_kpolar(config_description['k_polar'])
	end

	if config_description['k_tropical'] ~= nil then
		planet_specific_configuration:set_ktropical(config_description['k_tropical'])
	end

	if config_description['humidity_alt_max'] ~= nil then
		planet_specific_configuration:set_humidityaltmax(config_description['humidity_alt_max'])
	end

	if config_description['temp_dec_per_km'] ~= nil then
		planet_specific_configuration:set_tempdecperkm(config_description['temp_dec_per_km'])
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

	if config_description['enable_oceans'] ~= nil then
		planet_specific_configuration:enable_oceans(config_description['enable_oceans'])
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

	if config_description['oceansdetails_specularpower'] ~= nil then
		planet_specific_configuration:set_oceansdetailsspecularpower(config_description['oceansdetails_specularpower'])
	end



	if config_description['details_terrain_bump_bias'] ~= nil then
		planet_specific_configuration:set_detailsterrainbumpbias(config_description['details_terrain_bump_bias'])
	end

	if config_description['details_terrain_noise_scale'] ~= nil then
		planet_specific_configuration:set_detailsterrainnoisescale(config_description['details_terrain_noise_scale'])
	end

	if config_description['level_disturbance_scale'] ~= nil then
		planet_specific_configuration:set_leveldisturbancescale(config_description['level_disturbance_scale'])
	end

	if config_description['details_limit_sup'] ~= nil then
		planet_specific_configuration:set_detailslimitsup(config_description['details_limit_sup'])
	end

	if config_description['bump_details_limit_sup'] ~= nil then
		planet_specific_configuration:set_bumpdetailslimitsup(config_description['bump_details_limit_sup'])
	end

	if config_description['ground_bump_details_factor_depth_distance'] ~= nil then
		planet_specific_configuration:set_groundbumpdetailsfactordepthdistance(config_description['ground_bump_details_factor_depth_distance'])
	end

	if config_description['ground_detail_bump_nb_frac_loop'] ~= nil then
		planet_specific_configuration:set_grounddetailbumpnbfracloop(config_description['ground_detail_bump_nb_frac_loop'])
	end

	if config_description['ultra_details_max_distance'] ~= nil then
		planet_specific_configuration:set_ultradetailsmaxdistance(config_description['ultra_details_max_distance'])
	end

	if config_description['ground_bump_details_factor_depth_near_d1'] ~= nil then
		planet_specific_configuration:set_groundbumpdetailsfactordepthneard1(config_description['ground_bump_details_factor_depth_near_d1'])
	end

	if config_description['ground_bump_details_factor_depth_near_d2'] ~= nil then
		planet_specific_configuration:set_groundbumpdetailsfactordepthneard2(config_description['ground_bump_details_factor_depth_near_d2'])
	end

	if config_description['enable_ground_detail_bump'] ~= nil then
		planet_specific_configuration:enable_grounddetailbump(config_description['enable_ground_detail_bump'])
	end

	if config_description['enable_ultra_detail'] ~= nil then
		planet_specific_configuration:enable_ultradetail(config_description['enable_ultra_detail'])
	end

	if config_description['enable_ultra_detail_bump'] ~= nil then
		planet_specific_configuration:enable_ultradetailbump(config_description['enable_ultra_detail_bump'])
	end

	if config_description['enable_recursive_ultra_detail_textures'] ~= nil then
		planet_specific_configuration:enable_recursiveultradetailtextures(config_description['enable_recursive_ultra_detail_textures'])
	end

end

planetmod.create_rendered_planet = function(p_planet_layers, p_planet_bindings, p_naturaldrawing_layers, p_naturadrawing_bindings)

  local entity=Entity()
  entity:add_aspect(RENDERING_ASPECT)
  entity:add_aspect(RESOURCES_ASPECT)

  local renderer=PlanetRendering(TimeManagerRef(root_entity))
  renderer:attach_toentity(entity)

  -- ###########################################################

  for k, v in pairs(p_planet_bindings) do

	local layer_entry = v

	local pass_id = layer_entry.target_pass_id
	local rendering_id = layer_entry.rendering_id

	renderer:set_passforplanetlayerrenderid(rendering_id, pass_id)
  end

  local planetrenderlayer=RenderLayer()

  for k0, v0 in pairs(p_planet_layers) do
    --g:print("k0 is "..k0)

	local renderconfig=RenderConfig()

	for k, v in pairs(v0) do
	  --g:print("	k is "..k)

	  local rendercontext = RenderContext(k)

	  local fxparams = FxParams()
	  local fx_config = v['fx']

	  local rss=RenderStatesSet()

      local rs_in_config = fx_config['rs_in']
      for k2, v2 in pairs(rs_in_config) do
	    local curr_rs_in = v2
		rss:add_renderstate_in(curr_rs_in['ope'], curr_rs_in['value'])
		if curr_rs_in['value'] == 'extended' then
		  if curr_rs_in['extended_args'] ~= nil then
		    for k2, v2 in pairs(curr_rs_in['extended_args']) do
			  --g:print("	**-> "..k2.." "..v2)
			  rss:push_renderstate_in_extendedargs(0, v2)
			end
		  end
		end
	  end

	  local rs_out_config = fx_config['rs_out']
	  for k2, v2 in pairs(rs_out_config) do
	    local curr_rs_out = v2
		--g:print(curr_rs_out['ope']..'->'..curr_rs_out['value'])
		rss:add_renderstate_out(curr_rs_out['ope'], curr_rs_out['value'])
	  end
	  fxparams:set_renderstatesset(rss)

	  local shaders_config = fx_config['shaders']
	  for k2, v2 in pairs(shaders_config) do
	    local curr_shader = v2
	    --g:print(curr_shader['path']..'->'..curr_shader['mode'])
		fxparams:add_shaderfile(curr_shader['path'],curr_shader['mode'])
	  end

	  local tx_config = v['textures']		
	  for k2, v2 in ipairs(tx_config) do
	    --g:print(k2)
	    local textures = TexturesSet()
		for k3, v3 in pairs(v2) do
		  local tx = v3
		  --g:print(tx['path']..'->'..tx['stage'])
		  textures:set_texturefiletostage(tx['path'], tx['stage'])
		end
		rendercontext:add_texturesset(textures)
	  end

	  local shaderparams_config = v['shaders_params']
	  for k2, v2 in pairs(shaderparams_config) do
	    local param = v2
		--g:print(param['param_name']..'->'..param['shader_index']..','..param['register'])
		rendercontext:add_shaderparam(param['param_name'], param['shader_index'], param['register'])
	  end

	  local ro = v['rendering_order']
	  --g:print( 'ro ='..ro )

	  rendercontext:set_renderingorder(ro)

	  rendercontext:add_fxparams(fxparams)
	  renderconfig:add_rendercontext(rendercontext)

	end

	planetrenderlayer:add_renderconfig(renderconfig, k0)
  end

  -- ###########################################################

  for k, v in pairs(p_naturadrawing_bindings) do

	local layer_entry = v

	local pass_id = layer_entry.target_pass_id
	local rendering_id = layer_entry.rendering_id

	renderer:set_passfornaturallayerrenderid(rendering_id, pass_id)
  end

  local naturaldrawingrenderlayer=RenderLayer()

  for k0, v0 in pairs(p_naturaldrawing_layers) do
    --g:print("k0 is "..k0)

	local renderconfig=RenderConfig()
	for k, v in pairs(v0) do
	  --g:print("	k is "..k)

	  local rendercontext = RenderContext(k)

	  local fxparams = FxParams()
	  local fx_config = v['fx']

	  local rss=RenderStatesSet()

      local rs_in_config = fx_config['rs_in']
      for k2, v2 in pairs(rs_in_config) do
	    local curr_rs_in = v2
		rss:add_renderstate_in(curr_rs_in['ope'], curr_rs_in['value'])
		if curr_rs_in['value'] == 'extended' then
		  if curr_rs_in['extended_args'] ~= nil then
		    for k2, v2 in pairs(curr_rs_in['extended_args']) do
			  --g:print("	**-> "..k2.." "..v2)
			  rss:push_renderstate_in_extendedargs(0, v2)
			end
		  end
		end
	  end

	  local rs_out_config = fx_config['rs_out']
	  for k2, v2 in pairs(rs_out_config) do
	    local curr_rs_out = v2
		--g:print(curr_rs_out['ope']..'->'..curr_rs_out['value'])
		rss:add_renderstate_out(curr_rs_out['ope'], curr_rs_out['value'])
	  end
	  fxparams:set_renderstatesset(rss)

	  local shaders_config = fx_config['shaders']
	  for k2, v2 in pairs(shaders_config) do
	    local curr_shader = v2
	    --g:print(curr_shader['path']..'->'..curr_shader['mode'])
		fxparams:add_shaderfile(curr_shader['path'],curr_shader['mode'])
	  end

	  local tx_config = v['textures']		
	  for k2, v2 in ipairs(tx_config) do
	    --g:print(k2)
	    local textures = TexturesSet()
		for k3, v3 in pairs(v2) do
		  local tx = v3
		  --g:print(tx['path']..'->'..tx['stage'])
		  textures:set_texturefiletostage(tx['path'], tx['stage'])
		end
		rendercontext:add_texturesset(textures)
	  end

	  local shaderparams_config = v['shaders_params']
	  for k2, v2 in pairs(shaderparams_config) do
	    local param = v2
		--g:print(param['param_name']..'->'..param['shader_index']..','..param['register'])
		rendercontext:add_shaderparam(param['param_name'], param['shader_index'], param['register'])
	  end

	  local ro = v['rendering_order']
	  --g:print( 'ro ='..ro )

	  rendercontext:set_renderingorder(ro)

	  rendercontext:add_fxparams(fxparams)
	  renderconfig:add_rendercontext(rendercontext)
	  
	end

	naturaldrawingrenderlayer:add_renderconfig(renderconfig, k0)
  end

  renderer:configure(planetrenderlayer, naturaldrawingrenderlayer)
  return entity, renderer
end

planetmod.trash_planet = function(p_rendergraph, p_entity, p_renderer)

    p_renderer:unregister_from_rendering(p_rendergraph)
	p_renderer:release()
	p_renderer:detach_fromentity()

	p_entity:remove_aspect(RENDERING_ASPECT)
	p_entity:remove_aspect(RESOURCES_ASPECT)

end

planetmod.createmodelview = function(p_rendergraph, p_entity_id, p_passes_bindings, p_planet_specific_config_descr)

  local entity
  local renderer

  entity,renderer=planetmod.create_rendered_planet(planetmod.requested_rendering_layers, p_passes_bindings, planetmod.requested_naturaldrawing_layers, planetmod.requested_naturaldrawing_bindings)

  local specific_config = PlanetConfig()
  planetmod.setup_specific_config(p_planet_specific_config_descr, specific_config)
  specific_config:connect_wavepass(p_rendergraph, planetmod.wavepass_name, renderer)
  specific_config:apply(renderer)

  renderer:register_to_rendering(p_rendergraph)

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
  

  planetmod.trash_planet(p_rendergraph, entity, renderer)

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

planetmod.view.load = function(p_entity_id, p_planet_specific_config_descr, p_passes_bindings, p_planet_layers, p_naturaldrawing_bindings, p_naturaldrawing_layers, wavepass_name)

  planetmod.requested_rendering_layers = p_planet_layers

  planetmod.requested_naturaldrawing_layers = p_naturaldrawing_layers
  planetmod.requested_naturaldrawing_bindings = p_naturaldrawing_bindings

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

planetmod.compute_lights_level = function(global_camera_local_pos)

  local lightfactor = 0.0

  local viewwerPosVector = global_camera_local_pos

  viewwerPosVector:normalize();


  -- ambient light 
  lightfactor = lightfactor + (0.299 * environment.ambient_light.r + 0.587 * environment.ambient_light.g + 0.114 * environment.ambient_light.b)

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

    lightfactor = lightfactor + prodsca
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

    lightfactor = lightfactor + prodsca
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

    lightfactor = lightfactor + prodsca
  end
  
  return lightfactor
end