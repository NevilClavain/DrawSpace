
	include('skydome_model.lua')
	include('continent_model.lua')
	include('boulder_model.lua')
	include('spherebump_model.lua')
	include('metalcube_model.lua')
	include('spacebox_model.lua')


	g:add_resourceeventcb( "onresourceevent_dinoscene",
	function( event, resource_path, context )

      if event == ALL_ASSETS_LOADED then

	  
         eg:register_collider(continent.models['continent'].entity)
		 eg:register_collider(boulder.models['rock'].entity)
		 eg:register_rigidbody(spherebump.models['s'].entity)
		 eg:register_rigidbody(spherebump.models['s2'].entity)
		 eg:register_rigidbody(metalcube.models['c'].entity)
		 

      end
    end)

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
		layer_0 = 
		{
			target_pass_id = 'texture_pass',
			rendering_id = 'lit_rendering',
			lit_shader_update_func = continent.update_lit_from_scene_env
		}
	}	
	continent.view.load('continent', continent_passes_config)
	eg:add_child('root', 'continent', continent.models['continent'].entity)
	
	cont_pos_mat = Matrix()
    cont_pos_mat:translation( 0.0, skydome.innerRadius, 0.0 )

    cont_transform = RawTransform()
    cont_transform:configure(continent.models['continent'].entity,0)

    cont_transform:add_matrix( "pos", cont_pos_mat )



	skydome_passes_config = 
	{
		layer_0 = 
		{
			target_pass_id = 'texture_pass',
			rendering_id = 'main_rendering',
			lit_shader_update_func = skydome.update_from_scene_env
		}
	}
	skydome.view.load('dome', skydome_passes_config)
	eg:add_child('root', 'dome', skydome.models['dome'].entity)
	model.setup_rawtransformationschain(skydome.models['dome'].entity, 'dome', 0, skydome.scale)
	model.move.setpos('dome', 0.0, 0.0, 0.0)
	
	
	
	boulder_passes_config = 
	{
	    layer_0 = 
	    {
			target_pass_id = 'texture_pass',
			rendering_id = 'lit_rendering',
			lit_shader_update_func = boulder.update_lit_from_scene_env
		}
	}
	boulder.view.load('rock', boulder_passes_config)
	eg:add_child('root', 'rock', boulder.models['rock'].entity)

	boulder_pos_mat = Matrix()
    boulder_pos_mat:translation( -4010.0, skydome.innerRadius, -2740.0 )

    boulder_transform = RawTransform()
    boulder_transform:configure(boulder.models['rock'].entity,0)

    boulder_transform:add_matrix( "pos", boulder_pos_mat )

	
	spherebump_passes_config = 
	{
		layer_0 = 
		{
			target_pass_id = 'texture_pass',
	        rendering_id = 'lit_rendering',
			lit_shader_update_func = spherebump.update_from_scene_env
		}
	}

	sphere_pos_mat = Matrix()
	sphere_pos_mat:translation( -4010.0, skydome.innerRadius + 135.0, -2740.0 )
	spherebump.view.load('s', sphere_pos_mat, spherebump_passes_config)
	eg:add_child('root', 's', spherebump.models['s'].entity)

	sphere2_pos_mat = Matrix()
	sphere2_pos_mat:translation( -4010.0, skydome.innerRadius + 18.0, -2740.0 )
	spherebump.view.load('s2', sphere2_pos_mat, spherebump_passes_config)
	eg:add_child('root', 's2', spherebump.models['s2'].entity)
	
	metalcube_passes_config = 
	{
		layer_0 = 
		{
			target_pass_id = 'texture_pass',
			rendering_id = 'lit_rendering',
			lit_shader_update_func = metalcube.update_from_scene_env
		}
	}
	metalcube.view.load('c', {x = -4010.0, y = skydome.innerRadius + 25.0, z = -2740.0}, metalcube_passes_config)
	eg:add_child('root', 'c', metalcube.models['c'].entity)
	
	
	spacebox_layers =
	{
		[0] =	
		{
			spacebox_rendering =
			{
				fx =
				{
					shaders = 
					{
						{ path='texture_vs.hlsl',mode=SHADER_NOT_COMPILED },
						{ path='texture_ps.hlsl',mode=SHADER_NOT_COMPILED }
					},
					rs_in = 
					{
						{ ope=RENDERSTATE_OPE_ENABLEZBUFFER, value="false" }
					},
					rs_out =
					{
						{ ope=RENDERSTATE_OPE_ENABLEZBUFFER, value="false" }
					}
				},
				textures =
				{
					[1] = 
					{
						{path='spacebox_front5.png', stage=0}
					},
					[2] = 
					{
						{path='spacebox_back6.png', stage=0}
					},
					[3] = 
					{
						{path='spacebox_left2.png', stage=0}
					},
					[4] = 
					{
						{path='spacebox_right1.png', stage=0}
					},
					[5] = 
					{
						{path='spacebox_top3.png', stage=0}
					},
					[6] = 
					{
						{path='spacebox_bottom4.png', stage=0}
					}
				},
				vertex_textures =
				{
				},
				shaders_params = 
				{
				},
				rendering_order = 10
			}
		}
	}



	spacebox_passes_config = 
    {
	   layer_0 = 
	   {
			target_pass_id = 'texture_pass',
			rendering_id = 'spacebox_rendering',
			lit_shader_update_func = nil
	   }
    }
    spaceboxmod.view.load('spacebox0', spacebox_passes_config, spacebox_layers)
	model.setup_rawtransformationschain(spaceboxmod.models['spacebox0'].entity, 'spacebox0', 0, {x = 1000.0, y = 1000.0, z = 1000.0 } )
	eg:add_child('root', 'spacebox0', spaceboxmod.models['spacebox0'].entity)
	

	model.env.setgravity(1)
	
	model.env.setbkcolor('texture_pass', 0.05,0.05,0.09)

	model.camera.mvt:set_pos(-4000.0, skydome.innerRadius + 15.0, -2684.0)

	model.env.light.setstate( TRUE )
	
	model.env.light.setsphericaldir(0.0,0.0)

	model.env.ambientlight.setcolor(0.1, 0.1, 0.1)


	model.camera.speed = 50.0

	--model.env.fog.setdensity(0.001)
