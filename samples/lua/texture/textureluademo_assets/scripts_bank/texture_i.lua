
-- creer dans le final_pass un viewportquad avec un offset et y binder une texture lue depuis un fichier



g:add_resourceeventcb( "onresourceevent",
function( event, resource_path )

    if event == ALL_ASSETS_LOADED then
       g:deactivate_resourcessystem();
       rg:update_renderingqueues()
    end
end)

renderer_infos = {renderer:descr()}
g:print('Current renderer is '..renderer_infos[1]..', '..renderer_infos[2]..'x'..renderer_infos[3])


rg:create_pass_viewportquad('final_pass',-1.5)


finalpass_rss=RenderStatesSet()

finalpass_rss:add_renderstate_in(RENDERSTATE_OPE_SETTEXTUREFILTERTYPE, "point")
--finalpass_rss:add_renderstate_in(RENDERSTATE_OPE_SETFILLMODE,"line")
finalpass_rss:add_renderstate_out(RENDERSTATE_OPE_SETTEXTUREFILTERTYPE, "linear")
--finalpass_rss:add_renderstate_out(RENDERSTATE_OPE_SETFILLMODE,"solid")

textures = TexturesSet()
textures:set_texturefiletostage('mars.jpg', 0)

fxparams = FxParams()
fxparams:add_shaderfile('texture_vs.hlsl',SHADER_NOT_COMPILED)
fxparams:add_shaderfile('texture_ps.hlsl',SHADER_NOT_COMPILED)
fxparams:set_renderstatesset(finalpass_rss)


rendercontext = RenderContext('final_pass')
rendercontext:add_fxparams(fxparams)
rendercontext:add_texturesset(textures)

renderconfig=RenderConfig()
renderconfig:add_rendercontext(rendercontext)
rg:configure_pass_viewportquad_resources('final_pass',renderconfig)


text_renderer=TextRendering()
text_renderer:configure(root_entity, "fps", 10, 40, 255, 0, 255, "??? fps")

g:add_appruncb( "run",
function()

  time_infos = { root_entity:read_timemanager() }
  output_infos = renderer:descr() .." "..time_infos[3].. " fps "

  text_renderer:update(10, 30, 255, 0, 0, output_infos)

end)

g:activate_resourcessystem();
