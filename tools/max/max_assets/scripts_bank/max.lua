
max_init_cbfunc = nil
max_apprun_cbfunc = nil

text_x_position = 140
hmi_mode=FALSE

ctrl_key = FALSE
last_key = 0

mouse_right = FALSE
mouse_left = FALSE


help_text = ""

all_assets_loaded = false


g:add_resourceeventcb( "onresourceevent",
function( event, resource_path, context )

    if context == "init" then

      local evt_out = ""

      if event == BLOB_LOADED then
        evt_out = "loaded :"..resource_path
      elseif event == ASSET_SETLOADEDBLOB then
        evt_out = "set :"..resource_path
      elseif event == SHADERCACHE_CREATION then
        evt_out = "shader cache creation"
      elseif event == SHADER_COMPILATION then
        evt_out = "compiling :"..resource_path
      elseif event == SHADER_COMPILED then
        evt_out = "compilation done :"..resource_path
      elseif event == ALL_ASSETS_LOADED then
        evt_out = "All assets loaded !"
        g:deactivate_resourcessystem()
        rg:update_renderingqueues()
        all_assets_loaded = true
      --else
        --evt_out = "? : "..event
       end

      if evt_out ~= "" then
        local content = gui:get_widgettext("max.layout", "Label_resources")
        gui:set_widgettext("max.layout", "Label_resources", content.."\n"..evt_out)
      end

    end
end)


mvt_mod = Module("mvtmod", "mvts")
mvt_mod:load()
g:print(mvt_mod:get_descr().. ' loaded')

commons.init_final_pass(rg, 'final_pass')
rg:create_child('final_pass', 'texture_pass', 0)

rg:set_pass_targetclearcolor('texture_pass', 80, 80, 80)

--text_renderer=TextRendering()
--text_renderer:configure(root_entity, "fps", 80, 40, 255, 0, 255, "??? fps")

--move_renderer=TextRendering()
--move_renderer:configure(root_entity, "move", 80, 60, 255, 0, 255, "...")

--animsinfos_renderer=TextRendering()
--animsinfos_renderer:configure(root_entity, "anims", 80, 80, 255, 0, 255, "...")

operation_renderer=TextRendering()
operation_renderer:configure(root_entity, "current_operation", 80, 100, 255, 0, 255, "")

dbg_renderer=TextRendering()
dbg_renderer:configure(root_entity, "debug", 80, 100, 255, 0, 255, "...")
dbg_string = ""

root_entity:add_aspect(PHYSICS_ASPECT)


model.createmainfreecamera(0.0, 0.0, 0.0, mvt_mod)


-- ///////////////////////////////

eg:set_camera(model.camera.entity)





--rg:update_renderingqueues()


light_theta = SyncAngle()
light_phi = SyncAngle()

light_theta:init_fromtimeaspectof(root_entity, 0.0)
light_phi:init_fromtimeaspectof(root_entity,0.0)


move_light_vector = function(dx, dy)
		
  light_theta:inc(dx * 5)
  light_phi:inc(-dy * 5)

  model.env.light.setsphericaldir(light_theta:get_value(), light_phi:get_value())

end

g:add_mousemovecb( "onmousemove",function( xm, ym, dx, dy )  

  if hmi_mode == TRUE then
    if mouse_left == TRUE and mouse_right == FALSE then
      transformations_update( dx )

    elseif mouse_left == TRUE and mouse_right == TRUE then      
      move_light_vector(dx, dy)

    else
      gui:on_mousemove( xm, ym, dx, dy )
	end
  else

    local mvt_info = { model.camera.mvt:read() }
    if mouse_left == TRUE and mouse_right == FALSE then
      transformations_update( dx )

    elseif mouse_left == TRUE and mouse_right == TRUE then      
      move_light_vector(dx, dy)

    else
      if mouse_right == FALSE then
	    model.camera.mvt:update(mvt_info[4],mvt_info[1],mvt_info[2],mvt_info[3],-dy / 4.0,-dx / 4.0, 0)
      else
	    model.camera.mvt:update(mvt_info[4],mvt_info[1],mvt_info[2],mvt_info[3],0,0,-dx)
      end
    end
  end

end)


g:add_mouserightbuttondowncb( "onmouserightbuttondown", 
function( xm, ym )

  mouse_right = TRUE
  if hmi_mode == TRUE then
    gui:on_mouserightbuttondown()    
  end

end)

g:add_mouserightbuttonupcb( "onmouserightbuttonup", 
function( xm, ym )

  mouse_right = FALSE
  if hmi_mode == TRUE then
    gui:on_mouserightbuttonup()    
  end
end)

g:add_mouseleftbuttondowncb( "onmouseleftbuttondown", 
function( xm, ym )

  mouse_left = TRUE
  if hmi_mode == TRUE then
    gui:on_mouseleftbuttondown()  
  end
end)

g:add_mouseleftbuttonupcb( "onmouseleftbuttonup", 
function( xm, ym )

  mouse_left = FALSE
  if hmi_mode == TRUE then
    gui:on_mouseleftbuttonup()
  end
end)

g:add_keydowncb( "keydown",
function( key )

  if hmi_mode == TRUE then
    gui:on_keydown( key )
  else

    --Q key
    if key == 81 then 

      local mvt_info = { model.camera.mvt:read() }
	  model.camera.mvt:update(model.camera.speed,mvt_info[1],mvt_info[2],mvt_info[3],0,0,0)
      
    --W key
    elseif key == 87 then

      local mvt_info = { model.camera.mvt:read() }
	  model.camera.mvt:update(-model.camera.speed,mvt_info[1],mvt_info[2],mvt_info[3],0,0,0)
    elseif key == 17 then
      ctrl_key = TRUE
    end
  end

end)

g:add_keyupcb( "keyup",
function( key )  

  if hmi_mode == TRUE then
    if key == 112 then -- VK_F1
      if hmi_mode==TRUE then
        hmi_mode=FALSE
        gui:show_mousecursor(FALSE)
        g:set_mousecursorcircularmode(TRUE)
      end 
	else
	  gui:on_keyup( key )
	end    
  else

    last_key = key

    --Q key
    if key == 81 then
    
      local mvt_info = { model.camera.mvt:read() }
	  model.camera.mvt:update(0.0,mvt_info[1],mvt_info[2],mvt_info[3],0,0,0)

    --W key
    elseif key == 87 then
      local mvt_info = { model.camera.mvt:read() }
	  model.camera.mvt:update(0.0,mvt_info[1],mvt_info[2],mvt_info[3],0,0,0)
    elseif key == 17 then
      ctrl_key = FALSE
    -- VK_F1
    elseif key == 112 then

      if hmi_mode == FALSE then
        hmi_mode = TRUE
        gui:show_mousecursor(TRUE)
        g:set_mousecursorcircularmode(FALSE)
	  end   
    end

  end
end)

g:add_oncharcb( "onchar",
function( char, scan )  
  gui:on_char( char )
end)

g:add_appruncb( "run",
function()  

  time_infos = { root_entity:read_timemanager() }

  local timescale = commons.print_timescale(time_infos[1])

  local output_infos = "[MODEL VIEWER]    " ..renderer:descr() .." "..time_infos[3].. " fps "..time_infos[2].." timescale = "..timescale
  --text_renderer:update(text_x_position, 30, 255, 0, 0, output_infos)
  gui:set_widgettext("max.layout", "Label_infos", output_infos)

  model.printscenelist()
  gui:set_widgettext("max.layout", "Label_scene", model.printscenelist())

  local target_infos = ""
  

  if model.target ~= nil then

    if model.target ~= "" then
	  if model.entities[model.target] ~= nil then

	    target_infos = "selection = ["..model.target.."] ("..model.entities[model.target].model_classname..")\n"

        if model.entities[model.target].entity:has_aspect(BODY_ASPECT) == TRUE then

	      target_infos = target_infos.. " (is body)\n"

	    else

          local transform_entry = model.transformations[model.target]

          local pos_x = transform_entry['pos_mat']:get_value(3,0)
	      local pos_y = transform_entry['pos_mat']:get_value(3,1)
	      local pos_z = transform_entry['pos_mat']:get_value(3,2)

	      local scale_x = transform_entry['scale_mat']:get_value(0,0)
	      local scale_y = transform_entry['scale_mat']:get_value(1,1)
	      local scale_z = transform_entry['scale_mat']:get_value(2,2)

	      local rot_x = transform_entry['rotx_deg_angle']
	      local rot_y = transform_entry['roty_deg_angle']
	      local rot_z = transform_entry['rotz_deg_angle']

	      target_infos = target_infos.. " rot = "..rot_x.. " "..rot_y.." "..rot_z.."\npos = "..pos_x.." "..pos_y.." "..pos_z.."\nscale = "..scale_x.. " "..scale_y.." "..scale_z.."\n"

	    end
		
		if all_assets_loaded and model.entities[model.target].entity:has_aspect(ANIMATION_ASPECT) == TRUE then
		   target_infos = target_infos.."ANIMATED\n"

           target_infos = target_infos..model.anims.dump( model.entities[model.target].entity).."\n"
  
           local current_animation_name
           local current_animation_ticks_per_seconds
		   local current_animation_ticks_duration
           local current_animation_seconds_duration
           local current_animation_ticks_progress 
           local current_animation_seconds_progress 
		   
		   
		   current_animation_name, 
           current_animation_ticks_per_seconds, 
           current_animation_ticks_duration,
           current_animation_seconds_duration, 
           current_animation_ticks_progress, 
           current_animation_seconds_progress = model.entities[model.target].entity:read_currentanimationinfos()

		   target_infos = target_infos.."anim="..current_animation_name
												--[[
													.." ticks/s = "..current_animation_ticks_per_seconds..
													" "..current_animation_ticks_progress.."/"..current_animation_ticks_duration.." ticks "..
													current_animation_seconds_progress.."/"..current_animation_seconds_duration.. " s"
													]]
		end
		
      else
	    target_infos = target_infos.. " UNKNOWN"
	  end
    end
  else
    target_infos = target_infos.. " UNKNOWN"
  end

  --move_renderer:update(text_x_position, 70, 255, 255, 255, target_infos)
  gui:set_widgettext("max.layout", "Label_model",target_infos)

  --animsinfos_renderer:update(text_x_position, 90, 255, 255, 255, target_anims_infos)

  operation_renderer:update(text_x_position, 120, 255, 255, 255, model.current_operation)

  dbg_renderer:update(text_x_position, 150, 0, 255, 0, dbg_string)


  local mvt_info = { model.camera.mvt:read() }
  model.camera.mvt:update(mvt_info[4],mvt_info[1],mvt_info[2],mvt_info[3],0,0,0)

  if max_apprun_cbfunc ~= nil then
    max_apprun_cbfunc()
  end

end)

g:add_animationeventcb( "onanimationevent",
function( id, event, animation_name )
  if event == ANIMATION_END then
  
	 --dbg_string = animation_name..'-> '..id..' rand anim mode '..model.entities[id]['rand_anim_mode']..' current_animation_loop '..model.entities[id]['current_animation_loop'].. ' pool size = '.. model.entities[id]['entity']:read_animationpoolsize()

	 if animation_name ~= "transition_animation" then

       if model.entities[id]['rand_anim_mode'] == TRUE then

	     local selected_index = model.anims.compute_random_anim_index(model.entities[id]['do_something_distribution'], model.entities[id]['anim_action_distribution'], animations_rand_engine, model.entities[id]['rand_anims'], model.entities[id]['main_idle_anim'])
         model.entities[id]['current_animation_loop'] = selected_index
	     model.anims.pushanim(id, model.entities[id]['current_animation_loop'])

	   else

	     if model.entities[id]['current_animation_loop'] ~= -1 then	   
	       model.anims.pushanim(id, model.entities[id]['current_animation_loop'])
	     end

	   end
     end
  end
end)

root_entity:configure_world(environment.gravity_state, environment.gravity.x, environment.gravity.y, environment.gravity.z )


gui:load_layout("max.layout","guiskins/layouts/max_widgets.conf")
gui:set_layout("max.layout")
gui:show_gui(TRUE)

gui:add_pushbuttonclickedcb( "onpushbutton",
function( layout, widget )
  
  -- dbg_renderer:update(350, 20, 255, 0, 255, layout.. " "..widget)

  if widget == "Button_Quit" then
    g:quit()
  end

  if widget == "Button_AnimsDump" and model.target ~= "" and model.entities[model.target].entity:has_aspect(ANIMATION_ASPECT) == TRUE then
    model.anims.dump()
  end

  if widget == "Button_AnimsRun" and model.target ~= "" and model.entities[model.target].entity:has_aspect(ANIMATION_ASPECT) == TRUE then
	local index, error = g:stoi(gui:get_widgettext("max.layout", "Editbox_AnimIndex"))
	if error == 1 then
	  --g:print('CONV ERROR')
	else
	  model.anims.run(index)
	end	
  end

  if widget == "Button_AnimsRunLoop" and model.target ~= "" and model.entities[model.target].entity:has_aspect(ANIMATION_ASPECT) == TRUE then
	local index, error = g:stoi(gui:get_widgettext("max.layout", "Editbox_AnimIndex"))
	if error == 1 then
	  --g:print('CONV ERROR')
	else
	  model.anims.runloop(index)
	end	
  end

  if widget == "Button_AnimsRand" and model.target ~= "" and model.entities[model.target].entity:has_aspect(ANIMATION_ASPECT) == TRUE then
    model.anims.rand()
  end

  if widget == "Button_AnimsStop" and model.target ~= "" and model.entities[model.target].entity:has_aspect(ANIMATION_ASPECT) == TRUE then
    model.anims.stop()
  end

  --[[
  if widget == "Button_SceneContent" then
    model.printscenelist()
    gui:set_widgettext("max.layout", "Label_scene", model.printscenelist())
  end
  ]]

  if widget == "Button_ResestPos" then
    model.move.resetpos()
  end

  if widget == "Button_ResetRot" then
    model.move.resetrot()
  end

  if widget == "Button_ResetScale" then
    model.move.resetscale()
  end

  if widget == "Button_PosX" then
    model.move.posx()
  end

  if widget == "Button_PosY" then
    model.move.posy()
  end

  if widget == "Button_PosZ" then
    model.move.posz()
  end

  if widget == "Button_RotX" then
    model.move.rotx()
  end

  if widget == "Button_RotY" then
    model.move.roty()
  end

  if widget == "Button_RotZ" then
    model.move.rotz()
  end

  if widget == "Button_ScaleX" then
    model.move.scalex()
  end

  if widget == "Button_ScaleY" then
    model.move.scaley()
  end

  if widget == "Button_ScaleZ" then
    model.move.scalez()
  end

  if widget == "Button_ScaleXYZ" then
    model.move.scalexyz()
  end


  if widget == "Button_MesheFileInfos" then

    help_text = ""
	help_text = help_text..'<modelname>.dump.load()\n'
	help_text = help_text..'<modelname>.dump.show()\n'
	help_text = help_text..'<modelname>.dump.unload()\n'
    gui:set_widgettext("max.layout", "Label_help", help_text)

  end

  if widget == "Button_ModelView" then

    help_text = ""
	help_text = help_text..'<modelname>.view.load(<entity_id>)\n'
	help_text = help_text..'<modelname>.view.load(<entity_id>, {x = <value>, y = <value>, z = <value>})\n'
	help_text = help_text..'<modelname>.view.unload(<entity_id>)\n'
    gui:set_widgettext("max.layout", "Label_help", help_text)

  end

  if widget == "Button_TargetSel" then
    help_text = ""
	help_text = help_text..'model.target = <entity_id>\n'	
    gui:set_widgettext("max.layout", "Label_help", help_text)

  end

  if widget == "Button_ModelMove" then
    help_text = ""
	help_text = help_text..'model.move.posx()\n'
	help_text = help_text..'model.move.posy()\n'
	help_text = help_text..'model.move.posz()\n'
	help_text = help_text..'model.move.rotx()\n'
	help_text = help_text..'model.move.roty()\n'
	help_text = help_text..'model.move.rotz()\n'
	help_text = help_text..'model.move.scalex()\n'
	help_text = help_text..'model.move.scaley()\n'
	help_text = help_text..'model.move.scalez()\n'
	help_text = help_text..'model.move.scalexyz()\n'
	help_text = help_text..'model.move.resetpos()\n'
	help_text = help_text..'model.move.resetrot()\n'
	help_text = help_text..'model.move.resetscale()\n'
	help_text = help_text..'model.move.setpos(<entity_id>, <x>, <y>, <z>)\n'
    gui:set_widgettext("max.layout", "Label_help", help_text)

  end

  if widget == "Button_ModelAnims" then
    help_text = ""
	help_text = help_text..'model.anims.dump()\n'
	help_text = help_text..'model.anims.run(<anim_index>)\n'
	help_text = help_text..'model.anims.runloop(<anim_index>)\n'
	help_text = help_text..'model.anims.rand()\n'
	help_text = help_text..'model.anims.stop()\n'
    gui:set_widgettext("max.layout", "Label_help", help_text)

  end

  if widget == "Button_Env" then
    help_text = ""
	help_text = help_text..'model.camera.speed = <speed>\n'
	help_text = help_text..'model.env.setgravity(<bool>)\n'
	help_text = help_text..'model.env.setgravitydir(<x>, <y>, <z>)\n'
	help_text = help_text..'model.env.setbkcolor(<pass_id>, <r>, <g>, <b>)\n'
	help_text = help_text..'model.env.fog.setdensity(<density>)\n'
	help_text = help_text..'model.env.fog.setcolor(<r>, <g>, <b>)\n'
	help_text = help_text..'model.env.light.setdir(<x>, <y>, <z>)\n'
    help_text = help_text..'model.env.light.setsphericaldir(<theta>, <phi>)\n'
	help_text = help_text..'model.env.ambientlight.setcolor(<r>, <g>, <b>)\n'
	help_text = help_text..'model.env.light.setstate(<bool>)\n'
	help_text = help_text..'model.env.light.setcolor(<r>, <g>, <b>)\n'
    gui:set_widgettext("max.layout", "Label_help", help_text)

  end

end)

gui:set_mousecursorimage("AlfiskoSkin/MouseArrow")
g:set_mousecursorcircularmode(TRUE)
g:show_mousecursor(FALSE)


if modelscenefile ~= "" then
  g:print('Loading scene file : '..modelscenefile)
  g:do_file(modelscenefile)
else
  g:print('No scene file')
end


if max_init_cbfunc ~= nil then
  max_init_cbfunc()
end

g:signal_renderscenebegin("eg")
g:activate_resourcessystem("init")



