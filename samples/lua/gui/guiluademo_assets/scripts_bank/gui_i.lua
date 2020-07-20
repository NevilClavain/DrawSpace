

gui=Gui()

gui:init()

--[[
gui:set_resourcespath("./testskin")
gui:load_scheme("AlfiskoSkin.scheme")
gui:load_layout("main.layout","testskin/layouts/main_widgets.conf")
gui:set_layout("main.layout")
--]]

gui:set_resourcespath("./xfskin2")
gui:load_scheme("xfskin.scheme")
gui:load_layout("planetsetup.layout","xfskin2/layouts/planetsetup_widgets.conf")
gui:load_layout("planetseedssetup.layout","xfskin2/layouts/planetseedssetup_widgets.conf")


gui:set_layout("planetsetup.layout")

gui:show_gui(TRUE)


dbg_text_renderer=TextRendering()
dbg_text_renderer:configure(root_entity, "debug", 350, 20, 255, 0, 255, "")



g:add_keydowncb( "keydown",
function( key )  
  gui:on_keydown( key )
end)

g:add_keyupcb( "keyup",
function( key )  
  gui:on_keyup( key )
end)

g:add_oncharcb( "onchar",
function( char, scan )  
  gui:on_char( char )
end)

g:add_mousemovecb( "onmousemove",
function( xm, ym, dx, dy )  
  gui:on_mousemove( xm, ym, dx, dy )
end)


g:add_mouseleftbuttondowncb( "onmouselbdown",
function( xm, ym )
  gui:on_mouseleftbuttondown()
end)

g:add_mouseleftbuttonupcb( "onmouselbup",
function( xm, ym )
  gui:on_mouseleftbuttonup()
end)


g:add_mouserightbuttondowncb( "onmouserbdown",
function( xm, ym )
  gui:on_mouserightbuttondown()
end)

g:add_mouserightbuttonupcb( "onmouserbup",
function( xm, ym )
  gui:on_mouserightbuttonup()
end)

gui:set_mousecursorimage("xfskin/MouseCursor")
gui:show_mousecursor(TRUE)
g:show_mousecursor(FALSE)


gui:add_pushbuttonclickedcb( "onpushbutton",
function( layout, widget )
  
  -- dbg_text_renderer:update(350, 20, 255, 0, 255, layout.. " "..widget)

  if layout == 'planetsetup.layout' and widget == "Quit_Button" then
    g:quit()

  elseif layout == 'planetsetup.layout' and widget == "SeedsSetup_Button" then
    gui:set_layout("planetseedssetup.layout")

  elseif layout == 'planetseedssetup.layout' and widget == "Close_Button" then
    gui:set_layout("planetsetup.layout")

  end

end)



