

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


g:print('Current renderer is '..model.renderer_infos[1]..', '..model.renderer_infos[2]..'x'..model.renderer_infos[3])
renderer_infos = {renderer:descr()}
g:print('Current resolution is '..renderer_infos[2].." "..renderer_infos[3])


mvt_mod = Module("mvtmod", "mvts")
mvt_mod:load()
g:print(mvt_mod:get_descr().. ' loaded')


commons.init_final_pass_water_mask(rg, 'final_pass')

rg:create_child('final_pass', 'texture_pass', 0)
rg:create_child('final_pass', 'texturemirror_pass', 1)




rg:create_child('final_pass', 'bump_pass', 2, RENDERPURPOSE_FLOATVECTOR)
rg:set_pass_depthclearstate('bump_pass', TRUE)
rg:set_pass_targetclearstate('bump_pass', TRUE)
rg:set_pass_targetclearcolor('bump_pass', 0, 0, 0, 0)



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




model.createmaincamera(0.0, 3.0, 0.0, mvt_mod)
eg:set_camera(model.camera.entity)

waves=SyncAngle()
waves_inc = TRUE

waves:init_fromtimeaspectof(root_entity,0.0)

text_renderer=TextRendering()
text_renderer:configure(root_entity, "fps", 320, 30, 255, 0, 255, "??? fps")


skydome_passes_config = 
{
	texture_pass = 
	{
		rendering_id = 'main_rendering',
		lit_shader_update_func = skydome.update_from_scene_env
	},
	texturemirror_pass =
	{
		rendering_id = 'mirror_rendering',
		lit_shader_update_func = skydome.update_from_scene_env_mirror	
	}
}
skydome.view.load('dome', skydome_passes_config, 'root')
model.move.setpos('dome', 0.0, 0.0, 0.0)

land_passes_config = 
{
	texture_pass = 
	{
		rendering_id = 'lit_rendering',
		lit_shader_update_func = land.update_from_scene_env
	},
	texturemirror_pass =
	{
		rendering_id = 'lit_mirror_rendering',
		lit_shader_update_func = land.update_from_scene_env_mirror	
	}
}
land.view.load('l', {x = 0.0, y = skydome.innerRadius, z = 0.0}, land_passes_config, 'root')


metalcube_passes_config = 
{
	texture_pass = 
	{
		rendering_id = 'lit_rendering',
		lit_shader_update_func = metalcube.update_from_scene_env
	},
	texturemirror_pass =
	{
		rendering_id = 'lit_mirror_rendering',
		lit_shader_update_func = metalcube.update_from_scene_env_mirror	
	}
}

metalcube.view.load('cube', {x = 0.0, y = skydome.innerRadius + 0.7, z = 0.0}, metalcube_passes_config, 'root' )


spherebump_passes_config = 
{
	texture_pass = 
	{
	    rendering_id = 'lit_rendering',
		lit_shader_update_func = spherebump.update_from_scene_env
	},
	texturemirror_pass =
	{
		rendering_id = 'lit_mirror_rendering',
		lit_shader_update_func = spherebump.update_from_scene_env_mirror	
	}
}
spherebump.view.load('sphere', {x = 0.0, y = skydome.innerRadius + 5.0, z = 0.0}, spherebump_passes_config, 'root' )


waterquad_passes_config = 
{
	texture_pass = 
	{
		rendering_id = 'main_rendering',
		lit_shader_update_func = waterquad.update_from_scene_env
	},
	bump_pass = 
	{
		rendering_id = 'bump_rendering',
		lit_shader_update_func = waterquad.update_from_scene_env_bump
	}
}
waterquad.view.load('water', waterquad_passes_config, 'root')
model.move.setpos('water', 0.0, skydome.innerRadius, 0.0)

waterquad.models['water']['renderer']:set_passnodetexturefrompass(rg, 'wave_pass', 'bump_pass', 0)


model.env.setbkcolor('texture_pass', 0.05,0.05,0.09)
	
model.camera.mvt:set_pos(0.0, 8.0, 30.0)

model.env.light.setstate( TRUE )
	
model.env.light.setsphericaldir(0.0,50.0)

model.env.ambientlight.setcolor(0.1, 0.1, 0.1)
	
model.camera.mvt:set_pos(0.0, skydome.innerRadius + 8.0, 20.0)
--model.camera.mvt:set_pos(0.0, 8.0, 0.0)
model.camera.speed = 50.0


model.env.setgravity(1)





-- ///////////////////////////////



rg:update_renderingqueues()

mouse_right = FALSE



g:add_mousemovecb( "onmousemove",
function( xm, ym, dx, dy )  

	if hmi_mode == TRUE then
		gui:on_mousemove( xm, ym, dx, dy )
	else
      local mvt_info = { model.camera.mvt:read() }
	  if mouse_right == FALSE then
  	    model.camera.mvt:update(mvt_info[4],mvt_info[1],mvt_info[2],mvt_info[3],-dy / 4.0,-dx / 4.0, 0)
      else
	    model.camera.mvt:update(mvt_info[4],mvt_info[1],mvt_info[2],mvt_info[3],0,0,-dx)
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
	  model.camera.mvt:update(5.0,mvt_info[1],mvt_info[2],mvt_info[3],0,0,0)
 	end
  
      
  --W key
  elseif key == 87 then

    if hmi_mode == FALSE then
      local mvt_info = { model.camera.mvt:read() }
	  model.camera.mvt:update(-5.0,mvt_info[1],mvt_info[2],mvt_info[3],0,0,0)
	end

  end

end)

g:add_keyupcb( "keyup",
function( key )  



  --Q key
  if key == 81 then

    if hmi_mode==FALSE then
      local mvt_info = { model.camera.mvt:read() }
	  model.camera.mvt:update(0.0,mvt_info[1],mvt_info[2],mvt_info[3],0,0,0)
    end
    
  --W key
  elseif key == 87 then

    if hmi_mode == FALSE then
      local mvt_info = { model.camera.mvt:read() }
	  model.camera.mvt:update(0.0,mvt_info[1],mvt_info[2],mvt_info[3],0,0,0)
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
    model.camera.mvt:update(mvt_info[4],mvt_info[1],mvt_info[2],mvt_info[3],0,0,0)

	time_infos = { root_entity:read_timemanager() }
	output_infos = renderer:descr() .." "..time_infos[3].. " fps"

	text_renderer:update(520, 30, 255, 0, 0, output_infos)


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



gui=Gui()
gui:init()
gui:set_resourcespath("./testskin")
gui:load_scheme("AlfiskoSkin.scheme")
gui:load_layout("main.layout","testskin/layouts/main_widgets.conf")
gui:set_layout("main.layout")
gui:show_gui(TRUE)


g:show_mousecursor(FALSE)
g:set_mousecursorcircularmode(TRUE)



add_cube = function()

  local instance_name = 'cube_'..cube_instance
  metalcube.view.load(instance_name, {x = 0.0, y = skydome.innerRadius + 37.9, z = 0.0}, metalcube_passes_config, 'root' )

  cube_instance = cube_instance + 1

end



destroy_all_cubes = function()

  j = 0
  while j < cube_instance do 
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
	rg:update_renderingqueues()

  elseif layout == 'main.layout' and widget == "Button_Destroy" then
	
	destroy_all_cubes()	

	rg:update_renderingqueues()
	
  end
end)


set_water_bump = function(bias)
	waterquad.models['water']['renderer']:set_shaderrealvector('bump_pass', 'bump_bias', bias, 0.0, 0.0, 0.0)
end

g:signal_renderscenebegin("eg")

