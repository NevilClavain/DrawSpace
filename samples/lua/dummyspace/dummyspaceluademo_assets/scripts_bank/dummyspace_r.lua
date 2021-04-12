


g:signal_rendersceneend("eg")

--planet_revol:release()
planet_transform:release()



eg:remove('asteroid_cam')
commons.trash.camera(camera2_entity, camera2_pos)  
--boulder_transform:release()
rock_free_transfo:release()
boulder.view.unload('rock')

spaceboxmod.view.unload('spacebox0')
ceresplanet.view.unload('ceres')

bellerophon_entity:release_info("entity_name")
bellerophon_entity:remove_aspect(INFOS_ASPECT)
bellerophon.view.unload('ship')


model.releasemaincamera()



g:remove_appruncb('run')


rg:remove_pass('texture_pass')


commons.trash.final_pass(rg, 'final_pass')

text_renderer:release()

rg:cleanup_renderingqueues()
g:release_assets()


g:show_mousecursor(TRUE)
g:set_mousecursorcircularmode(FALSE)

root_entity:release_world()
root_entity:remove_aspect(PHYSICS_ASPECT)


mvt_mod:unload()