

g:set_texturesrootpath("test_data/textures_bank")
g:set_shadersrootpath("test_data/shaders_bank")
g:set_meshesrootpath("test_data/meshes_bank")
g:set_scriptsrootpath("test_data/scripts_bank")

g:enable_shadersdescrinfinalpath(TRUE)

--g:set_virtualfs("test_data.bank")

run=function()
	commons.utils.startup('testspace_i.lua')	
end

stop=function()
	commons.utils.shutdown('testspace_r.lua')
end

-- restart	
rs=function()
	stop()
	run()
end

max=function()
    modelscenefile = "model_scene0.lua"
	g:do_file('max.lua')
end

