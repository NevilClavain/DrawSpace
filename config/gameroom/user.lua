
gui:set_resourcespath("./guiskins")
gui:load_scheme("xfskin.scheme")
gui:load_scheme("AlfiskoSkin.scheme")


local luaextensions_descriptions = {}
local luaextensions_memallocs = {}


local assets_dir = "planet2luademo_assets"
--local assets_dir = "nebulaeluademo_assets"
--local assets_dir = "textureluademo_assets"
--local assets_dir = "waterluademo_assets"
--local assets_dir = "guiluademo_assets"
--local assets_dir = "dummyspaceluademo_assets"

g:set_texturesrootpath(assets_dir.."/textures_bank")
g:set_shadersrootpath(assets_dir.."/shaders_bank")
g:set_meshesrootpath(assets_dir.."/meshes_bank")
g:set_scriptsrootpath(assets_dir.."/scripts_bank")

g:enable_shadersdescrinfinalpath(TRUE)

luaextensions_descriptions["mesh"] = g:register_extension("mesh")
luaextensions_descriptions["skybox"] = g:register_extension("skybox")
luaextensions_descriptions["impostors"] = g:register_extension("impostors")
luaextensions_descriptions["planets"] = g:register_extension("planets")
luaextensions_descriptions["strings"] = g:register_extension("strings")

show_ext=function()
  g:print('########## Active LUA extensions ##########')
  for k,v in pairs(luaextensions_descriptions) do
    -- memorize total mem allocs foreach extension
    local alloctotalsize = g:get_extensionsalloctotalsize(k)

    g:print('  -> ' .. v)
	luaextensions_memallocs[k] = alloctotalsize
  end
  g:print('###########################################')
end

--g:set_virtualfs("test_data.bank")

run=function()

  -- display list of loaded lua extensions
  show_ext()

  g:dump_allextensionsalloc();

  --commons.utils.startup('gui_i.lua')
  commons.utils.startup('planet2_i.lua')
  --commons.utils.startup('nebulae_i.lua')
  --commons.utils.startup('texture_i.lua')
  --commons.utils.startup('water_i.lua')
  --commons.utils.startup('dummyspace_i.lua')
end

stop=function()


  --commons.utils.shutdown('gui_r.lua')
  commons.utils.shutdown('planet2_r.lua')
  --commons.utils.shutdown('nebulae_r.lua')
  --commons.utils.shutdown('texture_r.lua')
  --commons.utils.shutdown('water_r.lua')
  --commons.utils.shutdown('dummyspace_r.lua')

  for k,v in pairs(luaextensions_descriptions) do
    -- memorize total mem allocs foreach extension
    local alloctotalsize = g:get_extensionsalloctotalsize(k)

    --g:print('  -> ' .. v .. ' - mem allocs = ' .. alloctotalsize)
	if alloctotalsize ~= luaextensions_memallocs[k] then
	  g:print(' LUA EXTENSION MEM LEAK : '..' -> ' .. v .. ' - mem allocs = '.. luaextensions_memallocs[k] .. ' versus ' .. alloctotalsize)
	end
  end
  g:dump_allextensionsalloc();

end

-- restart	
rs=function()
	stop()
	run()
end

max=function()

	local assets_dir = "max_assets"

	g:set_texturesrootpath(assets_dir.."/textures_bank")
	g:set_shadersrootpath(assets_dir.."/shaders_bank")
	g:set_meshesrootpath(assets_dir.."/meshes_bank")
	g:set_scriptsrootpath(assets_dir.."/scripts_bank")

	g:enable_shadersdescrinfinalpath(TRUE)

    modelscenefile = "dinos_scene.lua"
	--modelscenefile = "model_raptor_scene.lua"
	--modelscenefile = "model_trex_scene.lua"
	--modelscenefile = "model_bellerophon_scene.lua"
	--modelscenefile = "model_land_scene.lua"
	--modelscenefile = "model_ceresplanet_scene.lua"
	--modelscenefile = "model_asteroid_scene.lua"
	g:do_file('max.lua')
end



