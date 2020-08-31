

g:signal_rendersceneend("eg")

model.releasemaincamera()

land.view.unload('l')
skydome.view.unload('dome')

destroy_all_cubes()

text_renderer:release()

g:remove_appruncb('run')




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


gui = nil

y_cube = nil

