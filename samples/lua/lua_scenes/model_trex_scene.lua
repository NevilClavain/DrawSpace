

	
	include('trex_model.lua')
	
	
	trex_passes_config = 
	{
		layer_0 = 
		{
			target_pass_id = 'texture_pass',
			rendering_id = 'lit_rendering',
			lit_shader_update_func = trex.update_from_scene_env
		}
	}
	trex.view.load('trex', trex_passes_config)
	eg:add_child('root', 'trex', trex.models['trex'].entity)
	model.setup_rawtransformationschain(trex.models['trex'].entity, 'trex', 0, trex.scale)

	model.move.setpos('trex', 0.0, 0.0, 0.0)
	
	model.env.setbkcolor('texture_pass', 0.05,0.05,0.09)
	
	model.camera.mvt:set_pos(0.0, 8.0, 30.0)

	model.env.light.setstate( TRUE )
	
	model.env.light.setsphericaldir(0.0,0.0)

	model.env.ambientlight.setcolor(0.1, 0.1, 0.1)
	
	model.camera.speed = 50.0

	model.target = 'trex' -- select model

