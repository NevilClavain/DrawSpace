

	
	include('bellerophon_model.lua')
	
	
	bellerophon_passes_config = 
	{
		layer_0 = 
		{
			target_pass_id = 'texture_pass',
			rendering_id = 'lit_rendering',
			lit_shader_update_func = bellerophon.update_lit_from_scene_env
		}
	}
	bellerophon.view.load('b', {x = 0.0, y = 0.0, z = 0.0 }, bellerophon_passes_config)
	eg:add_child('root', 'b', bellerophon.models['b'].entity)
		
	model.env.setbkcolor('texture_pass', 0.0,0.0,1.0)
	
	model.camera.mvt:set_pos(0.0, 0.0, 430.0)

	model.env.light.setstate( TRUE )
	
	model.env.light.setsphericaldir(0.0,0.0)

	model.env.ambientlight.setcolor(0.1, 0.1, 0.1)
	
	model.camera.speed = 500.0

