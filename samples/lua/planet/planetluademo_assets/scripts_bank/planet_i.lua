
include('spacebox_model.lua')
include('spherebump_model.lua')
include('bellerophon_model.lua')
include('planet_model.lua')


local speed_factor = 1500.0

local relative_ack = FALSE

local left_shift = FALSE
local left_ctrl = FALSE

local tab = FALSE

local time_factor = 0

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
         eg:register_rigidbody(bellerophon_entity)
         eg:register_rigidbody(spherebump.models['sphere'].entity)

         resurgam_planet_config:set_resourcesready( TRUE )
         resurgam_planet_config:updated()

       end
    else
       evt_out = "? : "..event       
    end
    resources_event = evt_out
end)


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

  elseif camera == ship_cam then

    eg:set_camera(camera2_entity)
  end
end

free_cam = 0
ship_cam = 1

--current_cam = free_cam
current_cam = ship_cam


ship_cam_fps_yaw=SyncAngle()
ship_cam_fps_pitch=SyncAngle()
ship_cam_fps_yaw:init_fromtimeaspectof(root_entity, 0.0)
ship_cam_fps_pitch:init_fromtimeaspectof(root_entity,0.0)



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

text5_renderer=TextRendering()
text5_renderer:configure(root_entity, "resource_infos", 320, 130, 255, 0, 255, "resources...")


root_entity:add_aspect(PHYSICS_ASPECT)
root_entity:configure_world(GRAVITY_DISABLED, 1.0, 1.0, 1.0)


model.createmainfreecamera(0.0, 0.0, 0.0, mvt_mod)
eg:add_child('root','model.camera.entity',model.camera.entity)


mouse_right = FALSE




g:add_mousemovecb( "onmousemove",
function( xm, ym, dx, dy )  

  if current_cam == free_cam and left_ctrl == FALSE then

    local mvt_info = { model.camera.mvt:read() }
    if mouse_right == FALSE then
  	  model.camera.mvt:update(mvt_info[4],mvt_info[1],mvt_info[2],mvt_info[3],-dy / 4.0,-dx / 4.0, 0)
    else
	  model.camera.mvt:update(mvt_info[4],mvt_info[1],mvt_info[2],mvt_info[3],0,0,-dx)
    end

    if time_factor ~= 0 then
      root_entity:update_timescale(NORMAL_TIME)
    end

  elseif current_cam == ship_cam and left_ctrl == FALSE then

    local mvt_info = { camera2_pos:read() }

	ship_cam_fps_yaw:inc(-dx / 1.0)
	ship_cam_fps_pitch:inc(-dy / 1.0)

    camera2_pos:update(ship_cam_fps_yaw:get_value(),ship_cam_fps_pitch:get_value(),mvt_info[3],mvt_info[4],mvt_info[5],mvt_info[6], mvt_info[7], mvt_info[8], mvt_info[9])

    if time_factor ~= 0 then
      root_entity:update_timescale(NORMAL_TIME)
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

      if left_shift == TRUE then
        model.camera.mvt:update(speed_factor * 3000.0, mvt_info[1],mvt_info[2],mvt_info[3],0,0,0)
      elseif tab == TRUE then
        model.camera.mvt:update(speed_factor * 150.0, mvt_info[1],mvt_info[2],mvt_info[3],0,0,0)
      else
        model.camera.mvt:update(speed_factor,mvt_info[1],mvt_info[2],mvt_info[3],0,0,0)
      end
	  
	else
      bellerophon_rigibody_transform:update_forcestate("main prop", TRUE)
	end
  --W key
  elseif key == 87 then

    if current_cam == free_cam then
      local mvt_info = { model.camera.mvt:read() }

      if left_shift == TRUE then
        model.camera.mvt:update(-speed_factor * 3000.0, mvt_info[1],mvt_info[2],mvt_info[3],0,0,0)
      elseif tab == TRUE then
        model.camera.mvt:update(-speed_factor * 150.0, mvt_info[1],mvt_info[2],mvt_info[3],0,0,0)
      else
        model.camera.mvt:update(-speed_factor,mvt_info[1],mvt_info[2],mvt_info[3],0,0,0)
      end
	  
	else
      bellerophon_rigibody_transform:update_forcestate("reverse prop", TRUE)
	end    
  
  elseif key == 16 then -- left shift

    left_shift = TRUE

  elseif key == 9 then -- tab

    tab = TRUE

  elseif key == 65 then --'A'
    bellerophon_rigibody_transform:update_torquestate("roll_left", TRUE)
    root_entity:update_timescale(NORMAL_TIME)

  elseif key == 90 then --'Z'
    bellerophon_rigibody_transform:update_torquestate("roll_right", TRUE)
    root_entity:update_timescale(NORMAL_TIME)

  elseif key == 37 then --VK_LEFT
    bellerophon_rigibody_transform:update_torquestate("yaw_left", TRUE)
    root_entity:update_timescale(NORMAL_TIME)

  elseif key == 38 then --VK_UP
    bellerophon_rigibody_transform:update_torquestate("pitch_down", TRUE)
    root_entity:update_timescale(NORMAL_TIME)

  elseif key == 39 then --VK_RIGHT
    bellerophon_rigibody_transform:update_torquestate("yaw_right", TRUE)
    root_entity:update_timescale(NORMAL_TIME)

  elseif key == 40 then --VK_DOWN
    bellerophon_rigibody_transform:update_torquestate("pitch_up", TRUE)
    root_entity:update_timescale(NORMAL_TIME)

  elseif key == 68 then --'D'
    

  elseif key == 67 then --'C'
    

  elseif key == 70 then --'F'


  elseif key == 71 then --'G'


  elseif key == 86 then --'V'
    

  elseif key == 66 then --'B'

  elseif key == 17 then --left CTRL

    gui:show_mousecursor(TRUE)
	g:set_mousecursorcircularmode(FALSE)

    left_ctrl = TRUE

    
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
      bellerophon_rigibody_transform:update_forcestate("main prop", FALSE)
	end    

  --W key
  elseif key == 87 then

    if current_cam == free_cam then

      local mvt_info = { model.camera.mvt:read() }
	  model.camera.mvt:update(0.0,mvt_info[1],mvt_info[2],mvt_info[3],0,0,0)
	else
      bellerophon_rigibody_transform:update_forcestate("reverse prop", FALSE)
	end    

  -- VK_F1
  elseif key == 112 then  

    current_cam = current_cam + 1
	if current_cam == 2 then
	  current_cam = 0
	end

	set_camera(current_cam)


  elseif key == 16 then -- left shift

    left_shift = FALSE

  elseif key == 9 then -- tab

    tab = FALSE
    
  elseif key == 76 then --'L'
    bellerophon_rigibody_transform:zero_speed()

  elseif key == 77 then --'M'
    bellerophon_rigibody_transform:zero_angularspeed()

  elseif key == 65 then --'A'
    bellerophon_rigibody_transform:update_torquestate("roll_left", FALSE)
    
  elseif key == 90 then --'Z'
    bellerophon_rigibody_transform:update_torquestate("roll_right", FALSE)
    
  elseif key == 37 then --VK_LEFT
    bellerophon_rigibody_transform:update_torquestate("yaw_left", FALSE)
    
  elseif key == 38 then --VK_UP
    bellerophon_rigibody_transform:update_torquestate("pitch_down", FALSE)
    
  elseif key == 39 then --VK_RIGHT
    bellerophon_rigibody_transform:update_torquestate("yaw_right", FALSE)
    
  elseif key == 40 then --VK_DOWN
    bellerophon_rigibody_transform:update_torquestate("pitch_up", FALSE)
    

  elseif key == 68 then --'D'
    

  elseif key == 67 then --'C'
    

  elseif key == 70 then --'F'


  elseif key == 71 then --'G'


  elseif key == 86 then --'V'
    

  elseif key == 66 then --'B'

  elseif key == 17 then --left CTRL

    gui:show_mousecursor(FALSE)
    g:set_mousecursorcircularmode(TRUE)

    left_ctrl = FALSE
    
  else
    --g:print('key code = '..key) 
  end

  gui:on_keyup( key )
end)



g:add_appruncb( "run",
function()


  local time_infos = { root_entity:read_timemanager() }
  output_infos = renderer:descr() .." "..time_infos[3].. " fps "

  text_renderer:update(10, 30, 255, 0, 0, output_infos)

  text2_renderer:update(10, 70, 255, 0, 0, time_infos[2])

  local timescale = commons.print_timescale(time_infos[1])

  time_factor = time_infos[1]

  --[[
  local planet_rev_angle = planet_revol:read_currentangle()
  text3_renderer:update(10, 110, 255, 0, 0, timescale..' revol angle = '..planet_rev_angle)
  ]]

  text3_renderer:update(10, 110, 255, 0, 0, timescale)

  text5_renderer:update(10, 150, 255, 0, 0, resources_event)

  -- display planet infos 

  local planet_infos = commons.procedural.planet.read_infos(resurgam_planet_config)

  local current_cam_id

  if current_cam == free_cam then
    current_cam_id = "model.camera"
  else
    current_cam_id = "ship_camera"
  end

  local relative_state
  
  local is_relative = planet_infos['viewsInfos'][current_cam_id]['relative']

  if is_relative ~= 0 then
    
    local altitude = planet_infos["viewsInfos"][current_cam_id]["altitude"]
	local display_altitude
	local altitude_units

	if altitude > 5000.0 then
	  display_altitude = g:format_real(altitude / 1000.0,2)
	  altitude_unit = " km"
	else
	  display_altitude = g:format_real(altitude,1)
	  altitude_unit = " m"
	end

    --[[
    relative_state = "RELATIVE"..' '..g:format_real(planet_infos["viewsInfos"][current_cam_id]["relative_altitude"],4)..' '..display_altitude..altitude_unit..
                    ' pminh='..g:format_real(planet_infos["viewsInfos"][current_cam_id]["current_patch_min_height"],2)..
                    ' pmaxh='..g:format_real(planet_infos["viewsInfos"][current_cam_id]["current_patch_max_height"],2)..
                    ' pcurrh='..g:format_real(planet_infos["viewsInfos"][current_cam_id]["current_patch_current_height"],2)..
                    ' xloc='..g:format_real(planet_infos["viewsInfos"][current_cam_id]["camera_local_pos_x"],2)..
                    ' yloc='..g:format_real(planet_infos["viewsInfos"][current_cam_id]["camera_local_pos_y"],2)..
                    ' zloc='..g:format_real(planet_infos["viewsInfos"][current_cam_id]["camera_local_pos_z"],2)
                    ]]

    relative_state = "RELATIVE"..' '..g:format_real(planet_infos["viewsInfos"][current_cam_id]["relative_altitude"],4)..' '..display_altitude..altitude_unit                    
                    


    if relative_ack == FALSE then

      -- transition
      if current_cam == ship_cam then
      
        eg:unregister_rigidbody(bellerophon_entity)
        eg:remove('camera2_entity')
        eg:remove('ship')
    
        eg:add_child(planet_name, 'ship', bellerophon_entity)
        eg:add_child('ship','camera2_entity', camera2_entity)
        eg:register_rigidbody(bellerophon_entity)

        set_camera(current_cam)

      elseif current_cam == free_cam then

        eg:remove('model.camera.entity')
        eg:add_child(planet_name,'model.camera.entity',model.camera.entity)
        
        set_camera(current_cam)

      end
      
      relative_ack = TRUE
    end

  else
    relative_state = ""


    if relative_ack == TRUE then

      -- transition
      if current_cam == ship_cam then

        eg:unregister_rigidbody(bellerophon_entity)
        eg:remove('camera2_entity')
        eg:remove('ship')
    
        eg:add_child('root', 'ship', bellerophon_entity)
        eg:add_child('ship','camera2_entity', camera2_entity)
        eg:register_rigidbody(bellerophon_entity)

        set_camera(current_cam)

      elseif current_cam == free_cam then

        eg:remove('model.camera.entity')
        eg:add_child('root','model.camera.entity',model.camera.entity)
        
        set_camera(current_cam)


      end

      relative_ack = FALSE
    end
  end
  
 
  text4_renderer:update(10, 90, 255, 0, 0, 'cam_id=' ..current_cam_id..' subpasses='..planet_infos['delayedSingleSubPassQueueSize']..
                                            ' LOD='..planet_infos["viewsInfos"][current_cam_id]["currentLOD"]..' '..relative_state)
                                            

  local mvt_info = { model.camera.mvt:read() }
  model.camera.mvt:update(mvt_info[4],mvt_info[1],mvt_info[2],mvt_info[3],0,0,0)

end)

g:signal_renderscenebegin("eg")


atmo = function( state )
	resurgam_planet_config:enable_atmosphere( state )
	resurgam_planet_config:updated()
end


spacebox_passes_bindings = 
{
	binding_0 = 
	{
        target_pass_id = 'texture_pass',
		rendering_id = 'spacebox_rendering',
		lit_shader_update_func = nil
	}
}
spaceboxmod.view.load('spacebox0', spacebox_passes_bindings)
model.setup_rawtransformationschain(spaceboxmod.models['spacebox0'].entity, 'spacebox0', 0, {x = 1000.0, y = 1000.0, z = 1000.0 } )

eg:add_child('root', 'spacebox0', spaceboxmod.models['spacebox0'].entity)


spherebump_passes_bindings = 
{
	binding_0 = 
	{
        target_pass_id = 'texture_pass',
	    rendering_id = 'lit_rendering',
		lit_shader_update_func = spherebump.update_from_scene_env
	}
}

spherebump.view.load('sphere', {x = 0.0, y = 0.0, z = -62.0}, spherebump_passes_bindings)

eg:add_child('root', 'sphere', spherebump.models['sphere'].entity)


bellerophon_passes_bindings = 
{
	binding_0 = 
	{
        target_pass_id = 'texture_pass',
		rendering_id = 'lit_rendering',
		lit_shader_update_func = bellerophon.update_lit_from_scene_env
	}
}
bellerophon.view.load('ship', {x = -160.0, y = 0.0, z = -500.0 }, bellerophon_passes_bindings)

bellerophon_entity = bellerophon.models['ship'].entity

eg:add_child('root', 'ship', bellerophon_entity)

bellerophon_entity:add_aspect(INFOS_ASPECT)
bellerophon_entity:setup_info( "entity_name", "Bellorophon" )


bellerophon_rigibody_transform = bellerophon.models['ship'].rigibody_transform

bellerophon_rigibody_transform:configure_mass(50.0)

bellerophon_rigibody_transform:configure_force("main prop", Vector(0.0, 0.0, -5000.0, 0.0), LOCALE_FORCE, FALSE)
bellerophon_rigibody_transform:configure_force("reverse prop", Vector(0.0, 0.0, 5000.0, 0.0), LOCALE_FORCE, FALSE)

bellerophon_rigibody_transform:configure_torque("pitch_down", Vector(-150000.0, 0.0, 0.0, 0.0), LOCALE_FORCE, FALSE)
bellerophon_rigibody_transform:configure_torque("pitch_up", Vector(150000.0, 0.0, 0.0, 0.0), LOCALE_FORCE, FALSE)

bellerophon_rigibody_transform:configure_torque("roll_left", Vector(0.0, 0.0, 150000.0, 0.0), LOCALE_FORCE, FALSE)
bellerophon_rigibody_transform:configure_torque("roll_right", Vector(0.0, 0.0, -150000.0, 0.0), LOCALE_FORCE, FALSE)

bellerophon_rigibody_transform:configure_torque("yaw_left", Vector(0.0, 150000.0, 0.0, 0.0), LOCALE_FORCE, FALSE)
bellerophon_rigibody_transform:configure_torque("yaw_right", Vector(0.0, -150000.0, 0.0, 0.0), LOCALE_FORCE, FALSE)


local planet_specific_config_descr =
{
	resources_path						         = "planetluademo_assets/shaders_bank",
	climate_vshader						         = "planet_ht_vs.hlsl",
	climate_pshader						         = "planet_ht_ps.hlsl",
	climate_vshader_compiled			         = FALSE,
	climate_pshader_compiled			         = FALSE,
	collision_vshader					         = "planet_hm_vs.hlsl",
	collision_pshader					         = "planet_hm_ps.hlsl",
	collision_vshader_compiled			         = FALSE,
	collision_pshader_compiled			         = FALSE,

    enable_collisionmeshe_display                = FALSE, --TRUE,
    collisionmeshe_display_vshader               = "color_vs.hlsl",
    collisionmeshe_display_pshader               = "color_ps.hlsl",
    collisionmeshe_display_vshader_compiled      = FALSE,
    collisionmeshe_display_pshader_compiled      = FALSE,

	planet_ray							         = 6500.0,
    gravity_acc                                  = 9.81,
	plains_amplitude					         = 600.0,
	mountains_amplitude					         = 10000.0,
	vertical_offset						         = 20.0,
	mountains_offset					         = 0.0,
	plains_seed1						         = 89189.0,
	plains_seed2						         = 233.0,
	mix_seed1							         = 18901.0,
	mix_seed2							         = 17566.0,
	terrainbump_factor					         = 16.0,
	splat_transition_up_relative_alt	         = 1.095,
	splat_transition_down_relative_alt	         = 1.0040,
	splat_texture_resol					         = 16,
    zbuffer_activation_relative_alt              = 1.004,
	atmo_kr								         = 0.0033,
	fog_alt_limit						         = 30000.0,
	fog_density							         = 0.000031,
	beach_limit							         = 25.0,
	landplace_patch						         = FALSE,
	enable_atmosphere					         = TRUE,
	atmo_thickness                               = 160.0,
    flatclouds_altitude                          = 70.0
}

planet_passes_bindings = 
{
	binding_0 = 
	{
        target_pass_id = 'texture_pass',
		rendering_id = 'surface_rendering',
        --rendering_id = 'surface_wireframe_rendering',
		lit_shader_update_func = nil
	},
	binding_1 = 
	{
        target_pass_id = 'texture_pass',
		rendering_id = 'atmo_rendering',
		lit_shader_update_func = nil
	},
	binding_2 = 
	{
        target_pass_id = 'texture_pass',
		rendering_id = 'flatclouds_rendering',
		lit_shader_update_func = nil
	}
}


planet_name = 'Resurgam'
planetmod.view.load(planet_name, planet_passes_bindings, planet_specific_config_descr)

resurgam_planet_entity = planetmod.models[planet_name].entity
resurgam_planet_config = planetmod.models[planet_name].specific_config

eg:add_child('root', planet_name, resurgam_planet_entity)


resurgam_planet_entity:add_aspect(TRANSFORM_ASPECT)

planet_transform = RawTransform()
planet_transform:configure(resurgam_planet_entity,0)


planet_revol = RevolutionTransform()
planet_revol:configure(resurgam_planet_entity, 1.0, 1)



planet_pos_mat = Matrix()
planet_pos_mat:translation( 0.0, 0.0, -40620000.0 )
planet_transform:add_matrix( "pos", planet_pos_mat )

g:print("Planet creation done...")


renderer_descr, renderer_width, renderer_height, renderer_fullscreen, viewport_width, viewport_height = renderer:descr()

camera2_entity, camera2_pos=commons.create_fps_camera(0.0, 80.0, 0.0, viewport_width,viewport_height, mvt_mod, "ship_camera")


camera2_entity:setup_info( "referent_body", "Bellorophon" )
eg:add_child('ship','camera2_entity', camera2_entity)


model.env.setbkcolor('texture_pass', 0.0,0.0,0.0)

model.camera.mvt:set_pos(0.0, 0.0, 10.0)


model.env.light.setstate( TRUE )
model.env.light.setdir(-1.0, 0.0, 0.0)
model.env.ambientlight.setcolor(0.1, 0.1, 0.1)

model.env.fog.setdensity(0.0)

update_planet_lights(resurgam_planet_config)




gui:load_layout("timecontrol.layout","guiskins/layouts/timecontrol_widgets.conf")
gui:set_layout("timecontrol.layout")
gui:show_gui(TRUE)


gui:add_pushbuttonclickedcb( "onpushbutton",
function( layout, widget )
  
  if widget == "Button_Pause" then

	root_entity:update_timescale(FREEZE)

  elseif widget == "Button_x1" then

	root_entity:update_timescale(NORMAL_TIME)

  elseif widget == "Button_x10" then

	root_entity:update_timescale(MUL10_TIME)

  elseif widget == "Button_x100" then

	root_entity:update_timescale(MUL100_TIME)

  elseif widget == "Button_1sec_1day" then

	root_entity:update_timescale(SEC_1DAY_TIME)

  elseif widget == "Button_1sec_1hour" then

	root_entity:update_timescale(SEC_1HOUR_TIME)

  elseif widget == "Button_0Rots" then

    bellerophon_rigibody_transform:zero_angularspeed()

  elseif widget == "Button_0Transl" then
    
	bellerophon_rigibody_transform:zero_speed()

  end

  

end)

gui:set_mousecursorimage("xfskin/MouseCursor")

g:show_mousecursor(FALSE)

gui:show_mousecursor(FALSE)
g:set_mousecursorcircularmode(TRUE)


set_camera(current_cam)

g:activate_resourcessystem("init")

root_entity:update_time(2128)