

include('land_model.lua')
include('skydome_model.lua')
include('waterquad_model.lua')
include('metalcube_model.lua')
include('spherebump_model.lua')
include('impostors_model.lua')


environment.reflector_pos.x = 0.0
environment.reflector_pos.y = skydome.innerRadius
environment.reflector_pos.z = 0.0

environment.reflector_normale.x = 0.0
environment.reflector_normale.y = 1.0
environment.reflector_normale.z = 0.0

hmi_mode=FALSE

cube_instance=0

resources_event = "..."

light_theta = SyncAngle()
light_phi = SyncAngle()

light_theta:init_fromtimeaspectof(root_entity, 0.0)
light_phi:init_fromtimeaspectof(root_entity, 8.0)

move_light_vector = function(dx, dy)
		
  light_theta:inc(dx * 5)
  light_phi:inc(-dy * 5)

  model.env.light.setsphericaldir(light_theta:get_value(), light_phi:get_value())

end


g:add_resourceeventcb( "onresourceevent",
function( event, resource_path, context )

    local evt_out

    if event == BLOB_LOAD then
       evt_out = "loading :"..resource_path
    elseif event == BLOB_LOADED then
       evt_out = "loaded :"..resource_path
    elseif event == ASSET_SETLOADEDBLOB then
       evt_out = "set :"..resource_path
    elseif event == SHADERCACHE_CREATION then
       evt_out = "shader cache creation"
    elseif event == SHADER_COMPILATION then
       evt_out = "compiling :"..resource_path
    elseif event == SHADER_COMPILED then
       evt_out = "compilation done :"..resource_path
    elseif event == ALL_ASSETS_LOADED then
       evt_out = "All assets loaded !"
       g:deactivate_resourcessystem()
       rg:update_renderingqueues()

       if context == "init" then
         eg:register_collider(land.models['l'].entity)
		 eg:register_rigidbody(metalcube.models['cube'].entity)
		 eg:register_rigidbody(spherebump.models['sphere'].entity)

	   else
	    
		 -- here, context = cube entity instance name 
	     root_entity:register_rigidbody(metalcube.models[context].entity)

       end
    else
       evt_out = "? : "..event       
    end
    resources_event = evt_out
end)


g:print('Current renderer is '..model.renderer_infos[1]..', '..model.renderer_infos[2]..'x'..model.renderer_infos[3])
renderer_infos = {renderer:descr()}
g:print('Current resolution is '..renderer_infos[2].." "..renderer_infos[3])


rg:create_pass_viewportquad('final_pass')

local finalpass_rss=RenderStatesSet()

finalpass_rss:add_renderstate_in(RENDERSTATE_OPE_SETTEXTUREFILTERTYPE, "point")
finalpass_rss:add_renderstate_out(RENDERSTATE_OPE_SETTEXTUREFILTERTYPE, "linear")

local textures = TexturesSet()

local fxparams = FxParams()
fxparams:add_shaderfile('water_mask_vs.hlsl',SHADER_NOT_COMPILED)
fxparams:add_shaderfile('water_mask_ps.hlsl',SHADER_NOT_COMPILED)
fxparams:set_renderstatesset(finalpass_rss)


rendercontext = RenderContext('final_pass')
rendercontext:add_fxparams(fxparams)
rendercontext:add_texturesset(textures)
rendercontext:add_shaderparam("water_color", 1, 0)
rendercontext:add_shaderparam("surface_normale", 1, 1)
rendercontext:add_shaderparam("debug_mode", 1, 2)
rendercontext:add_shaderparam("water_specular_power", 1, 3)

renderconfig=RenderConfig()
renderconfig:add_rendercontext(rendercontext)
rg:configure_pass_viewportquad_resources('final_pass',renderconfig)
rg:set_viewportquadshaderrealvector('final_pass', 'water_color', 1, 1, 1, 1.0)
rg:set_viewportquadshaderrealvector('final_pass', 'surface_normale', environment.reflector_normale.x, environment.reflector_normale.y, environment.reflector_normale.z, 1.0)
rg:set_viewportquadshaderrealvector('final_pass', 'debug_mode', 0.0, 0.0, 0.0, 0.0)
rg:set_viewportquadshaderrealvector('final_pass', 'water_specular_power', 50.0, 0.0, 0.0, 0.0)


rg:create_child('final_pass', 'texture_pass', 0)
rg:create_child('final_pass', 'texturemirror_pass', 1)




rg:create_child('final_pass', 'bump_pass', 2, RENDERPURPOSE_FLOATVECTOR)
rg:set_pass_depthclearstate('bump_pass', TRUE)
rg:set_pass_targetclearstate('bump_pass', TRUE)
rg:set_pass_targetclearcolor('bump_pass', 0, 0, 0, 0)


rg:create_child('final_pass', 'texturerefrac_pass', 3)
rg:set_pass_targetclearstate('texturerefrac_pass', TRUE)
rg:set_pass_targetclearcolor('texturerefrac_pass', 0, 56, 67, 0)


rg:create_child('final_pass', 'waternormales_pass', 4)

rg:create_child('final_pass', 'waterhalfvector_pass', 5, RENDERPURPOSE_FLOATVECTOR)


rg:create_child('final_pass', 'wave_pass', NO_TEXTURESTAGE_CONNECTION, RENDERPURPOSE_COLOR, RENDERTARGET_GPU, FALSE, 512, 512)

rg:create_pass_viewportquad('wave_pass')

wave_fxparams = FxParams()
wave_fxparams:add_shaderfile('water_waves_vs.hlsl',SHADER_NOT_COMPILED)
wave_fxparams:add_shaderfile('water_waves_ps.hlsl',SHADER_NOT_COMPILED)

wave_textures = TexturesSet()
wave_rendercontext = RenderContext('wave_pass')
wave_rendercontext:add_fxparams(wave_fxparams)
wave_rendercontext:add_texturesset(wave_textures)
wave_rendercontext:add_shaderparam("waves", 1, 0)

waves_renderconfig=RenderConfig()
waves_renderconfig:add_rendercontext(wave_rendercontext)
rg:configure_pass_viewportquad_resources('wave_pass',waves_renderconfig)

root_entity:add_aspect(PHYSICS_ASPECT)

root_entity:configure_world(environment.gravity_state, environment.gravity.x, environment.gravity.y, environment.gravity.z )



model.createmainfpscamera(0.0, skydome.innerRadius + 8.0, 20.0)
eg:add_child('root','model.camera.entity',model.camera.entity)

eg:set_camera(model.camera.entity)

waves=SyncAngle()
waves_inc = TRUE

waves:init_fromtimeaspectof(root_entity,0.0)

text_renderer=StringRendering()
text_renderer:configure(root_entity, "fps", 320, 30, 255, 0, 255, "??? fps")


text2_renderer=StringRendering()
text2_renderer:configure(root_entity, "resource_infos", 620, 130, 255, 0, 255, "resources...")


objectid_renderer=StringRendering()
objectid_renderer:configure(root_entity, "objectid", 600, 600, 255, 0, 255, "sphere")

objectdistance_renderer=StringRendering()
objectdistance_renderer:configure(root_entity, "objectdistance", 600, 600, 255, 0, 255, "...")



-- space impostor

space_impostor_rendering_config =
{
	main_rendering =	
	{
		fx =
		{
			shaders = 
			{
				{ path='spaceimpostor_vs.hlsl',mode=SHADER_NOT_COMPILED },
				{ path='spaceimpostor_ps.hlsl',mode=SHADER_NOT_COMPILED }
			},
			rs_in = 
			{
				{ ope=RENDERSTATE_OPE_ENABLEZBUFFER, value="true" }
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
				{ path='map.jpg', stage=0 },
			}
		},
		shaders_params = 
		{ 
			{ param_name = "vflags", shader_index = 0, register = 24 },
			{ param_name = "flags", shader_index = 1, register = 0 },
			{ param_name = "color", shader_index = 1, register = 1 },
		},
		rendering_order = 1200
	}
}

space_impostor_passes_binding = 
{	
	binding_0 = 
	{
		target_pass_id = 'texture_pass',
		rendering_id = 'main_rendering',
		lit_shader_update_func = function( p_pass_id, p_environment_table, p_entity_id )

			local renderer = impostors.models[p_entity_id]['renderer']
			renderer:set_shaderrealvector( p_pass_id, 'vflags', 0.0, 0.0, 0.0, 0.0 )
			renderer:set_shaderrealvector( p_pass_id, 'flags', 0.0, 0.0, 0.0, 0.0 )
			renderer:set_shaderrealvector( p_pass_id, 'color', 0.0, 0.0, 0.0, 0.0 )
		end
	}
}

impostors_descriptors_array = ImpostorsDescriptionsArray()

impostors_descriptors_array:add()
impostors_descriptors_array:set_scale(0, 2.0, 2.0)
impostors_descriptors_array:set_position(0, 3.0, 0.0, 0.0)

impostors_descriptors_array:add()
impostors_descriptors_array:set_scale(1, 2.0, 2.0)
impostors_descriptors_array:set_position(1, -3.0, 0.0, 0.0)


impostors.view.load('impostors0', impostors_descriptors_array, space_impostor_passes_binding, space_impostor_rendering_config)

eg:add_child('root', 'impostors0', impostors.models['impostors0'].entity)

impostors.models['impostors0'].entity:add_aspect(TRANSFORM_ASPECT)

space_impostor_transform = RawTransform()
space_impostor_transform:configure(impostors.models['impostors0'].entity,0)

space_impostor_pos_mat = Matrix()
space_impostor_pos_mat:translation( 0.0, skydome.innerRadius + 10.0, 0.0 )

space_impostor_transform:add_matrix( "pos", space_impostor_pos_mat )

-- screen impostor

screen_impostor_rendering_config =
{
	main_rendering =	
	{
		fx =
		{
			shaders = 
			{
				{ path='screenimpostor_vs.hlsl',mode=SHADER_NOT_COMPILED },
				{ path='screenimpostor_ps.hlsl',mode=SHADER_NOT_COMPILED }
			},
			rs_in = 
			{
				{ ope=RENDERSTATE_OPE_ENABLEZBUFFER, value="true" },
				{ ope=RENDERSTATE_OPE_ALPHABLENDENABLE, value="true" },
				{ ope=RENDERSTATE_OPE_ALPHABLENDOP, value="add" },
				{ ope=RENDERSTATE_OPE_ALPHABLENDFUNC, value="always" },
				{ ope=RENDERSTATE_OPE_ALPHABLENDDEST, value="one" },
				{ ope=RENDERSTATE_OPE_ALPHABLENDSRC, value="srcalpha" },
			},
			rs_out =
			{
				{ ope=RENDERSTATE_OPE_ENABLEZBUFFER, value="false" },
				{ ope=RENDERSTATE_OPE_ALPHABLENDENABLE, value="false" }
			}
		},
		textures =
		{
			[1] = 
			{
				{ path='star.bmp', stage=0 },
			}
		},
		shaders_params = 
		{ 
			{ param_name = "scale", shader_index = 0, register = 24 },
			{ param_name = "flags", shader_index = 1, register = 0 },
			{ param_name = "color", shader_index = 1, register = 1 },
		},
		rendering_order = 11000
	}
}

screen_impostor_passes_binding = 
{	
	binding_0 = 
	{
		target_pass_id = 'texture_pass',
		rendering_id = 'main_rendering',
		lit_shader_update_func = function( p_pass_id, p_environment_table, p_entity_id )
			local renderer = impostors.models[p_entity_id]['renderer']
			renderer:set_shaderrealvector( p_pass_id, 'scale', 1.0, 1.0, 0.0, 0.0 )
			renderer:set_shaderrealvector( p_pass_id, 'flags', 0.0, 0.0, 0.0, 0.0 )
			renderer:set_shaderrealvector( p_pass_id, 'color', 1.0, 1.0, 1.0, 1.0 )
		end
	}
}

impostors_descriptors_array_2 = ImpostorsDescriptionsArray()
impostors_descriptors_array_2:add()

impostors.view.load('impostors1', impostors_descriptors_array_2, screen_impostor_passes_binding, screen_impostor_rendering_config)

eg:add_child('root', 'impostors1', impostors.models['impostors1'].entity)

impostors.models['impostors1'].entity:add_aspect(TRANSFORM_ASPECT)


screen_impostor_transform = RawTransform()
screen_impostor_transform:configure(impostors.models['impostors1'].entity,0)

screen_impostor_pos_mat = Matrix()
screen_impostor_pos_mat:translation( 0.0, skydome.innerRadius + 20.0, 0.0 )

screen_impostor_transform:add_matrix( "pos", screen_impostor_pos_mat )


-- sprite

sprite_rendering_config =
{
	main_rendering =	
	{
		fx =
		{
			shaders = 
			{
				{ path='spriteimpostor_vs.hlsl',mode=SHADER_NOT_COMPILED },
				{ path='spriteimpostor_ps.hlsl',mode=SHADER_NOT_COMPILED }
			},
			rs_in = 
			{
				{ ope=RENDERSTATE_OPE_ENABLEZBUFFER, value="true" }
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
				{ path='wolverine.jpg', stage=0 },
			}
		},
		shaders_params = 
		{ 
			{ param_name = "pos2D", shader_index = 0, register = 5 },
			{ param_name = "flags", shader_index = 1, register = 0 },
			{ param_name = "color", shader_index = 1, register = 1 },
		},
		rendering_order = 20000
	}
}

sprites_passes_binding = 
{	
	binding_0 = 
	{
		target_pass_id = 'texture_pass',
		rendering_id = 'main_rendering',
		lit_shader_update_func = function( p_pass_id, p_environment_table, p_entity_id )
			local renderer = impostors.models[p_entity_id]['renderer']
			renderer:set_shaderrealvector( p_pass_id, 'pos2D', -0.7, -0.7, 0.0, 0.0 )
			renderer:set_shaderrealvector( p_pass_id, 'flags', 0.0, 0.0, 0.0, 0.0 )
			renderer:set_shaderrealvector( p_pass_id, 'color', 1.0, 1.0, 1.0, 1.0 )
		end
	}
}

sprite_descriptors_array = ImpostorsDescriptionsArray()
sprite_descriptors_array:add()

sprite_descriptors_array:set_scale(0, 0.1, 0.1, 0.0)


impostors.view.load('sprite', sprite_descriptors_array, sprites_passes_binding, sprite_rendering_config)
eg:add_child('root', 'sprite', impostors.models['sprite'].entity)


-- collimator sprite

collimatorsprite_rendering_config =
{
	main_rendering =	
	{
		fx =
		{
			shaders = 
			{
				{ path='spriteimpostor_vs.hlsl',mode=SHADER_NOT_COMPILED },
				{ path='spriteimpostor_ps.hlsl',mode=SHADER_NOT_COMPILED }
			},
			rs_in = 
			{
				{ ope=RENDERSTATE_OPE_ENABLEZBUFFER, value="true" }
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
				{ path='collimator.jpg', stage=0 },
			}
		},
		shaders_params = 
		{ 
			{ param_name = "pos2D", shader_index = 0, register = 5 },
			{ param_name = "flags", shader_index = 1, register = 0 },
			{ param_name = "color", shader_index = 1, register = 1 },
			{ param_name = "keycolor", shader_index = 1, register = 2 },
		},
		rendering_order = 30000
	}
}

collimatorsprite_passes_binding = 
{	
	binding_0 = 
	{
		target_pass_id = 'texture_pass',
		rendering_id = 'main_rendering',
		lit_shader_update_func = function( p_pass_id, p_environment_table, p_entity_id )
			local renderer = impostors.models[p_entity_id]['renderer']
			renderer:set_shaderrealvector( p_pass_id, 'pos2D', 0.0, 0.0, 0.0, 0.0 )
			renderer:set_shaderrealvector( p_pass_id, 'flags', 0.0, 1.0, 0.0, 0.0 )
			renderer:set_shaderrealvector( p_pass_id, 'color', 1.0, 1.0, 1.0, 1.0 )
			renderer:set_shaderrealvector( p_pass_id, 'keycolor', 0.0, 0.0, 0.0, 1.0 )
		end
	}
}

collimatorsprite_descriptors_array = ImpostorsDescriptionsArray()
collimatorsprite_descriptors_array:add()

renderer_aspect_ratio =  model.renderer_infos[5] / model.renderer_infos[6];

--collimatorsprite_descriptors_array:set_scale(0, 0.1, 0.1 * renderer_aspect_ratio, 0.0)
collimatorsprite_descriptors_array:set_scale(0, 0.05, 0.05 * renderer_aspect_ratio, 0.0)

impostors.view.load('collimator', collimatorsprite_descriptors_array, collimatorsprite_passes_binding, collimatorsprite_rendering_config)
eg:add_child('root', 'collimator', impostors.models['collimator'].entity)


-----------------------------------

skydome_passes_bindings = 
{
	binding_0 = 
	{
		target_pass_id = 'texture_pass',
		rendering_id = 'main_rendering',
		lit_shader_update_func = skydome.update_from_scene_env
	},
	binding_1 =
	{
		target_pass_id = 'texturemirror_pass',
		rendering_id = 'mirror_rendering',
		lit_shader_update_func = skydome.update_from_scene_env_mirror	
	}
}
skydome.view.load('dome', skydome_passes_bindings)
eg:add_child('root', 'dome', skydome.models['dome'].entity)

model.setup_rawtransformationschain(skydome.models['dome'].entity, 'dome', 0, skydome.scale)

model.move.setpos('dome', 0.0, 0.0, 0.0)

land_passes_bindings = 
{
	binding_0 = 
	{
		target_pass_id = 'texture_pass',
		rendering_id = 'lit_rendering',
		lit_shader_update_func = land.update_from_scene_env
	},
	binding_1 =
	{
		target_pass_id = 'texturemirror_pass',
		rendering_id = 'lit_mirror_rendering',
		lit_shader_update_func = land.update_from_scene_env_mirror	
	},
	binding_2 =
	{
		target_pass_id = 'texturerefrac_pass',
		rendering_id = 'lit_rendering',
		lit_shader_update_func = land.update_from_scene_env
	}

}
land.view.load('l', land_passes_bindings)
eg:add_child('root', 'l', land.models['l'].entity)

land_pos_mat = Matrix()
land_pos_mat:translation( 0.0, skydome.innerRadius, 0.0 )

land_transform = RawTransform()
land_transform:configure(land.models['l'].entity,0)

land_transform:add_matrix( "pos_land", land_pos_mat )





metalcube_passes_bindings = 
{
	binding_0 = 
	{
		target_pass_id = 'texture_pass',
		rendering_id = 'lit_rendering',
		lit_shader_update_func = metalcube.update_from_scene_env
	},
	binding_1 =
	{
		target_pass_id = 'texturemirror_pass',
		rendering_id = 'lit_mirror_rendering',
		lit_shader_update_func = metalcube.update_from_scene_env_mirror	
	}
}

cube_pos_mat = Matrix()
cube_pos_mat:translation( 0.0, skydome.innerRadius + 1.7, 0.0 )
metalcube.view.load('cube', cube_pos_mat, metalcube_passes_bindings)
eg:add_child('root', 'cube', metalcube.models['cube'].entity)


spherebump_passes_bindings = 
{
	binding_0 = 
	{
		target_pass_id = 'texture_pass',
	    rendering_id = 'lit_rendering',
		lit_shader_update_func = spherebump.update_from_scene_env
	},
	binding_1 =
	{
		target_pass_id = 'texturemirror_pass',
		rendering_id = 'lit_mirror_rendering',
		lit_shader_update_func = spherebump.update_from_scene_env_mirror	
	}
}

sphere_pos_mat = Matrix()
sphere_pos_mat:translation( 0.0, skydome.innerRadius + 2.0, 0.0 )
spherebump.view.load('sphere', sphere_pos_mat, spherebump_passes_bindings)
eg:add_child('root', 'sphere', spherebump.models['sphere'].entity)

waterquad_passes_bindings = 
{
	binding_0 = 
	{
		target_pass_id = 'texture_pass',
		rendering_id = 'main_rendering',
		lit_shader_update_func = waterquad.update_from_scene_env
	},
	binding_1 = 
	{
		target_pass_id = 'bump_pass',
		rendering_id = 'bump_rendering',
		lit_shader_update_func = waterquad.update_from_scene_env_bump
	},
	binding_2 = 
	{
		target_pass_id = 'waternormales_pass',
		rendering_id = 'normales_rendering',
		lit_shader_update_func = waterquad.update_normales_from_scene_env
	},
	binding_3 = 
	{
		target_pass_id = 'waterhalfvector_pass',
		rendering_id = 'halfvector_rendering',
		lit_shader_update_func = waterquad.update_halfvector_from_scene_env
	}
}
waterquad.view.load('water', waterquad_passes_bindings)
eg:add_child('root', 'water', waterquad.models['water'].entity)
model.setup_rawtransformationschain(waterquad.models['water'].entity, 'water', 0)
model.move.setpos('water', 0.0, skydome.innerRadius, 0.0)
waterquad.models['water']['renderer']:set_passnodetexturefrompass(rg, 'wave_pass', 'bump_pass', 0)


wireframecube_rendering_config = 
{
	main_rendering = 
	{
		fx = 
		{
			shaders = 
			{
				{ path='color_vs.hlsl',mode=SHADER_NOT_COMPILED },
				{ path='color_ps.hlsl',mode=SHADER_NOT_COMPILED }
			},
			rs_in = 
			{
				{ ope=RENDERSTATE_OPE_ENABLEZBUFFER, value="true"	}		
			},
			rs_out =
			{
				{ ope=RENDERSTATE_OPE_ENABLEZBUFFER, value="false" }
			}
		},

		rendering_order = 10000,

		shaders_params = 
		{ 
			{ param_name = "color", shader_index = 1, register = 0 }
		}
	}
}

wireframe_passes_bindings = 
{
	binding_0 = 
	{
		target_pass_id = 'texture_pass',
		rendering_id = 'main_rendering',
		lit_shader_update_func = nil
	}
}

vertex_array = 
{
	{-0.9999, 0.9999, 0.9999},
	{0.9999, 0.9999, 0.9999},
	{0.9999, -0.9999, 0.9999},
	{-0.9999, -0.9999, 0.9999},
	{-0.9999, 0.9999, -0.9999},
	{0.9999, 0.9999, -0.9999},
	{0.9999, -0.9999, -0.9999},
	{-0.9999, -0.9999, -0.9999},
}

indexes_array = 
{
	{ 0, 1 },
	{ 1, 2 },
	{ 2, 3 },
	{ 3, 0 },
	{ 4, 5 },
	{ 5, 6 },
	{ 6, 7 },
	{ 7, 4 },
	{ 0, 4 },
	{ 1, 5 },
	{ 2, 6 },
	{ 3, 7 }
}

wireframecube_entity, wireframecube_renderer = commons.create_rendered_linemeshe(wireframecube_rendering_config, "wireframe_cube", wireframe_passes_bindings, vertex_array, indexes_array)
wireframecube_renderer:register_to_rendering(rg)

wireframecube_renderer:set_shaderrealvector( "texture_pass", 'color', 1.0, 0.0, 0.0, 1.0 )

eg:add_child('sphere', 'wireframe_cube', wireframecube_entity)

--eg:add_child('root', 'wireframe_cube', wireframecube_entity)
--model.setup_rawtransformationschain(wireframecube_entity, 'wireframe_cube', 0)
--model.move.setpos('wireframe_cube', 0.0, skydome.innerRadius + 15.9, 0.0)





model.env.setbkcolor('texture_pass', 0.05,0.05,0.09)
	

model.env.light.setstate( TRUE )
	
model.env.light.setsphericaldir(0.0, 8.0)

model.env.ambientlight.setcolor(0.1, 0.1, 0.1)
	


model.camera.speed = 50.0


model.env.setgravity(1)


fps_yaw=SyncAngle()
fps_pitch=SyncAngle()
fps_yaw:init_fromtimeaspectof(root_entity, 0.0)
fps_pitch:init_fromtimeaspectof(root_entity,0.0)




-- ///////////////////////////////



-- rg:update_renderingqueues()

mouse_right = FALSE



g:add_mousemovecb( "onmousemove",
function( xm, ym, dx, dy )  

	if hmi_mode == TRUE then
		gui:on_mousemove( xm, ym, dx, dy )
	else

	  if mouse_right == TRUE then
	    move_light_vector(dx, dy)
	  else
        local mvt_info = { model.camera.mvt:read() }

	    fps_yaw:inc(-dx / 1.0)
	    fps_pitch:inc(-dy / 1.0)

	    model.camera.mvt:update(fps_yaw:get_value(),fps_pitch:get_value(),mvt_info[3],mvt_info[4],mvt_info[5],mvt_info[6], mvt_info[7], mvt_info[8], mvt_info[9])
	  end
	end
end)

g:add_mouseleftbuttondowncb( "onmouselbdown",
function( xm, ym )
	if hmi_mode == TRUE then
		gui:on_mouseleftbuttondown() 
	end
end)

g:add_mouseleftbuttonupcb( "onmouselbup",
function( xm, ym )
	if hmi_mode == TRUE then
		gui:on_mouseleftbuttonup()
	end
end)


g:add_mouserightbuttondowncb( "onmouserbdown",
function( xm, ym )
	if hmi_mode == TRUE then
	  gui:on_mouserightbuttondown()
	else
	  mouse_right = TRUE
	end
end)

g:add_mouserightbuttonupcb( "onmouserbup",
function( xm, ym )
	if hmi_mode == TRUE then
	  gui:on_mouserightbuttonup()
	else
	  mouse_right = FALSE
	end
end)

g:add_keydowncb( "keydown",
function( key )

  --Q key
  if key == 81 then 

    if hmi_mode == FALSE then
      local mvt_info = { model.camera.mvt:read() }
	  model.camera.mvt:update(fps_yaw:get_value(),fps_pitch:get_value(),mvt_info[3],mvt_info[4],mvt_info[5],mvt_info[6], mvt_info[7], mvt_info[8], 12.0) 
 	end
  
      
  --W key
  elseif key == 87 then

    if hmi_mode == FALSE then
      local mvt_info = { model.camera.mvt:read() }
	  model.camera.mvt:update(fps_yaw:get_value(),fps_pitch:get_value(),mvt_info[3],mvt_info[4],mvt_info[5],mvt_info[6], mvt_info[7], mvt_info[8], -12.0) 
	end

  end

end)

g:add_keyupcb( "keyup",
function( key )  



  --Q key
  if key == 81 then

    if hmi_mode==FALSE then
      local mvt_info = { model.camera.mvt:read() }
	  model.camera.mvt:update(fps_yaw:get_value(),fps_pitch:get_value(),mvt_info[3],mvt_info[4],mvt_info[5],mvt_info[6], mvt_info[7], mvt_info[8], 0.0) 
    end
    
  --W key
  elseif key == 87 then

    if hmi_mode == FALSE then
      local mvt_info = { model.camera.mvt:read() }	  
	  model.camera.mvt:update(fps_yaw:get_value(),fps_pitch:get_value(),mvt_info[3],mvt_info[4],mvt_info[5],mvt_info[6], mvt_info[7], mvt_info[8], 0.0) 
	end
  -- VK_F1
  elseif key == 112 then

    if hmi_mode == FALSE then
      hmi_mode = TRUE
      g:show_mousecursor(TRUE)
      g:set_mousecursorcircularmode(FALSE)


    elseif hmi_mode==TRUE then
      hmi_mode=FALSE
      g:show_mousecursor(FALSE)
      g:set_mousecursorcircularmode(TRUE)
    end
  
  end

end)


g:add_appruncb( "run",
function()  

    local mvt_info = { model.camera.mvt:read() }

	local localpos = Vector(0.0, 0.0, 0.0, 1.0)
    local collimator_pos_x, collimator_pos_y, is_behind = spherebump.models['sphere'].entity:project_localpoint(localpos)
	if is_behind == TRUE then

	  -- hide by setting 2D pos out of the viewport
	  collimator_pos_x = 10.0
	  collimator_pos_y = 10.0
	end

	local collimator_renderer = impostors.models['collimator']['renderer']
	collimator_renderer:set_shaderrealvector( 'texture_pass', 'pos2D', collimator_pos_x, collimator_pos_y, 0.0, 0.0 )
	

	
    local objectid_text_pos_x = collimator_pos_x - 0.05;
    local objectid_text_pos_y = collimator_pos_y + 0.1;
    local objectid_tpos_x = model.renderer_infos[2] * 0.5 * (objectid_text_pos_x + 1.0);
    local objectid_tpos_y = model.renderer_infos[3] - (model.renderer_infos[3] * 0.5 * (objectid_text_pos_y + 1.0));
	objectid_renderer:update(objectid_tpos_x, objectid_tpos_y, 0, 255, 0, "Sphere")


	
    local objectdistance_text_pos_x = collimator_pos_x - 0.05;
    local objectdistance_text_pos_y = collimator_pos_y - 0.1;
    local objectdistance_tpos_x = model.renderer_infos[2] * 0.5 * (objectdistance_text_pos_x + 1.0);
    local objectdistance_tpos_y = model.renderer_infos[3] - (model.renderer_infos[3] * 0.5 * (objectdistance_text_pos_y + 1.0));

	local distance_to_object = spherebump.models['sphere'].entity:localpoint_distancefromcamera(localpos)
	objectdistance_renderer:update(objectdistance_tpos_x, objectdistance_tpos_y, 0, 255, 0, g:format_real(distance_to_object, 2))
	


	
	time_infos = { root_entity:read_timemanager() }
	output_infos = renderer:descr() .." "..time_infos[3].. " fps"

	text_renderer:update(520, 30, 255, 0, 0, output_infos)


	text2_renderer:update(520, 60, 255, 0, 0, resources_event)

	


    if waves_inc == TRUE then

        if waves:get_value() < 200.0 then
            waves:inc( 1.0 )
        else
            waves_inc = FALSE
		end
    else

        if waves:get_value() > 0.0 then
            waves:dec( 1.0 )
        else
            waves_inc = TRUE
		end
	end

	rg:set_viewportquadshaderrealvector('wave_pass', 'waves', waves:get_value(), 0.0, 0.0, 0.0)

end)



gui:load_layout("main.layout","guiskins/layouts/main_widgets.conf")
gui:set_layout("main.layout")
gui:show_gui(TRUE)


g:show_mousecursor(FALSE)
g:set_mousecursorcircularmode(TRUE)



add_cube = function()

  local instance_name = 'cube_'..cube_instance

  local cube_pos_mat = Matrix()
  cube_pos_mat:translation( 0.0, skydome.innerRadius + 37.9, 0.0 )

  metalcube.view.load(instance_name, cube_pos_mat, metalcube_passes_bindings)
  eg:add_child('root', instance_name, metalcube.models[instance_name].entity)

  cube_instance = cube_instance + 1

  -- here, context = cube entity instance name
  g:activate_resourcessystem(instance_name)

end



destroy_all_cubes = function()

  j = 0
  while j < cube_instance do 

    eg:unregister_rigidbody(metalcube.models['cube_'..j].entity)

    metalcube.view.unload('cube_'..j)
	j = j + 1
  end
  cube_instance = 0
end



gui:add_pushbuttonclickedcb( "onpushbutton",
function( layout, widget )
  
  if layout == 'main.layout' and widget == "Quit" then
    g:quit()
  
  elseif layout == 'main.layout' and widget == "Button_Create" then

	add_cube()	
	

  elseif layout == 'main.layout' and widget == "Button_Destroy" then
	
	destroy_all_cubes()	

	rg:update_renderingqueues()
	
  end
end)


set_water_bump = function(bias)
  waterquad.models['water']['renderer']:set_shaderrealvector('bump_pass', 'bump_bias', bias, 0.0, 0.0, 0.0)
  waterquad.bump_bias = bias -- memorize new value for the case where model.env.update_entities_shaders() is called
end

set_water_color = function(r, g, b)
  rg:set_viewportquadshaderrealvector('final_pass', 'water_color', r, g, b, 1.0)
end

set_water_spec = function(s)
  rg:set_viewportquadshaderrealvector('final_pass', 'water_specular_power', s, 0.0, 0.0, 0.0)
end

dmode = function(mode)
  rg:set_viewportquadshaderrealvector('final_pass', 'debug_mode', mode, 0.0, 0.0, 0.0)
end

g:signal_renderscenebegin("eg")

g:activate_resourcessystem("init")

set_water_bump(5.0)

