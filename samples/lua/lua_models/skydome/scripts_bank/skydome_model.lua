skydome = {}

skydome.view = {}

-- stockage des instances modeles : paire {entity, renderer}
skydome.models = {}

skydome.rendering_config = 
{ 
	main_rendering =
	{
		fx = 
		{
			shaders = 
			{
				{ path='skydome_vs.hlsl',mode=SHADER_NOT_COMPILED },
				{ path='skydome_ps.hlsl',mode=SHADER_NOT_COMPILED }
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
		rendering_order = 22,
		shaders_params = 
		{ 
			{ param_name = "flags",                    shader_index = 0, register = 24 },
			{ param_name = "reflector_pos",            shader_index = 0, register = 25 },
			{ param_name = "reflector_dir",            shader_index = 0, register = 26 },
			{ param_name = "v_light0_dir",             shader_index = 1, register = 10 },
			{ param_name = "v_atmo_scattering_flag_0", shader_index = 1, register = 18 },
			{ param_name = "v_atmo_scattering_flag_1", shader_index = 1, register = 19 },
			{ param_name = "v_atmo_scattering_flag_2", shader_index = 1, register = 20 },
			{ param_name = "v_atmo_scattering_flag_3", shader_index = 1, register = 21 },
			{ param_name = "v_atmo_scattering_flag_4", shader_index = 1, register = 22 },
		}	
	},
	meshes_loader_params =
	{
		normale_generation_mode = NORMALES_AUTO_SMOOTH,
		tb_generation_mode = TB_DISCARDED
	},
	meshes_loader_params =
	{
		normale_generation_mode = NORMALES_AUTO_SMOOTH,
		tb_generation_mode = TB_DISCARDED
	},
	mirror_rendering =
	{
		fx = 
		{
			shaders = 
			{
				{ path='skydome_vs.hlsl',mode=SHADER_NOT_COMPILED },
				{ path='skydome_ps.hlsl',mode=SHADER_NOT_COMPILED }
			},
			rs_in = 
			{
				{ ope=RENDERSTATE_OPE_ENABLEZBUFFER, value="false" },
				{ ope=RENDERSTATE_OPE_ALPHABLENDENABLE, value="true" },
				{ ope=RENDERSTATE_OPE_ALPHABLENDOP, value="add" },
				{ ope=RENDERSTATE_OPE_ALPHABLENDFUNC, value="always" },
				{ ope=RENDERSTATE_OPE_ALPHABLENDDEST, value="invsrcalpha" },
				{ ope=RENDERSTATE_OPE_ALPHABLENDSRC, value="srcalpha" },
				{ ope=RENDERSTATE_OPE_SETCULLING, value="none" }
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
		rendering_order = 22,
		shaders_params = 
		{ 
			{ param_name = "flags",                    shader_index = 0, register = 24 },
			{ param_name = "reflector_pos",            shader_index = 0, register = 25 },
			{ param_name = "reflector_dir",            shader_index = 0, register = 26 },
			{ param_name = "v_light0_dir",             shader_index = 1, register = 10 },
			{ param_name = "v_atmo_scattering_flag_0", shader_index = 1, register = 18 },
			{ param_name = "v_atmo_scattering_flag_1", shader_index = 1, register = 19 },
			{ param_name = "v_atmo_scattering_flag_2", shader_index = 1, register = 20 },
			{ param_name = "v_atmo_scattering_flag_3", shader_index = 1, register = 21 },
			{ param_name = "v_atmo_scattering_flag_4", shader_index = 1, register = 22 },
		}	
	},
	meshes_loader_params =
	{
		normale_generation_mode = NORMALES_AUTO_SMOOTH,
		tb_generation_mode = TB_DISCARDED
	},
	meshes_loader_params =
	{
		normale_generation_mode = NORMALES_AUTO_SMOOTH,
		tb_generation_mode = TB_DISCARDED
	}
}

skydome.skyfromspace_ESun = 8.7
skydome.skyfromatmo_ESun = 70.0
skydome.groundfromspace_ESun = 24.0
skydome.groundfromatmo_ESun = 12.0

skydome.atmoThickness = 1600.0
skydome.outerRadius = 70000.0
skydome.innerRadius = skydome.outerRadius - skydome.atmoThickness

skydome.waveLength_x = 0.650;
skydome.waveLength_y = 0.570;
skydome.waveLength_z = 0.475;	
skydome.km = 0.0010;
skydome.kr = 0.0033;
skydome.scaleDepth = 0.25;

skydome.update_from_scene_env = function( p_pass_id, p_environment_table, p_entity_id )

    local renderer = skydome.models[p_entity_id]['renderer']

	renderer:set_shaderrealvector( p_pass_id, 'flags', 0, 0, 0, 0 )

	renderer:set_shaderrealvector( p_pass_id, 'v_light0_dir', p_environment_table.light0.direction.x, p_environment_table.light0.direction.y, p_environment_table.light0.direction.z, p_environment_table.light0.direction.w )

    renderer:set_shaderrealvector( p_pass_id, 'v_atmo_scattering_flag_0', skydome.outerRadius, skydome.innerRadius, skydome.outerRadius*skydome.outerRadius, skydome.innerRadius*skydome.innerRadius )
	renderer:set_shaderrealvector( p_pass_id, 'v_atmo_scattering_flag_1', skydome.scaleDepth, 1.0/skydome.scaleDepth, 1.0 / (skydome.outerRadius - skydome.innerRadius), (1.0 / (skydome.outerRadius - skydome.innerRadius) ) / skydome.scaleDepth  )
	renderer:set_shaderrealvector( p_pass_id, 'v_atmo_scattering_flag_2', 1.0 / g:pow(skydome.waveLength_x, 4.0), 1.0 / g:pow(skydome.waveLength_y, 4.0), 1.0 / g:pow(skydome.waveLength_z, 4.0), 0 )
	renderer:set_shaderrealvector( p_pass_id, 'v_atmo_scattering_flag_3', skydome.kr, skydome.km, 4.0 * skydome.kr * PI, 4.0 * skydome.km * PI )
	renderer:set_shaderrealvector( p_pass_id, 'v_atmo_scattering_flag_4', skydome.skyfromspace_ESun, skydome.skyfromatmo_ESun, skydome.groundfromspace_ESun, skydome.groundfromatmo_ESun )
end

skydome.update_from_scene_env_mirror = function( p_pass_id, p_environment_table, p_entity_id )

    local renderer = skydome.models[p_entity_id]['renderer']

	renderer:set_shaderrealvector( p_pass_id, 'flags', 1, 0, 0, 0 )
	renderer:set_shaderrealvector( p_pass_id, 'reflector_pos', p_environment_table.reflector_pos.x, p_environment_table.reflector_pos.y, p_environment_table.reflector_pos.z, 1.0 )
	renderer:set_shaderrealvector( p_pass_id, 'reflector_dir', p_environment_table.reflector_normale.x, p_environment_table.reflector_normale.y, p_environment_table.reflector_normale.z, 1.0 )

	renderer:set_shaderrealvector( p_pass_id, 'v_light0_dir', p_environment_table.light0.direction.x, p_environment_table.light0.direction.y, p_environment_table.light0.direction.z, p_environment_table.light0.direction.w )

    renderer:set_shaderrealvector( p_pass_id, 'v_atmo_scattering_flag_0', skydome.outerRadius, skydome.innerRadius, skydome.outerRadius*skydome.outerRadius, skydome.innerRadius*skydome.innerRadius )
	renderer:set_shaderrealvector( p_pass_id, 'v_atmo_scattering_flag_1', skydome.scaleDepth, 1.0/skydome.scaleDepth, 1.0 / (skydome.outerRadius - skydome.innerRadius), (1.0 / (skydome.outerRadius - skydome.innerRadius) ) / skydome.scaleDepth  )
	renderer:set_shaderrealvector( p_pass_id, 'v_atmo_scattering_flag_2', 1.0 / g:pow(skydome.waveLength_x, 4.0), 1.0 / g:pow(skydome.waveLength_y, 4.0), 1.0 / g:pow(skydome.waveLength_z, 4.0), 0 )
	renderer:set_shaderrealvector( p_pass_id, 'v_atmo_scattering_flag_3', skydome.kr, skydome.km, 4.0 * skydome.kr * PI, 4.0 * skydome.km * PI )
	renderer:set_shaderrealvector( p_pass_id, 'v_atmo_scattering_flag_4', skydome.skyfromspace_ESun, skydome.skyfromatmo_ESun, skydome.groundfromspace_ESun, skydome.groundfromatmo_ESun )
end

skydome.createmodelview = function(p_rendergraph, p_entity_id, p_passes_bindings)
 
  local entity
  local renderer

  entity, renderer = commons.create_rendered_meshe(skydome.rendering_config, 'skydome.ac', 'sphere', p_passes_bindings)
  renderer:register_to_rendering(p_rendergraph)
  
 
  --p_entitygraph:add_child(p_parent_entity_id,p_entity_id,entity)

  local pair = { ['entity'] = entity, ['renderer'] = renderer }

  skydome.models[p_entity_id] = pair

  return entity
end

skydome.trashmodelview = function(p_rendergraph, p_entitygraph, p_entity_id)

  local entity = skydome.models[p_entity_id]['entity']
  local renderer = skydome.models[p_entity_id]['renderer']


  commons.trash.meshe(p_rendergraph, entity, renderer)
  p_entitygraph:remove(p_entity_id)

  local pair = skydome.models[p_entity_id]
  pair['entity'] = nil
  pair['renderer'] = nil

  skydome.models[p_entity_id] = nil
end

skydome.view.unload = function(p_entity_id)
 
  local found_id = FALSE
  for k, v in pairs(skydome.models) do

    if k == p_entity_id then
	  found_id = TRUE
	end
  end

  if found_id == TRUE then
     model.view.unload(skydome.trashmodelview,p_entity_id)
  else
    g:print('Unknown entity '..p_entity_id)
  end
end


skydome.scale = 
{
	x = skydome.outerRadius, y = skydome.outerRadius, z = skydome.outerRadius
}

skydome.view.load = function(p_entity_id, p_passes_bindings)

  local found_id = FALSE
  for k, v in pairs(skydome.models) do

    if k == p_entity_id then
	  found_id = TRUE
	end
  end

  if found_id == TRUE then
    g:print('Entity '..p_entity_id..' already exists')
  else
    model.view.load('skydome model', skydome.createmodelview, p_passes_bindings, nil, p_entity_id)
  end
end