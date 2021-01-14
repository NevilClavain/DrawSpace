
model.anims = {}

animations_rand_engine=RandomEngine()
animations_rand_engine:set_seedfromtime()


model.anims.compute_random_anim_index = function(p_do_something_generator, p_action_generator, p_random_engine, p_rand_anims, p_main_idle_anim)
  
   local index = p_do_something_generator:generate(p_random_engine)

   if index == 2 then
      local action_index = p_action_generator:generate(p_random_engine)
	  return p_rand_anims[action_index]
   else

      return p_main_idle_anim 
   end   
end

model.anims.pushanim = function(p_entity_id, p_index)
  local entity_properties_entry = model.entities[p_entity_id]
  local entity = entity_properties_entry['entity']
  local animations_names = {entity:read_animationsnames()}
  entity:push_animation(animations_names[p_index])
end

model.anims.run = function(p_index)
  model.anims.pushanim(model.target, p_index)
end

model.anims.runloop = function( p_index)
  model.entities[model.target]['current_animation_loop'] = p_index
  model.anims.pushanim(model.target, p_index)
end

model.anims.rand = function()

  model.entities[model.target]['rand_anim_mode'] = TRUE

  local random_anims = model.entities[model.target]['rand_anims']
  local idle_anim = model.entities[model.target]['main_idle_anim']
  local do_something_distribution = model.entities[model.target]['do_something_distribution']
  local anim_action_distribution = model.entities[model.target]['anim_action_distribution']

  local selected_index = model.anims.compute_random_anim_index(do_something_distribution, anim_action_distribution, animations_rand_engine,random_anims,idle_anim)
  
  model.entities[model.target]['current_animation_loop'] = selected_index
  model.anims.pushanim(model.target, selected_index)
end

model.anims.stop = function()
    model.entities[model.target]['rand_anim_mode'] = FALSE
	model.entities[model.target]['current_animation_loop'] = -1
end

model.anims.dump = function(entity)

  local text = ""

  --[[
  local entity_properties_entry = model.entities[ent]
  local entity = entity_properties_entry['entity']
  ]]

  animations_names = {entity:read_animationsnames()}

  for k, v in pairs(animations_names) do
    text = text..'animation '..k..' ->'..v.."\n"
  end

  return text
end

