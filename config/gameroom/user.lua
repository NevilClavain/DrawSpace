
gui:set_resourcespath("./guiskins")
gui:load_scheme("xfskin.scheme")
gui:load_scheme("AlfiskoSkin.scheme")

include ('app_list.lua')


local luaextensions_descriptions = {}
local luaextensions_memallocs = {}




local current_app = 'planet2'



local assets_dir = app_list[current_app].assets_dir
g:set_texturesrootpath(assets_dir.."/textures_bank")
g:set_shadersrootpath(assets_dir.."/shaders_bank")
g:set_meshesrootpath(assets_dir.."/meshes_bank")
g:set_scriptsrootpath(assets_dir.."/scripts_bank")

g:enable_shadersdescrinfinalpath(TRUE)

luaextensions_descriptions["mesh"]			= g:register_extension("mesh")
luaextensions_descriptions["skybox"]		= g:register_extension("skybox")
luaextensions_descriptions["impostors"]		= g:register_extension("impostors")
luaextensions_descriptions["planets"]		= g:register_extension("planets")
luaextensions_descriptions["strings"]		= g:register_extension("strings")
luaextensions_descriptions["revolution"]	= g:register_extension("revolution")
luaextensions_descriptions["orbit"]			= g:register_extension("orbit")
luaextensions_descriptions["freemvt"]		= g:register_extension("freemvt")
luaextensions_descriptions["fpsmvt"]		= g:register_extension("fpsmvt")


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

  commons.utils.startup(app_list[current_app].in_file)
end

stop=function()

  commons.utils.shutdown(app_list[current_app].out_file)

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



