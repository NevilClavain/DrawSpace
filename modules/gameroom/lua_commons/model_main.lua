

model = {}

model.entities = {}

model.camera = {}

model.camera.speed = 50.0

model.target = ""
model.current_operation = ""

model.renderer_infos = {renderer:descr()}

environment = 
{
	ambient_light = {r = 0.35, g = 0.35, b = 0.35, a = 0.0 },
	lights_enabled = {x = 1.0, y = 0.0, z = 0.0, w = 0.0 },
	light0 = 
	{
		color = { r = 1.0, g = 1.0, b = 1.0, a = 1.0 },
		direction = { x = -0.1, y = -1.0, z = 0.0, w = 1.0 },
	},


	fog_intensity = 0.00022,
	fog_color = 
	{
		r = 0.55,
		g = 0.55,
		b = 0.99, 
	},

	reflector_pos = {x = 1.0, y = 0.0, z = 0.0, w = 0.0 },
	reflector_normale = {x = 1.0, y = 0.0, z = 0.0, w = 0.0 },

	gravity = { x = 0.0, y = -9.81, z = 0.0},
	gravity_state = GRAVITY_DISABLED
}

include("model_dump.lua")
include("model_view.lua")
include("model_transformations.lua")
include("model_anims.lua")
include("model_env.lua")

model.printscenelist = function()
  local text = ""
  local i = 1
  for k, v in pairs(model.entities) do
	text=text..i..' - {'..k..'}'..v.model_classname.."\n"
	i = i + 1
  end
  return text
end

model.createmainfreecamera = function(p_x, p_y, p_z, p_mvt_mod)

  model.camera.entity, model.camera.mvt=commons.create_free_camera(p_x, p_y, p_z, model.renderer_infos[5],model.renderer_infos[6], p_mvt_mod, "model.camera")
  --eg:add_child('root','model.camera.entity',model.camera.entity)

end

model.createmainfpscamera = function(p_x, p_y, p_z)

  model.camera.entity, model.camera.mvt=commons.create_fps_camera(p_x, p_y, p_z, model.renderer_infos[5],model.renderer_infos[6], "model.camera")
  --eg:add_child('root','model.camera.entity',model.camera.entity)

end

model.releasemaincamera = function()
  --eg:remove('model.camera.entity')
  commons.trash.camera(model.camera.entity, model.camera.mvt)  
end