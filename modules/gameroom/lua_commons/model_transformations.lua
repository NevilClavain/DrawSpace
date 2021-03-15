

model.move = {}
model.transformations = {}

--[[
une entree :
	key : entity id

	value : 
		pos_mat        (Matrix)
		scale_mat      (Matrix)
		rotx_mat       (Matrix)
		roty_mat       (Matrix)
		rotz_mat       (Matrix)
		rotx_deg_angle
		roty_deg_angle
		rotz_deg_angle
		transformation_input_mode (enum)
		transform (RawTransform)
	]]

MODEL_TRANSFORMATION_INPUTMODE_NONE = -1

MODEL_TRANSFORMATION_INPUTMODE_POSX = 0
MODEL_TRANSFORMATION_INPUTMODE_POSY = 1
MODEL_TRANSFORMATION_INPUTMODE_POSZ = 2

MODEL_TRANSFORMATION_INPUTMODE_ROTX = 3
MODEL_TRANSFORMATION_INPUTMODE_ROTY = 4
MODEL_TRANSFORMATION_INPUTMODE_ROTZ = 5

MODEL_TRANSFORMATION_INPUTMODE_SCALEX = 6
MODEL_TRANSFORMATION_INPUTMODE_SCALEY = 7
MODEL_TRANSFORMATION_INPUTMODE_SCALEZ = 8

MODEL_TRANSFORMATION_INPUTMODE_SCALEXYZ = 9


transformations_update = function(p_delta)

  if model.target == "" then
    return
  end

  if model.transformations[model.target] ~= nil then

	  local transform_entry = model.transformations[model.target]

	  if transform_entry['transformation_input_mode'] == MODEL_TRANSFORMATION_INPUTMODE_POSX then

		local pos_x = transform_entry['pos_mat']:get_value(3,0)
		pos_x = pos_x + p_delta
		transform_entry['pos_mat']:set_value(3,0,pos_x) 
	  end

	  if transform_entry['transformation_input_mode'] == MODEL_TRANSFORMATION_INPUTMODE_POSY then

		local pos_y = transform_entry['pos_mat']:get_value(3,1)
		pos_y = pos_y + p_delta
		transform_entry['pos_mat']:set_value(3,1,pos_y) 
	  end

	  if transform_entry['transformation_input_mode'] == MODEL_TRANSFORMATION_INPUTMODE_POSZ then

		local pos_z = transform_entry['pos_mat']:get_value(3,2)
		pos_z = pos_z + p_delta
		transform_entry['pos_mat']:set_value(3,2,pos_z) 
	  end

	  if transform_entry['transformation_input_mode'] == MODEL_TRANSFORMATION_INPUTMODE_ROTX then
		transform_entry['rotx_deg_angle'] = transform_entry['rotx_deg_angle'] + p_delta
	  end

	  if transform_entry['transformation_input_mode'] == MODEL_TRANSFORMATION_INPUTMODE_ROTY then
		transform_entry['roty_deg_angle'] = transform_entry['roty_deg_angle'] + p_delta
	  end

	  if transform_entry['transformation_input_mode'] == MODEL_TRANSFORMATION_INPUTMODE_ROTZ then
		transform_entry['rotz_deg_angle'] = transform_entry['rotz_deg_angle'] + p_delta
	  end

	  if transform_entry['transformation_input_mode'] == MODEL_TRANSFORMATION_INPUTMODE_SCALEX then

		local scale_x = transform_entry['scale_mat']:get_value(0,0)
		scale_x = scale_x + 0.01 * p_delta
		if scale_x < 0.001 then
		  scale_x = 0.001
		end
		transform_entry['scale_mat']:set_value(0,0,scale_x) 
	  end

	  if transform_entry['transformation_input_mode'] == MODEL_TRANSFORMATION_INPUTMODE_SCALEY then

		local scale_y = transform_entry['scale_mat']:get_value(1,1)
		scale_y = scale_y + 0.01 * p_delta
		if scale_y < 0.001 then
		  scale_y = 0.001
		end
		transform_entry['scale_mat']:set_value(1,1,scale_y) 
	  end

	  if transform_entry['transformation_input_mode'] == MODEL_TRANSFORMATION_INPUTMODE_SCALEZ then

		local scale_z = transform_entry['scale_mat']:get_value(2,2)
		scale_z = scale_z + 0.01 * p_delta
		if scale_z < 0.001 then
		  scale_z = 0.001
		end
		transform_entry['scale_mat']:set_value(2,2,scale_z) 
	  end

	  if transform_entry['transformation_input_mode'] == MODEL_TRANSFORMATION_INPUTMODE_SCALEXYZ then

		local scale_x = transform_entry['scale_mat']:get_value(0,0)
		local scale_y = transform_entry['scale_mat']:get_value(1,1)
		local scale_z = transform_entry['scale_mat']:get_value(2,2)

		scale_x = scale_x + 0.01 * p_delta
		scale_y = scale_y + 0.01 * p_delta
		scale_z = scale_z + 0.01 * p_delta

		if scale_x < 0.001 then
		  scale_x = 0.001
		end

		if scale_y < 0.001 then
		  scale_y = 0.001
		end

		if scale_z < 0.001 then
		  scale_z = 0.001
		end

		transform_entry['scale_mat']:set_value(0,0,scale_x)
		transform_entry['scale_mat']:set_value(1,1,scale_y) 
		transform_entry['scale_mat']:set_value(2,2,scale_z)

	  end

	  transform_entry['rotx_mat']:rotation(1.0, 0.0, 0.0, commons.utils.deg_to_rad(transform_entry['rotx_deg_angle']))
	  transform_entry['roty_mat']:rotation(0.0, 1.0, 0.0, commons.utils.deg_to_rad(transform_entry['roty_deg_angle']))
	  transform_entry['rotz_mat']:rotation(0.0, 0.0, 1.0, commons.utils.deg_to_rad(transform_entry['rotz_deg_angle']))

	  local transform = transform_entry['transform']

	  transform:update_matrix("pos",transform_entry['pos_mat'])
	  transform:update_matrix("rotx",transform_entry['rotx_mat'])
	  transform:update_matrix("roty",transform_entry['roty_mat'])
	  transform:update_matrix("rotz",transform_entry['rotz_mat'])
	  transform:update_matrix("scale",transform_entry['scale_mat'])

  else
     g:print("cannot transform because "..model.target.." is a BODY model");

  end

end

model.setup_rawtransformationschain = function(p_entity, p_entity_id, p_initial_scale)

    local rotx_deg_angle = 0.0
    local roty_deg_angle = 0.0
    local rotz_deg_angle = 0.0

    local pos_mat = Matrix()
    pos_mat:translation( 0.0, 0.0, 0.0 )

    local scale_mat = Matrix()

    if p_initial_scale ~= nil then
	  scale_mat:scale( p_initial_scale['x'], p_initial_scale['y'], p_initial_scale['z'] )
    else
  	  scale_mat:scale( 1.0, 1.0, 1.0 )
    end

    local rotx_mat = Matrix()
    rotx_mat:rotation(1.0, 0.0, 0.0, commons.utils.deg_to_rad(rotx_deg_angle))

    local roty_mat = Matrix()
    roty_mat:rotation(0.0, 1.0, 0.0, commons.utils.deg_to_rad(roty_deg_angle))

    local rotz_mat = Matrix()
    rotz_mat:rotation(0.0, 0.0, 1.0, commons.utils.deg_to_rad(rotz_deg_angle))

    local transform = RawTransform()

    transform:configure(p_entity,0)
  
    transform:add_matrix( "scale", scale_mat )
    transform:add_matrix( "roty", roty_mat )
    transform:add_matrix( "rotx", rotx_mat )
    transform:add_matrix( "rotz", rotz_mat )
    transform:add_matrix( "pos", pos_mat )
      
    local transform_entry = 
    { 
  	['pos_mat'] = pos_mat,
  	['scale_mat'] = scale_mat,
	['rotx_mat'] = rotx_mat,
	['roty_mat'] = roty_mat,
	['rotz_mat'] = rotz_mat,
	['rotx_deg_angle'] = rotx_deg_angle,
	['roty_deg_angle'] = roty_deg_angle,
  	['rotz_deg_angle'] = rotz_deg_angle,
	['transformation_input_mode'] = MODEL_TRANSFORMATION_INPUTMODE_NONE,
	['transform'] = transform 	
    }
  
    model.transformations[p_entity_id] = transform_entry

end

model.move.posx = function()

  if model.transformations[model.target] ~= nil then
    model.transformations[model.target]['transformation_input_mode'] = MODEL_TRANSFORMATION_INPUTMODE_POSX
    model.current_operation = "Positionning model along X axis"
  end
end

model.move.posy = function()

  if model.transformations[model.target] ~= nil then
    model.transformations[model.target]['transformation_input_mode'] = MODEL_TRANSFORMATION_INPUTMODE_POSY
    model.current_operation = "Positionning model along Y axis"
  end
end

model.move.posz = function()

  if model.transformations[model.target] ~= nil then
    model.transformations[model.target]['transformation_input_mode'] = MODEL_TRANSFORMATION_INPUTMODE_POSZ
    model.current_operation = "Positionning model along Z axis"
  end
end

model.move.rotx = function()

  if model.transformations[model.target] ~= nil then
    model.transformations[model.target]['transformation_input_mode'] = MODEL_TRANSFORMATION_INPUTMODE_ROTX
    model.current_operation = "Rotating model along X axis"
  end
end

model.move.roty = function()
  if model.transformations[model.target] ~= nil then
    model.transformations[model.target]['transformation_input_mode'] = MODEL_TRANSFORMATION_INPUTMODE_ROTY
    model.current_operation = "Rotating model along Y axis"
  end
end

model.move.rotz = function()
  if model.transformations[model.target] ~= nil then
    model.transformations[model.target]['transformation_input_mode'] = MODEL_TRANSFORMATION_INPUTMODE_ROTZ
    model.current_operation = "Rotating model along Z axis"
  end
end

model.move.scalex = function()
  if model.transformations[model.target] ~= nil then
    model.transformations[model.target]['transformation_input_mode'] = MODEL_TRANSFORMATION_INPUTMODE_SCALEX
    model.current_operation = "Scaling model along X axis"
  end
end

model.move.scaley = function()
  if model.transformations[model.target] ~= nil then
    model.transformations[model.target]['transformation_input_mode'] = MODEL_TRANSFORMATION_INPUTMODE_SCALEY
    model.current_operation = "Scaling model along Y axis"
  end
end

model.move.scalez = function()
  if model.transformations[model.target] ~= nil then
    model.transformations[model.target]['transformation_input_mode'] = MODEL_TRANSFORMATION_INPUTMODE_SCALEZ
    model.current_operation = "Scaling model along Z axis"
  end
end

model.move.scalexyz = function()
  if model.transformations[model.target] ~= nil then
    model.transformations[model.target]['transformation_input_mode'] = MODEL_TRANSFORMATION_INPUTMODE_SCALEXYZ
    model.current_operation = "Scaling model on X-Y-Z axis"
  end
end

model.move.resetpos = function()

  if model.transformations[model.target] ~= nil then
    local transform_entry = model.transformations[model.target]

    local pos_x = transform_entry['pos_mat']:get_value(3,0)
    local pos_y = transform_entry['pos_mat']:get_value(3,1)
    local pos_z = transform_entry['pos_mat']:get_value(3,2)

    pos_x = 0
    pos_y = 0
    pos_z = 0

    transform_entry['pos_mat']:set_value(3,0,pos_x) 
    transform_entry['pos_mat']:set_value(3,1,pos_y) 
    transform_entry['pos_mat']:set_value(3,2,pos_z) 

    local transform = transform_entry['transform']

    transform:update_matrix("pos",transform_entry['pos_mat'])
  end
end

model.move.resetrot = function()

  if model.transformations[model.target] ~= nil then
    local transform_entry = model.transformations[model.target]

    transform_entry['rotx_deg_angle'] = 0
    transform_entry['roty_deg_angle'] = 0
    transform_entry['rotz_deg_angle'] = 0

    transform_entry['rotx_mat']:rotation(1.0, 0.0, 0.0, commons.utils.deg_to_rad(transform_entry['rotx_deg_angle']))
    transform_entry['roty_mat']:rotation(0.0, 1.0, 0.0, commons.utils.deg_to_rad(transform_entry['roty_deg_angle']))
    transform_entry['rotz_mat']:rotation(0.0, 0.0, 1.0, commons.utils.deg_to_rad(transform_entry['rotz_deg_angle']))

    local transform = transform_entry['transform']

    transform:update_matrix("rotx",transform_entry['rotx_mat'])
    transform:update_matrix("roty",transform_entry['roty_mat'])
    transform:update_matrix("rotz",transform_entry['rotz_mat'])
  end
end

model.move.resetscale = function()

  if model.transformations[model.target] ~= nil then
    local transform_entry = model.transformations[model.target]

    local scale_x = transform_entry['scale_mat']:get_value(0,0)
    local scale_y = transform_entry['scale_mat']:get_value(1,1)
    local scale_z = transform_entry['scale_mat']:get_value(2,2)

    scale_x = 1
    scale_y = 1
    scale_z = 1

    transform_entry['scale_mat']:set_value(0,0,scale_x) 
    transform_entry['scale_mat']:set_value(1,1,scale_y) 
    transform_entry['scale_mat']:set_value(2,2,scale_z)

    local transform = transform_entry['transform']

    transform:update_matrix("scale",transform_entry['scale_mat'])
  end
end

model.move.setpos = function(p_entity_id, x, y, z)

  if model.transformations[p_entity_id] ~= nil then
    model.transformations[p_entity_id]['pos_mat']:set_value(3,0,x) 
    model.transformations[p_entity_id]['pos_mat']:set_value(3,1,y) 
    model.transformations[p_entity_id]['pos_mat']:set_value(3,2,z) 
    model.transformations[p_entity_id]['transform']:update_matrix("pos",model.transformations[p_entity_id]['pos_mat'])
  end
end

model.move.setscalexyz = function(p_entity_id, s)

  if model.transformations[p_entity_id] ~= nil then
    model.transformations[p_entity_id]['scale_mat']:set_value(0,0,s) 
    model.transformations[p_entity_id]['scale_mat']:set_value(1,1,s) 
    model.transformations[p_entity_id]['scale_mat']:set_value(2,2,s) 
    model.transformations[p_entity_id]['transform']:update_matrix("scale",model.transformations[p_entity_id]['scale_mat'])
  end
end