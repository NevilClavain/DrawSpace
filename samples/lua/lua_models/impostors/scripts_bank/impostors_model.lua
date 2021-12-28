
impostors = {}
impostors.view = {}

-- stockage des instances modeles : paire {entity, renderer}
impostors.models = {}

impostors.requested_ro = 1000
impostors.requested_texture = ""

impostors.world = {}

impostors.world.rendering_config =
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
		}
	}
}

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

	local textures = TexturesSet()
	textures:set_texturefiletostage(impostors.requested_texture, 0)
	rendercontext:add_texturesset(textures)

	--g:print('Impostors : requested rendering order = '..impostors.requested_ro)
	rendercontext:set_renderingorder(impostors.requested_ro)

	renderconfig:add_rendercontext(rendercontext)

  end

  --renderer:configure(renderconfig)

  return entity, renderer
end

impostors.trash_impostors = function(p_rendergraph, p_entity, p_renderer)

    p_renderer:unregister_from_rendering(p_rendergraph)
	p_renderer:detach_fromentity()

	p_entity:remove_aspect(RENDERING_ASPECT)
	p_entity:remove_aspect(RESOURCES_ASPECT)

end


impostors.createmodelview = function(p_rendergraph, p_entity_id, p_passes_bindings)

  local entity
  local renderer

  entity, renderer = impostors.create_rendered_impostors(impostors.world.rendering_config, p_passes_bindings)
  renderer:register_to_rendering(p_rendergraph)


  local pair = {}
  pair['entity'] = entity
  pair['renderer'] = renderer

  impostors.models[p_entity_id] = pair
  
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

impostors.view.load = function(p_entity_id, p_descriptor, p_passes_bindings, p_ro, p_texture)

  impostors.requested_ro = p_ro
  impostors.requested_texture = p_texture

  local found_id = FALSE
  for k, v in pairs(impostors.models) do

    if k == p_entity_id then
	  found_id = TRUE
	end
  end

  if found_id == TRUE then
    g:print('Entity '..p_entity_id..' already exists')
  else
    model.view.load('impostors model', impostors.createmodelview, p_passes_bindings, nil, p_entity_id)
  end  
end
