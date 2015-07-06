
scene_ready = false


ds:DisplayFramerate( 1 )

ds:CreateWorld( "world0", Vector( 0.0, -9.81, 0.0, 0.0 ) )
ds:CreateSceneNodeGraph( "scene0" )


ds:DisplayCurrentCamera( "scene0", 1 )

collisions_pass[1] = "texture_pass"


ds:SetScenegraphEventCallback( function( evt_type, scenegraph_name )
  
  if evt_type == 0 and scene_ready then
  
  end

end
)




keyboard = Keyboard()
mouse = Mouse()

ds:LoadMouseScript( "mousehandler.lua" )
ds:LoadKeyUpScript( "keyuphandler.lua" )
ds:LoadKeyDownScript( "keydownhandler.lua" )
print( "keyboard and mouse handlers loaded..." )


fps0 = FpsMovementNode( "fps0" )
fps0:SetYMvt( 1 )
fps0:LinkTo( "scene0", "scene0" )
print( "fps0 loaded..." )

cam0 = CameraPointNode( "cam0" )
cam0:LinkTo( "scene0", "fps0" )
print( "cam0 loaded..." )

ds:SetSceneNodeGraphCurrentCamera( "scene0", "cam0" )



tf_spacebox = TransformationNode( "tf_spacebox", 1 )
tf_spacebox:LinkTo( "scene0", "scene0" )
sb_scale = Matrix()
sb_scale:Scale( 25, 25, 25 )
tf_spacebox:UpdateMatrix( 0, sb_scale )

sb0 = SpaceboxNode( "sb0" )
sb0:RegisterPassSlot( "texture_pass" )
sb0:SetPassSlotFxName( "texture_pass", "spacebox_fx" )
sb0:SetPassSlotTextureName( "texture_pass", "spacebox_front", 0, 0 )
sb0:SetPassSlotTextureName( "texture_pass", "spacebox_back", 1, 0 )
sb0:SetPassSlotTextureName( "texture_pass", "spacebox_left", 2, 0 )
sb0:SetPassSlotTextureName( "texture_pass", "spacebox_right", 3, 0 )
sb0:SetPassSlotTextureName( "texture_pass", "spacebox_top", 4, 0 )
sb0:SetPassSlotTextureName( "texture_pass", "spacebox_bottom", 5, 0 )

sb0:AddPassSlotShaderParam( "texture_pass", "intensity", 1, 12, Vector( 1.0, -2.0, 3.0, -4.0 ) )
sb0:LinkTo( "scene0", "tf_spacebox" )
print( "spacebox loaded..." )




scene_ready = true

