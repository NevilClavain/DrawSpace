



model.dump = function(entity)

  text = ""
  local list_size = entity:read_meshesfiledescriptionssize()   	
  for i = 1, list_size do
    local model_root_infos = { entity:read_meshesfiledescription(i, 'root') }
	text = text..i..' - model file = '..model_root_infos[1]..' num_meshes = '..model_root_infos[3]..' num_animations = '..model_root_infos[5].."\n"
	
	text = text..'Meshes : \n'

    local meshes_id_list = { entity:read_meshesfiledescription(i, 'meshes_list') }
	for k, v in pairs(meshes_id_list) do
	  text = text..k.. ' - ' ..v.."\n"

      local meshe_infos = { entity:read_meshesfiledescription(i, 'meshe', k) }
	  text = text..'node id = '..meshe_infos[1].. ' name = ['..meshe_infos[2]..']'.."\n"	 
	  text = text..'has positions = '..meshe_infos[3]..' has faces = '..meshe_infos[4]..' has normales = '..meshe_infos[5]..' has tbn = '..meshe_infos[6].."\n"
	  text = text..'num vertices = '..meshe_infos[7]..' num faces = '..meshe_infos[8]..' num uvchannels = '..meshe_infos[9]..' num bones = '..meshe_infos[10].."\n"
	end

	--[[
	text = text..'Animations : \n'
    local anims_id_list = { entity:read_meshesfiledescription(i, 'anims_list') }
	for k, v in pairs(anims_id_list) do	 
      local anim_infos = { entity:read_meshesfiledescription(i, 'anim', k) }
	  text = text..k.. ' - ' ..v..' name = '..anim_infos[1]..' ticks/sec = '..anim_infos[2]..' duration = '..anim_infos[3]..'s number of channels = '..anim_infos[4].."\n"
	end
	]]
  end

  return text
end