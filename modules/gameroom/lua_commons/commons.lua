
commons = {}

commons.utils = {}
commons.trash = {}

commons.procedural = {}

commons.procedural.nebulae = {}
commons.procedural.planet = {}


commons.utils.PI = 3.1415927

REFLECTIONS_OFF=0
REFLECTIONS_ON=1


commons.utils.startup=function(startup_lua_file)

	g:log(DEBUG,"STARTUP BEGIN")
	g:dump_mem()
	gameroom_mem_alloc_size = g:total_mem()
	g:do_file(startup_lua_file)
	g:log(DEBUG,"STARTUP END")

end

commons.utils.shutdown=function(shutdown_lua_file)

	g:log(DEBUG,"SHUTDOWN BEGIN")
	g:do_file(shutdown_lua_file)
	g:dump_mem()
	if gameroom_mem_alloc_size ~= g:total_mem() then
	  g:log(FATAL,"GAMEROOM MEM LEAK DETECTED : "..gameroom_mem_alloc_size.. " -> " ..g:total_mem() )
	  g:ds_exception( "GAMEROOM MEM LEAK DETECTED : "..gameroom_mem_alloc_size.. " -> " ..g:total_mem() )
	end
	g:log(DEBUG,"SHUTDOWN END")

end

commons.utils.deg_to_rad = function(angle)
	return ( ( angle * commons.utils.PI ) / 180.0 )
end

commons.utils.init_matrix = function(array)

    local m = Matrix()
	local count = 0
	for k, v in pairs(array) do
	   --g:print(v)

	   local col = count % 4
	   local row = g:round(count / 4)

	   --g:print("c,r="..col.." "..row)

	   m:set_value(row, col, v)
	   count = count + 1
	end
	return m
end

commons.init_final_pass = function(p_rendergraph, p_passid)

	p_rendergraph:create_pass_viewportquad(p_passid)

	local finalpass_rss=RenderStatesSet()

	finalpass_rss:add_renderstate_in(RENDERSTATE_OPE_SETTEXTUREFILTERTYPE, "point")
	finalpass_rss:add_renderstate_out(RENDERSTATE_OPE_SETTEXTUREFILTERTYPE, "linear")

	local textures = TexturesSet()

	local fxparams = FxParams()
	fxparams:add_shaderfile('texture_vs.hlsl',SHADER_NOT_COMPILED)
	fxparams:add_shaderfile('texture_ps.hlsl',SHADER_NOT_COMPILED)
	fxparams:set_renderstatesset(finalpass_rss)

	local rendercontext = RenderContext(p_passid)
	rendercontext:add_fxparams(fxparams)
	rendercontext:add_texturesset(textures)

	local renderconfig=RenderConfig()
	renderconfig:add_rendercontext(rendercontext)
	p_rendergraph:configure_pass_viewportquad_resources(p_passid,renderconfig)

end

commons.create_static_camera = function(p_x, p_y, p_z, p_viewport_width, p_viewport_height, p_module, p_name)

	local camera_entity=Entity()
	camera_entity:add_aspect(TRANSFORM_ASPECT)
	camera_entity:add_aspect(CAMERA_ASPECT)
	
	camera_entity:configure_camera(p_viewport_width,p_viewport_height, 1.0, 90000000000.0, p_name)

	camera_entity:add_aspect(INFOS_ASPECT)
	camera_entity:setup_info( "entity_name", p_name )

	local pos = Matrix();
	pos:translation(p_x, p_y, p_z)

	local cam_transform = RawTransform()
	cam_transform:configure(camera_entity,0)
	cam_transform:add_matrix("pos",pos)

	return camera_entity, cam_transform
end


commons.create_fps_camera = function(p_x, p_y, p_z, p_viewport_width, p_viewport_height, p_module, p_name)

	local camera_entity=Entity()
	camera_entity:add_aspect(TRANSFORM_ASPECT)
	camera_entity:add_aspect(CAMERA_ASPECT)

	camera_entity:configure_camera(p_viewport_width,p_viewport_height, 1.0, 90000000000.0, p_name)

	camera_entity:add_aspect(INFOS_ASPECT)
	camera_entity:setup_info( "entity_name", p_name )


	local fps_transfo=FPSTransform()
	fps_transfo:instanciate_transformimpl(p_module)
	fps_transfo:configure(camera_entity,0,0,p_x,p_y,p_z,TRUE,0)

	return camera_entity, fps_transfo
end

commons.create_free_camera = function(p_x, p_y, p_z, p_viewport_width, p_viewport_height, p_module, p_name)
	local camera_entity=Entity()
	camera_entity:add_aspect(TRANSFORM_ASPECT)
	camera_entity:add_aspect(CAMERA_ASPECT)

	camera_entity:configure_camera(p_viewport_width,p_viewport_height, 1.0, 90000000000.0, p_name)

	camera_entity:add_aspect(INFOS_ASPECT)
	camera_entity:setup_info( "entity_name", p_name )


	local free_transfo=FreeTransform()
	free_transfo:instanciate_transformimpl(p_module)
	free_transfo:configure(camera_entity,0,p_x, p_y, p_z,0)

	return camera_entity, free_transfo
end

commons.create_rendering_from_module = function(p_layers, p_module, p_rendering_impl_id, rendering_passes_array)

	local entity=Entity()
	entity:add_aspect(RENDERING_ASPECT)
	entity:add_aspect(RESOURCES_ASPECT)

	local renderlayer=RenderLayer()
	
	for k0, v0 in pairs(p_layers) do

		local renderconfig=RenderConfig()

		for k, v in pairs(v0) do

			local rendercontext = RenderContext(k)

			local fxparams = FxParams()
			local fx_config = v['fx']

			local rss=RenderStatesSet()

			local rs_in_config = fx_config['rs_in']
			for k2, v2 in pairs(rs_in_config) do
				local curr_rs_in = v2
				--g:print(curr_rs_in['ope']..'->'..curr_rs_in['value'])
				rss:add_renderstate_in(curr_rs_in['ope'], curr_rs_in['value'])
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
		
		
			local vtx_config = v['vertex_textures']
			for k2, v2 in ipairs(vtx_config) do

				local vtextures = TexturesSet()
				for k3, v3 in pairs(v2) do
					local tx = v3
					--g:print(tx['path']..'->'..tx['stage'])
					vtextures:set_texturefiletostage(tx['path'], tx['stage'])
				end
				rendercontext:add_vertextexturesset(vtextures)
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

		renderlayer:add_renderconfig(renderconfig)
	end

	local rendering=Rendering(TimeManagerRef(root_entity))

	for k, v in pairs(rendering_passes_array) do

		local layer_entry = v

		local pass_id = layer_entry.target_pass_id
		local rendering_id = layer_entry.rendering_id

		rendering:set_passforrenderid(rendering_id, pass_id)
	end

	rendering:instanciate_renderingimpl(p_module,p_rendering_impl_id)
	rendering:attach_toentity(entity)
	rendering:configure(renderlayer)

	return entity,rendering
end

commons.create_rendered_meshe = function(p_config, p_meshefile, p_meshe_name, rendering_passes_array)

	local meshe_entity=Entity()
	meshe_entity:add_aspect(RENDERING_ASPECT)
	meshe_entity:add_aspect(TRANSFORM_ASPECT)
	meshe_entity:add_aspect(RESOURCES_ASPECT)

	local renderconfig=RenderConfig()

	local renderer=MesheRendering()
	renderer:attach_toentity(meshe_entity)


	for k, v in pairs(rendering_passes_array) do

		local layer_entry = v

		local pass_id = layer_entry.target_pass_id
		local rendering_id = layer_entry.rendering_id

		renderer:set_passforrenderid(rendering_id, pass_id)
	end


	for k, v in pairs(p_config) do
		--g:print(k)

		if k == 'meshes_loader_params' then
		  for k2, v2 in pairs(v) do
		    --g:print(k2)

			if k2 == 'normale_generation_mode' then
			  renderer:set_normalegenerationmode(v2)

			elseif k2 == 'tb_generation_mode' then
			  renderer:set_tbgenerationmode(v2)

			elseif k2 == 'normales_transform' then
			  renderer:set_setnormaletransformation(v2)

			end
		  end

		else

			local rendercontext = RenderContext(k)

			local fxparams = FxParams()
			local fx_config = v['fx']

			local rss=RenderStatesSet()

			local rs_in_config = fx_config['rs_in']
			for k2, v2 in pairs(rs_in_config) do
				local curr_rs_in = v2
				--g:print(curr_rs_in['ope']..'->'..curr_rs_in['value'])
				rss:add_renderstate_in(curr_rs_in['ope'], curr_rs_in['value'])
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
		
		
			local vtx_config = v['vertex_textures']
			for k2, v2 in ipairs(vtx_config) do

				local vtextures = TexturesSet()
				for k3, v3 in pairs(v2) do
					local tx = v3
					--g:print(tx['path']..'->'..tx['stage'])
					vtextures:set_texturefiletostage(tx['path'], tx['stage'])
				end
				rendercontext:add_vertextexturesset(vtextures)
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
	end

	renderer:configure(renderconfig, p_meshefile, p_meshe_name)

	return meshe_entity, renderer
end


--specific to lit shader, so this should be removed 
-- create a table with all required parameters for lit vertex and pixel shaders (lit.vso, lit.pso)
commons.setup_lit_shader_params = function()

	shaders_params_table = {}
	
	shaders_params_table[0] = { param_name = "lights_enabled_v", shader_index = 0, register = 24 }
	shaders_params_table[1] = { param_name = "light0_dir_v", shader_index = 0, register = 25 }
	shaders_params_table[2] = { param_name = "flags_v", shader_index = 0, register = 26 }
	shaders_params_table[3] = { param_name = "reflectorPos", shader_index = 0, register = 27 }
	shaders_params_table[4] = { param_name = "reflectorNormale", shader_index = 0, register = 28 }
	shaders_params_table[5] = { param_name = "ambient_color", shader_index = 1, register = 0 }
	shaders_params_table[6] = { param_name = "lights_enabled", shader_index = 1, register = 1 }
	shaders_params_table[7] = { param_name = "light0_color", shader_index = 1, register = 2 }
	shaders_params_table[8] = { param_name = "light0_dir", shader_index = 1, register = 3 }
	shaders_params_table[9] = { param_name = "flags", shader_index = 1, register = 7 }
	shaders_params_table[10] = { param_name = "self_emissive", shader_index = 1, register = 8 }
	shaders_params_table[11] = { param_name = "absorption", shader_index = 1, register = 9 }
	shaders_params_table[12] = { param_name = "color", shader_index = 1, register = 10 }
	shaders_params_table[13] = { param_name = "color_source", shader_index = 1, register = 11 }
	shaders_params_table[14] = { param_name = "fog_color", shader_index = 1, register = 12 }
	shaders_params_table[15] = { param_name = "flags2", shader_index = 1, register = 13 }

	return shaders_params_table
end


--specific to lit shader, so this should be removed 
commons.update_lights = function( p_pass_id, p_lights_table, p_renderer_tables )

	for k, v in pairs(p_renderer_tables) do
		
		local renderer = v

		renderer:set_shaderrealvector( p_pass_id, 'ambient_color', p_lights_table.ambient_light.r, p_lights_table.ambient_light.g, p_lights_table.ambient_light.b, p_lights_table.ambient_light.a )
		renderer:set_shaderrealvector( p_pass_id, 'lights_enabled', p_lights_table.lights_enabled.x, p_lights_table.lights_enabled.y, p_lights_table.lights_enabled.z, p_lights_table.lights_enabled.w )
		renderer:set_shaderrealvector( p_pass_id, 'light0_color', p_lights_table.light0.color.r, p_lights_table.light0.color.g, p_lights_table.light0.color.b, p_lights_table.light0.color.a )
		renderer:set_shaderrealvector( p_pass_id, 'light0_dir', p_lights_table.light0.direction.x, p_lights_table.light0.direction.y, p_lights_table.light0.direction.z, p_lights_table.light0.direction.w )

		renderer:set_shaderrealvector( p_pass_id, 'light0_dir_v', p_lights_table.light0.direction.x, p_lights_table.light0.direction.y, p_lights_table.light0.direction.z, p_lights_table.light0.direction.w )
		renderer:set_shaderrealvector( p_pass_id, 'lights_enabled_v', p_lights_table.lights_enabled.x, p_lights_table.lights_enabled.y, p_lights_table.lights_enabled.z, p_lights_table.lights_enabled.w )
	end
end

--specific to lit shader, so this should be removed 
commons.setup_lit_flags = function( p_pass_id, p_renderer_tables, p_mirror, p_reflector_pos, p_reflector_normale, p_fog_intensity, p_fog_color )
	for k, v in pairs(p_renderer_tables) do
		local renderer = v

		renderer:set_shaderrealinvector( p_pass_id, 'flags_v', 0, p_mirror)
		renderer:set_shaderrealinvector( p_pass_id, 'flags_v', 1, p_fog_intensity)

		renderer:set_shaderrealvector( p_pass_id, 'fog_color', p_fog_color.r, p_fog_color.g, p_fog_color.b, 1.0 )
		
		renderer:set_shaderrealvector( p_pass_id, 'reflectorPos', p_reflector_pos.x, p_reflector_pos.y, p_reflector_pos.z, 1.0 )
		renderer:set_shaderrealvector( p_pass_id, 'reflectorNormale', p_reflector_normale.x, p_reflector_normale.y, p_reflector_normale.z, 1.0 )
	end
end

--specific to lit shader, so this should be removed 
commons.setup_lit_flags_simple = function( p_pass_id, p_renderer_tables, p_fog_intensity, p_fog_color )
	for k, v in pairs(p_renderer_tables) do
		local renderer = v

		renderer:set_shaderrealinvector( p_pass_id, 'flags_v', 1, p_fog_intensity)
		renderer:set_shaderrealvector( p_pass_id, 'fog_color', p_fog_color.r, p_fog_color.g, p_fog_color.b, 1.0 )
	end
end

commons.update_planet_lights = function( p_lights_table, p_planet_specific_config )

  if p_lights_table.ambient_light ~= nil then
    p_planet_specific_config:enable_light( 0, TRUE )
    p_planet_specific_config:set_lightcolor( 0, p_lights_table.ambient_light.r, p_lights_table.ambient_light.g, p_lights_table.ambient_light.b )
  end

  if p_lights_table.light0 ~= nil then
    p_planet_specific_config:enable_light( 1, p_lights_table.lights_enabled.x )
    p_planet_specific_config:set_lightcolor( 1, p_lights_table.light0.color.r, p_lights_table.light0.color.g, p_lights_table.light0.color.b )
    p_planet_specific_config:set_lightdir( 1, p_lights_table.light0.direction.x, p_lights_table.light0.direction.y, p_lights_table.light0.direction.z )
  end

  if p_lights_table.light1 ~= nil then
    p_planet_specific_config:enable_light( 2, p_lights_table.lights_enabled.y )
    p_planet_specific_config:set_lightcolor( 2, p_lights_table.light1.color.r, p_lights_table.light1.color.g, p_lights_table.light1.color.b )
    p_planet_specific_config:set_lightdir( 2, p_lights_table.light1.direction.x, p_lights_table.light1.direction.y, p_lights_table.light1.direction.z )
  end

  if p_lights_table.light2 ~= nil then
    p_planet_specific_config:enable_light( 3, p_lights_table.lights_enabled.z )
    p_planet_specific_config:set_lightcolor( 3, p_lights_table.light2.color.r, p_lights_table.light2.color.g, p_lights_table.light2.color.b )
    p_planet_specific_config:set_lightdir( 3, p_lights_table.light2.direction.x, p_lights_table.light2.direction.y, p_lights_table.light2.direction.z )
  end

 p_planet_specific_config:updated()

end

commons.apply_material = function(p_material, p_renderer, p_pass_id)
	
	local texture_size = 512.0
	local bump_bias = 0.0
	local specular_power = 0.0
	local spec_mask_mode = 0


	if p_material['bump_mapping'] ~= nil then
	  bump_bias = p_material['bump_mapping']['bias']
	  texture_size = p_material['bump_mapping']['texture_size']
	end

	if p_material['specular_power'] ~= nil then
		specular_power = p_material['specular_power']
	end

	if p_material['spec_mask_mode'] ~= nil then
		spec_mask_mode = p_material['spec_mask_mode']
	end

	--g:print( 'specular_power ='..specular_power )
	--g:print( 'bump_bias ='..bump_bias )
	--g:print( 'texture_size ='..texture_size )
	--g:print( 'spec_mask_mode ='..spec_mask_mode )

	p_renderer:set_shaderrealvector( p_pass_id, 'flags', specular_power, bump_bias, texture_size, spec_mask_mode )

	if p_material['color_source'] ~= nil then
		p_renderer:set_shaderrealvector( p_pass_id, 'color_source', p_material['color_source']['r'], p_material['color_source']['g'], p_material['color_source']['b'], p_material['color_source']['a'] )
	end

	if p_material['simple_color'] ~= nil then
		p_renderer:set_shaderrealvector( p_pass_id, 'color', p_material['simple_color']['r'], p_material['simple_color']['g'], p_material['simple_color']['b'], p_material['simple_color']['a'] )
	end

	if p_material['self_emissive'] ~= nil then
		p_renderer:set_shaderrealvector( p_pass_id, 'self_emissive', p_material['self_emissive']['r'], p_material['self_emissive']['g'], p_material['self_emissive']['b'], p_material['self_emissive']['a'] )
	end

	if p_material['light_absorption'] ~= nil then
		p_renderer:set_shaderrealvector( p_pass_id, 'absorption', p_material['light_absorption']['r'], p_material['light_absorption']['g'], p_material['light_absorption']['b'], p_material['light_absorption']['a'] )
	end
end


commons.print_vector=function(msg,vector)
	g:print(msg..vector:get_x().. ' '..vector:get_y().. ' '..vector:get_z())	
end


commons.print_timescale = function(timescale)

  text_timescale = { [NORMAL_TIME] = "x1", [MUL2_TIME] = "x2", [MUL4_TIME] = "x4", [MUL10_TIME] = "x10",
	
				     [MUL100_TIME] = "x100", [MUL500_TIME] = "x500", [SEC_1HOUR_TIME] = "1s = 1h", 
					 [SEC_1DAY_TIME] = "1s = 1d", [SEC_30DAYS_TIME] = "1s = 30d", [SEC_1YEAR_TIME] = "1s = 1y", 
					 [DIV2_TIME] = " x0.5", [DIV4_TIME] = " x0.25", [DIV10_TIME] = " x0.1", [FREEZE] = "PAUSED"
				   }

  return text_timescale[timescale]

end


-- find random vector perpendicular to vec_in (dot product vec_in * res_vec equals 0)
commons.procedural.find_normal_vector=function(vec_in,random_engine,vec_out)

	local real_distr=Distribution("uniform_real_distribution", -1.0, 1.0)

	local xp = 0.0
	local yp = 0.0

	while xp == 0.0 and yp == 0.0 do
		xp = real_distr:generate(random_engine)
		yp = real_distr:generate(random_engine)
	end
	local zp = (-1.0 / vec_in:get_z()) * ((xp * vec_in:get_x()) + (yp * vec_in:get_y()))

	vec_out:set_x(xp)
	vec_out:set_y(yp)
	vec_out:set_z(zp)
end


commons.procedural.planet.read_infos=function(planet_specific_configuration)

	local infos_description = {}

	infos_description['delayedSingleSubPassQueueSize'] = planet_specific_configuration:get_outparam("OUT_delayedSingleSubPassQueueSize")

	local views_infos = { planet_specific_configuration:get_outparam("OUT_viewsInfos") }

	local nb_views = views_infos[1]

	local formatted_views_infos = {}

	for i = 0, nb_views-1, 1 do
		
		local views_infos_entry = {}
		local offset = (8 * i) + 2
		local camera_name = views_infos[offset]
		local current_lod = views_infos[offset + 1]
		local relative = views_infos[offset + 2]
		local rel_alt = views_infos[offset + 3]
		local alt = views_infos[offset + 4]

		local current_patch_max_height = views_infos[offset + 5]
		local current_patch_min_height = views_infos[offset + 6]
		local current_patch_current_height = views_infos[offset + 7]

		views_infos_entry['currentLOD'] = current_lod
		views_infos_entry['relative'] = relative
		views_infos_entry['relative_altitude'] = rel_alt
		views_infos_entry['altitude'] = alt

		views_infos_entry['current_patch_max_height'] = current_patch_max_height
		views_infos_entry['current_patch_min_height'] = current_patch_min_height
		views_infos_entry['current_patch_current_height'] = current_patch_current_height

		formatted_views_infos[camera_name] = views_infos_entry

	end

	infos_description['viewsInfos'] = formatted_views_infos

	
	return infos_description
end


commons.procedural.nebulae.build_specific_config=function(nebulae_specific_configuration,random_engine)

	local color_distr=Distribution("uniform_real_distribution", 0.1, 0.8)

	local nb_color_distr=Distribution("uniform_int_distribution", 0, 1)
	
	local nb_branches_distr=Distribution("uniform_int_distribution", 1, 5)
	local nb_branches = nb_branches_distr:generate(random_engine)


	local mono_color = nb_color_distr:generate(random_engine)
	local mono_base_color_r = color_distr:generate(random_engine)	
	local mono_base_color_g = color_distr:generate(random_engine)
	local mono_base_color_b = color_distr:generate(random_engine)

    for i = 0, nb_branches - 1, 1 do	

		local base_color_r = color_distr:generate(random_engine)	
		local base_color_g = color_distr:generate(random_engine)
		local base_color_b = color_distr:generate(random_engine)

		local color_vector;

		if mono_color == 0 then 
			color_vector = Vector(base_color_r, base_color_g, base_color_b, 1.0)
		else
			color_vector = Vector(mono_base_color_r, mono_base_color_g, mono_base_color_b, 1.0)
		end

		commons.procedural.nebulae.build_branche(nebulae_specific_configuration,random_engine,color_vector)
	end
end


commons.procedural.nebulae.build_branche=function(nebulae_specific_configuration,random_engine,base_color_vector)
   -- generation du vecteur direction

	local dir_vector_distr=Distribution("uniform_real_distribution", -1.0, 1.0)

	local xdir = 0.0
	local ydir = 0.0
	local zdir = 0.0

	while xdir == 0.0 and ydir == 0.0 and zdir == 0.0 do
		xdir = dir_vector_distr:generate(random_engine)
		ydir = dir_vector_distr:generate(random_engine)
		zdir = dir_vector_distr:generate(random_engine)
	end

	local dir_vector = Vector(xdir, ydir, zdir, 1.0)
	dir_vector:normalize()

	commons.print_vector('dir_vector : ', dir_vector)

	local pos_vector = Vector()
	local prev_pos_vector = Vector(0.0, 0.0, 0.0, 0.0)
	local diff_pos_vector = Vector(0.0, 0.0, 0.0, 0.0)

		-- nbre de blocs sur la branche
	local distr1=Distribution("uniform_int_distribution", 2, 5)
	local nb_blocs_branches = distr1:generate(random_engine)

	local step_distr=Distribution("uniform_real_distribution", 0.1, 1.9)
	g:print('nb blocs par branches : '..nb_blocs_branches)

	local delta_distr=Distribution("uniform_real_distribution", 0.15, 0.533)

	commons.print_vector('base_color_vector : ', base_color_vector)

	local black_bloc_distr=Distribution("uniform_int_distribution", 0, 7)

	local branch_scale = 0.0
	for i = 0, nb_blocs_branches - 1, 1 do		
		local step=step_distr:generate(random_engine)
		branch_scale = branch_scale + step

		pos_vector:copy(dir_vector)
		pos_vector:scale(branch_scale)

		commons.print_vector('pos_vector : ', pos_vector)

		diff_pos_vector:copy(pos_vector)
		diff_pos_vector:sub_with(prev_pos_vector)
		commons.print_vector('diff_pos_vector : ', diff_pos_vector)
		prev_pos_vector:copy(pos_vector)


		local delta_vector=Vector()	
		commons.procedural.find_normal_vector(dir_vector,random_engine,delta_vector)
		delta_vector:scale(delta_distr:generate(random_engine))

		local bloc_index = nebulae_specific_configuration:create_bloc()

		local set_black_color = black_bloc_distr:generate(random_engine)

		if set_black_color == 0 then 
			nebulae_specific_configuration:set_bloccolor(bloc_index, 0, 0, 0)
		else
			nebulae_specific_configuration:set_bloccolor(bloc_index, base_color_vector:get_x(), base_color_vector:get_y(), base_color_vector:get_z())
		end

		nebulae_specific_configuration:set_blocscale(bloc_index, 3.0 * diff_pos_vector:length())
		nebulae_specific_config:set_blocposition(bloc_index, pos_vector:get_x() + delta_vector:get_x(), 
													pos_vector:get_y() + delta_vector:get_y(), 
													pos_vector:get_z() + delta_vector:get_z())

		commons.procedural.nebulae.generate_texture_uv_coords(nebulae_specific_configuration, bloc_index, random_engine, 100, 0, 7)
		commons.procedural.nebulae.generate_mask_uv_coords(nebulae_specific_configuration, bloc_index, random_engine, 100, 0, 3)

	end
end




commons.procedural.nebulae.generate_texture_uv_coords = function(nebulae_specific_configuration, bloc_index, random_engine, list_size, min, max)
  
	local distr=Distribution("uniform_int_distribution", min, max)

	for i = 0, list_size - 1, 1 do
		local u = distr:generate(random_engine)
		local v = distr:generate(random_engine)
		nebulae_specific_configuration:add_bloctextureuvpair(bloc_index, u, v)
	end

	distr=0;
end

commons.procedural.nebulae.generate_mask_uv_coords = function(nebulae_specific_configuration, bloc_index, random_engine, list_size, min, max)
  
  local distr=Distribution("uniform_int_distribution", min, max)

  for i = 0, list_size - 1, 1 do
    local u = distr:generate(random_engine)
	local v = distr:generate(random_engine)
	nebulae_specific_configuration:add_blocmaskuvpair(bloc_index, u, v)
  end
  distr = 0;
end







commons.trash.rendering = function(p_rendergraph, p_module, p_entity, p_renderer)

	p_renderer:unregister_from_rendering(p_rendergraph)
	p_renderer:release()
	p_renderer:detach_fromentity()
	p_renderer:trash_renderingimpl(p_module)

	p_entity:remove_aspect(RENDERING_ASPECT)
	p_entity:remove_aspect(RESOURCES_ASPECT)

end

commons.trash.meshe = function(p_rendergraph, p_entity, p_renderer)

	p_renderer:unregister_from_rendering(p_rendergraph)
	p_renderer:release()
	p_renderer:detach_fromentity()

	p_entity:remove_aspect(TRANSFORM_ASPECT)
	p_entity:remove_aspect(RENDERING_ASPECT)
	p_entity:remove_aspect(RESOURCES_ASPECT)

end



commons.trash.camera = function(p_camera_entity, p_mvt_transfo)

	p_mvt_transfo:release()
	p_camera_entity:release_camera()
	p_camera_entity:remove_aspect(TRANSFORM_ASPECT)
	p_camera_entity:remove_aspect(CAMERA_ASPECT)

	p_camera_entity:release_info("entity_name")
	p_camera_entity:remove_aspect(INFOS_ASPECT)
end

commons.trash.final_pass = function(p_rendergraph, p_passid)
	p_rendergraph:release_pass_viewportquad_resources(p_passid)
	p_rendergraph:remove_pass_viewportquad(p_passid)
end

-- ready-to-use materials

commons.unlit_texture_material =
{
	color_source = { r = 1.0, g = 1.0, b = 1.0, a = 1.0 },
	simple_color = { r = 0.0, g = 0.0, b = 0.0, a = 0.0 },
	light_absorption = { r = 1.0, g = 1.0, b = 1.0, a = 1.0 },
	self_emissive = { r = 1.0, g = 1.0, b = 1.0, a = 1.0 },
}

commons.unlit_color_material =
{
	color_source = { r = 0.0, g = 0.0, b = 0.0, a = 0.0 },
	simple_color = { r = 0.0, g = 1.0, b = 0.0, a = 0.0 },
	light_absorption = { r = 1.0, g = 1.0, b = 1.0, a = 1.0 },
	self_emissive = { r = 1.0, g = 1.0, b = 1.0, a = 1.0 },
}


