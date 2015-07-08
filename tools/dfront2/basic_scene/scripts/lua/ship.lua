
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

ds:LoadMouseScript( "ship_mousehandler.lua" )
ds:LoadKeyUpScript( "ship_keyuphandler.lua" )
ds:LoadKeyDownScript( "ship_keydownhandler.lua" )
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

sb0:LinkTo( "scene0", "tf_spacebox" )
print( "spacebox loaded..." )



tf_ship = TransformationNode( "tf_ship", 2 )
tf_ship:LinkTo( "scene0", "scene0" )

ship_pos = Matrix()
ship_pos:Translation( 0, 0, -500 )


ship_rot = Matrix()
ship_rot:Rotation( Vector( 0, 1, 0, 1 ), 270 )

tf_ship:UpdateMatrix( 0, ship_pos )
tf_ship:UpdateMatrix( 1, ship_rot )



ship = ChunkNode( "bellerophon" )
ship:SetMesheName( "bellerophon_meshe" )

ship:RegisterPassSlot( "texture_pass" )
ship:SetPassSlotFxName( "texture_pass", "texture_fx" )
ship:SetPassSlotTextureName( "texture_pass", "bellerophon_texture", 0 )

ship:RegisterPassSlot( "vfogmask_pass" )
ship:SetPassSlotFxName( "vfogmask_pass", "vfogmasknocull_fx" )
ship:AddPassSlotShaderParam( "vfogmask_pass", "color", 1, 0, Vector( 0.0, 0.0, 0.0, 1.0 ) )



ship:LinkTo( "scene0", "tf_ship" )



tf_flame0 = TransformationNode( "tf_flame0", 2 )
tf_flame0:LinkTo( "scene0", "tf_ship" )

flame0_pos = Matrix()
flame0_pos:Translation( 100, 0, 300 )

flame0_scale = Matrix()
flame0_scale:Scale( 20, 20, 20 )

tf_flame0:UpdateMatrix( 0, flame0_pos )
tf_flame0:UpdateMatrix( 1, flame0_scale )





tf_flame1 = TransformationNode( "tf_flame1", 2 )
tf_flame1:LinkTo( "scene0", "tf_ship" )

flame1_pos = Matrix()
flame1_pos:Translation( -100, 0, 300 )

flame1_scale = Matrix()
flame1_scale:Scale( 20, 20, 20 )

tf_flame1:UpdateMatrix( 0, flame1_pos )
tf_flame1:UpdateMatrix( 1, flame1_scale )





flame0 = ChunkNode( "flame0" )
flame0:SetMesheName( "reactorflame_meshe" )


flame0:RegisterPassSlot( "vfogmask_pass" )
flame0:SetPassSlotFxName( "vfogmask_pass", "vfogmasknocull_fx" )
flame0:AddPassSlotShaderParam( "vfogmask_pass", "color", 1, 0, Vector( 1.0, 1.0, 1.0, 1.0 ) )


flame0:RegisterPassSlot( "vfogzback_pass" )
flame0:SetPassSlotFxName( "vfogzback_pass", "vfogzback_fx" )

flame0:RegisterPassSlot( "vfogzfront_pass" )
flame0:SetPassSlotFxName( "vfogzfront_pass", "vfogzfront_fx" )


flame0:LinkTo( "scene0", "tf_flame0" )






flame1 = ChunkNode( "flame1" )
flame1:SetMesheName( "reactorflame_meshe" )


flame1:RegisterPassSlot( "vfogmask_pass" )
flame1:SetPassSlotFxName( "vfogmask_pass", "vfogmasknocull_fx" )
flame1:AddPassSlotShaderParam( "vfogmask_pass", "color", 1, 0, Vector( 1.0, 1.0, 1.0, 1.0 ) )


flame1:RegisterPassSlot( "vfogzback_pass" )
flame1:SetPassSlotFxName( "vfogzback_pass", "vfogzback_fx" )

flame1:RegisterPassSlot( "vfogzfront_pass" )
flame1:SetPassSlotFxName( "vfogzfront_pass", "vfogzfront_fx" )


flame1:LinkTo( "scene0", "tf_flame1" )




scene_ready = true

