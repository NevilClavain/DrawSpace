

g:signal_rendersceneend("eg")


sb_transform:release()
skybox_entity:remove_aspect(TRANSFORM_ASPECT)
commons.trash.rendering(rg, sb_mod, skybox_entity, skybox_renderer)
eg:remove('skybox_entity')


ground_body:release()
ground_body:detach_fromentity(ground_entity)
ground_entity:remove_aspect(BODY_ASPECT)
commons.trash.meshe(rg, ground_entity, ground_renderer)
eg:remove('ground_entity')


sphere_body:release()
sphere_body:detach_fromentity(sphere_entity)
sphere_entity:remove_aspect(BODY_ASPECT)
commons.trash.meshe(rg, sphere_entity, sphere_renderer)
eg:remove('sphere_entity')

cube_body:release()
cube_body:detach_fromentity(cube_entity)
cube_entity:remove_aspect(BODY_ASPECT)
commons.trash.meshe(rg, cube_entity, cube_renderer)
eg:remove('cube_entity')



land_body:release()
land_body:detach_fromentity(land_entity)
land_entity:remove_aspect(BODY_ASPECT)
commons.trash.meshe(rg, land_entity, land_renderer)
eg:remove('land_entity')



commons.trash.camera( camera_entity, fps_transfo)
eg:remove('camera_entity')

destroy_all_cubes()

text_renderer:release()

g:remove_appruncb('run')



rg:release_pass_viewportquad_resources('wave_pass')
rg:remove_pass_viewportquad('wave_pass')
rg:remove_pass('wave_pass')
rg:remove_pass('bump_pass')

rg:remove_pass('texture_pass')
rg:remove_pass('texturemirror_pass')

commons.trash.final_pass(rg, 'final_pass')


rg:update_renderingqueues()
g:release_assets()

g:remove_mousemovecb("onmousemove")
g:remove_keydowncb("keydown")
g:remove_keyupcb("keyup")


gui:show_gui(FALSE)
gui:unload_alllayouts()
gui:unload_allschemes()
gui:release()

g:show_mousecursor(TRUE)
g:set_mousecursorcircularmode(FALSE)

root_entity:release_world()

root_entity:remove_aspect(PHYSICS_ASPECT)


sb_mod:unload()
mvt_mod:unload()


-- classes instances cleanup

sb_mod = nil
text_renderer = nil

camera_entity = nil
fps_transfo = nil

ground_entity = nil
ground_renderer = nil
ground_body = nil

cube_entity = nil
cube_renderer = nil
cube_body = nil

cube_rot_mat=nil
cube_pos_mat = nil
cube_final_mat = nil

skybox_entity = nil
skybox_renderer = nil
sb_transform = nil

land_entity = nil
land_renderer = nil

fps_yaw = nil
fps_pitch = nil
waves = nil
cube_rot = nil

gui = nil

y_cube = nil


bump_rendercontext = nil
bump_fxparams = nil
bump_textures = nil
bump_rss = nil

color_rendercontext = nil
color_fxparams = nil
color_textures = nil
color_rss = nil

wave_textures = nil
wave_fxparams = nil
wave_rendercontext = nil
waves_renderconfig = nil
