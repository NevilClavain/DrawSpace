
include('spacebox_model.lua')

ctrl_key = FALSE


resources_event = "..."


g:add_resourceeventcb( "onresourceevent",
function( event, resource_path )

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
    else
       evt_out = "? : "..event       
    end
    resources_event = evt_out
end)




g:print('Current renderer is '..model.renderer_infos[1]..', '..model.renderer_infos[2]..'x'..model.renderer_infos[3])
renderer_infos = {renderer:descr()}
g:print('Current resolution is '..renderer_infos[2].." "..renderer_infos[3])


mvt_mod = Module("mvtmod", "mvts")
mvt_mod:load()
g:print(mvt_mod:get_descr().. ' loaded')

commons.init_final_pass(rg, 'final_pass')


rg:create_child('final_pass', 'transfer_pass', 0)

rg:create_pass_viewportquad('transfer_pass')
transferpass_rss=RenderStatesSet()
transferpass_rss:add_renderstate_in(RENDERSTATE_OPE_SETTEXTUREFILTERTYPE, "point")
transferpass_rss:add_renderstate_out(RENDERSTATE_OPE_SETTEXTUREFILTERTYPE, "linear")
transfer_textures = TexturesSet()
transfer_fxparams = FxParams()
transfer_fxparams:add_shaderfile('transfer2_vs.hlsl',SHADER_NOT_COMPILED)
transfer_fxparams:add_shaderfile('transfer2_ps.hlsl',SHADER_NOT_COMPILED)
transfer_fxparams:set_renderstatesset(transferpass_rss)

transfer_rendercontext = RenderContext('transfer_pass')
transfer_rendercontext:add_fxparams(transfer_fxparams)
transfer_rendercontext:add_shaderparam("camera_params", 1, 0)
transfer_rendercontext:add_shaderparam("view_matrix", 1, 1)
transfer_rendercontext:add_shaderparam("pos_matrix", 1, 5)
transfer_rendercontext:add_shaderparam("resol", 1, 10)

-- nebulae params creation
transfer_rendercontext:add_shaderparam("emissioncolor", 1, 11)
transfer_rendercontext:add_shaderparam("absorptioncolor", 1, 12)
transfer_rendercontext:add_shaderparam("flags", 1, 13)
transfer_rendercontext:add_shaderparam("densities", 1, 14)
transfer_rendercontext:add_shaderparam("rotatey", 1, 15)
transfer_rendercontext:add_shaderparam("seeds1", 1, 16)
transfer_rendercontext:add_shaderparam("seeds2", 1, 17)
transfer_rendercontext:add_shaderparam("pos1", 1, 18)
transfer_rendercontext:add_shaderparam("pos2", 1, 19)
transfer_rendercontext:add_shaderparam("pos3", 1, 20)




transfer_rendercontext:add_texturesset(transfer_textures)

transfer_renderconfig=RenderConfig()
transfer_renderconfig:add_rendercontext(transfer_rendercontext)
rg:configure_pass_viewportquad_resources('transfer_pass',transfer_renderconfig)

rg:create_child('transfer_pass', 'texture_pass', 0)


model.createmainfreecamera(0.0, 0.0, 0.0, mvt_mod)
eg:set_camera(model.camera.entity)


camera_width, camera_height, zn, zf = model.camera.entity:read_cameraparams()

g:print('camera params = '..camera_width..' '..camera_height..' '..zn..' '..zf )
rg:set_viewportquadshaderrealvector('transfer_pass', 'camera_params', camera_width, camera_height, zn, zf)
rg:set_viewportquadshaderrealvector('transfer_pass', 'resol', renderer_infos[2], renderer_infos[3], 0.0, 0.0)


text_renderer=TextRendering()
text_renderer:configure(root_entity, "resource_infos", 320, 130, 255, 0, 255, "resources...")




-- nebulae params set
neb_params = 
{
    emission_color = 
    {
        r = 0.3,
        g = 0.15,
        b = 0.0
	},
    absorption_color = 
    {
        r = 0.9,
        g = 0.7,
        b = 0.915
	},
    nb_spheres = 3,
    sphere0_light_intensity = 0.23,
    densities = 
    {
        9.5,
        9.7,
        8.6,
        11.0
	},
    seeds =
    {
        { 1088, 2132 },
        { 345, 54014 },
        { 47, 12 },
        { 100234, 899 }
    },

    twisty = 
    {
        1.0,
        1.0,
        1.0,
        1.0
	},

    positions =
    {
        { x = 0.0, y = 1.0, z = 0.0 },
        { x = 0.0, y = -1.0, z = 0.0 },
        { x = 0.7, y = -1.0, z = 0.0 }
	}
}

neb_update = function()

    rg:set_viewportquadshaderrealvector('transfer_pass', 'emissioncolor', 
            neb_params.emission_color.r, neb_params.emission_color.g, neb_params.emission_color.b, 0.0)

    rg:set_viewportquadshaderrealvector('transfer_pass', 'absorptioncolor', 
            neb_params.absorption_color.r, neb_params.absorption_color.g, neb_params.absorption_color.b, 0.0)

    rg:set_viewportquadshaderrealvector('transfer_pass', 'densities', 
            neb_params.densities[1], neb_params.densities[2], neb_params.densities[3], neb_params.densities[4] )

    rg:set_viewportquadshaderrealvector('transfer_pass', 'flags', neb_params.nb_spheres, neb_params.sphere0_light_intensity, 0.0, 0.0 )

    rg:set_viewportquadshaderrealvector('transfer_pass', 'seeds1', neb_params.seeds[1][1], neb_params.seeds[2][1], neb_params.seeds[3][1], neb_params.seeds[4][1] )
    rg:set_viewportquadshaderrealvector('transfer_pass', 'seeds2', neb_params.seeds[1][2], neb_params.seeds[2][2], neb_params.seeds[3][2], neb_params.seeds[4][2] )

    rg:set_viewportquadshaderrealvector('transfer_pass', 'rotatey', neb_params.twisty[1], neb_params.twisty[2], neb_params.twisty[3], neb_params.twisty[4] )

    rg:set_viewportquadshaderrealvector('transfer_pass', 'pos1', neb_params.positions[1].x, neb_params.positions[1].y, neb_params.positions[1].z, 0.0 )
    rg:set_viewportquadshaderrealvector('transfer_pass', 'pos2', neb_params.positions[2].x, neb_params.positions[2].y, neb_params.positions[2].z, 0.0 )
    rg:set_viewportquadshaderrealvector('transfer_pass', 'pos3', neb_params.positions[3].x, neb_params.positions[3].y, neb_params.positions[3].z, 0.0 )

end



rand_engine=RandomEngine()
rand_engine:set_seedfromtime()

emission_color_distr=Distribution("uniform_real_distribution", 0.0, 0.3)
absorption_color_distr=Distribution("uniform_real_distribution", 0.7, 1.0)
light_intensity_distr=Distribution("uniform_real_distribution", 0.0, 1.0)
densities_distr=Distribution("uniform_real_distribution", 7.0, 11.0)
nb_spheres_distr=Distribution("uniform_int_distribution", 0, 3)
twisty_distr=Distribution("uniform_int_distribution", 0, 1)
seed_distr=Distribution("uniform_real_distribution", 0.0, 99000.0)
pos_distr=Distribution("uniform_real_distribution", -10.0, 10.0)
pos_scale_distr=Distribution("uniform_real_distribution", 1.0, 1.5)

neb_params.emission_color.r = emission_color_distr:generate(rand_engine)
neb_params.emission_color.g = emission_color_distr:generate(rand_engine)
neb_params.emission_color.b = emission_color_distr:generate(rand_engine)

neb_params.absorption_color.r = absorption_color_distr:generate(rand_engine)
neb_params.absorption_color.g = absorption_color_distr:generate(rand_engine)
neb_params.absorption_color.b = absorption_color_distr:generate(rand_engine)

neb_params.sphere0_light_intensity = light_intensity_distr:generate(rand_engine)



neb_params.nb_spheres = nb_spheres_distr:generate(rand_engine)

for i = 1, neb_params.nb_spheres + 1, 1 do

  neb_params.densities[i] = densities_distr:generate(rand_engine)
  neb_params.twisty[i] = twisty_distr:generate(rand_engine)

  neb_params.seeds[i][1] = seed_distr:generate(rand_engine)
  neb_params.seeds[i][2] = seed_distr:generate(rand_engine)

end

for i = 1, neb_params.nb_spheres, 1 do

  local vec_pos = Vector()

  while vec_pos:get_x() == 0 do
    vec_pos:set_x(pos_distr:generate(rand_engine))
  end

  while vec_pos:get_y() == 0 do
    vec_pos:set_y(pos_distr:generate(rand_engine))
  end

  while vec_pos:get_z() == 0 do
    vec_pos:set_z(pos_distr:generate(rand_engine))
  end

  vec_pos:normalize()
  vec_pos:scale(pos_scale_distr:generate(rand_engine))

  neb_params.positions[i].x = vec_pos:get_x()
  neb_params.positions[i].y = vec_pos:get_y()
  neb_params.positions[i].z = vec_pos:get_z()

end

neb_update()

container_angle_y_deg = 0.0
container_angle_x_deg = 0.0


g:activate_resourcessystem();

mouse_right = FALSE


g:add_mousemovecb( "onmousemove",
function( xm, ym, dx, dy )  

  if ctrl_key == TRUE then

    container_angle_y_deg = container_angle_y_deg + dx
    container_angle_x_deg = container_angle_x_deg + dy

  else
    local mvt_info = { model.camera.mvt:read() }
    if mouse_right == FALSE then
  	  model.camera.mvt:update(mvt_info[4],mvt_info[1],mvt_info[2],mvt_info[3],-dy / 4.0,-dx / 4.0, 0)
    else
	  model.camera.mvt:update(mvt_info[4],mvt_info[1],mvt_info[2],mvt_info[3],0,0,-dx)
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

  local mvt_info = { model.camera.mvt:read() }
  model.camera.mvt:update(mvt_info[4],mvt_info[1],mvt_info[2],mvt_info[3],0,0,0)

  -- inject camera matrix into 
  local view_mat = Matrix()
  view_mat:store_entitytransformation(model.camera.entity, TRANSFORMATION_VIEW_MATRIX)

  -- inverse view to get camera matrix
  view_mat:inverse()


  -- sphere positionning in world
  local sphere_pos = Matrix()
  sphere_pos:translation( 0.0, 0.0, 0.0 )

  local sphere_scale = Matrix()
  sphere_scale:scale(1.0, 1.0, 1.0)

  local sphere_roty = Matrix()
  sphere_roty:rotation(0.0, 1.0, 0.0, commons.utils.deg_to_rad(container_angle_y_deg))

  local sphere_rotx = Matrix()
  sphere_rotx:rotation(1.0, 0.0, 0.0, commons.utils.deg_to_rad(container_angle_x_deg))

  local sphere_mat_1 = Matrix()
  sphere_mat_1:set_product(sphere_rotx, sphere_roty)

  local sphere_mat_2 = Matrix()
  sphere_mat_2:set_product(sphere_scale, sphere_mat_1)


  local sphere_mat = Matrix()
  sphere_mat:set_product(sphere_mat_2, sphere_pos)

  rg:set_viewportquadshaderrealmatrix('transfer_pass', 'view_matrix', view_mat)
  rg:set_viewportquadshaderrealmatrix('transfer_pass', 'pos_matrix', sphere_mat)


end)

g:add_keydowncb( "keydown",
function( key )

  --Q key
  if key == 81 then 
    local mvt_info = { model.camera.mvt:read() }

	model.camera.mvt:update(5.0,mvt_info[1],mvt_info[2],mvt_info[3],0,0,0)
      
  --W key
  elseif key == 87 then

    local mvt_info = { model.camera.mvt:read() }

	model.camera.mvt:update(-5.0,mvt_info[1],mvt_info[2],mvt_info[3],0,0,0)

  elseif key == 17 then
    ctrl_key = TRUE

  end

end)

g:add_keyupcb( "keyup",
function( key )  

  last_key = key

  --Q key
  if key == 81 then
    local mvt_info = { model.camera.mvt:read() }

	model.camera.mvt:update(0.0,mvt_info[1],mvt_info[2],mvt_info[3],0,0,0)
    
  --W key
  elseif key == 87 then
    local mvt_info = { model.camera.mvt:read() }

	model.camera.mvt:update(0.0,mvt_info[1],mvt_info[2],mvt_info[3],0,0,0)

  elseif key == 17 then
    ctrl_key = FALSE
     
  end
end)


g:show_mousecursor(FALSE)
g:set_mousecursorcircularmode(TRUE)

g:signal_renderscenebegin("eg")

spacebox_passes_config = 
{
	layer_0 = 
	{
        target_pass_id = 'texture_pass',
		rendering_id = 'layer0_rendering',
		lit_shader_update_func = nil
	}
}
spaceboxmod.view.load('spacebox0', spacebox_passes_config, 'root')


model.env.setbkcolor('texture_pass', 0.0,0.0,0.0)

model.camera.mvt:set_pos(0.0, 0.0, 10.0)


model.env.light.setstate( TRUE )
model.env.light.setdir(1.0, -0.4, 0.0)
model.env.ambientlight.setcolor(0.1, 0.1, 0.1)

