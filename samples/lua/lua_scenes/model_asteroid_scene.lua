

	
	include('asteroid_model.lua')

	
	asteroid_passes_bindings = 
	{
		binding_0 = 
		{
			target_pass_id = 'texture_pass',
			rendering_id = 'lit_rendering',
			lit_shader_update_func = asteroid.update_lit_from_scene_env
		}
	}
	asteroid.view.load('asteroid', asteroid_passes_bindings)
	eg:add_child('root', 'asteroid', asteroid.models['asteroid'].entity)
	model.setup_rawtransformationschain(asteroid.models['asteroid'].entity, 'asteroid', 0)	
	
	model.env.setbkcolor('texture_pass', 0.05,0.05,0.09)
	
	

	model.env.light.setstate( TRUE )
	
	model.env.light.setsphericaldir(0.0,0.0)

	model.env.ambientlight.setcolor(0.1, 0.1, 0.1)
	
	model.camera.mvt:set_pos(0.0, 0.0, 7000.0)
	model.camera.speed = 1500.0

	model.target='asteroid'
	
	model.env.fog.setdensity(0.0)