

local speed_factor = 90.0

renderers = {}
nb_renderers = 0

lights = 
{
	ambient_light = {r = 0.05, g = 0.05, b = 0.05, a = 0.0 },
	lights_enabled = {x = 1.0, y = 0.0, z = 0.0, w = 0.0 },
	light0 = 
	{
		color = { r = 1.0, g = 0.0, b = 0.0, a = 1.0 },
		direction = { x = 3.0, y = -1.0, z = 0.0, w = 1.0 },
	}
}

create_sphere = function()

	local entity_config = 
	{ 
		main_rendering = 
		{
			fx = 
			{
				shaders = 
				{
					{ path='lit_vs.cso',mode=SHADER_COMPILED },
					{ path='lit_ps.cso',mode=SHADER_COMPILED }
				},
				rs_in = 
				{
					{ ope=RENDERSTATE_OPE_ENABLEZBUFFER, value="true" },
					{ ope=RENDERSTATE_OPE_SETCULLING, value="cw" },
					{ ope=RENDERSTATE_OPE_SETTEXTUREFILTERTYPE, value="linear" }					
				},
				rs_out =
				{
					{ ope=RENDERSTATE_OPE_ENABLEZBUFFER, value="false" },
					{ ope=RENDERSTATE_OPE_SETCULLING, value="cw" },
					{ ope=RENDERSTATE_OPE_SETTEXTUREFILTERTYPE, value="none" }
				}
			},
			textures =
			{
				[1] = 
				{
					{ path='marbre.jpg', stage=0},
					{ path='bump.bmp', stage=1}
				}
			},
			vertex_textures =
			{
			},
			rendering_order = 10000,
			shaders_params = commons.setup_lit_shader_params()
		},
		meshes_loader_params =
		{
			normale_generation_mode = NORMALES_AUTO_SMOOTH,
			tb_generation_mode = TB_AUTO
		}
	}
	local entity = nil
	local renderer = nil
	entity,renderer = commons.create_rendered_meshe(entity_config, 'sphere.ac', 'sphere', {main_rendering = 'texture_pass'})

	entity:add_aspect(INFOS_ASPECT)
	entity:setup_info( "entity_name", "bump mapped sphere" )

	renderer:register_to_rendering(rg)
	eg:add_child('root','sphere_entity',entity)

	--[[
	entity:add_aspect(BODY_ASPECT)
	local body=Body()

	body:attach_toentity(entity)

	body:configure_shape(SHAPE_SPHERE, 20.0)


	local sphere_pos_mat = Matrix()
	
	sphere_pos_mat:translation( 0.0, 0.0, -60.0 )
	

	body:configure_attitude(sphere_pos_mat)

	body:configure_mass(80.0)

	body:configure_mode(BODY_MODE)
	]]


	local scale = Matrix();
	scale:scale(1.0, 1.0, 1.0)

	local pos_mat = Matrix()
	pos_mat:translation( 0.0, 0.0, -62.0 )


	local transform = RawTransform()
	transform:configure(entity)
	transform:add_matrix("scaling",scale)
	transform:add_matrix( "pos", pos_mat )


	local sphere_material =
	{
		specular_power = 200.0,
		color_source = { r = 1.0, g = 1.0, b = 1.0, a = 1.0 },
		simple_color = { r = 1.0, g = 0.0, b = 0.0, a = 0.0 },
		light_absorption = { r = 0.0, g = 0.0, b = 0.0, a = 0.0 },
		self_emissive = { r = 0.0, g = 0.0, b = 0.0, a = 0.0 },
		bump_mapping = { texture_size = 512, bias = 0.45 }
	}

	commons.apply_material( sphere_material, renderer, 'texture_pass')
	renderer:set_shaderrealvector( 'texture_pass', 'flags2', 0.0, 0.0, 0.0, 0.0 )

	--return entity,renderer,body
	return entity,renderer,transform
end



create_ship = function()

	local entity_config = 
	{ 
		ship_rendering = 
		{
			fx = 
			{
				shaders = 
				{
					{ path='lit_vs.cso',mode=SHADER_COMPILED },
					{ path='lit_ps.cso',mode=SHADER_COMPILED }
				},
				rs_in = 
				{
					{ ope=RENDERSTATE_OPE_ENABLEZBUFFER, value="true" }
				},
				rs_out =
				{
					{ ope=RENDERSTATE_OPE_ENABLEZBUFFER, value="false"}
				}
			},
			textures =
			{
				[1] = 
				{
					--{ path='SurveyShip_color.jpg', stage=0},
					--{ path='SurveyShip_color.jpg', stage=1}
					{ path='bellerophon.jpg', stage=0},
					{ path='bellerophon.jpg', stage=1}
				}
			},
			vertex_textures =
			{
			},
			rendering_order = 10000,
			shaders_params = commons.setup_lit_shader_params()
		},
		meshes_loader_params =
		{
			normale_generation_mode = NORMALES_AUTO_SMOOTH,
			tb_generation_mode = TB_AUTO
		}
	}

	local entity = nil
	local renderer = nil

	--entity,renderer = commons.create_rendered_meshe(ship_entity_config, 'survey.ac', '001_hull', {ship_rendering = 'texture_pass'})
	entity,renderer = commons.create_rendered_meshe(entity_config, 'bellerophon.ac', 'wavefront obj', {ship_rendering = 'texture_pass'})
	entity:add_aspect(INFOS_ASPECT)
	entity:setup_info( "entity_name", "Bellorophon" )



	entity:add_aspect(BODY_ASPECT)
	local body=Body()

	body:attach_toentity(entity)

	body:configure_shape(SHAPE_BOX, 110.0, 80.0, 170.0)


	local ship_pos_mat = Matrix()

	ship_pos_mat:translation( -300.0, 0.0, -400.0 )

	body:configure_attitude(ship_pos_mat)

	body:configure_mass(50.0)

	body:configure_mode(BODY_MODE)

	body:configure_force("main prop", Vector(0.0, 0.0, -5000.0, 0.0), LOCALE_FORCE, FALSE)
	body:configure_force("reverse prop", Vector(0.0, 0.0, 5000.0, 0.0), LOCALE_FORCE, FALSE)

	body:configure_torque("pitch_down", Vector(-150000.0, 0.0, 0.0, 0.0), LOCALE_FORCE, FALSE)
	body:configure_torque("pitch_up", Vector(150000.0, 0.0, 0.0, 0.0), LOCALE_FORCE, FALSE)

	body:configure_torque("roll_left", Vector(0.0, 0.0, 150000.0, 0.0), LOCALE_FORCE, FALSE)
	body:configure_torque("roll_right", Vector(0.0, 0.0, -150000.0, 0.0), LOCALE_FORCE, FALSE)

	body:configure_torque("yaw_left", Vector(0.0, 150000.0, 0.0, 0.0), LOCALE_FORCE, FALSE)
	body:configure_torque("yaw_right", Vector(0.0, -150000.0, 0.0, 0.0), LOCALE_FORCE, FALSE)


	renderer:register_to_rendering(rg)
	eg:add_child('root','ship_entity',entity)



	ship_material =
	{
		color_source = { r = 1.0, g = 1.0, b = 1.0, a = 1.0 },
		simple_color = { r = 0.0, g = 0.0, b = 0.0, a = 0.0 },
		light_absorption = { r = 0.0, g = 0.0, b = 0.0, a = 0.0 },
		self_emissive = { r = 0.0, g = 0.0, b = 0.0, a = 0.0 },
		bump_mapping = { texture_size = 1024, bias = 0.45 }
	}

	commons.apply_material( ship_material, renderer, 'texture_pass')
	renderer:set_shaderrealvector( 'texture_pass', 'flags2', 0.0, 0.0, 0.0, 0.0 )

	return entity,renderer,body
end

create_skybox = function()

	local entity_layers =
	{
		layer_0 = 
		{
			layer0_rendering =	
			{
				fx =
				{
					shaders = 
					{
						{ path='texture_vs.cso',mode=SHADER_COMPILED },
						{ path='texture_ps.cso',mode=SHADER_COMPILED }
					},
					rs_in = 
					{
					},
					rs_out =
					{
					}		
				},
				textures =
				{
					[1] = 
					{
						{path='neb_front5.png', stage=0}
					},
					[2] = 
					{
						{path='neb_back6.png', stage=0}
					},
					[3] = 
					{
						{path='neb_left2.png', stage=0}
					},
					[4] = 
					{
						{path='neb_right1.png', stage=0}
					},
					[5] = 
					{
						{path='neb_top3.png', stage=0}
					},
					[6] = 
					{
						{path='neb_bottom4.png', stage=0}
					}
				},
				vertex_textures =
				{
				},
				shaders_params = 
				{
				},
				rendering_order = 1000
			}
		}
	}

	local entity = nil
	local renderer = nil

	entity,renderer=commons.create_rendering_from_module(entity_layers,sb_mod,"skyboxRender", {layer0_rendering = 'texture_pass'})

	entity:add_aspect(INFOS_ASPECT)
	entity:setup_info( "entity_name", "skybox" )

	renderer:register_to_rendering(rg)
	eg:add_child('root','skybox_entity',entity)


	entity:add_aspect(TRANSFORM_ASPECT)
	local sb_scale = Matrix();
	sb_scale:scale(1000.0, 1000.0, 1000.0)

	local transform = RawTransform()
	transform:configure(entity)
	transform:add_matrix("sb_scaling",sb_scale)

	return entity,renderer,transform
end

create_planet = function()

	local entity_layers =
	{ 
		surface_layer = 
		{
			surface_rendering =	
			{
				fx =
				{
					shaders = 
					{
						{ path='planet_surface_vs.cso',mode=SHADER_COMPILED },
						{ path='planet_surface_ps.cso',mode=SHADER_COMPILED }
					},
					rs_in = 
					{
						{ ope=RENDERSTATE_OPE_ENABLEZBUFFER, value="false" },
						{ ope=RENDERSTATE_OPE_SETTEXTUREFILTERTYPE, value="linear" },
						{ ope=RENDERSTATE_OPE_SETVERTEXTEXTUREFILTERTYPE, value="linear" }
						
						--,{ ope=RENDERSTATE_OPE_SETFILLMODE, value="line" }
						
					},
					rs_out =
					{
						{ ope=RENDERSTATE_OPE_ENABLEZBUFFER, value="false"},
						{ ope=RENDERSTATE_OPE_SETTEXTUREFILTERTYPE, value="none" },
						{ ope=RENDERSTATE_OPE_SETVERTEXTEXTUREFILTERTYPE, value="none" }
						
						--,{ ope=RENDERSTATE_OPE_SETFILLMODE, value="solid" }
					}		
				},
				textures =
				{
					[1] = 
					{
						{ path='earth_th_pixels_16.jpg', stage=0 },
						{ path='earth_th_splatting_16.jpg', stage=1 },
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
		atmosphere_layer =
		{
			atmo_rendering =	
			{
				fx =
				{
					shaders = 
					{
						{ path='planet_atmosphere_vs.cso',mode=SHADER_COMPILED },
						{ path='planet_atmosphere_ps.cso',mode=SHADER_COMPILED }
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
			}
		}
	}

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

	local entity = nil
	local renderer = nil

	entity,renderer=commons.create_rendering_from_module(entity_layers,pl_mod,"planetsRender", { surface_rendering='texture_pass', atmo_rendering='texture_pass'})

	local specific_config = PlanetSpecificConfig()
	commons.procedural.planet.setup_specific_config(planet_specific_config_descr, specific_config)
	specific_config:apply(renderer)


	entity:add_aspect(INFOS_ASPECT)
	entity:setup_info( "entity_name", "test planet" )

	renderer:register_to_rendering(rg)

	eg:add_child('root','planet_entity',entity)

	entity:add_aspect(TRANSFORM_ASPECT)
	

	specific_config:updated()

	return entity,renderer,specific_config
end

set_camera = function(camera)

  if camera == free_cam then
    eg:set_camera(camera_entity)
    gui:show_mousecursor(FALSE)
    g:set_mousecursorcircularmode(TRUE)

  elseif camera == ship_cam then
    eg:set_camera(camera2_entity)
    gui:show_mousecursor(TRUE)
	g:set_mousecursorcircularmode(FALSE)
  end
end

free_cam = 0
ship_cam = 1

--current_cam = free_cam
current_cam = ship_cam

fog_intensity = 0.0

fog_color = 
{
	r = 0.75,
	g = 0.75,
	b = 0.99, 
}



mouse_right = FALSE


renderer_descr, renderer_width, renderer_height, renderer_fullscreen, viewport_width, viewport_height = renderer:descr()

g:print('rendering infos : '..renderer_descr..', '..renderer_width..'x'..renderer_height..' fullscreen='..renderer_fullscreen..' viewport='..viewport_width..'x'..viewport_height)

g:print("Skybox module loading begin...")
sb_mod = Module("skyboxmod", "skybox")
sb_mod:load()
g:print(sb_mod:get_descr().. ' loaded')

g:print("Mvt module loading begin...")
mvt_mod = Module("mvtmod", "mvts")
mvt_mod:load()
g:print(mvt_mod:get_descr().. ' loaded')


g:print("Planets module loading begin...")
pl_mod = Module("planetsmod", "planets")
pl_mod:load()
g:print(pl_mod:get_descr().. ' loaded')


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


camera_entity, camera_mvt=commons.create_free_camera(0.0, 0.0, 0.0, viewport_width,viewport_height, mvt_mod, "free_camera")
eg:add_child('root','camera_entity',camera_entity)



commons.update_lights( 'texture_pass', lights, renderers )


commons.setup_lit_flags( 'texture_pass', renderers, REFLECTIONS_OFF, reflectorPos, reflectorNormale, fog_intensity, fog_color)


g:print("Planet creation begin...")
planet_entity,planet_renderer,planet_specific_config = create_planet()
planet_transform = RawTransform()
planet_transform:configure(planet_entity)

planet_pos_mat = Matrix()
planet_pos_mat:translation( 0.0, 0.0, -40620000.0 )
planet_transform:add_matrix( "pos", planet_pos_mat )
g:print("Planet creation done...")



skybox_entity,skybox_renderer,sb_transform = create_skybox()



sphere_entity, sphere_renderer, sphere_transform = create_sphere()
renderers[nb_renderers] = sphere_renderer
nb_renderers = nb_renderers + 1


ship_entity, ship_renderer, ship_body = create_ship()
renderers[nb_renderers] = ship_renderer
nb_renderers = nb_renderers + 1



camera2_entity, camera2_pos=commons.create_static_camera(0.0, 110.0, 300.0, viewport_width,viewport_height, mvt_mod, "ship_camera")

camera2_entity:setup_info( "referent_body", "Bellorophon" )

eg:add_child('ship_entity','camera2_entity',camera2_entity)








-- ///////////////////////////////


commons.update_lights( 'texture_pass', lights, renderers )
commons.setup_lit_flags_simple( 'texture_pass', renderers, fog_intensity, fog_color)








--set_camera(current_cam)

g:print("Updating rendering queues...")
rg:update_renderingqueues()



g:add_mousemovecb( "onmousemove",
function( xm, ym, dx, dy )  

  if current_cam == free_cam then
	local mvt_info = { camera_mvt:read() }

	if mouse_right == FALSE then
	  camera_mvt:update(mvt_info[4],mvt_info[1],mvt_info[2],mvt_info[3],-dy / 4.0,-dx / 4.0, 0)
	else
	  camera_mvt:update(mvt_info[4],mvt_info[1],mvt_info[2],mvt_info[3],0,0,-dx)
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
      local mvt_info = { camera_mvt:read() }
	  camera_mvt:update(speed_factor,mvt_info[1],mvt_info[2],mvt_info[3],0,0,0)
	else
	  ship_body:update_forcestate("main prop", TRUE)
	end
  --W key
  elseif key == 87 then
    
    if current_cam == free_cam then
      local mvt_info = { camera_mvt:read() }
	  camera_mvt:update(-speed_factor,mvt_info[1],mvt_info[2],mvt_info[3],0,0,0)
	else
	  ship_body:update_forcestate("reverse prop", TRUE)
	end
  
  elseif key == 16 then -- left shift
    if current_cam == free_cam then
      speed_factor = 2000000.0
	end

  elseif key == 65 then --'A'
    ship_body:update_torquestate("roll_left", TRUE)

  elseif key == 90 then --'Z'
    ship_body:update_torquestate("roll_right", TRUE)

  elseif key == 37 then --VK_LEFT
    ship_body:update_torquestate("yaw_left", TRUE)

  elseif key == 38 then --VK_UP
    ship_body:update_torquestate("pitch_down", TRUE)

  elseif key == 39 then --VK_RIGHT
    ship_body:update_torquestate("yaw_right", TRUE)

  elseif key == 40 then --VK_DOWN
    ship_body:update_torquestate("pitch_up", TRUE)

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
      local mvt_info = { camera_mvt:read() }
	  camera_mvt:update(0.0,mvt_info[1],mvt_info[2],mvt_info[3],0,0,0)
	else
	  ship_body:update_forcestate("main prop", FALSE)
	end

  --W key
  elseif key == 87 then

    if current_cam == free_cam then
      local mvt_info = { camera_mvt:read() }
	  camera_mvt:update(0.0,mvt_info[1],mvt_info[2],mvt_info[3],0,0,0)
	else
	  ship_body:update_forcestate("reverse prop", FALSE)
	end

  -- VK_F1
  elseif key == 112 then  

    current_cam = current_cam + 1
	if current_cam == 2 then
	  current_cam = 0
	end

	set_camera(current_cam)

  elseif key == 16 then -- left shift
    
    if current_cam == free_cam then
      speed_factor = 90.0
	end

  elseif key == 76 then --'L'

    ship_body:zero_speed()

  elseif key == 77 then --'M'

    ship_body:zero_angularespeed()

  elseif key == 65 then --'A'
    ship_body:update_torquestate("roll_left", FALSE)

  elseif key == 90 then --'Z'
    ship_body:update_torquestate("roll_right", FALSE)

  elseif key == 37 then --VK_LEFT
    ship_body:update_torquestate("yaw_left", FALSE)

  elseif key == 38 then --VK_UP
    ship_body:update_torquestate("pitch_down", FALSE)

  elseif key == 39 then --VK_RIGHT
    ship_body:update_torquestate("yaw_right", FALSE)

  elseif key == 40 then --VK_DOWN
    ship_body:update_torquestate("pitch_up", FALSE)

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


  local time_infos = { root_entity:read_timemanager() }
  output_infos = renderer:descr() .." "..time_infos[3].. " fps "

  text_renderer:update(10, 30, 255, 0, 0, output_infos)

  text2_renderer:update(10, 70, 255, 0, 0, time_infos[2])

  local timescale = commons.print_timescale(time_infos[1])

  text3_renderer:update(10, 110, 255, 0, 0, timescale)

  -- display planet infos 

  local planet_infos = commons.procedural.planet.read_infos(planet_specific_config)

  local current_cam_id

  if current_cam == free_cam then
    current_cam_id = "free_camera"
  else
    current_cam_id = "ship_camera"
  end

  local relative_state
  
  local is_relative = planet_infos['viewsInfos']['ship_camera']['relative']

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

    relative_state = "RELATIVE"..' '..g:format_real(planet_infos["viewsInfos"][current_cam_id]["relative_altitude"],4)..' '..display_altitude..altitude_unit
											
  else
    relative_state = ""
  end

  text4_renderer:update(300, 70, 255, 0, 0, 'cam_id=' ..current_cam_id..' subpasses='..planet_infos['delayedSingleSubPassQueueSize']..
											' LOD='..planet_infos["viewsInfos"][current_cam_id]["currentLOD"]..' '..relative_state)

  local mvt_info = { camera_mvt:read() }
  camera_mvt:update(mvt_info[4],mvt_info[1],mvt_info[2],mvt_info[3],0,0,0)
end)


atmo = function( state )
	planet_specific_config:enable_atmosphere( state )
	planet_specific_config:updated()
end

lA = function()

  lights.light0.color.r = 1.0
  lights.light0.color.g = 1.0
  lights.light0.color.b = 1.0

  lights.light0.direction.x = 3.0
  lights.light0.direction.y = -1.0
  lights.light0.direction.z = 0.0

  commons.update_lights( 'texture_pass', lights, renderers )

  commons.update_planet_lights( lights, planet_specific_config)
  planet_specific_config:updated()

end

lB = function()

  lights.light0.color.r = 1.0
  lights.light0.color.g = 0.0
  lights.light0.color.b = 0.0

  lights.light0.direction.x = 0.0
  lights.light0.direction.y = -1.0
  lights.light0.direction.z = 0.0

  commons.update_lights( 'texture_pass', lights, renderers )

  commons.update_planet_lights( lights, planet_specific_config)
  planet_specific_config:updated()

end


--g:show_mousecursor(FALSE)
--g:set_mousecursorcircularmode(TRUE)


g:signal_renderscenebegin("eg")

lA()

commons.update_planet_lights( lights, planet_specific_config)
planet_specific_config:updated()

gui=Gui()
gui:init()

gui:set_resourcespath("./xfskin2")
gui:load_scheme("xfskin.scheme")
gui:load_layout("timecontrol.layout","xfskin2/layouts/timecontrol_widgets.conf")

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

    ship_body:zero_angularespeed()

  elseif widget == "Button_0Transl" then
    
	ship_body:zero_speed()

  end

  

end)


gui:set_mousecursorimage("xfskin/MouseCursor")

g:show_mousecursor(FALSE)


set_camera(current_cam)


root_entity:update_time(280)


