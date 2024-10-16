
include('spacebox_model.lua')
include('spherebump_model.lua')
include('bellerophon_model.lua')
include('procplanet_model.lua')
include('impostors_model.lua')


local speed_factor = 500.0

local relative_ack = FALSE

-- pressed keys
local left_shift = FALSE
local left_ctrl = FALSE
local space = FALSE
-- pressed keys

local tab = FALSE

local time_factor = 0

local relative_alt = 0.0;

local viewer_under_water = FALSE

resources_event = "..."

planet_name = 'Resurgam'

local free_cam = 0
local ship_cam = 1

local current_cam = free_cam
--local current_cam = ship_cam

collimator_table = {}


set_freecam_on_planet = function(longitud, latitud, altitud_meters, planet_config)

  local planet_ray_meters = planet_config.planet_ray * 1000.0;
  local pos_ray = planet_ray_meters + altitud_meters;
 
  local xyz_pos = { g:stoc(pos_ray, longitud, latitud ) }

  model.createmainfreecamera(xyz_pos[1], xyz_pos[2], xyz_pos[3])

  source = Vector( xyz_pos[1], xyz_pos[2], xyz_pos[3], 1.0 )
  dest = Vector( 0.0, 0.0, 0.0, 1.0 )


  -- watch planet center 
  model.camera.mvt:lookat(source, dest)

  -- and then rotate 90 degres on x-axis
  for i = 0, 900, 1 do
    model.camera.mvt:rotate_x(0.1)
  end

  eg:add_child(planet_name,'model.camera.entity', model.camera.entity)

end

create_ship = function(attitude_and_pos_mat)

  bellerophon_passes_bindings = 
  {    
	binding_0 = 
	{
        target_pass_id = 'texture_pass',
		rendering_id = 'lit_rendering',
		lit_shader_update_func = bellerophon.update_lit_from_scene_env
	},   
	binding_1 = 
	{
        target_pass_id = 'oceanmask_pass',
		rendering_id = 'color_rendering',
		lit_shader_update_func = nil
	}
  }

  bellerophon.view.load('ship', attitude_and_pos_mat, bellerophon_passes_bindings)

  local ship_entity = bellerophon.models['ship'].entity

  ship_entity:add_aspect(INFOS_ASPECT)
  ship_entity:setup_info( "entity_name", "Bellorophon" )

  local rigibody_transform = bellerophon.models['ship'].rigibody_transform

  rigibody_transform:configure_mass(1500.0)

  rigibody_transform:configure_force("main prop", Vector(0.0, 0.0, -5000.0, 0.0), LOCALE_FORCE, FALSE)
  rigibody_transform:configure_force("reverse prop", Vector(0.0, 0.0, 5000.0, 0.0), LOCALE_FORCE, FALSE)

  rigibody_transform:configure_force("up prop", Vector(0.0, 3500.0, 0.0, 0.0), LOCALE_FORCE, FALSE)
  

  rigibody_transform:configure_torque("pitch_down", Vector(-150000.0, 0.0, 0.0, 0.0), LOCALE_FORCE, FALSE)
  rigibody_transform:configure_torque("pitch_up", Vector(150000.0, 0.0, 0.0, 0.0), LOCALE_FORCE, FALSE)

  rigibody_transform:configure_torque("roll_left", Vector(0.0, 0.0, 150000.0, 0.0), LOCALE_FORCE, FALSE)
  rigibody_transform:configure_torque("roll_right", Vector(0.0, 0.0, -150000.0, 0.0), LOCALE_FORCE, FALSE)

  rigibody_transform:configure_torque("yaw_left", Vector(0.0, 150000.0, 0.0, 0.0), LOCALE_FORCE, FALSE)
  rigibody_transform:configure_torque("yaw_right", Vector(0.0, -150000.0, 0.0, 0.0), LOCALE_FORCE, FALSE)

  local ship_renderer = bellerophon.models['ship']['renderer']
  ship_renderer:set_shaderrealvector( 'oceanmask_pass', 'color', 0, 0, 1, 1 ) -- z set to 1.0 to render correctly the ship underwater (see space_final_composition_ps.hlsl)

  return ship_entity, rigibody_transform
end


set_body_on_planet = function(longitud, latitud, altitud_meters, planet_config)

  local planet_ray_meters = planet_config.planet_ray * 1000.0;
  local pos_ray = planet_ray_meters + altitud_meters;
 
  local xyz_pos = { g:stoc(pos_ray, longitud, latitud ) }

  source = Vector( xyz_pos[1], xyz_pos[2], xyz_pos[3], 1.0 )
  dest = Vector( 0.0, 0.0, 0.0, 1.0 )

  -- positioning my ship
  local pos_mat = Matrix()
  pos_mat:translation(xyz_pos[1], xyz_pos[2], xyz_pos[3])

  -- my ship watches planet center 
  local rot_quat = Quaternion()
  rot_quat:lookat(source, dest)



  -- and then rotate 90 degres on x-axis
  local x_axis_vec = Vector(1.0, 0.0, 0.0, 1.0)

  for i = 0, 900, 1 do
    --model.camera.mvt:rotate_x(0.1)
	g:rotate_quaternion_x(0.1, x_axis_vec, rot_quat)

  end



  local rot_mat = Matrix()
  rot_quat:rotationmat_from(rot_mat)

  local result_mat = Matrix()
  result_mat:set_product(rot_mat, pos_mat)
  
  bellerophon_entity, bellerophon_rigibody_transform = create_ship(result_mat)
    
  eg:add_child(planet_name, 'ship', bellerophon_entity)

end



create_collimator = function(collimator_id, name)

  -- the impostor...

  local collimatorsprite_rendering_config =
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

  local collimatorsprite_passes_binding = 
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

  local collimatorsprite_descriptors_array = ImpostorsDescriptionsArray()
  collimatorsprite_descriptors_array:add()

  local renderer_aspect_ratio =  model.renderer_infos[5] / model.renderer_infos[6];

  collimatorsprite_descriptors_array:set_scale(0, 0.05, 0.05 * renderer_aspect_ratio, 0.0)

  impostors.view.load(collimator_id, collimatorsprite_descriptors_array, collimatorsprite_passes_binding, collimatorsprite_rendering_config)
  eg:add_child('root', collimator_id, impostors.models[collimator_id].entity)

  -- the object id
  local objectid_renderer=StringRendering()
  objectid_renderer:configure(root_entity, collimator_id, 0, 0, 255, 0, 255, "...")



  -- register
  local collimator_entry = 
  {
	collimator_renderer				= impostors.models[collimator_id]['renderer'],
	collimator_objectid_renderer	= objectid_renderer,
	collimator_name                 = name
  }

  collimator_table[collimator_id] = collimator_entry

end

update_collimator = function(collimator_id, connected_entity)

  local localpos = Vector(0.0, 0.0, 0.0, 1.0)
  local collimator_pos_x, collimator_pos_y, is_behind = connected_entity:project_localpoint(localpos)
  if is_behind == TRUE then

	-- hide by setting 2D pos out of the viewport
	collimator_pos_x = 10.0
	collimator_pos_y = 10.0
  end

  local collimator_renderer = collimator_table[collimator_id].collimator_renderer
  collimator_renderer:set_shaderrealvector('texture_pass', 'pos2D', collimator_pos_x, collimator_pos_y, 0.0, 0.0)


  local objectid_text_pos_x = collimator_pos_x - 0.05;
  local objectid_text_pos_y = collimator_pos_y + 0.1;
  local objectid_tpos_x = model.renderer_infos[2] * 0.5 * (objectid_text_pos_x + 1.0);
  local objectid_tpos_y = model.renderer_infos[3] - (model.renderer_infos[3] * 0.5 * (objectid_text_pos_y + 1.0));

  local objectid_renderer = collimator_table[collimator_id].collimator_objectid_renderer
  objectid_renderer:update(objectid_tpos_x, objectid_tpos_y, 0, 255, 0, collimator_table[collimator_id].collimator_name)

end






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

    enable_collisionmeshe_display                = FALSE,
    collisionmeshe_display_vshader               = "color_vs.hlsl",
    collisionmeshe_display_pshader               = "color_ps.hlsl",
    collisionmeshe_display_vshader_compiled      = FALSE,
    collisionmeshe_display_pshader_compiled      = FALSE,

    main_pass                                    = 'texture_pass',
    reflection_pass                              = 'texturemirror_pass',
    bump_pass                                    = 'bump_pass',
    oceanmask_pass                               = 'oceanmask_pass',

	planet_ray							         = 100.0,
    gravity_acc                                  = 9.81,
	plains_seed1						         = 89189.0,
	plains_seed2						         = 233.0,
	mix_seed1							         = 18901.0,
	mix_seed2							         = 17566.0,
	terrainbump_factor					         = 16.0,
	splat_transition_up_relative_alt	         = 1.095,

	--splat_transition_down_relative_alt	         = 1.0040,
	splat_transition_down_relative_alt	         = 1.010,

	splat_texture_resol					         = 16,
    zbuffer_activation_relative_alt              = 1.045,
	atmo_kr								         = 0.0033,
	fog_alt_limit						         = 60.0,
	fog_density							         = 0.01,



	-- earth-like planet
	
	plains_amplitude					         = 100.0,
	mountains_amplitude					         = 320.0,
	vertical_offset						         = 20.0,
	mountains_offset					         = 0.0,
	temp_scale									 = 1.0,
	lim_polar									 = 0.18,
	lim_tropical							     = 0.77,
	k_polar										 = 0.25,
	k_tropical									 = 0.75,
	humidity_alt_max							 = 90.0,
	temp_dec_per_km								 = 180.0,
	beach_limit							         = 2.0,
	enable_oceans                                = TRUE,
	

	-- oceanic planet
	--[[
	plains_amplitude					         = 600.0,
	mountains_amplitude					         = 0.0, -- 16000.0,
	vertical_offset						         = -420.0, --20.0,
	mountains_offset					         = 0.0,
	temp_scale									 = 1.0,
	lim_polar									 = 0.48,
	lim_tropical							     = 0.87,
	k_polar										 = 0.45,
	k_tropical									 = 0.75,
	humidity_alt_max							 = 90.0,
	temp_dec_per_km								 = 8.0,
	beach_limit							         = 2.0,
	enable_oceans                                = TRUE,
	]]


	-- cold planet

	--[[
	plains_amplitude					         = 600.0,
	mountains_amplitude					         = 13000.0,
	vertical_offset						         = 20.0,
	mountains_offset					         = 0.0,
	temp_scale									 = 0.3,
	lim_polar									 = 0.76,
	lim_tropical							     = 0.80,
	k_polar										 = 0.15,
	k_tropical									 = 0.63,
	humidity_alt_max							 = 9500.0,
	temp_dec_per_km								 = 55.0,
	beach_limit							         = 2.0,
	enable_oceans                                = FALSE,
	]]

	--desertic planet with oceans and a few humidity

	--[[
	plains_amplitude					         = 600.0,
	mountains_amplitude					         = 13000.0,
	vertical_offset						         = 20.0,
	mountains_offset					         = 0.0,
	temp_scale									 = 1.0,
	lim_polar									 = 0.20,
	lim_tropical							     = 0.66,
	k_polar										 = 0.3,
	k_tropical									 = 0.88,
	humidity_alt_max							 = 90.0,
	temp_dec_per_km								 = 5.0,
	beach_limit							         = 6.0,
	enable_oceans                                = TRUE,
	]]

	-- arid planet, no oceans, no humidity
	--[[
	plains_amplitude					         = 100.0,
	mountains_amplitude					         = 320.0,
	vertical_offset						         = 20.0,
	mountains_offset					         = 0.0,
	temp_scale									 = 1.0,
	lim_polar									 = 0.12,
	lim_tropical							     = 0.46,
	k_polar										 = 0.66,
	k_tropical									 = 0.88,
	humidity_alt_max							 = 0.5,
	temp_dec_per_km								 = 3.0,
	beach_limit							         = 6.0,
	enable_oceans                                = TRUE,
	]]

	-- hot planet, with humidity
	--[[
	plains_amplitude					         = 100.0,
	mountains_amplitude					         = 320.0,
	vertical_offset						         = 20.0,
	mountains_offset					         = 0.0,
	temp_scale									 = 1.0,
	lim_polar									 = 0.32,
	lim_tropical							     = 0.56,
	k_polar										 = 0.66,
	k_tropical									 = 0.88,
	humidity_alt_max							 = 750.0,
	temp_dec_per_km								 = 6.0,
	beach_limit							         = 6.0,
	enable_oceans                                = TRUE,
	]]



	landplace_patch						         = FALSE,

	enable_atmosphere					         = TRUE,
	atmo_thickness                               = 4.0,
    flatclouds_altitude                          = 1.0,
    wave_pass_resol                              = 512,
    ocean_bump_factor                            = 0.85,
    
	oceansdetails_specularpower					 = 50.0,

	
	details_terrain_bump_bias					 = 3.5,
	

	details_terrain_noise_scale					 = 40.0,
	level_disturbance_scale						 = 0.24,


	details_limit_sup							 = 1.40,

	ultra_details_max_distance					 = 4550,

	enable_ultra_detail							 = TRUE,
	enable_ultra_detail_bump					 = TRUE,
	enable_recursive_ultra_detail_textures		 = TRUE

}


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
         --eg:register_rigidbody(spherebump.models['sphere'].entity)

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




ship_cam_fps_yaw=SyncAngle()
ship_cam_fps_pitch=SyncAngle()
ship_cam_fps_yaw:init_fromtimeaspectof(root_entity, 0.0)
ship_cam_fps_pitch:init_fromtimeaspectof(root_entity,0.0)



g:print('Current renderer is '..model.renderer_infos[1]..', '..model.renderer_infos[2]..'x'..model.renderer_infos[3])
renderer_infos = {renderer:descr()}
g:print('Current resolution is '..renderer_infos[2].." "..renderer_infos[3])



rg:create_pass_viewportquad('final_pass')
local finalpass_rss=RenderStatesSet()

finalpass_rss:add_renderstate_in(RENDERSTATE_OPE_SETTEXTUREFILTERTYPE, "point")
finalpass_rss:add_renderstate_out(RENDERSTATE_OPE_SETTEXTUREFILTERTYPE, "linear")

local textures = TexturesSet()

local fxparams = FxParams()
fxparams:add_shaderfile('space_final_composition_vs.hlsl',SHADER_NOT_COMPILED)
fxparams:add_shaderfile('space_final_composition_ps.hlsl',SHADER_NOT_COMPILED)
fxparams:set_renderstatesset(finalpass_rss)


rendercontext = RenderContext('final_pass')
rendercontext:add_fxparams(fxparams)
rendercontext:add_texturesset(textures)
--rendercontext:add_shaderparam("water_color", 1, 0)
--rendercontext:add_shaderparam("surface_normale", 1, 1)

rendercontext:add_shaderparam("relative_alt", 1, 0)
rendercontext:add_shaderparam("underwater_lightfactor", 1, 1)
rendercontext:add_shaderparam("debug_mode", 1, 2)


renderconfig=RenderConfig()
renderconfig:add_rendercontext(rendercontext)
rg:configure_pass_viewportquad_resources('final_pass',renderconfig)
--rg:set_viewportquadshaderrealvector('final_pass', 'water_color', 1, 1, 1, 1.0)
--rg:set_viewportquadshaderrealvector('final_pass', 'surface_normale', environment.reflector_normale.x, environment.reflector_normale.y, environment.reflector_normale.z, 1.0)
rg:set_viewportquadshaderrealvector('final_pass', 'debug_mode', 0.0, 0.0, 0.0, 0.0)



rg:create_child('final_pass', 'texture_pass', 0, RENDERPURPOSE_FLOATVECTOR)
rg:set_pass_targetclearstate( 'texture_pass', FALSE )
rg:set_pass_depthclearstate( 'texture_pass', TRUE )


rg:create_child('final_pass', 'texturemirror_pass', 1)

rg:set_pass_targetclearcolor('texturemirror_pass', 0, 0, 0)

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

waves=SyncAngle()
waves_inc = TRUE
waves:init_fromtimeaspectof(root_entity,0.0)



rg:create_child('final_pass', 'oceanmask_pass', 3)
rg:set_pass_targetclearstate( 'oceanmask_pass', TRUE )
rg:set_pass_depthclearstate( 'oceanmask_pass', TRUE )
rg:set_pass_targetclearcolor('oceanmask_pass', 0, 0, 0, 0)



rg:create_child('final_pass', 'ocean_specular_pass', 4, RENDERPURPOSE_FLOATVECTOR)
rg:set_pass_targetclearstate( 'ocean_specular_pass', TRUE )
rg:set_pass_targetclearcolor( 'ocean_specular_pass', 0, 0, 255, 255 )



text_renderer=StringRendering()
text_renderer:configure(root_entity, "fps", 320, 30, 255, 0, 255, "??? fps")

text2_renderer=StringRendering()
text2_renderer:configure(root_entity, "datetime", 320, 70, 255, 0, 255, "xxxxxxx")


text3_renderer=StringRendering()
text3_renderer:configure(root_entity, "timescale", 320, 110, 255, 0, 255, "xxxxxxx")


text4_renderer=StringRendering()
text4_renderer:configure(root_entity, "planets_infos", 450, 70, 255, 0, 255, "xxxxxxx")

text5_renderer=StringRendering()
text5_renderer:configure(root_entity, "resource_infos", 320, 130, 255, 0, 255, "resources...")

text6_renderer=StringRendering()
text6_renderer:configure(root_entity, "camera pos", 320, 150, 255, 0, 255, "...")


root_entity:add_aspect(PHYSICS_ASPECT)
root_entity:configure_world(GRAVITY_DISABLED, 1.0, 1.0, 1.0)



--model.createmainfreecamera(0.0, 200.0, 10.0)
--eg:add_child('root','model.camera.entity',model.camera.entity)

mouse_right = FALSE




g:add_mousemovecb( "onmousemove",
function( xm, ym, dx, dy ) 

  if current_cam == free_cam and left_ctrl == FALSE then

    local mvt_info = { model.camera.mvt:read() }
    if mouse_right == FALSE then
  	  model.camera.mvt:update(mvt_info[4],mvt_info[1],mvt_info[2],mvt_info[3],-dy / 8.0,-dx / 8.0, 0)
    else
	  model.camera.mvt:update(mvt_info[4],mvt_info[1],mvt_info[2],mvt_info[3],0,0,-dx)
    end

    if time_factor ~= 0 then
      root_entity:update_timescale(NORMAL_TIME)
    end

  elseif current_cam == ship_cam and left_ctrl == FALSE then

  --[[
    local mvt_info = { camera2_pos:read() }

	ship_cam_fps_yaw:inc(-dx / 1.0)
	ship_cam_fps_pitch:inc(-dy / 1.0)

    camera2_pos:update(ship_cam_fps_yaw:get_value(),ship_cam_fps_pitch:get_value(),mvt_info[3],mvt_info[4],mvt_info[5],mvt_info[6], mvt_info[7], mvt_info[8], mvt_info[9])

    if time_factor ~= 0 then
      root_entity:update_timescale(NORMAL_TIME)
    end
    ]]

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
      elseif space == TRUE then
        model.camera.mvt:update(speed_factor * 0.06, mvt_info[1],mvt_info[2],mvt_info[3],0,0,0)
      else
        model.camera.mvt:update(speed_factor * 0.6 ,mvt_info[1],mvt_info[2],mvt_info[3],0,0,0)
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
      elseif space == TRUE then
        model.camera.mvt:update(-speed_factor * 0.06, mvt_info[1],mvt_info[2],mvt_info[3],0,0,0)
      else
        model.camera.mvt:update(-speed_factor * 0.6, mvt_info[1],mvt_info[2],mvt_info[3],0,0,0)
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

	bellerophon_rigibody_transform:update_forcestate("up prop", TRUE)
    

  elseif key == 67 then --'C'
    

  elseif key == 70 then --'F'


  elseif key == 71 then --'G'


  elseif key == 86 then --'V'
    

  elseif key == 66 then --'B'

  elseif key == 17 then --left CTRL

    gui:show_mousecursor(TRUE)
	g:set_mousecursorcircularmode(FALSE)

    left_ctrl = TRUE


  elseif key == 32 then --space

    space = TRUE
    
  else
	-- g:print('key code = '..key)
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

	bellerophon_rigibody_transform:update_forcestate("up prop", FALSE)
    

  elseif key == 67 then --'C'
    

  elseif key == 70 then --'F'


  elseif key == 71 then --'G'


  elseif key == 86 then --'V'
    

  elseif key == 66 then --'B'

  elseif key == 17 then --left CTRL

    gui:show_mousecursor(FALSE)
    g:set_mousecursorcircularmode(TRUE)

    left_ctrl = FALSE

  elseif key == 32 then --space

    space = FALSE
    
  else
    --g:print('key code = '..key) 
  end

  gui:on_keyup( key )
end)



g:add_appruncb( "run",
function()

  local time_infos = { root_entity:read_timemanager() }
  output_infos = renderer:descr() .." "..time_infos[3].. " fps "

  update_collimator('collimator_ship', bellerophon.models['ship'].entity)
  update_collimator('collimator_freecam', model.camera.entity)



  



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

  -- display planet infos 

  local planet_infos = planetmod.read_infos(resurgam_planet_config)

  local current_cam_id

  if current_cam == free_cam then
    current_cam_id = "model.camera"
  else
    current_cam_id = "ship_camera"
  end

  local relative_state
  
  local relative_alt = planet_infos["viewsInfos"][current_cam_id]["relative_altitude"]
  rg:set_viewportquadshaderrealvector('final_pass', 'relative_alt', relative_alt, 0.0, 0.0, 0.0)

  local global_camera_pos = Vector(planet_infos["viewsInfos"][current_cam_id]["global_camera_pos_x"],
                                    planet_infos["viewsInfos"][current_cam_id]["global_camera_pos_y"],
                                    planet_infos["viewsInfos"][current_cam_id]["global_camera_pos_z"],
                                    1.0)

  local local_camera_pos = Vector(planet_infos["viewsInfos"][current_cam_id]["local_camera_pos_x"],
                                    planet_infos["viewsInfos"][current_cam_id]["local_camera_pos_y"],
                                    planet_infos["viewsInfos"][current_cam_id]["local_camera_pos_z"],
                                    1.0)

  local longitud_camera_pos = planet_infos["viewsInfos"][current_cam_id]["longitud"]
  local latitud_camera_pos = planet_infos["viewsInfos"][current_cam_id]["latitud"]

  local temperature = planet_infos["viewsInfos"][current_cam_id]["temperature"]
  local humidity = planet_infos["viewsInfos"][current_cam_id]["humidity"]

  local display_temp = g:format_real(temperature * 100.0,0)
  local display_humidity = g:format_real(humidity * 100.0,0)


  --text6_renderer:update(10, 180, 255, 0, 0, "local pos: "..local_camera_pos:get_x().." "..local_camera_pos:get_y().." "..local_camera_pos:get_z())
  text6_renderer:update(10, 180, 255, 0, 0, "base temp = "..display_temp.." %% humidity = "..display_humidity.." %% long = "..longitud_camera_pos.." lat = "..latitud_camera_pos.." - pos = "..
							local_camera_pos:get_x().." "..local_camera_pos:get_y().." "..local_camera_pos:get_z())


  local planet_light_level = planetmod.compute_lights_level(global_camera_pos)

  rg:set_viewportquadshaderrealvector('final_pass', 'underwater_lightfactor', planet_light_level, planet_specific_config_descr.enable_oceans, 0.0, 0.0)
  


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

    
    

    relative_state = "RELATIVE"..' '..g:format_real(relative_alt,6)..' '..display_altitude..altitude_unit

    if planet_specific_config_descr.enable_oceans == TRUE then
      if relative_alt < 1.0 then

        if viewer_under_water == FALSE then
          viewer_under_water = TRUE -- transition

          model.env.fog.setcolor(0.17 * planet_light_level, 0.36 * planet_light_level, 0.48 * planet_light_level) -- fog color = planet gloabal water color
          model.env.fog.setdensity(0.001)
        end

      else

        if viewer_under_water == TRUE then
          viewer_under_water = FALSE -- transition

          model.env.fog.setcolor(0.45 * planet_light_level, 0.63 * planet_light_level, 0.78 * planet_light_level) -- fog color = planet fog color
          model.env.fog.setdensity(0.0001)
        end

      end
    end

                    


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

water_bump = function( state )
	resurgam_planet_config:set_oceanbumpfactor( state )
	resurgam_planet_config:updated()
end

ocean_spec = function(power)
    resurgam_planet_config:set_oceansdetailsspecularpower( power )
	resurgam_planet_config:updated()
end



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

spacebox_passes_bindings = 
{
	binding_0 = 
	{
        target_pass_id = 'texture_pass',
		rendering_id = 'spacebox_rendering',
		lit_shader_update_func = nil
	}
}
spaceboxmod.view.load('spacebox0', spacebox_passes_bindings, spacebox_layers)
model.setup_rawtransformationschain(spaceboxmod.models['spacebox0'].entity, 'spacebox0', 0, {x = 1000.0, y = 1000.0, z = 1000.0 } )

eg:add_child('root', 'spacebox0', spaceboxmod.models['spacebox0'].entity)

--[[
spherebump_passes_bindings = 
{
	binding_0 = 
	{
        target_pass_id = 'texture_pass',
	    rendering_id = 'lit_rendering',
		lit_shader_update_func = spherebump.update_from_scene_env
	}
}


sphere_pos_mat = Matrix()
sphere_pos_mat:translation( 0.0, 0.0, -62.0 )
spherebump.view.load('sphere', sphere_pos_mat, spherebump_passes_bindings)


eg:add_child('root', 'sphere', spherebump.models['sphere'].entity)
]]


planet_layers =
{ 
	--surface_layer = 
	[0] = 
	{
		surface_rendering =	
		{
			fx =
			{
				shaders = 
				{
					{ path='planet_surface_vs.hlsl',mode=SHADER_NOT_COMPILED },
					{ path='planet_surface_ps.hlsl',mode=SHADER_NOT_COMPILED }
				},
				rs_in = 
				{
					{ 
						ope=RENDERSTATE_OPE_SETTEXTUREFILTERTYPE, 
						value="extended",
						extended_args = 
						{
							"linear",
							"linear",
							"linear_uvwrap",
							"linear_uvwrap",
							"linear_uvwrap",
							"linear_uvwrap",
							"linear_uvwrap",
							"linear"
						}
					},
					{ ope=RENDERSTATE_OPE_SETVERTEXTEXTUREFILTERTYPE, value="linear" }
												
				},
				rs_out =
				{
					{ ope=RENDERSTATE_OPE_SETTEXTUREFILTERTYPE, value="none" },
					{ ope=RENDERSTATE_OPE_SETVERTEXTEXTUREFILTERTYPE, value="none" }						
				}		
			},
			textures =
			{
				[1] = 
				{
					{ path='earth_th_pixels_16.jpg', stage=0 },
					{ path='splatting.jpg', stage=1 },
					{ path='TexturesCom_RockGrassy0049_1_seamless_S.jpg', stage=2 },
					{ path='TexturesCom_RockGrassy0048_1_seamless_S.jpg', stage=3 },					
					{ path='TexturesCom_Grass0031_1_seamless_S.jpg', stage=4 },
					{ path='TexturesCom_Grass0103_1_seamless_S.jpg', stage=5 },
					{ path='TexturesCom_Snow0158_9_seamless_S.jpg', stage=6 }
				}
			},
			vertex_textures =
			{
			},
			shaders_params = 
			{
			},
			rendering_order = 5000
		},

		surface_wireframe_rendering =	
		{
			fx =
			{
				shaders = 
				{
					{ path='planet_surface_vs.hlsl',mode=SHADER_NOT_COMPILED },
					{ path='planet_surface_ps.hlsl',mode=SHADER_NOT_COMPILED }
				},
				rs_in = 
				{
					{ ope=RENDERSTATE_OPE_SETFILLMODE, value="line" }
						
				},
				rs_out =
				{
					{ ope=RENDERSTATE_OPE_SETFILLMODE, value="solid" }
				}		
			},
			textures =
			{
				[1] = 
				{
					{ path='earth_th_pixels_16.jpg', stage=0 }
				}
			},
			vertex_textures =
			{
			},
			shaders_params = 
			{
			},
			rendering_order = 5000
		}
	},

	--atmosphere_layer
	[1] = 
	{
		atmo_rendering =	
		{
			fx =
			{
				shaders = 
				{
					{ path='planet_atmosphere_vs.hlsl',mode=SHADER_NOT_COMPILED },
					{ path='planet_atmosphere_ps.hlsl',mode=SHADER_NOT_COMPILED }
				},
				rs_in = 
				{
					{ ope=RENDERSTATE_OPE_ENABLEZBUFFER, value="false" },
					{ ope=RENDERSTATE_OPE_ALPHABLENDENABLE, value="true" },
					{ ope=RENDERSTATE_OPE_ALPHABLENDOP, value="add" },
					{ ope=RENDERSTATE_OPE_ALPHABLENDFUNC, value="always" },
					{ ope=RENDERSTATE_OPE_ALPHABLENDDEST, value="invsrcalpha" },
					{ ope=RENDERSTATE_OPE_ALPHABLENDSRC, value="srcalpha" },
					{ ope=RENDERSTATE_OPE_SETCULLING, value="ccw" }
				},
				rs_out =
				{
					{ ope=RENDERSTATE_OPE_ENABLEZBUFFER, value="false" },
					{ ope=RENDERSTATE_OPE_ALPHABLENDENABLE, value="false" },
					{ ope=RENDERSTATE_OPE_SETCULLING, value="cw" }
				}
			},
			textures =
			{
			},
			vertex_textures =
			{
			},
			shaders_params = 
			{
			},
			rendering_order = 4500
		},

		atmo_mirror_rendering =	
		{
			fx =
			{
				shaders = 
				{
					{ path='planet_atmosphere_vs.hlsl',mode=SHADER_NOT_COMPILED },
					{ path='planet_atmosphere_ps.hlsl',mode=SHADER_NOT_COMPILED }
				},
				rs_in = 
				{
					{ ope=RENDERSTATE_OPE_ENABLEZBUFFER, value="false" },
					{ ope=RENDERSTATE_OPE_ALPHABLENDENABLE, value="true" },
					{ ope=RENDERSTATE_OPE_ALPHABLENDOP, value="add" },
					{ ope=RENDERSTATE_OPE_ALPHABLENDFUNC, value="always" },
					{ ope=RENDERSTATE_OPE_ALPHABLENDDEST, value="invsrcalpha" },
					{ ope=RENDERSTATE_OPE_ALPHABLENDSRC, value="srcalpha" },
				},
				rs_out =
				{
					{ ope=RENDERSTATE_OPE_ENABLEZBUFFER, value="false" },
					{ ope=RENDERSTATE_OPE_ALPHABLENDENABLE, value="false" },
				}
			},
			textures =
			{
			},
			vertex_textures =
			{
			},
			shaders_params = 
			{
			},
			rendering_order = 4500		
		},
	},

	--flatclouds_layer
	[2] = 
	{
		flatclouds_rendering =	
		{
			fx =
			{
				shaders = 
				{
					{ path='planet_flatclouds_vs.hlsl', mode=SHADER_NOT_COMPILED },
					{ path='planet_flatclouds_ps.hlsl', mode=SHADER_NOT_COMPILED }
				},
				
				rs_in = 
				{
					{ ope=RENDERSTATE_OPE_SETTEXTUREFILTERTYPE, value="linear" },
					{ ope=RENDERSTATE_OPE_ALPHABLENDENABLE, value="true" },
					{ ope=RENDERSTATE_OPE_ALPHABLENDOP, value="add" },
					{ ope=RENDERSTATE_OPE_ALPHABLENDFUNC, value="always" },
					{ ope=RENDERSTATE_OPE_ALPHABLENDDEST, value="invsrcalpha" },
					{ ope=RENDERSTATE_OPE_ALPHABLENDSRC, value="srcalpha" },

						
				},
				rs_out =
				{
					{ ope=RENDERSTATE_OPE_SETTEXTUREFILTERTYPE, value="none" },
					{ ope=RENDERSTATE_OPE_ALPHABLENDENABLE, value="false" },
				}
			},
			textures =
			{
				[1] = 
				{
					{ path='se_asia_clouds_8k.jpg', stage=0 }
				}
			},
			vertex_textures =
			{
			},
			shaders_params = 
			{
			},
			rendering_order = 5500
		},

		flatclouds_mirror_rendering =	
		{
			fx =
			{
				shaders = 
				{
					{ path='planet_flatclouds_vs.hlsl', mode=SHADER_NOT_COMPILED },
					{ path='planet_flatclouds_ps.hlsl', mode=SHADER_NOT_COMPILED }
				},
				
				rs_in = 
				{
					{ ope=RENDERSTATE_OPE_SETTEXTUREFILTERTYPE, value="linear" },
					{ ope=RENDERSTATE_OPE_ALPHABLENDENABLE, value="true" },
					{ ope=RENDERSTATE_OPE_ALPHABLENDOP, value="add" },
					{ ope=RENDERSTATE_OPE_ALPHABLENDFUNC, value="always" },
					{ ope=RENDERSTATE_OPE_ALPHABLENDDEST, value="invsrcalpha" },
					{ ope=RENDERSTATE_OPE_ALPHABLENDSRC, value="srcalpha" },						
				},
				rs_out =
				{
					{ ope=RENDERSTATE_OPE_SETTEXTUREFILTERTYPE, value="none" },
					{ ope=RENDERSTATE_OPE_ALPHABLENDENABLE, value="false" },
				}
			},
			textures =
			{
				[1] = 
				{
					{ path='se_asia_clouds_8k.jpg', stage=0 }
				}
			},
			vertex_textures =
			{
			},
			shaders_params = 
			{
			},
			rendering_order = 5500		
		}

	},
	
	--ocean_layer
	[3] = 
	{
		oceans_rendering = 
		{
			fx =
			{
				shaders = 
				{
					{ path='planet_ocean_vs.hlsl', mode=SHADER_NOT_COMPILED },
					{ path='planet_ocean_ps.hlsl', mode=SHADER_NOT_COMPILED }
				},
				
				rs_in = 
				{
					{ ope=RENDERSTATE_OPE_ENABLEZBUFFER, value="true" },
					--{ ope=RENDERSTATE_OPE_SETFILLMODE, value="line" }
						
				},
				rs_out =
				{
					{ ope=RENDERSTATE_OPE_ENABLEZBUFFER, value="false"},
					--{ ope=RENDERSTATE_OPE_SETFILLMODE, value="solid" }
				}
			},
			textures =
			{
			},
			vertex_textures =
			{
			},
			shaders_params = 
			{
			},
			rendering_order = 4900
			--rendering_order = 5100
		},
		oceans_bump_rendering = 
		{
			fx = 
			{
				shaders = 
				{
					{ path='planet_ocean_bump_vs.hlsl',mode=SHADER_NOT_COMPILED },
					{ path='planet_ocean_bump_ps.hlsl',mode=SHADER_NOT_COMPILED }
				},
				rs_in = 
				{					
					{ ope=RENDERSTATE_OPE_SETTEXTUREFILTERTYPE, value="linear_uvwrap" },
					{ ope=RENDERSTATE_OPE_ENABLEZBUFFER, value="true"}
				},
				rs_out =
				{
					{ ope=RENDERSTATE_OPE_ENABLEZBUFFER, value="false" },
					{ ope=RENDERSTATE_OPE_SETTEXTUREFILTERTYPE, value="none" }
				}
			},
			textures =
			{
			},
			vertex_textures =
			{
			},
			rendering_order = 10000,
			shaders_params = 
			{			
			}
		},
		oceans_specular_rendering = 
		{
			fx =
			{
				shaders = 
				{
					{ path='planet_ocean_specular_vs.hlsl', mode=SHADER_NOT_COMPILED },
					{ path='planet_ocean_specular_ps.hlsl', mode=SHADER_NOT_COMPILED }
				},
				
				rs_in = 
				{
					--{ ope=RENDERSTATE_OPE_ENABLEZBUFFER, value="true" },
					--{ ope=RENDERSTATE_OPE_SETFILLMODE, value="line" }
						
				},
				rs_out =
				{
					--{ ope=RENDERSTATE_OPE_ENABLEZBUFFER, value="false"},
					--{ ope=RENDERSTATE_OPE_SETFILLMODE, value="solid" }
				}
			},
			textures =
			{
			},
			vertex_textures =
			{
			},
			shaders_params = 
			{
			},
			rendering_order = 4900
		}
	}
}

foliage_layers =
{
	-- debug marble sphere
	[0] = 
	{
		debugmarblesphere_main_rendering = 
		{
			fx = 
			{
				shaders = 
				{
					{ path='planet_foliage_vs.hlsl',mode=SHADER_NOT_COMPILED },
					{ path='planet_foliage_ps.hlsl',mode=SHADER_NOT_COMPILED }
				},
				rs_in = 
				{
					{ ope=RENDERSTATE_OPE_ENABLEZBUFFER, value="true" },
					{ ope=RENDERSTATE_OPE_SETTEXTUREFILTERTYPE, value="linear" },
					{ ope=RENDERSTATE_OPE_SETCULLING, value="none" }
				},
				rs_out =
				{
					{ ope=RENDERSTATE_OPE_ENABLEZBUFFER, value="false" },
					{ ope=RENDERSTATE_OPE_SETTEXTUREFILTERTYPE, value="none" },
					{ ope=RENDERSTATE_OPE_SETCULLING, value="cw" }
				}
			},			
			textures = 
			{
				[1] = 
				{
					{ path='marbre.jpg', stage=0 }
				}
			},
			shaders_params = 
			{
			},
			rendering_order = 15000
		}
	},
	-- grass
	[1] = 
	{
		grass_main_rendering = 
		{
			fx = 
			{
				shaders = 
				{
					{ path='planet_foliage_vs.hlsl',mode=SHADER_NOT_COMPILED },
					{ path='planet_foliage_ps.hlsl',mode=SHADER_NOT_COMPILED }
				},
				rs_in = 
				{
					{ ope=RENDERSTATE_OPE_ENABLEZBUFFER, value="true" },
					{ ope=RENDERSTATE_OPE_SETTEXTUREFILTERTYPE, value="linear" },
					{ ope=RENDERSTATE_OPE_SETCULLING, value="none" }
				},
				rs_out =
				{
					{ ope=RENDERSTATE_OPE_ENABLEZBUFFER, value="false" },
					{ ope=RENDERSTATE_OPE_SETTEXTUREFILTERTYPE, value="none" },
					{ ope=RENDERSTATE_OPE_SETCULLING, value="cw" }
				}
			},			
			textures = 
			{
				[1] = 
				{
					{ path='green_alpha_tall_grass.png', stage=0 }
				}
			},
			shaders_params = 
			{
			},
			rendering_order = 15000
		}
	},

	[2] = 
	{
		tree_main_rendering = 
		{
			fx = 
			{
				shaders = 
				{
					{ path='planet_foliage_vs.hlsl',mode=SHADER_NOT_COMPILED },
					{ path='planet_foliage_ps.hlsl',mode=SHADER_NOT_COMPILED }
				},
				rs_in = 
				{
					{ ope=RENDERSTATE_OPE_ENABLEZBUFFER, value="true" },
					{ ope=RENDERSTATE_OPE_SETTEXTUREFILTERTYPE, value="linear" },
					{ ope=RENDERSTATE_OPE_SETCULLING, value="none" }
				},
				rs_out =
				{
					{ ope=RENDERSTATE_OPE_ENABLEZBUFFER, value="false" },
					{ ope=RENDERSTATE_OPE_SETTEXTUREFILTERTYPE, value="none" },
					{ ope=RENDERSTATE_OPE_SETCULLING, value="cw" }
				}
			},			
			textures = 
			{
				[1] = 
				{
					{ path='tree2_tex.png', stage=0 }
				}
			},
			shaders_params = 
			{
			},
			rendering_order = 15000
		}
	}, 

	[3] = 
	{
		isolated_tree_main_rendering = 
		{
			fx = 
			{
				shaders = 
				{
					{ path='planet_foliage_vs.hlsl',mode=SHADER_NOT_COMPILED },
					{ path='planet_foliage_ps.hlsl',mode=SHADER_NOT_COMPILED }
				},
				rs_in = 
				{
					{ ope=RENDERSTATE_OPE_ENABLEZBUFFER, value="true" },
					{ ope=RENDERSTATE_OPE_SETTEXTUREFILTERTYPE, value="linear" },
					{ ope=RENDERSTATE_OPE_SETCULLING, value="none" }
				},
				rs_out =
				{
					{ ope=RENDERSTATE_OPE_ENABLEZBUFFER, value="false" },
					{ ope=RENDERSTATE_OPE_SETTEXTUREFILTERTYPE, value="none" },
					{ ope=RENDERSTATE_OPE_SETCULLING, value="cw" }
				}
			},			
			textures = 
			{
				[1] = 
				{
					{ path='tree2_tex.png', stage=0 }
				}
			},
			shaders_params = 
			{
			},
			rendering_order = 15000
		}
	}
}



foliage_parameters = 
{
	-- debug marble sphere
	--[[
	[0] = 
	{
		meshe = 
		{
			file = 'sphere.ac',
			id = 'sphere'
		},

		global_lit = TRUE,
		detailed_lit = TRUE,
		local_seed = 100,

		altitud_max = -1.0,

		temperature_range = 
		{
			min = 0.0,
			max = 1.0,
		},

		humidity_range = 
		{
			min = 0.0,
			max = 1.0,
		},

		random_params = 
		{
			appearance = 0.99,

			nb_poles = 
			{
				min = 0,
				max = 1
			},

			pole_ray = 
			{
				min = 0.01,
				max = 0.6
			},

			nbpoints_per_pole =
			{
				min = 1,
				max = 3
			}
		}
	},
	]]

	-- grass
	[1] = 
	{	
		meshe = 
		{
			file = 'grassA.ac',
			id = 'rect'
		},

		global_lit = TRUE,
		detailed_lit = FALSE,
		local_seed = 101,

		altitud_max = -1.0,

		temperature_range = 
		{
			min = 0.3,
			max = 1.0,
		},

		humidity_range = 
		{
			min = 0.40,
			max = 1.0,
		},

		random_params = 
		{
			appearance = 1.0,

			nb_poles = 
			{
				min = 2,
				max = 12
			},

			pole_ray = 
			{
				min = 0.01,
				max = 0.08
			},

			nbpoints_per_pole =
			{
				min = 2,
				max = 8
			}
		}
	},


	-- tree forests
	[2] = 
	{	
		meshe = 
		{
			file = 'tree0.ac',
			id = 'Plane.001'
		},

		global_lit = TRUE,
		detailed_lit = TRUE,
		local_seed = 102,

		altitud_max = 100.0,

		temperature_range = 
		{
			min = 0.3,
			max = 1.0,
		},

		humidity_range = 
		{
			min = 0.40,
			max = 1.0,
		},

		random_params = 
		{
			appearance = 0.05,

			nb_poles = 
			{
				min = 4,
				max = 8
			},

			pole_ray = 
			{
				min = 0.5,
				max = 0.99
			},

			nbpoints_per_pole =
			{
				min = 20,
				max = 30
			}
		}
	},

	-- iolated tree
	[3] = 
	{	
		meshe = 
		{
			file = 'tree0.ac',
			id = 'Plane.001'
		},

		global_lit = TRUE,
		detailed_lit = TRUE,
		local_seed = 103,

		altitud_max = 100.0,

		temperature_range = 
		{
			min = 0.3,
			max = 1.0,
		},

		humidity_range = 
		{
			min = 0.40,
			max = 1.0,
		},

		random_params = 
		{
			appearance = 0.20,

			nb_poles = 
			{
				min = 1,
				max = 1
			},

			pole_ray = 
			{
				min = 0.02,
				max = 0.08
			},

			nbpoints_per_pole =
			{
				min = 1,
				max = 1
			}
		}
	},

}


foliage_passes_bindings =
{
	--[[
    binding_0 = 
	{
        target_pass_id = 'texture_pass',
		rendering_id = 'debugmarblesphere_main_rendering'
	},
	]]

    binding_1 = 
	{
        target_pass_id = 'texture_pass',
		rendering_id = 'grass_main_rendering'
	},

    binding_2 = 
	{
        target_pass_id = 'texture_pass',
		rendering_id = 'tree_main_rendering'
	},

    binding_3 = 
	{
        target_pass_id = 'texture_pass',
		rendering_id = 'isolated_tree_main_rendering'
	}

}


if planet_specific_config_descr.enable_oceans == TRUE then

  local planet_passes_bindings = 
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
	},
	binding_3 = 
	{
        target_pass_id = 'oceanmask_pass',
		rendering_id = 'oceans_rendering',
		lit_shader_update_func = nil
	},
	binding_4 = 
	{
		target_pass_id = 'bump_pass',
		rendering_id = 'oceans_bump_rendering',
		lit_shader_update_func = nil
	},    
	binding_5 = 
	{
        target_pass_id = 'texturemirror_pass',
		rendering_id = 'flatclouds_mirror_rendering',
		lit_shader_update_func = nil
	},        
	binding_6 = 
	{
        target_pass_id = 'texturemirror_pass',
		rendering_id = 'atmo_mirror_rendering',
		lit_shader_update_func = nil
	},
	binding_7 = 
	{
        target_pass_id = 'ocean_specular_pass',
		rendering_id = 'oceans_specular_rendering',
		lit_shader_update_func = nil
	},
  }

  planetmod.view.load(planet_name, planet_specific_config_descr, planet_passes_bindings, planet_layers, foliage_passes_bindings, foliage_layers, foliage_parameters, 'wave_pass')

else

  local planet_passes_bindings = 
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

  planetmod.view.load(planet_name, planet_specific_config_descr, planet_passes_bindings, planet_layers, foliage_passes_bindings, foliage_layers, 'wave_pass')
end




resurgam_planet_entity = planetmod.models[planet_name].entity
resurgam_planet_config = planetmod.models[planet_name].specific_config

eg:add_child('root', planet_name, resurgam_planet_entity)


resurgam_planet_entity:add_aspect(TRANSFORM_ASPECT)


-- with an orbit

--[[

planet_transform = RawTransform()
planet_transform:configure(resurgam_planet_entity,0)

planet_pos_mat = Matrix()
planet_pos_mat:translation( 0.0, 0.0, -140620000.0 )
planet_transform:add_matrix( "pos", planet_pos_mat )


planet_orbit = OrbitTransform()
local planet_orbit_descr = 
{
	orbit_ray = 60400000,
	excentricity = 1.0,
	angle = 0.0,
	orbit_duration = 0.025,
	orbit_offset_rot = 0.0,
	orbit_pan_angle = 0.0,
	orbit_tilt_angle = 0.0,
	revol_axe_inclination = 33
}

planet_orbit:configure(resurgam_planet_entity, 

	planet_orbit_descr.orbit_ray,
	planet_orbit_descr.excentricity,
	planet_orbit_descr.angle,
	planet_orbit_descr.orbit_duration,
	planet_orbit_descr.orbit_offset_rot,
	planet_orbit_descr.orbit_pan_angle,
	planet_orbit_descr.orbit_tilt_angle,
	planet_orbit_descr.revol_axe_inclination,

	1 )



planet_revol = RevolutionTransform()
planet_revol:configure(resurgam_planet_entity, 1.0, 2)

]]

-- direct positioning

planet_transform = RawTransform()
planet_transform:configure(resurgam_planet_entity,0)

planet_pos_mat = Matrix()
planet_pos_mat:translation( 0.0, 0.0, -40620000.0 )
--planet_pos_mat:translation( 0.0, 0.0, 0.0 )
planet_transform:add_matrix( "pos", planet_pos_mat )

planet_revol = RevolutionTransform()
planet_revol:configure(resurgam_planet_entity, 1.0, 1)
--planet_revol:configure(resurgam_planet_entity, 1000, 2)




g:print("Planet creation done...")


-- on planet

set_body_on_planet(66.403, -26.193, 80.0, planet_specific_config_descr)

-- on space
--bellerophon_entity, bellerophon_rigibody_transform = create_ship_at_pos(-160.0, 0.0, -500.0)

--eg:add_child('root', 'ship', bellerophon_entity)









-- on planet

set_freecam_on_planet(112.82, 2.23, 5.2, planet_specific_config_descr)



-- in space

--model.createmainfreecamera(100, 200, -40420000.0)
--eg:add_child('root','model.camera.entity', model.camera.entity)





renderer_descr, renderer_width, renderer_height, renderer_fullscreen, viewport_width, viewport_height = renderer:descr()

camera2_entity, camera2_pos=commons.create_fps_camera(0.0, 50.0, 35.0, viewport_width,viewport_height, "ship_camera")


camera2_entity:setup_info( "referent_body", "Bellorophon" )
eg:add_child('ship','camera2_entity', camera2_entity)



-- collimators

-- collimator sprite


create_collimator('collimator_ship', 'Bellerophon spaceship')

create_collimator('collimator_freecam', 'free camera')













model.env.setbkcolor('texture_pass', 0.0,0.0,0.0)




model.env.light.setstate( TRUE )
model.env.light.setdir(-1.0, 0.0, 0.0)
model.env.light.setcolor(0.99, 0.99, 0.99)
model.env.ambientlight.setcolor(0.1, 0.1, 0.1)
-- model.env.ambientlight.setcolor(0.0, 0.0, 0.0)

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

dmode = function(mode)
	rg:set_viewportquadshaderrealvector('final_pass', 'debug_mode', mode, 0.0, 0.0, 0.0)
end
