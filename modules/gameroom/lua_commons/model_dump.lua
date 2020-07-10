
model.dump = {}


model.dump.load = function(entity_name, model_file_path)

  local meshe_entity=Entity()

  meshe_entity:add_aspect(RESOURCES_ASPECT)
  meshe_entity:configure_mesheresource(model_file_path)
  
  eg:add_child('root', entity_name, meshe_entity)
  return meshe_entity
end

model.dump.unload = function(meshe_entity)

  local entity_name = eg:find_entityname(meshe_entity)
  meshe_entity:release_mesheresource()
  meshe_entity:remove_aspect(RESOURCES_ASPECT)

  eg:remove(entity_name)
end

model.dump.show = function(entity)

  model.text = ""
  local list_size = entity:read_meshesfiledescriptionssize()   	
  for i = 1, list_size do
    local model_root_infos = { entity:read_meshesfiledescription(i, 'root') }
	model.text = model.text..i..' - model file = '..model_root_infos[1]..' num_meshes = '..model_root_infos[3]..' num_animations = '..model_root_infos[5].."\n"
	
	model.text = model.text..'Meshes : \n'

    local meshes_id_list = { entity:read_meshesfiledescription(i, 'meshes_list') }
	for k, v in pairs(meshes_id_list) do
	  model.text = model.text..k.. ' - ' ..v.."\n"

      local meshe_infos = { entity:read_meshesfiledescription(i, 'meshe', k) }
	  model.text = model.text..'node id = '..meshe_infos[1].. ' name = ['..meshe_infos[2]..']'.."\n"	 
	  model.text = model.text..'has positions = '..meshe_infos[3]..' has faces = '..meshe_infos[4]..' has normales = '..meshe_infos[5]..' has tbn = '..meshe_infos[6].."\n"
	  model.text = model.text..'num vertices = '..meshe_infos[7]..' num faces = '..meshe_infos[8]..' num uvchannels = '..meshe_infos[9]..' num bones = '..meshe_infos[10].."\n"
	end

	model.text = model.text..'Animations : \n'
    local anims_id_list = { entity:read_meshesfiledescription(i, 'anims_list') }
	for k, v in pairs(anims_id_list) do	 
      local anim_infos = { entity:read_meshesfiledescription(i, 'anim', k) }
	  model.text = model.text..k.. ' - ' ..v..' name = '..anim_infos[1]..' ticks/sec = '..anim_infos[2]..' duration = '..anim_infos[3]..'s number of channels = '..anim_infos[4].."\n"
	end
  end

  if model.dump.showcontentfunc ~= nil then
    model.dump.showcontentfunc()
  end

end