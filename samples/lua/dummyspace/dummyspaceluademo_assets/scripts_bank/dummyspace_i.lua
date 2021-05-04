
include('spacebox_model.lua')
include('asteroid_model.lua')
include('boulder_model.lua')
include('bellerophon_model.lua')

ctrl_key = FALSE

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
       g:deactivate_resourcessystem();
       rg:update_renderingqueues()

       if context == "init" then

         eg:register_collider(asteroid.models['ceres'].entity)
         eg:register_collider(boulder.models['rock'].entity)
         eg:register_rigidbody(bellerophon_entity)

       end

    else
       evt_out = "? : "..event       
    end
    resources_event = evt_out
end)

g:print('Current renderer is '..model.renderer_infos[1]..', '..model.renderer_infos[2]..'x'..model.renderer_infos[3])
renderer_infos = {renderer:descr()}
g:print('Current resolution is '..renderer_infos[2].." "..renderer_infos[3])


set_camera = function(camera)


  if camera == free_cam then

    eg:set_camera(model.camera.entity)

  elseif camera == asteroid_cam then

    eg:set_camera(camera2_entity)

  elseif camera == ship_cam then

    eg:set_camera(camera3_entity)

  end
end

free_cam = 0
asteroid_cam = 1
ship_cam = 2

current_cam = free_cam
--current_cam = asteroid_cam


mvt_mod = Module("mvtmod", "mvts")
mvt_mod:load()
g:print(mvt_mod:get_descr().. ' loaded')

commons.init_final_pass(rg, 'final_pass')

rg:create_child('final_pass', 'texture_pass', 0)
rg:set_pass_targetclearstate( 'texture_pass', FALSE )
rg:set_pass_depthclearstate( 'texture_pass', TRUE )


model.createmainfreecamera(0.0, 0.0, 0.0, mvt_mod)
eg:add_child('root','model.camera.entity',model.camera.entity)

camera_width, camera_height, zn, zf = model.camera.entity:read_cameraparams()

g:print('camera params = '..camera_width..' '..camera_height..' '..zn..' '..zf )

text_renderer=TextRendering()
text_renderer:configure(root_entity, "resource_infos", 320, 130, 255, 0, 255, "resources...")

container_angle_y_deg = 0.0
container_angle_x_deg = 0.0


g:activate_resourcessystem("init");

mouse_right = FALSE


root_entity:add_aspect(PHYSICS_ASPECT)
root_entity:configure_world(GRAVITY_DISABLED, 1.0, 1.0, 1.0)



g:add_mousemovecb( "onmousemove",
function( xm, ym, dx, dy )  

  if current_cam == free_cam then
    local mvt_info = { model.camera.mvt:read() }
    if mouse_right == FALSE then
  	  model.camera.mvt:update(mvt_info[4],mvt_info[1],mvt_info[2],mvt_info[3],-dy / 4.0,-dx / 4.0, 0)
    else
      model.camera.mvt:update(mvt_info[4],mvt_info[1],mvt_info[2],mvt_info[3],0,0,-dx)
    end
  else
    local mvt_info = { rock_free_transfo:read() }
    if mouse_right == FALSE then
  	  rock_free_transfo:update(mvt_info[4],mvt_info[1],mvt_info[2],mvt_info[3],-dy / 4.0,-dx / 4.0, 0)
    else
      rock_free_transfo:update(mvt_info[4],mvt_info[1],mvt_info[2],mvt_info[3],0,0,-dx)
    end
  end

end)

g:add_mouserightbuttondowncb( "onmouserightbuttondown", 
function( xm, ym )
  mouse_right = TRUE
end)

g:add_mouserightbuttonupcb( "onmouserightbuttonup", 
function( xm, ym )
  mouse_right = FALSE
end)

g:add_appruncb( "run",
function()

  text_renderer:update(10, 150, 255, 0, 0, resources_event)

  if current_cam == free_cam then
    local mvt_info = { model.camera.mvt:read() }
    model.camera.mvt:update(mvt_info[4],mvt_info[1],mvt_info[2],mvt_info[3],0,0,0)
  else
    local mvt_info = { rock_free_transfo:read() }
    rock_free_transfo:update(mvt_info[4],mvt_info[1],mvt_info[2],mvt_info[3],0,0,0)
  end

  
  planet_rot_mat:rotation(0.0, 1.0, 0.0, commons.utils.deg_to_rad(ceres_angles:get_value()))
  planet_transform:update_matrix( "rot", planet_rot_mat )
  ceres_angles:inc( 5.0 )
 
  

end)

g:add_keydowncb( "keydown",
function( key )

  --Q key
  if key == 81 then 


    if current_cam == free_cam then
      local mvt_info = { model.camera.mvt:read() }
      model.camera.mvt:update(2500.0,mvt_info[1],mvt_info[2],mvt_info[3],0,0,0)

    elseif current_cam == asteroid_cam then
      local mvt_info = { rock_free_transfo:read() }
      rock_free_transfo:update(500.0,mvt_info[1],mvt_info[2],mvt_info[3],0,0,0)

    elseif current_cam == ship_cam then
      bellerophon_rigibody_transform:update_forcestate("main prop", TRUE)
    end

      
  --W key
  elseif key == 87 then


    if current_cam == free_cam then
      local mvt_info = { model.camera.mvt:read() }
      model.camera.mvt:update(-2500.0,mvt_info[1],mvt_info[2],mvt_info[3],0,0,0)

    elseif current_cam == asteroid_cam then
      local mvt_info = { rock_free_transfo:read() }
      rock_free_transfo:update(-500.0,mvt_info[1],mvt_info[2],mvt_info[3],0,0,0)

    elseif current_cam == ship_cam then
      bellerophon_rigibody_transform:update_forcestate("reverse prop", TRUE)
    end

  elseif key == 17 then
    ctrl_key = TRUE


  elseif key == 65 then --'A'

    if current_cam == ship_cam then
      bellerophon_rigibody_transform:update_torquestate("roll_left", TRUE)
      root_entity:update_timescale(NORMAL_TIME)
    end

  elseif key == 90 then --'Z'

    if current_cam == ship_cam then
      bellerophon_rigibody_transform:update_torquestate("roll_right", TRUE)
      root_entity:update_timescale(NORMAL_TIME)
    end

  elseif key == 37 then --VK_LEFT

    if current_cam == ship_cam then
      bellerophon_rigibody_transform:update_torquestate("yaw_left", TRUE)
      root_entity:update_timescale(NORMAL_TIME)
    end

  elseif key == 38 then --VK_UP

    if current_cam == ship_cam then
      bellerophon_rigibody_transform:update_torquestate("pitch_down", TRUE)
      root_entity:update_timescale(NORMAL_TIME)
    end

  elseif key == 39 then --VK_RIGHT

    if current_cam == ship_cam then
      bellerophon_rigibody_transform:update_torquestate("yaw_right", TRUE)
      root_entity:update_timescale(NORMAL_TIME)
    end

  elseif key == 40 then --VK_DOWN

    if current_cam == ship_cam then
      bellerophon_rigibody_transform:update_torquestate("pitch_up", TRUE)
      root_entity:update_timescale(NORMAL_TIME)
    end

  end

end)

g:add_keyupcb( "keyup",
function( key )  

  last_key = key

  --Q key
  if key == 81 then

    if current_cam == free_cam then
      local mvt_info = { model.camera.mvt:read() }
      model.camera.mvt:update(0.0,mvt_info[1],mvt_info[2],mvt_info[3],0,0,0)

    elseif current_cam == asteroid_cam then
      local mvt_info = { rock_free_transfo:read() }
      rock_free_transfo:update(0.0,mvt_info[1],mvt_info[2],mvt_info[3],0,0,0)

    elseif current_cam == ship_cam then
      bellerophon_rigibody_transform:update_forcestate("main prop", FALSE)
    end
    
  --W key
  elseif key == 87 then

    if current_cam == free_cam then
      local mvt_info = { model.camera.mvt:read() }
      model.camera.mvt:update(0.0,mvt_info[1],mvt_info[2],mvt_info[3],0,0,0)

    elseif current_cam == asteroid_cam then
      local mvt_info = { rock_free_transfo:read() }
      rock_free_transfo:update(0.0,mvt_info[1],mvt_info[2],mvt_info[3],0,0,0)

    elseif current_cam == ship_cam then
      bellerophon_rigibody_transform:update_forcestate("reverse prop", FALSE)
    end

  elseif key == 76 then --'L'

    if current_cam == ship_cam then
      bellerophon_rigibody_transform:zero_speed()
    end

  elseif key == 77 then --'M'

    if current_cam == ship_cam then
      bellerophon_rigibody_transform:zero_angularspeed()
    end

  elseif key == 65 then --'A'

    if current_cam == ship_cam then
      bellerophon_rigibody_transform:update_torquestate("roll_left", FALSE)
    end
    
  elseif key == 90 then --'Z'

    if current_cam == ship_cam then
      bellerophon_rigibody_transform:update_torquestate("roll_right", FALSE)
    end
    
  elseif key == 37 then --VK_LEFT

    if current_cam == ship_cam then
      bellerophon_rigibody_transform:update_torquestate("yaw_left", FALSE)
    end
    
  elseif key == 38 then --VK_UP

    if current_cam == ship_cam then
      bellerophon_rigibody_transform:update_torquestate("pitch_down", FALSE)
    end
    
  elseif key == 39 then --VK_RIGHT

    if current_cam == ship_cam then
      bellerophon_rigibody_transform:update_torquestate("yaw_right", FALSE)
    end
    
  elseif key == 40 then --VK_DOWN

    if current_cam == ship_cam then
      bellerophon_rigibody_transform:update_torquestate("pitch_up", FALSE)	
    end

  -- VK_F1
  elseif key == 112 then

    eg:unregister_collider(boulder.models['rock'].entity)
    eg:remove('asteroid_cam')
    eg:remove('rock')
    
    eg:add_child('ceres', 'rock', boulder.models['rock'].entity)
    eg:add_child('rock','asteroid_cam', camera2_entity)
    eg:register_collider(boulder.models['rock'].entity)

    -- set again asteroid cam because it has been erased
    if current_cam == asteroid_cam then
      set_camera(current_cam)
    end

  -- VK_F2
  elseif key == 113 then
    eg:unregister_collider(boulder.models['rock'].entity)
    eg:remove('asteroid_cam')
    eg:remove('rock')
    
    eg:add_child('root', 'rock', boulder.models['rock'].entity)
    eg:add_child('rock','asteroid_cam', camera2_entity)
    eg:register_collider(boulder.models['rock'].entity)

    -- set again asteroid cam because it has been erased
    if current_cam == asteroid_cam then
      set_camera(current_cam)
    end

  -- VK_F3
  elseif key == 114 then

    eg:unregister_rigidbody(bellerophon_entity)
    eg:remove('camera3_entity')
    eg:remove('ship')
    
    eg:add_child('ceres', 'ship', bellerophon_entity)
    eg:add_child('ship','camera3_entity', camera3_entity)
    eg:register_rigidbody(bellerophon_entity)

    -- set again asteroid cam because it has been erased
    if current_cam == ship_cam then
      set_camera(current_cam)
    end

  -- VK_F4
  elseif key == 115 then

    eg:unregister_rigidbody(bellerophon_entity)
    eg:remove('camera3_entity')
    eg:remove('ship')
    
    eg:add_child('root', 'ship', bellerophon_entity)
    eg:add_child('ship','camera3_entity', camera3_entity)
    eg:register_rigidbody(bellerophon_entity)

    -- set again asteroid cam because it has been erased
    if current_cam == ship_cam then
      set_camera(current_cam)
    end

  -- VK_F5
  elseif key == 116 then  

    current_cam = current_cam + 1
	if current_cam == 3 then
	  current_cam = 0
	end
	set_camera(current_cam)

  -- VK_F7
  elseif key == 118 then 

    eg:remove('model.camera.entity')
    eg:add_child('ceres','model.camera.entity',model.camera.entity)
  
    if current_cam == free_cam then
      set_camera(current_cam)
    end

  -- VK_F8
  elseif key == 119 then  

    eg:remove('model.camera.entity')
    eg:add_child('root','model.camera.entity',model.camera.entity)

  
    if current_cam == free_cam then
      set_camera(current_cam)
    end

  elseif key == 37 then --VK_LEFT

    bellerophon_rigibody_transform:update_forcestate("lateral prop", FALSE)

  elseif key == 17 then
    ctrl_key = FALSE
     
  end
end)


g:show_mousecursor(FALSE)
g:set_mousecursorcircularmode(TRUE)

g:signal_renderscenebegin("eg")

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


asteroid_passes_bindings = 
{
	binding_0 = 
	{
		target_pass_id = 'texture_pass',
		rendering_id = 'lit_rendering',
		lit_shader_update_func = asteroid.update_lit_from_scene_env
	}
}
asteroid.view.load('ceres', asteroid_passes_bindings)
eg:add_child('root', 'ceres', asteroid.models['ceres'].entity)

ceres_planet_entity = asteroid.models['ceres'].entity

planet_transform = RawTransform()
planet_transform:configure(ceres_planet_entity,0)

ceres_angles = SyncAngle()
ceres_angles:init_fromtimeaspectof(root_entity,0.0)

planet_rot_mat = Matrix()
planet_rot_mat:rotation(0.0, 1.0, 0.0, commons.utils.deg_to_rad(ceres_angles:get_value()))
planet_transform:add_matrix( "rot", planet_rot_mat )

planet_pos_mat = Matrix()
planet_pos_mat:translation( 0.0, 0.0, -5000.0 )
planet_transform:add_matrix( "pos", planet_pos_mat )

ceres_planet_entity:add_aspect(PHYSICS_ASPECT)
ceres_planet_entity:configure_world(GRAVITY_DISABLED, 1.0, 1.0, 1.0)




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
--eg:add_child('ceres', 'rock', boulder.models['rock'].entity)

rock_free_transfo=FreeTransform()
rock_free_transfo:instanciate_transformimpl(mvt_mod)

rock_free_transfo:configure(boulder.models['rock'].entity, 0, 0.0, 0.0, -800.0, 0)
--rock_free_transfo:configure(boulder.models['rock'].entity, 0, 0.0, 0.0, 4200.0, 0)

renderer_descr, renderer_width, renderer_height, renderer_fullscreen, viewport_width, viewport_height = renderer:descr()
camera2_entity, camera2_pos=commons.create_static_camera(0.0, 10.0, 60.0, viewport_width,viewport_height, mvt_mod, "rock_camera")
eg:add_child('rock','asteroid_cam', camera2_entity)




bellerophon_passes_bindings = 
{
	binding_0 = 
	{
        target_pass_id = 'texture_pass',
		rendering_id = 'lit_rendering',
		lit_shader_update_func = bellerophon.update_lit_from_scene_env
	}
}
--bellerophon.view.load('ship', {x = 340.0, y = 0.0, z = -800.0 }, bellerophon_passes_bindings)
bellerophon.view.load('ship', {x = 340.0, y = 0.0, z = 4200.0 }, bellerophon_passes_bindings)

bellerophon_entity = bellerophon.models['ship'].entity

--eg:add_child('root', 'ship', bellerophon_entity)
eg:add_child('ceres', 'ship', bellerophon_entity)

bellerophon_entity:add_aspect(INFOS_ASPECT)
bellerophon_entity:setup_info( "entity_name", "Bellorophon" )


bellerophon_rigibody_transform = bellerophon.models['ship'].rigibody_transform

bellerophon_rigibody_transform:configure_mass(50.0)

bellerophon_rigibody_transform:configure_force("main prop", Vector(0.0, 0.0, -5000.0, 0.0), LOCALE_FORCE, FALSE)
bellerophon_rigibody_transform:configure_force("reverse prop", Vector(0.0, 0.0, 5000.0, 0.0), LOCALE_FORCE, FALSE)
bellerophon_rigibody_transform:configure_force("lateral prop", Vector(-5000.0, 0.0, 0.0, 0.0), LOCALE_FORCE, FALSE)

bellerophon_rigibody_transform:configure_torque("pitch_down", Vector(-150000.0, 0.0, 0.0, 0.0), LOCALE_FORCE, FALSE)
bellerophon_rigibody_transform:configure_torque("pitch_up", Vector(150000.0, 0.0, 0.0, 0.0), LOCALE_FORCE, FALSE)

bellerophon_rigibody_transform:configure_torque("roll_left", Vector(0.0, 0.0, 150000.0, 0.0), LOCALE_FORCE, FALSE)
bellerophon_rigibody_transform:configure_torque("roll_right", Vector(0.0, 0.0, -150000.0, 0.0), LOCALE_FORCE, FALSE)

bellerophon_rigibody_transform:configure_torque("yaw_left", Vector(0.0, 150000.0, 0.0, 0.0), LOCALE_FORCE, FALSE)
bellerophon_rigibody_transform:configure_torque("yaw_right", Vector(0.0, -150000.0, 0.0, 0.0), LOCALE_FORCE, FALSE)



camera3_entity, camera3_pos=commons.create_static_camera(0.0, 70.0, 0.0, viewport_width,viewport_height, mvt_mod, "ship_camera")
camera3_entity:setup_info( "referent_body", "Bellorophon" )
eg:add_child('ship','camera3_entity', camera3_entity)


model.env.setbkcolor('texture_pass', 0.0,0.0,0.0)

model.camera.mvt:set_pos(0.0, 0.0, 10.0)


model.env.light.setstate( TRUE )
model.env.light.setdir(1.0, -0.4, 0.0)
model.env.ambientlight.setcolor(0.45, 0.45, 0.45)
model.env.fog.setdensity(0.0)

set_camera(current_cam)

