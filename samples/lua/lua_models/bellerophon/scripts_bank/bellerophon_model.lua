
bellerophon = {}

bellerophon.view = {}

-- stockage des instances modeles : paire {entity, renderer}
bellerophon.models = {}

bellerophon.rendering_config = 
{ 
    lit_rendering = 
	{
		fx = 
		{
			shaders = 
			{
				{ path='lit_vs.hlsl',mode=SHADER_NOT_COMPILED },
				{ path='lit_ps.hlsl',mode=SHADER_NOT_COMPILED }
			},
			rs_in = 
			{
				{ ope=RENDERSTATE_OPE_ENABLEZBUFFER, value="true" }
			},
			rs_out =
			{
				{ ope=RENDERSTATE_OPE_ENABLEZBUFFER, value="false" }
			}
		},
		textures =
		{
			[1] = 
			{
				{ path='bellerophon.jpg', stage=0},
				{ path='bellerophon.jpg', stage=1}		
			}
		},
		vertex_textures =
		{
		},
		rendering_order = 10000,
		shaders_params = 
		{ 
			{ param_name = "lights_enabled_v", shader_index = 0, register = 24 },
			{ param_name = "light0_dir_v", shader_index = 0, register = 25 },
			{ param_name = "flags_v", shader_index = 0, register = 26 },
			{ param_name = "reflectorPos", shader_index = 0, register = 27 },
			{ param_name = "reflectorNormale", shader_index = 0, register = 28 },
			{ param_name = "ambient_color", shader_index = 1, register = 0 },
			{ param_name = "lights_enabled", shader_index = 1, register = 1 },
			{ param_name = "light0_color", shader_index = 1, register = 2 },
			{ param_name = "light0_dir", shader_index = 1, register = 3 },
			{ param_name = "flags", shader_index = 1, register = 7 },
			{ param_name = "self_emissive", shader_index = 1, register = 8 },
			{ param_name = "absorption", shader_index = 1, register = 9 },
			{ param_name = "color", shader_index = 1, register = 10 },
			{ param_name = "color_source", shader_index = 1, register = 11 },
			{ param_name = "fog_color", shader_index = 1, register = 12 },
			{ param_name = "flags2", shader_index = 1, register = 13 }
		}
	},
	color_rendering = 
	{
		fx = 
		{
			shaders = 
			{
				{ path='color_vs.hlsl',mode=SHADER_NOT_COMPILED },
				{ path='color_ps.hlsl',mode=SHADER_NOT_COMPILED }
			},
			rs_in = 
			{
				{ ope=RENDERSTATE_OPE_ENABLEZBUFFER, value="true" }
			},
			rs_out =
			{
				{ ope=RENDERSTATE_OPE_ENABLEZBUFFER, value="false" }
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
			{ param_name = "color", shader_index = 1, register = 0 }
		}
	},
	meshes_loader_params =
	{
		normale_generation_mode = NORMALES_AUTO_SMOOTH,
		tb_generation_mode = TB_AUTO
	}
}

bellerophon.lit_material =
{
	--specular_power = 500.0,
	color_source = { r = 1.0, g = 1.0, b = 1.0, a = 1.0 },
	simple_color = { r = 0.0, g = 0.0, b = 0.0, a = 0.0 },
	light_absorption = { r = 0.0, g = 0.0, b = 0.0, a = 0.0 },
	self_emissive = { r = 0.0, g = 0.0, b = 0.0, a = 0.0 },
	bump_mapping = { texture_size = 1024, bias = 0.045 }
}


bellerophon.update_lit_from_scene_env = function( p_pass_id, p_environment_table, p_entity_id )

    local renderer = bellerophon.models[p_entity_id]['renderer']

	renderer:set_shaderrealvector( p_pass_id, 'ambient_color', p_environment_table.ambient_light.r, p_environment_table.ambient_light.g, p_environment_table.ambient_light.b, p_environment_table.ambient_light.a )
	renderer:set_shaderrealvector( p_pass_id, 'lights_enabled', p_environment_table.lights_enabled.x, p_environment_table.lights_enabled.y, p_environment_table.lights_enabled.z, p_environment_table.lights_enabled.w )
	renderer:set_shaderrealvector( p_pass_id, 'light0_color', p_environment_table.light0.color.r, p_environment_table.light0.color.g, p_environment_table.light0.color.b, p_environment_table.light0.color.a )
	renderer:set_shaderrealvector( p_pass_id, 'light0_dir', p_environment_table.light0.direction.x, p_environment_table.light0.direction.y, p_environment_table.light0.direction.z, p_environment_table.light0.direction.w )

	renderer:set_shaderrealvector( p_pass_id, 'light0_dir_v', p_environment_table.light0.direction.x, p_environment_table.light0.direction.y, p_environment_table.light0.direction.z, p_environment_table.light0.direction.w )
	renderer:set_shaderrealvector( p_pass_id, 'lights_enabled_v', p_environment_table.lights_enabled.x, p_environment_table.lights_enabled.y, p_environment_table.lights_enabled.z, p_environment_table.lights_enabled.w )
	
	renderer:set_shaderrealinvector( p_pass_id, 'flags_v', 0, 0)
	renderer:set_shaderrealinvector( p_pass_id, 'flags_v', 1, p_environment_table.fog_intensity)
	
	renderer:set_shaderrealvector( p_pass_id, 'fog_color', p_environment_table.fog_color.r, p_environment_table.fog_color.g, p_environment_table.fog_color.b, 1.0 )
		
	renderer:set_shaderrealvector( p_pass_id, 'reflectorPos', p_environment_table.reflector_pos.x, p_environment_table.reflector_pos.y, p_environment_table.reflector_pos.z, 1.0 )
	renderer:set_shaderrealvector( p_pass_id, 'reflectorNormale', p_environment_table.reflector_normale.x, p_environment_table.reflector_normale.y, p_environment_table.reflector_normale.z, 1.0 )

	commons.apply_material( bellerophon.lit_material, renderer, p_pass_id)

	-- flag for bump mapping normales inputs
	renderer:set_shaderrealvector( p_pass_id, 'flags2', 0.0, 0.0, 0.0, 0.0 )

end



bellerophon.createlitmodelview = function(p_rendergraph, p_entity_id, p_body_attitude_and_pos_mat, p_passes_bindings)
  
  local entity
  local renderer

  entity, renderer = commons.create_rendered_meshe(bellerophon.rendering_config, 'bellerophon.ac', 'wavefront obj', p_passes_bindings)
  renderer:register_to_rendering(p_rendergraph)
  
  local rigibody_transform = RigdBodyTransform()

  rigibody_transform:attach_toentity(entity,0)
  rigibody_transform:configure_shape(SHAPE_BOX, 50.0, 28.0, 68.0)
  rigibody_transform:configure_attitude(p_body_attitude_and_pos_mat)
   
  local pair = { ['entity'] = entity, ['renderer'] = renderer, ['rigibody_transform'] = rigibody_transform }

  bellerophon.models[p_entity_id] = pair

  return entity
end

bellerophon.trashmodelview = function(p_rendergraph, p_entitygraph, p_entity_id)

  local entity = bellerophon.models[p_entity_id]['entity']
  local renderer = bellerophon.models[p_entity_id]['renderer']
  local rigibody_transform = bellerophon.models[p_entity_id]['rigibody_transform']

  rigibody_transform:release()

  commons.trash.meshe(p_rendergraph, entity, renderer)
  p_entitygraph:remove(p_entity_id)

  local pair = bellerophon.models[p_entity_id]
  pair['entity'] = nil
  pair['renderer'] = nil
  pair['rigibody_transform'] = nil

  bellerophon.models[p_entity_id] = nil
end

bellerophon.view.unload = function(p_entity_id)
 
  local found_id = FALSE
  for k, v in pairs(bellerophon.models) do

    if k == p_entity_id then
	  found_id = TRUE
	end
  end

  if found_id == TRUE then
     model.view.unload(bellerophon.trashmodelview,p_entity_id)
  else
    g:print('Unknown entity '..p_entity_id)
  end
end

bellerophon.view.load = function(p_entity_id, p_attitude_and_pos_mat, p_passes_bindings)

  local found_id = FALSE
  for k, v in pairs(bellerophon.models) do

    if k == p_entity_id then
	  found_id = TRUE
	end
  end

  if found_id == TRUE then
    g:print('Entity '..p_entity_id..' already exists')
  else
    model.view.loadbody('bellerophon model', bellerophon.createlitmodelview, p_passes_bindings, nil, p_entity_id, p_attitude_and_pos_mat)
  end  
end