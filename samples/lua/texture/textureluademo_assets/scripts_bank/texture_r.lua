
g:signal_rendersceneend("eg")

text_renderer:release()

g:remove_appruncb('run')

rg:release_pass_viewportquad_resources('final_pass')
rg:remove_pass_viewportquad('final_pass')

rg:update_renderingqueues()
g:release_assets()
