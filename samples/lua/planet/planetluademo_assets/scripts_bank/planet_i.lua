
include('spacebox_model.lua')
include('spherebump_model.lua')
include('bellerophon_model.lua')
include('planet_model.lua')


local speed_factor = 90.0


update_planet_lights = function( p_planet_specific_config )

  p_planet_specific_config:enable_light( 0, TRUE )
  p_planet_specific_config:set_lightcolor( 0,  environment.ambient_light.r, environment.ambient_light.g, environment.ambient_light.b )


  p_planet_specific_config:enable_light( 1, environment.lights_enabled.x )
  p_planet_specific_config:set_lightcolor( 1, environment.light0.color.r, environment.light0.color.g, environment.light0.color.b )
  p_planet_specific_config:set_lightdir( 1, environment.light0.direction.x, environment.light0.direction.y, environment.light0.direction.z )

  p_planet_specific_config:updated()

end

set_camera = function(camera)

  if camera == free_cam then

    eg:set_camera(model.camera.entity)
    gui:show_mousecursor(FALSE)
    g:set_mousecursorcircularmode(TRUE)

  elseif camera == ship_cam then

  

    --eg:set_camera(camera2_entity)
    --gui:show_mousecursor(TRUE)
	--g:set_mousecursorcircularmode(FALSE)
  end
end

free_cam = 0
ship_cam = 1

current_cam = free_cam
-- current_cam = ship_cam



g:print('Current renderer is '..model.renderer_infos[1]..', '..model.renderer_infos[2]..'x'..model.renderer_infos[3])
renderer_infos = {renderer:descr()}
g:print('Current resolution is '..renderer_infos[2].." "..renderer_infos[3])


mvt_mod = Module("mvtmod", "mvts")
mvt_mod:load()
g:print(mvt_mod:get_descr().. ' loaded')

commons.init_final_pass(rg, 'final_pass')

rg:create_child('final_pass', 'texture_pass', 0)
rg:set_pass_targetclearstate( 'texture_pass', FALSE )
rg:set_pass_depthclearstate( 'texture_pass', TRUE )

text_renderer=TextRendering()
text_renderer:configure(root_entity, "fps", 320, 30, 255, 0, 255, "??? fps")

text2_renderer=TextRendering()
text2_renderer:configure(root_entity, "datetime", 320, 70, 255, 0, 255, "xxxxxxx")


text3_renderer=TextRendering()
text3_renderer:configure(root_entity, "timescale", 320, 110, 255, 0, 255, "xxxxxxx")


text4_renderer=TextRendering()
text4_renderer:configure(root_entity, "planets_infos", 450, 70, 255, 0, 255, "xxxxxxx")

root_entity:add_aspect(PHYSICS_ASPECT)
root_entity:configure_world(GRAVITY_DISABLED, 1.0, 1.0, 1.0)


model.createmaincamera(0.0, 0.0, 0.0, mvt_mod)



mouse_right = FALSE




g:add_mousemovecb( "onmousemove",
function( xm, ym, dx, dy )  

  if current_cam == free_cam then

    local mvt_info = { model.camera.mvt:read() }
    if mouse_right == FALSE then
  	  model.camera.mvt:update(mvt_info[4],mvt_info[1],mvt_info[2],mvt_info[3],-dy / 4.0,-dx / 4.0, 0)
    else
	  model.camera.mvt:update(mvt_info[4],mvt_info[1],mvt_info[2],mvt_info[3],0,0,-dx)
    end

  end


  gui:on_mousemove( xm, ym, dx, dy )
	
end)

g:add_mouseleftbuttondowncb( "onmouselbdown",
function( xm, ym )
  gui:on_mouseleftbuttondown()
end)

g:add_mouseleftbuttonupcb( "onmouselbup",
function( xm, ym )
  gui:on_mouseleftbuttonup()
end)


g:add_mouserightbuttondowncb( "onmouserbdown",
function( xm, ym )
  mouse_right = TRUE
  gui:on_mouserightbuttondown()
end)

g:add_mouserightbuttonupcb( "onmouserbup",
function( xm, ym )
  mouse_right = FALSE
  gui:on_mouserightbuttonup()
end)

g:add_keydowncb( "keydown",
function( key )

  --Q key
  if key == 81 then 

    if current_cam == free_cam then
      local mvt_info = { model.camera.mvt:read() }
	  model.camera.mvt:update(speed_factor,mvt_info[1],mvt_info[2],mvt_info[3],0,0,0)
	else

	end
  --W key
  elseif key == 87 then

    --if current_cam == free_cam then
      local mvt_info = { model.camera.mvt:read() }
	  model.camera.mvt:update(-speed_factor,mvt_info[1],mvt_info[2],mvt_info[3],0,0,0)
	--else

	--end    
  
  elseif key == 16 then -- left shift

  elseif key == 65 then --'A'


  elseif key == 90 then --'Z'


  elseif key == 37 then --VK_LEFT


  elseif key == 38 then --VK_UP


  elseif key == 39 then --VK_RIGHT


  elseif key == 40 then --VK_DOWN


  elseif key == 68 then --'D'
    

  elseif key == 67 then --'C'
    

  elseif key == 70 then --'F'


  elseif key == 71 then --'G'


  elseif key == 86 then --'V'
    

  elseif key == 66 then --'B'
    
  else
	--g:print('key code = '..key)
  end

  gui:on_keydown( key )
end)

g:add_keyupcb( "keyup",
function( key )  

  --Q key
  if key == 81 then

    if current_cam == free_cam then
      local mvt_info = { model.camera.mvt:read() }
	  model.camera.mvt:update(0.0,mvt_info[1],mvt_info[2],mvt_info[3],0,0,0)
	else

	end    

  --W key
  elseif key == 87 then

    if current_cam == free_cam then

      local mvt_info = { model.camera.mvt:read() }
	  model.camera.mvt:update(0.0,mvt_info[1],mvt_info[2],mvt_info[3],0,0,0)
	else

	end    

  -- VK_F1
  elseif key == 112 then  


  elseif key == 16 then -- left shift
    

  elseif key == 76 then --'L'


  elseif key == 77 then --'M'


  elseif key == 65 then --'A'

  elseif key == 90 then --'Z'

  elseif key == 37 then --VK_LEFT

  elseif key == 38 then --VK_UP

  elseif key == 39 then --VK_RIGHT

  elseif key == 40 then --VK_DOWN

  elseif key == 68 then --'D'
    

  elseif key == 67 then --'C'
    

  elseif key == 70 then --'F'


  elseif key == 71 then --'G'


  elseif key == 86 then --'V'
    

  elseif key == 66 then --'B'
    
  else
    --g:print('key code = '..key) 
  end

  gui:on_keyup( key )
end)



g:add_appruncb( "run",
function()

  local mvt_info = { model.camera.mvt:read() }
  model.camera.mvt:update(mvt_info[4],mvt_info[1],mvt_info[2],mvt_info[3],0,0,0)

  local time_infos = { root_entity:read_timemanager() }
  output_infos = renderer:descr() .." "..time_infos[3].. " fps "

  text_renderer:update(10, 30, 255, 0, 0, output_infos)

  text2_renderer:update(10, 70, 255, 0, 0, time_infos[2])

  local timescale = commons.print_timescale(time_infos[1])

  text3_renderer:update(10, 110, 255, 0, 0, timescale)


end)

g:signal_renderscenebegin("eg")


spacebox_passes_config = 
{
	texture_pass = 
	{
		rendering_id = 'layer0_rendering',
		lit_shader_update_func = nil
	}
}
spaceboxmod.view.load('spacebox0', spacebox_passes_config, 'root')


spherebump_passes_config = 
{
	texture_pass = 
	{
	    rendering_id = 'lit_rendering',
		lit_shader_update_func = spherebump.update_from_scene_env
	}
}

spherebump.view.load('sphere', {x = 0.0, y = 0.0, z = -62.0}, spherebump_passes_config, 'root' )


bellerophon_passes_config = 
{
	texture_pass = 
	{
		rendering_id = 'lit_rendering',
		lit_shader_update_func = bellerophon.update_lit_from_scene_env
	}
}
bellerophon.view.load('ship', {x = -160.0, y = 0.0, z = -500.0 }, bellerophon_passes_config, 'root')
		

local planet_specific_config_descr =
{
	resources_path						= "planetluademo_assets/shaders_bank",
	climate_vshader						= "planet_ht_vs.cso",
	climate_pshader						= "planet_ht_ps.cso",
	planet_ray							= 6500.0,
	plains_amplitude					= 600.0,
	mountains_amplitude					= 16000.0,
	vertical_offset						= 20.0,
	mountains_offset					= 0.0,
	plains_seed1						= 8099.0,
	plains_seed2						= 5662.0,
	mix_seed1							= 3111.0,
	mix_seed2							= 498.0,
	terrainbump_factor					= 16.0,
	splat_transition_up_relative_alt	= 1.095,
	splat_transition_down_relative_alt	= 1.0040,
	splat_texture_resol					= 16,
	atmo_kr								= 0.0033,
	fog_alt_limit						= 30000.0,
	fog_density							= 0.000031,
	beach_limit							= 25.0,
	landplace_patch						= FALSE,
	enable_atmosphere					= TRUE,
	atmo_thickness                      = 160.0
}

planet_passes_config = 
{
	texture_pass = 
	{
		rendering_id = 'surface_rendering',
		lit_shader_update_func = nil
	},
	texture_pass = 
	{
		rendering_id = 'atmo_rendering',
		lit_shader_update_func = nil
	}
}


planet_name = 'Resurgam'
planetmod.view.load(planet_name, planet_passes_config, 'root', planet_specific_config_descr)


resurgam_planet_entity = planetmod.models[planet_name].entity
resurgam_planet_config = planetmod.models[planet_name].specific_config

resurgam_planet_entity:add_aspect(TRANSFORM_ASPECT)

planet_transform = RawTransform()
planet_transform:configure(resurgam_planet_entity)

planet_pos_mat = Matrix()
planet_pos_mat:translation( 0.0, 0.0, -40620000.0 )
planet_transform:add_matrix( "pos", planet_pos_mat )

g:print("Planet creation done...")


model.env.setbkcolor('texture_pass', 0.0,0.0,0.0)

model.camera.mvt:set_pos(0.0, 0.0, 10.0)


model.env.light.setstate( TRUE )
model.env.light.setdir(1.0, -0.4, 0.0)
model.env.ambientlight.setcolor(0.1, 0.1, 0.1)

model.env.fog.setdensity(0.0)

update_planet_lights( resurgam_planet_config)
resurgam_planet_config:updated()

gui=Gui()
gui:init()
gui:set_resourcespath("./xfskin2")
gui:load_scheme("xfskin.scheme")
gui:load_layout("timecontrol.layout","xfskin2/layouts/timecontrol_widgets.conf")
gui:set_layout("timecontrol.layout")
gui:show_gui(TRUE)


g:show_mousecursor(FALSE)
g:set_mousecursorcircularmode(TRUE)

set_camera(current_cam)