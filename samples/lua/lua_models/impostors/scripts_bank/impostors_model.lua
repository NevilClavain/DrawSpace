
impostors = {}
spaceimpostors = {}
screenimpostors = {}

impostors.view = {}
spaceimpostors.view = {}
screenimpostors.view = {}

-- stockage des instances modeles : paire {entity, renderer}
impostors.models = {}

impostors.requested_ro = 1000
impostors.requested_texture = ""
impostors.requested_descriptors = nil

impostors.space = {}

spaceimpostors.rendering_config =
{
	main_rendering =	
	{
		fx =
		{
			shaders = 
			{
				{ path='spaceimpostor_vs.hlsl',mode=SHADER_NOT_COMPILED },
				{ path='spaceimpostor_ps.hlsl',mode=SHADER_NOT_COMPILED }
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
		shaders_params = 
		{ 
			{ param_name = "vflags", shader_index = 0, register = 24 },
			{ param_name = "flags", shader_index = 1, register = 0 },
			{ param_name = "color", shader_index = 1, register = 1 },
		}
	}
}

screenimpostors.rendering_config =
{
	main_rendering =	
	{
		fx =
		{
			shaders = 
			{
				{ path='screenimpostor_vs.hlsl',mode=SHADER_NOT_COMPILED },
				{ path='screenimpostor_ps.hlsl',mode=SHADER_NOT_COMPILED }
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
		shaders_params = 
		{ 
			{ param_name = "scale", shader_index = 0, register = 24 },
			{ param_name = "flags", shader_index = 1, register = 0 },
			{ param_name = "color", shader_index = 1, register = 1 },
		}
	}
}

impostors.update_spaceimpostor_from_scene_env = function( p_pass_id, p_environment_table, p_entity_id )

  local renderer = impostors.models[p_entity_id]['renderer']
  renderer:set_shaderrealvector( p_pass_id, 'vflags', 0.0, 0.0, 0.0, 0.0 )
  renderer:set_shaderrealvector( p_pass_id, 'flags', 0.0, 0.0, 0.0, 0.0 )
  renderer:set_shaderrealvector( p_pass_id, 'color', 0.0, 0.0, 0.0, 0.0 )
end

impostors.update_screenimpostor_from_scene_env = function( p_pass_id, p_environment_table, p_entity_id )

  local renderer = impostors.models[p_entity_id]['renderer']
  renderer:set_shaderrealvector( p_pass_id, 'scale', 1.0, 1.0, 0.0, 0.0 )
  renderer:set_shaderrealvector( p_pass_id, 'flags', 0.0, 0.0, 0.0, 0.0 )
  renderer:set_shaderrealvector( p_pass_id, 'color', 1.0, 1.0, 1.0, 1.0 )
end

impostors.create_rendered_impostors = function(p_config, p_rendering_passes_array)

  local entity=Entity()
  entity:add_aspect(RENDERING_ASPECT)
  entity:add_aspect(RESOURCES_ASPECT)

  
  local renderer=Impostors()
  renderer:attach_toentity(entity)
  
  local renderconfig=RenderConfig()

  for k, v in pairs(p_rendering_passes_array) do

	local layer_entry = v

	local pass_id = layer_entry.target_pass_id
	local rendering_id = layer_entry.rendering_id

	--g:print("###### "..rendering_id..' '..pass_id)
	renderer:set_passforrenderid(rendering_id, pass_id)
  end


  for k, v in pairs(p_config) do   
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

	rendercontext:add_fxparams(fxparams)


	local shaderparams_config = v['shaders_params']
	for k2, v2 in pairs(shaderparams_config) do
	  local param = v2
	    --g:print(param['param_name']..'->'..param['shader_index']..','..param['register'])
        rendercontext:add_shaderparam(param['param_name'], param['shader_index'], param['register'])
	end

	local textures = TexturesSet()
	textures:set_texturefiletostage(impostors.requested_texture, 0)
	rendercontext:add_texturesset(textures)

	--g:print('Impostors : requested rendering order = '..impostors.requested_ro)
	rendercontext:set_renderingorder(impostors.requested_ro)

	renderconfig:add_rendercontext(rendercontext)

  end
  
  renderer:configure(renderconfig, impostors.requested_descriptors)

  return entity, renderer
end

impostors.trash_impostors = function(p_rendergraph, p_entity, p_renderer)

    p_renderer:unregister_from_rendering(p_rendergraph)
	p_renderer:release()
	p_renderer:detach_fromentity()

	p_entity:remove_aspect(RENDERING_ASPECT)
	p_entity:remove_aspect(RESOURCES_ASPECT)

end


spaceimpostors.createmodelview = function(p_rendergraph, p_entity_id, p_passes_bindings)

  local entity
  local renderer

  entity, renderer = impostors.create_rendered_impostors(spaceimpostors.rendering_config, p_passes_bindings)
  renderer:register_to_rendering(p_rendergraph)


  local pair = {}
  pair['entity'] = entity
  pair['renderer'] = renderer

  impostors.models[p_entity_id] = pair  

  return entity
end

screenimpostors.createmodelview = function(p_rendergraph, p_entity_id, p_passes_bindings)

  local entity
  local renderer

  entity, renderer = impostors.create_rendered_impostors(screenimpostors.rendering_config, p_passes_bindings)
  renderer:register_to_rendering(p_rendergraph)

  local pair = {}
  pair['entity'] = entity
  pair['renderer'] = renderer

  impostors.models[p_entity_id] = pair  

  return entity
end


impostors.trashmodelview = function(p_rendergraph, p_entitygraph, p_entity_id)

  local entity = impostors.models[p_entity_id]['entity']
  local renderer = impostors.models[p_entity_id]['renderer']
    
  impostors.trash_impostors(p_rendergraph, entity, renderer)

  local pair = impostors.models[p_entity_id]
  pair['entity'] = nil
  pair['renderer'] = nil

  impostors.models[p_entity_id] = nil

end


impostors.view.unload = function(p_entity_id)
 
  local found_id = FALSE
  for k, v in pairs(impostors.models) do

    if k == p_entity_id then
	  found_id = TRUE
	end
  end

  if found_id == TRUE then
     model.view.unload(impostors.trashmodelview, p_entity_id)
  else
    g:print('Unknown entity '..p_entity_id)
  end
end

spaceimpostors.view.load = function(p_entity_id, p_descriptors_array, p_passes_bindings, p_ro, p_texture)

  impostors.requested_ro = p_ro
  impostors.requested_texture = p_texture
  impostors.requested_descriptors = p_descriptors_array

  local found_id = FALSE
  for k, v in pairs(impostors.models) do

    if k == p_entity_id then
	  found_id = TRUE
	end
  end

  if found_id == TRUE then
    g:print('Entity '..p_entity_id..' already exists')
  else
    model.view.load('impostors model', spaceimpostors.createmodelview, p_passes_bindings, nil, p_entity_id)
  end  
end


screenimpostors.view.load = function(p_entity_id, p_descriptors_array, p_passes_bindings, p_ro, p_texture)

  impostors.requested_ro = p_ro
  impostors.requested_texture = p_texture
  impostors.requested_descriptors = p_descriptors_array

  local found_id = FALSE
  for k, v in pairs(impostors.models) do

    if k == p_entity_id then
	  found_id = TRUE
	end
  end

  if found_id == TRUE then
    g:print('Entity '..p_entity_id..' already exists')
  else
    model.view.load('impostors model', screenimpostors.createmodelview, p_passes_bindings, nil, p_entity_id)
  end  

end