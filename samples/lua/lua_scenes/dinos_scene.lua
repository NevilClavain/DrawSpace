
	include('skydome_model.lua')
	include('continent_model.lua')
	include('boulder_model.lua')
	include('spherebump_model.lua')
	include('metalcube_model.lua')
	include('spacebox_model.lua')

	max_apprun_cbfunc = function()

      local normalized_light_dir = Vector(environment.light0.direction.x, environment.light0.direction.y, environment.light0.direction.z, 0.0)
	  normalized_light_dir:normalize()

	  local y = g:clamp(0.0, 1.0, -normalized_light_dir:get_y())	 
	  model.env.fog.setdensity(y * 0.0005)

	  local y0 = g:clamp(0.0, 1.0, 2.0 * normalized_light_dir:get_y())

	  --[[
	  raptor.lit_material.light_absorption.r = y0
	  raptor.lit_material.light_absorption.g = y0
	  raptor.lit_material.light_absorption.b = y0

	  raptor.update_from_scene_env('texture_pass', environment, 'r')
	  ]]

	  boulder.lit_material.light_absorption.r = y0
	  boulder.lit_material.light_absorption.g = y0
	  boulder.lit_material.light_absorption.b = y0

	  boulder.update_lit_from_scene_env('texture_pass', environment, 'rock')


	  spherebump.lit_material.light_absorption.r = y0
	  spherebump.lit_material.light_absorption.g = y0
	  spherebump.lit_material.light_absorption.b = y0

	  spherebump.update_from_scene_env('texture_pass', environment, 's')
	  spherebump.update_from_scene_env('texture_pass', environment, 's2')


    end
	
	continent_passes_config = 
	{
		texture_pass = 
		{
			rendering_id = 'lit_rendering',
			lit_shader_update_func = continent.update_lit_from_scene_env
		}
	}	
	continent.view.load('continent', {x = 0.0, y = skydome.innerRadius, z = 0.0}, continent_passes_config, 'root')
	
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
	
	
	boulder_passes_config = 
	{
	    texture_pass = 
	    {
			rendering_id = 'lit_rendering',
			lit_shader_update_func = boulder.update_lit_from_scene_env
		}
	}
	boulder.view.load('rock', {x = -4010.0, y = skydome.innerRadius, z = -2740.0}, boulder_passes_config, 'root' )


	spherebump_passes_config = 
	{
		texture_pass = 
		{
	        rendering_id = 'lit_rendering',
			lit_shader_update_func = spherebump.update_from_scene_env
		}
	}
	spherebump.view.load('s', {x = -4010.0, y = skydome.innerRadius + 135.0, z = -2740.0}, spherebump_passes_config, 'root' )
	spherebump.view.load('s2', {x = -4010.0, y = skydome.innerRadius + 18.0, z = -2740.0}, spherebump_passes_config, 'root' )


	metalcube_passes_config = 
	{
		texture_pass = 
		{
			rendering_id = 'lit_rendering',
			lit_shader_update_func = metalcube.update_from_scene_env
		}
	}
	metalcube.view.load('c', {x = -4010.0, y = skydome.innerRadius + 25.0, z = -2740.0}, metalcube_passes_config, 'root' )
	
	
	spacebox_passes_config = 
    {
	   texture_pass = 
	   {
		   rendering_id = 'layer0_rendering',
		   lit_shader_update_func = nil
	   }
    }
    spaceboxmod.view.load('spacebox0', spacebox_passes_config, 'root')

	model.env.setgravity(1)
	
	model.env.setbkcolor('texture_pass', 0.05,0.05,0.09)

	model.camera.mvt:set_pos(-4000.0, skydome.innerRadius + 15.0, -2684.0)

	model.env.light.setstate( TRUE )
	
	model.env.light.setsphericaldir(0.0,0.0)

	model.env.ambientlight.setcolor(0.1, 0.1, 0.1)


	model.camera.speed = 50.0

	--model.env.fog.setdensity(0.001)
