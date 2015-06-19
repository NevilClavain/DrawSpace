



ds:DisplayFramerate( 1 )

ds:CreateWorld( "world0", Vector( 0.0, -9.81, 0.0, 0.0 ) )
ds:CreateSceneNodeGraph( "scene0" )


ds:DisplayCurrentCamera( "scene0", 1 )



ds:SetScenegraphEventCallback( function ( evt_type, scenegraph_name )

  
  
  if evt_type == 0 then
  
    ds:AngleSpeedInc( roty_angle, 25 )
    roty:Rotation( roty_axis, roty_angle:GetValue() )
    tf2:UpdateMatrix( 1, roty )
  end

end
)




keyboard = Keyboard()
mouse = Mouse()

ds:LoadMouseScript( "mousehandler.lua" )
ds:LoadKeyUpScript( "keyuphandler.lua" )
ds:LoadKeyDownScript( "keydownhandler.lua" )
print( "keyboard and mouse handlers loaded..." )



tf_spacebox = TransformationNode( "tf_spacebox", 1 )
tf_spacebox:LinkTo( "scene0", "scene0" )
sb_scale = Matrix()
sb_scale:Scale( 25, 25, 25 )
tf_spacebox:UpdateMatrix( 0, sb_scale )

sb0 = SpaceboxNode( "sb0" )
sb0:RegisterPassSlot( "texture_pass" )
sb0:SetPassSlotFxName( "texture_pass", "spacebox_fx" )
sb0:SetPassSlotTextureName( "texture_pass", "texture_sb0", 0, 0 )
sb0:SetPassSlotTextureName( "texture_pass", "texture_sb2", 1, 0 )
sb0:SetPassSlotTextureName( "texture_pass", "texture_sb3", 2, 0 )
sb0:SetPassSlotTextureName( "texture_pass", "texture_sb1", 3, 0 )
sb0:SetPassSlotTextureName( "texture_pass", "texture_sb4", 4, 0 )
sb0:SetPassSlotTextureName( "texture_pass", "texture_shelby", 5, 0 )

sb0:AddPassSlotShaderParam( "texture_pass", "intensity", 1, 12, Vector( 1.0, -2.0, 3.0, -4.0 ) )
sb0:LinkTo( "scene0", "tf_spacebox" )
print( "spacebox loaded..." )



tf2 = TransformationNode( "tf2", 2 )
tf2:LinkTo( "scene0", "scene0" )


mat = Matrix()
mat:Translation( 0, 1, -10 )
tf2:UpdateMatrix( 0, mat )

roty_axis = Vector( 0, 1, 0, 1 )

roty = Matrix()
roty:Rotation( roty_axis, 0 )
tf2:UpdateMatrix( 1, roty )

roty_angle = Real()




cube0 = ChunkNode( "cube0" )
cube0:SetMesheName( "cube_meshe" )
cube0:RegisterPassSlot( "texture_pass" )
cube0:SetPassSlotFxName( "texture_pass", "texture_fx" )
cube0:SetPassSlotTextureName( "texture_pass", "texture_shelby", 0 )

cube0:LinkTo( "scene0", "tf2" )
print( "cube0 loaded..." )

localtf = TransformationNode( "localtf" )
localtf:LinkTo( "scene0", "cube0" )
mat2 = Matrix()
mat2:Translation( 0, 0, 3 )
localtf:AddMatrix( mat2 )

cube1 = ChunkNode( "cube1" )
cube1:SetMesheName( "cube_meshe" )
cube1:RegisterPassSlot( "texture_pass" )
cube1:SetPassSlotFxName( "texture_pass", "texture_fx" )
cube1:SetPassSlotTextureName( "texture_pass", "texture_sb0", 0 )

cube1:LinkTo( "scene0", "localtf" )
print( "cube1 loaded..." )




lin0 = LinearMovementNode( "lin0" )
lin0:SetInitpos( -5, 0, -4 )
lin0:SetDirection( 1, 0, 0 )
lin0:LinkTo( "scene0", "scene0" )
print( "lin0 loaded..." )

cube2 = ChunkNode( "cube2" )
cube2:SetMesheName( "cube_meshe" )
cube2:RegisterPassSlot( "texture_pass" )
cube2:SetPassSlotFxName( "texture_pass", "texture_fx" )
cube2:SetPassSlotTextureName( "texture_pass", "texture_sb0", 0 )

cube2:LinkTo( "scene0", "lin0" )
print( "cube2 loaded..." )






circ0 = CircularMovementNode( "circ0" )
circ0:SetCenterpos( 6, 0, -6 )
circ0:SetDeltaCenterpos( -2, 0, 0 )
circ0:SetAxis( 0, 1, 0 )
circ0:LinkTo( "scene0", "scene0" )
print( "circ0 loaded..." )
circ0:SetAngularSpeed( 45.0 )


sphere0 = ChunkNode( "sphere0" )
sphere0:SetMesheName( "sphere_meshe" )
sphere0:RegisterPassSlot( "texture_pass" )
sphere0:SetPassSlotFxName( "texture_pass", "wireframe_fx" )
sphere0:AddPassSlotShaderParam( "texture_pass", "color", 1, 0, Vector( 1.0, 0.0, 0.0, 1.0 ) )
sphere0:LinkTo( "scene0", "circ0" )
print( "sphere0 loaded..." )



tf_teapot = TransformationNode( "tf_teapot" )
tf_teapot:LinkTo( "scene0", "scene0" )


mat_teapot = Matrix()
mat_teapot:Translation( 0, 0, -30 )
tf_teapot:AddMatrix( mat_teapot )


teapot = ChunkNode( "teapot" )
teapot:SetMesheName( "teapot_meshe" )
teapot:RegisterPassSlot( "texture_pass" )
teapot:SetPassSlotFxName( "texture_pass", "wireframe_fx" )
teapot:AddPassSlotShaderParam( "texture_pass", "color", 1, 0, Vector( 1.0, 0.0, 1.0, 1.0 ) )

teapot:LinkTo( "scene0", "tf_teapot" )
print( "teapot loaded..." )




fps0 = FpsMovementNode( "fps0" )
fps0:SetYMvt( 1 )
fps0:LinkTo( "scene0", "scene0" )
print( "fps0 loaded..." )

cam0 = CameraPointNode( "cam0" )
cam0:LinkTo( "scene0", "fps0" )
print( "cam0 loaded..." )



free0 = FreeMovementNode( "free0" )
free0:SetInitpos( 0, 0, 25 )
free0:LinkTo( "scene0", "scene0" )
print( "free0 loaded..." )

cam1 = CameraPointNode( "cam1" )
cam1:LinkTo( "scene0", "free0" )
print( "cam1 loaded..." )

lg0 = LongLatMovementNode( "lg0" )
lg0:SetInitialAlt( 2.1 )
lg0:SetInitialLongitud( 89.2 )
lg0:SetInitialLatitud( 12.34 )
lg0:LinkTo( "scene0", "sphere0" )


cube3 = ChunkNode( "cube3" )
cube3:SetMesheName( "cube_meshe" )
cube3:RegisterPassSlot( "texture_pass" )
cube3:SetPassSlotFxName( "texture_pass", "texture_fx" )
cube3:SetPassSlotTextureName( "texture_pass", "texture_sb0", 0 )

cube3:LinkTo( "scene0", "lg0" )
print( "cube3 loaded..." )


collisions_pass[1] = "texture_pass"

body0 = InertBodyNode( "body0" )
body0:SetShapeDescrBox( Vector( 0.5, 0.5, 0.5, 1.0 ) )
body0:SetMass( 10.0 )

body0_pos = Matrix()
body0_pos:Translation( 0, 10, -10 )
body0:AddInitialAttitudeMatrix( body0_pos )

body0:LinkTo( "scene0", "scene0", "world0" )
        


