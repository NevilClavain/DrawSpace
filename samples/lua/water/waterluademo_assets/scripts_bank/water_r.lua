
eg:unregister_collider(land.models['l'].entity)
eg:unregister_rigidbody(metalcube.models['cube'].entity)
eg:unregister_rigidbody(spherebump.models['sphere'].entity)

g:signal_rendersceneend("eg")

eg:remove('model.camera.entity')
model.releasemaincamera()

land_transform:release()
land.view.unload('l')

skydome.view.unload('dome')
waterquad.view.unload('water')


impostors.view.unload('sprite')
impostors.view.unload('collimator')

screen_impostor_transform:release()
impostors.models['impostors1'].entity:remove_aspect(TRANSFORM_ASPECT)
impostors.view.unload('impostors1')

space_impostor_transform:release()
impostors.models['impostors0'].entity:remove_aspect(TRANSFORM_ASPECT)
impostors.view.unload('impostors0')


metalcube.view.unload('cube')
destroy_all_cubes()


commons.trash.meshe(rg, wireframecube_entity, wireframecube_renderer)
eg:remove('wireframe_cube')
--model.transformations['wireframe_cube'].transform:release()



spherebump.view.unload('sphere')


model.transformations['wireframe_circle'].transform:release()


commons.trash.meshe(rg, circle_entity, circle_renderer)
eg:remove('wireframe_circle')




objectdistance_renderer:release()
objectid_renderer:release()
text_renderer:release()
text2_renderer:release()

g:remove_appruncb('run')


rg:remove_pass('bump_pass')
rg:release_pass_viewportquad_resources('wave_pass')
rg:remove_pass_viewportquad('wave_pass')
rg:remove_pass('wave_pass')
rg:remove_pass('texture_pass')
rg:remove_pass('texturemirror_pass')
rg:remove_pass('texturerefrac_pass')
rg:remove_pass('waternormales_pass')
rg:remove_pass('waterhalfvector_pass')

commons.trash.final_pass(rg, 'final_pass')


rg:cleanup_renderingqueues()
g:release_assets()

g:remove_mousemovecb("onmousemove")
g:remove_keydowncb("keydown")
g:remove_keyupcb("keyup")


gui:show_gui(FALSE)
gui:unload_alllayouts()

g:show_mousecursor(TRUE)
g:set_mousecursorcircularmode(FALSE)


root_entity:release_world()
root_entity:remove_aspect(PHYSICS_ASPECT)

y_cube = nil

