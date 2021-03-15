

	
	include('raptor_model.lua')
	
	
	raptor_passes_config = 
	{
		layer_0 = 
		{
			target_pass_id = 'texture_pass',
			rendering_id = 'lit_rendering',
			lit_shader_update_func = raptor.update_from_scene_env
		}
	}
	raptor.view.load('my_raptor', raptor_passes_config)
	eg:add_child('root', 'my_raptor', raptor.models['my_raptor'].entity)
	model.setup_rawtransformationschain(raptor.models['my_raptor'].entity, 'my_raptor', raptor.scale)

	model.move.setpos('my_raptor', 0.0, 0.0, 0.0)
	
	model.env.setbkcolor('texture_pass', 0.05,0.05,0.09)
	
	model.camera.mvt:set_pos(0.0, 8.0, 30.0)

	model.env.light.setstate( TRUE )
	
	model.env.light.setsphericaldir(0.0,0.0)

	model.env.ambientlight.setcolor(0.1, 0.1, 0.1)
	
	model.camera.speed = 50.0

	model.target = 'my_raptor' -- select model

