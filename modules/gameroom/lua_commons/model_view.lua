
model.view = {}

model.view.load = function(p_modelname, p_modelviewload_function, p_passes_bindings, p_anims_parameters, p_entity_id, p_specific_config)

  local entity
  if p_specific_config == nil then 
    entity = p_modelviewload_function(rg, p_entity_id, p_passes_bindings)
  else
    entity = p_modelviewload_function(rg, p_entity_id, p_passes_bindings, p_specific_config)
  end


  -- loop pour execution lit_shader_update_func de chaque entree de p_passes_bindings
  for k, v in pairs(p_passes_bindings) do
    local pass_entry = v
    --pass_id = k
    local pass_id = pass_entry.target_pass_id
    if pass_entry.lit_shader_update_func ~= nil then
      pass_entry.lit_shader_update_func( pass_id, environment, p_entity_id)
    end
  end
  
  local passes_shaders_update_func = {}
  for k, v in pairs(p_passes_bindings) do
    --local pass_id = k

    local pass_entry = v
    local pass_id = pass_entry.target_pass_id
    passes_shaders_update_func[pass_id] = pass_entry.lit_shader_update_func
  end

  local entity_properties_entry =
  {
    ['entity'] = entity,
	['current_animation_loop'] = -1,
	['rand_anim_mode'] = FALSE,
	['passes_shaders_update_func'] = passes_shaders_update_func,
	['model_classname'] = p_modelname
  }

  model.entities[p_entity_id] = entity_properties_entry

  if p_anims_parameters ~= nil then

    local random_anims 
    local idle_anim
    local do_something_distribution
    local anim_action_distribution

	random_anims, idle_anim, do_something_distribution, anim_action_distribution = p_anims_parameters()

	model.entities[p_entity_id]['rand_anims'] = random_anims
	model.entities[p_entity_id]['main_idle_anim'] = idle_anim
	model.entities[p_entity_id]['do_something_distribution'] = do_something_distribution
	model.entities[p_entity_id]['anim_action_distribution'] = anim_action_distribution
    
  end

end

model.view.loadbody = function(p_modelname, p_modelviewload_function, p_passes_bindings, p_anims_parameters, p_entity_id, p_body_attitude_and_pos_mat)

  local entity = p_modelviewload_function(rg, p_entity_id, p_body_attitude_and_pos_mat, p_passes_bindings)

  -- loop pour execution lit_shader_update_func de chaque entree de p_passes_bindings
  for k, v in pairs(p_passes_bindings) do
    local pass_entry = v

    local pass_id = pass_entry.target_pass_id
    if pass_entry.lit_shader_update_func ~= nil then
      pass_entry.lit_shader_update_func( pass_id, environment, p_entity_id)
    end
  end

  local passes_shaders_update_func = {}
  for k, v in pairs(p_passes_bindings) do

    local pass_entry = v
    local pass_id = pass_entry.target_pass_id
    passes_shaders_update_func[pass_id] = pass_entry.lit_shader_update_func
  end

  local entity_properties_entry =
  {
    ['entity'] = entity,
	['current_animation_loop'] = -1,
	['rand_anim_mode'] = FALSE,
	['passes_shaders_update_func'] = passes_shaders_update_func,
	['model_classname'] = p_modelname
  }

  model.entities[p_entity_id] = entity_properties_entry

  if p_anims_parameters ~= nil then

    local random_anims 
    local idle_anim
    local do_something_distribution
    local anim_action_distribution

	random_anims, idle_anim, do_something_distribution, anim_action_distribution = p_anims_parameters()

	model.entities[p_entity_id]['rand_anims'] = random_anims
	model.entities[p_entity_id]['main_idle_anim'] = idle_anim
	model.entities[p_entity_id]['do_something_distribution'] = do_something_distribution
	model.entities[p_entity_id]['anim_action_distribution'] = anim_action_distribution
    
  end
end


model.view.unload = function(p_modelunload_function,p_entity_id)

  if model.transformations[p_entity_id] ~= nil then

    local transform_entry = model.transformations[p_entity_id]

    local transform = transform_entry['transform']
    transform:release()

    transform_entry['pos_mat'] = nil
    transform_entry['scale_mat'] = nil
    transform_entry['rotx_mat'] = nil
    transform_entry['roty_mat'] = nil
    transform_entry['rotz_mat'] = nil

    transform_entry['transform'] = nil

    model.transformations[p_entity_id] = nil
  end

  p_modelunload_function(rg, eg, p_entity_id)

  local entity_properties_entry = model.entities[p_entity_id]
  entity_properties_entry['entity'] = nil

  model.entities[p_entity_id] = nil

end