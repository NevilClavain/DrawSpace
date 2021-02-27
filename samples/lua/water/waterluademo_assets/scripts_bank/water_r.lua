

g:signal_rendersceneend("eg")

model.releasemaincamera()

land.view.unload('l')
skydome.view.unload('dome')
waterquad.view.unload('water')


metalcube.view.unload('cube')
destroy_all_cubes()

spherebump.view.unload('sphere')

destroy_all_cubes()

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

