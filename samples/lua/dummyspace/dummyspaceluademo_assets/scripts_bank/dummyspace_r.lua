


g:signal_rendersceneend("eg")

planet_revol:release()
planet_transform:release()
boulder_transform:release()


spaceboxmod.view.unload('spacebox0')

ceresplanet.view.unload('ceres')

model.releasemaincamera()

g:remove_appruncb('run')


rg:remove_pass('texture_pass')


commons.trash.final_pass(rg, 'final_pass')

text_renderer:release()

rg:cleanup_renderingqueues()
g:release_assets()


g:show_mousecursor(TRUE)
g:set_mousecursorcircularmode(FALSE)

mvt_mod:unload()