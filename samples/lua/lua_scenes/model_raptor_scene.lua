

	
	include('raptor_model.lua')
	
	
	raptor_passes_config = 
	{
		texture_pass = 
		{
			rendering_id = 'lit_rendering',
			lit_shader_update_func = raptor.update_from_scene_env
		}
	}
	raptor.view.load('r', raptor_passes_config, 'root')
	model.move.setpos('r', 0.0, 0.0, 0.0)
	
	model.env.setbkcolor('texture_pass', 0.05,0.05,0.09)
	
	model.camera.mvt:set_pos(0.0, 15.0, 20.0)

	model.env.light.setstate( TRUE )
	
	model.env.light.setsphericaldir(0.0,0.0)

	model.env.ambientlight.setcolor(0.1, 0.1, 0.1)
	
	model.camera.speed = 50.0

