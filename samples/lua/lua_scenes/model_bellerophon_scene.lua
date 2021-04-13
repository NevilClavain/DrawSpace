

	
	include('bellerophon_model.lua')
	
	g:add_resourceeventcb( "onresourceevent_bellerophon_model",
	function( event, resource_path, context )
	
		if event == ALL_ASSETS_LOADED then
		   g:deactivate_resourcessystem()
		   rg:update_renderingqueues()

		   if context == "init" then
			 eg:register_rigidbody(bellerophon.models['b'].entity)
		   end    
		end

	end)
	
	bellerophon_passes_bindings = 
	{
		binding_0 = 
		{
			target_pass_id = 'texture_pass',
			rendering_id = 'lit_rendering',
			lit_shader_update_func = bellerophon.update_lit_from_scene_env
		}
	}
	bellerophon.view.load('b', {x = 0.0, y = 0.0, z = 0.0 }, bellerophon_passes_bindings)
	eg:add_child('root', 'b', bellerophon.models['b'].entity)
	model.setup_rawtransformationschain(bellerophon.models['b'].entity, 'b', 1)
		
	model.env.setbkcolor('texture_pass', 0.0,0.0,1.0)
	
	model.camera.mvt:set_pos(0.0, 0.0, 430.0)

	model.env.light.setstate( TRUE )
	
	model.env.light.setsphericaldir(0.0,0.0)

	model.env.ambientlight.setcolor(0.1, 0.1, 0.1)
	
	model.camera.speed = 500.0
