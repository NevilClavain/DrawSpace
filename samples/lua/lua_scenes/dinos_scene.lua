
	include('skydome_model.lua')
	
	skydome_passes_config = 
	{
		texture_pass = 
		{
			rendering_id = 'main_rendering',
			lit_shader_update_func = skydome.update_from_scene_env
		}
	}
	skydome.view.load('dome', skydome_passes_config, 'root')
	model.move.setpos('dome', 0.0, 0.0, 0.0)
	

	
	

	model.env.setgravity(1)
	
	model.env.setbkcolor('texture_pass', 0.05,0.05,0.09)

	model.camera.mvt:set_pos(-4000.0, skydome.innerRadius + 15.0, -2684.0)

	model.env.light.setstate( TRUE )
	
	model.env.light.setsphericaldir(0.0,0.0)

	model.env.ambientlight.setcolor(0.1, 0.1, 0.1)


	model.camera.speed = 50.0

	--model.env.fog.setdensity(0.001)
