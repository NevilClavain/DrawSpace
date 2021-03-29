

	
	include('land_model.lua')

	
	land_passes_bindings = 
	{
		binding_0 = 
		{
			target_pass_id = 'texture_pass',
			rendering_id = 'lit_rendering',
			lit_shader_update_func = land.update_from_scene_env
		}
	}
	land.view.load('l', land_passes_bindings)
	eg:add_child('root', 'l', land.models['l'].entity)
	model.setup_rawtransformationschain(land.models['l'].entity, 'l')	
	
	model.env.setbkcolor('texture_pass', 0.05,0.05,0.09)
	
	

	model.env.light.setstate( TRUE )
	
	model.env.light.setsphericaldir(0.0,0.0)

	model.env.ambientlight.setcolor(0.1, 0.1, 0.1)
	
	model.camera.mvt:set_pos(0.0, 8.0, 0.0)
	model.camera.speed = 50.0

	

	model.target='l'
	
