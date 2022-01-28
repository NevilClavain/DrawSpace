
spaceboxmod = {}
spaceboxmod.view = {}

spaceboxmod.module = Module("skyboxmod", "spacebox")
spaceboxmod.module:load()
g:print(spaceboxmod.module:get_descr().. ' loaded')

-- stockage des instances modeles : paire {entity, renderer}
spaceboxmod.models = {}

spaceboxmod.requested_rendering_layers = nil

spaceboxmod.createmodelview = function(p_rendergraph, p_entity_id, p_passes_bindings)

  local entity
  local renderer

  entity, renderer=commons.create_rendering_from_module(spaceboxmod.requested_rendering_layers, spaceboxmod.module, "skyboxRender", p_passes_bindings)
  renderer:register_to_rendering(p_rendergraph)


  --p_entitygraph:add_child(p_parent_entity_id,p_entity_id,entity)

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
