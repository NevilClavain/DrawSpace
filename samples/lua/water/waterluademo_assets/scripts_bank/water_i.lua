

include('land_model.lua')
include('skydome_model.lua')
include('waterquad_model.lua')
include('metalcube_model.lua')
include('spherebump_model.lua')


environment.reflector_pos.x = 0.0
environment.reflector_pos.y = skydome.innerRadius
environment.reflector_pos.z = 0.0

environment.reflector_normale.x = 0.0
environment.reflector_normale.y = 1.0
environment.reflector_normale.z = 0.0

hmi_mode=FALSE

cube_instance=0


resources_event = "..."

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

renderconfig=RenderConfig()
renderconfig:add_rendercontext(rendercontext)
rg:configure_pass_viewportquad_resources('final_pass',renderconfig)
rg:set_viewportquadshaderrealvector('final_pass', 'water_color', 1, 1, 1, 1.0)
rg:set_viewportquadshaderrealvector('final_pass', 'surface_normale', environment.reflector_normale.x, environment.reflector_normale.y, environment.reflector_normale.z, 1.0)
rg:set_viewportquadshaderrealvector('final_pass', 'debug_mode', 0.0, 0.0, 0.0, 0.0)


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

rg:create_child('final_pass', 'waterhalfvector_pass', 5)


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

text_renderer=TextRendering()
text_renderer:configure(root_entity, "fps", 320, 30, 255, 0, 255, "??? fps")


text2_renderer=TextRendering()
text2_renderer:configure(root_entity, "resource_infos", 320, 130, 255, 0, 255, "resources...")


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

metalcube.view.load('cube', {x = 0.0, y = skydome.innerRadius + 1.7, z = 0.0}, metalcube_passes_bindings)
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
spherebump.view.load('sphere', {x = 0.0, y = skydome.innerRadius + 5.0, z = 0.0}, spherebump_passes_bindings)
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


model.env.setbkcolor('texture_pass', 0.05,0.05,0.09)
	

model.env.light.setstate( TRUE )
	
model.env.light.setsphericaldir(0.0,50.0)

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
      local mvt_info = { model.camera.mvt:read() }

	  fps_yaw:inc(-dx / 1.0)
	  fps_pitch:inc(-dy / 1.0)

	  model.camera.mvt:update(fps_yaw:get_value(),fps_pitch:get_value(),mvt_info[3],mvt_info[4],mvt_info[5],mvt_info[6], mvt_info[7], mvt_info[8], mvt_info[9])
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


    --model.camera.mvt:update(mvt_info[4],mvt_info[1],mvt_info[2],mvt_info[3],0,0,0)
	--model.camera.mvt:update(mvt_info[1],mvt_info[2],mvt_info[3],mvt_info[4],mvt_info[5])

	
	time_infos = { root_entity:read_timemanager() }
	output_infos = renderer:descr() .." "..time_infos[3].. " fps"

	text_renderer:update(520, 30, 255, 0, 0, output_infos)

	text2_renderer:update(10, 150, 255, 0, 0, resources_event)


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
  metalcube.view.load(instance_name, {x = 0.0, y = skydome.innerRadius + 37.9, z = 0.0}, metalcube_passes_bindings)
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
end

set_water_color = function(r, g, b)
	rg:set_viewportquadshaderrealvector('final_pass', 'water_color', r, g, b, 1.0)
end

dmode = function(mode)
	rg:set_viewportquadshaderrealvector('final_pass', 'debug_mode', mode, 0.0, 0.0, 0.0)
end

g:signal_renderscenebegin("eg")

g:activate_resourcessystem("init")

set_water_bump(2.4)

