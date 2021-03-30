

	
	include('ceresplanet_model.lua')

	
	ceresplanet_passes_bindings = 
	{
		binding_0 = 
		{
			target_pass_id = 'texture_pass',
			rendering_id = 'lit_rendering',
			lit_shader_update_func = ceresplanet.update_lit_from_scene_env
		}
	}
	ceresplanet.view.load('ceres', ceresplanet_passes_bindings)
	eg:add_child('root', 'ceres', ceresplanet.models['ceres'].entity)
	model.setup_rawtransformationschain(ceresplanet.models['ceres'].entity, 'ceres', 0)	
	
	model.env.setbkcolor('texture_pass', 0.05,0.05,0.09)
	
	

	model.env.light.setstate( TRUE )
	
	model.env.light.setsphericaldir(0.0,0.0)

	model.env.ambientlight.setcolor(0.1, 0.1, 0.1)
	
	model.camera.mvt:set_pos(0.0, 0.0, 2800.0)
	model.camera.speed = 500.0

	model.target='ceres'
	
	model.env.fog.setdensity(0.0)