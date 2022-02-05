
spaceboxmod = {}
spaceboxmod.view = {}

spaceboxmod.module = Module("skyboxmod", "spacebox")
spaceboxmod.module:load()
g:print(spaceboxmod.module:get_descr().. ' loaded')

-- stockage des instances modeles : paire {entity, renderer}
spaceboxmod.models = {}

spaceboxmod.requested_rendering_layers = nil





spaceboxmod.create_rendered_spacebox = function(p_layers, p_rendering_passes_array)

  local entity=Entity()
  entity:add_aspect(RENDERING_ASPECT)
  entity:add_aspect(RESOURCES_ASPECT)


  local renderer=SkyboxRendering()
  renderer:attach_toentity(entity)

  local renderlayer=RenderLayer()

  for k, v in pairs(p_rendering_passes_array) do

	local layer_entry = v

	local pass_id = layer_entry.target_pass_id
	local rendering_id = layer_entry.rendering_id

	renderer:set_passforrenderid(rendering_id, pass_id) -- to be continued
  end

  for k0, v0 in pairs(p_layers) do
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

	renderlayer:add_renderconfig(renderconfig, k0)
  end


  renderer:configure(renderlayer)

  return entity, renderer
end



spaceboxmod.createmodelview = function(p_rendergraph, p_entity_id, p_passes_bindings)

  local entity
  local renderer

  --entity, renderer=commons.create_rendering_from_module(spaceboxmod.requested_rendering_layers, spaceboxmod.module, "skyboxRender", p_passes_bindings)
  entity, renderer=spaceboxmod.create_rendered_spacebox(spaceboxmod.requested_rendering_layers, p_passes_bindings)

  renderer:register_to_rendering(p_rendergraph)


  entity:add_aspect(TRANSFORM_ASPECT)

  local pair = { ['entity'] = entity, ['renderer'] = renderer }
  spaceboxmod.models[p_entity_id] = pair

  return entity
  
end

spaceboxmod.trashmodelview = function(p_rendergraph, p_entitygraph, p_entity_id)

  local entity = spaceboxmod.models[p_entity_id]['entity']
  local renderer = spaceboxmod.models[p_entity_id]['renderer']

  entity:remove_aspect(TRANSFORM_ASPECT)

  commons.trash.rendering(p_rendergraph, spaceboxmod.module, entity, renderer)
  p_entitygraph:remove(p_entity_id)

  local pair = spaceboxmod.models[p_entity_id]
  pair['entity'] = nil
  pair['renderer'] = nil

  spaceboxmod.models[p_entity_id] = nil

end


spaceboxmod.view.unload = function(p_entity_id)
 
  local found_id = FALSE
  for k, v in pairs(spaceboxmod.models) do

    if k == p_entity_id then
	  found_id = TRUE
	end
  end

  if found_id == TRUE then
     model.view.unload(spaceboxmod.trashmodelview, p_entity_id)
  else
    g:print('Unknown entity '..p_entity_id)
  end
end

spaceboxmod.view.load = function(p_entity_id, p_passes_bindings, p_spacebox_layers)

  spaceboxmod.requested_rendering_layers = p_spacebox_layers

  local found_id = FALSE
  for k, v in pairs(spaceboxmod.models) do

    if k == p_entity_id then
	  found_id = TRUE
	end
  end

  if found_id == TRUE then
    g:print('Entity '..p_entity_id..' already exists')
  else
    model.view.load('spacebox model', spaceboxmod.createmodelview, p_passes_bindings, nil, p_entity_id)
  end  
end
