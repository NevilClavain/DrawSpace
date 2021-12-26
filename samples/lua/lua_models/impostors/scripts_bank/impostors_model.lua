
impostors = {}
impostors.view = {}

-- stockage des instances modeles : paire {entity, renderer}
impostors.models = {}


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

impostors.create_rendered_impostors = function(p_config, rendering_passes_array)

  local entity=Entity()
  entity:add_aspect(RENDERING_ASPECT)
  entity:add_aspect(RESOURCES_ASPECT)

  
  local impostors_renderer=Impostors()
  impostors_renderer:attach_toentity(entity)
  
  local renderconfig=RenderConfig()

  -- TO BE CONTINUED

  return entity, impostors_renderer
end

impostors.trash_impostors = function(p_rendergraph, p_entity, p_renderer)

	p_renderer:detach_fromentity()

	p_entity:remove_aspect(RENDERING_ASPECT)
	p_entity:remove_aspect(RESOURCES_ASPECT)

end


impostors.createmodelview = function(p_rendergraph, p_entity_id, p_passes_bindings)

  local entity
  local renderer

  entity, renderer = impostors.create_rendered_impostors(impostors.rendering_config, p_passes_bindings)

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

impostors.view.load = function(p_entity_id, p_descriptor, p_passes_bindings)

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
