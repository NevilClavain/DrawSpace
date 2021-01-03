

dbg_text_renderer:release()


gui:show_mousecursor(FALSE)
gui:show_gui(FALSE)
gui:unload_alllayouts()


g:remove_keydowncb("keydown")
g:remove_keyupcb("keyup")
g:remove_oncharcb("onchar")
g:remove_mousemovecb("onmousemove")
g:remove_mouseleftbuttondowncb("onmouselbdown")
g:remove_mouseleftbuttonupcb("onmouselbup")
g:remove_mouserightbuttondowncb("onmouserbdown")
g:remove_mouserightbuttonupcb("onmouserbup")

g:show_mousecursor(TRUE)
gui:remove_buttonclickedcb("onpushbutton")

