
waterquad = {}

waterquad.view = {}

-- stockage des instances modeles : paire {entity, renderer, body}
waterquad.models = {}


waterquad.rendering_config = 
{ 
	main_rendering = 
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
				{ ope=RENDERSTATE_OPE_ENABLEZBUFFER, value="true"	}		
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
    bump_rendering = 
	{
		fx = 
		{
			shaders = 
			{
				{ path='water_bump_vs.hlsl',mode=SHADER_NOT_COMPILED },
				{ path='water_bump_ps.hlsl',mode=SHADER_NOT_COMPILED }
			},
			rs_in = 
			{
				{ ope=RENDERSTATE_OPE_ENABLEZBUFFER, value="true"	}		
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
			{ param_name = 'bump_bias', shader_index = 1, register = 0 }
		}
	},
	meshes_loader_params =
	{
		normale_generation_mode = NORMALES_AUTO,
		tb_generation_mode = TB_AUTO
	}
}

waterquad.update_from_scene_env = function( p_pass_id, p_environment_table, p_entity_id )

  local renderer = waterquad.models[p_entity_id]['renderer']
  renderer:set_shaderrealvector( p_pass_id, 'color', 1.0, 0.0, 1.0, 1.0 )

end

waterquad.update_from_scene_env_bump = function( p_pass_id, p_environment_table, p_entity_id )

  local renderer = waterquad.models[p_entity_id]['renderer']
  renderer:set_shaderrealvector( p_pass_id, 'bump_bias', 1.0, 0.0, 0.0, 1.0 )

end


waterquad.createmodelview = function(p_rendergraph, p_entitygraph, p_entity_id, p_passes_bindings, p_parent_entity_id)
  
  local entity
  local renderer

  entity, renderer = commons.create_rendered_meshe(waterquad.rendering_config, 'water.ac', 'my_flat_mesh', p_passes_bindings)
  renderer:register_to_rendering(p_rendergraph)


  p_entitygraph:add_child(p_parent_entity_id,p_entity_id,entity)

  local pair = {}
  pair['entity'] = entity
  pair['renderer'] = renderer

  waterquad.models[p_entity_id] = pair

  return entity
end

waterquad.trashmodelview = function(p_rendergraph, p_entitygraph, p_entity_id)

  local entity = waterquad.models[p_entity_id]['entity']
  local renderer = waterquad.models[p_entity_id]['renderer']


  commons.trash.meshe(p_rendergraph, entity, renderer)
  p_entitygraph:remove(p_entity_id)

  local pair = waterquad.models[p_entity_id]
  pair['entity'] = nil
  pair['renderer'] = nil

  waterquad.models[p_entity_id] = nil
end

waterquad.models.dump = function()
  for k, v in pairs(waterquad.models) do
    g:print("waterquad entity instance -> "..k)
  end  
end

waterquad.view.unload = function(p_entity_id)
 
  found_id = FALSE
  for k, v in pairs(waterquad.models) do

    if k == p_entity_id then
	  found_id = TRUE
	end
  end

  if found_id == TRUE then
     model.view.unload(waterquad.trashmodelview,p_entity_id)
  else
    g:print('Unknown entity '..p_entity_id)
  end
end

waterquad.view.load = function(p_entity_id, p_passes_config, p_parent_entity_id)

  found_id = FALSE
  for k, v in pairs(waterquad.models) do

    if k == p_entity_id then
	  found_id = TRUE
	end
  end

  if found_id == TRUE then
    g:print('Entity '..p_entity_id..' already exists')
  else
    model.view.load('waterquad model', waterquad.createmodelview, p_passes_config, nil, nil, p_entity_id, p_parent_entity_id)
  end  
end