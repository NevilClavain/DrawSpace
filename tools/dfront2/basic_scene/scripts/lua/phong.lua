
scene_ready = false


ds:DisplayFramerate( 1 )

ds:CreateWorld( "world0", Vector( 0.0, -9.81, 0.0, 0.0 ) )
ds:CreateSceneNodeGraph( "scene0" )


ds:DisplayCurrentCamera( "scene0", 1 )

collisions_pass[1] = "texture_pass"


ds:SetScenegraphEventCallback( function( evt_type, scenegraph_name )
  
  if evt_type == 0 and scene_ready then
  
    ds:AngleSpeedInc( roty_angle, 25 )
    
    
    teapot_roty_mat:Rotation( roty_axis, roty_angle:GetValue() )
    
    tf_teapot:UpdateMatrix( 1, teapot_roty_mat )
    
  end

end
)


roty_angle = Real()

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

--sb0:RegisterPassSlot( "normales_pass" )
--sb0:SetPassSlotFxName( "normales_pass", "color_fx" )
--sb0:AddPassSlotShaderParam( "normales_pass", "color", 1, 0, Vector( 0.0, 0.0, 0.0, 1.0 ) )



sb0:LinkTo( "scene0", "tf_spacebox" )
print( "spacebox loaded..." )









body_ground = InertBodyNode( "ground" )
body_ground:SetShapeDescrBox( Vector( 60.0, 0.0, 60.0, 1.0 ) )
body_ground:SetMass( 0.0 )
body_ground_pos = Matrix()
body_ground_pos:Translation( 0, -5, 0 )
body_ground:AddInitialAttitudeMatrix( body_ground_pos )


body_ground:LinkTo( "scene0", "scene0", "world0" )

print( "body_ground loaded..." )

ground_chunk = ChunkNode( "ground_chunk" )
ground_chunk:SetMesheName( "grid_meshe" )
ground_chunk:RegisterPassSlot( "texture_pass" )
ground_chunk:SetPassSlotFxName( "texture_pass", "texture_fx" )
ground_chunk:SetPassSlotTextureName( "texture_pass", "ground", 0 )

ground_chunk:RegisterPassSlot( "normales_pass" )
ground_chunk:SetPassSlotFxName( "normales_pass", "normales_fx" )


ground_chunk:LinkTo( "scene0", "ground" )
print( "ground_chunk loaded..." )



body0 = InertBodyNode( "body0" )
body0:SetShapeDescrBox( Vector( 0.5, 0.5, 0.5, 1.0 ) )
body0:SetMass( 50.0 )
body0:SetInitialState( 1 )
body0_pos = Matrix()
body0_pos:Translation( 0, 0, -18 )
body0:AddInitialAttitudeMatrix( body0_pos )

body0:LinkTo( "scene0", "scene0", "world0" )

print( "body0 loaded..." )

cube0 = ChunkNode( "cube0" )
cube0:SetMesheName( "cube_meshe" )
cube0:RegisterPassSlot( "texture_pass" )

cube0:SetPassSlotFxName( "texture_pass", "texture_fx" )
cube0:SetPassSlotTextureName( "texture_pass", "texture_shelby", 0 )

cube0:RegisterPassSlot( "normales_pass" )
cube0:SetPassSlotFxName( "normales_pass", "color_fx" )
cube0:AddPassSlotShaderParam( "normales_pass", "color", 1, 0, Vector( 0.0, 0.0, 0.0, 1.0 ) )


cube0:LinkTo( "scene0", "body0" )
print( "cube0 loaded..." )


spheretf = TransformationNode( "spheretf" )
spheretf:LinkTo( "scene0", "scene0" )
mat2 = Matrix()
mat2:Translation( -4, -1, -8 )
spheretf:AddMatrix( mat2 )



sphere0 = ChunkNode( "sphere0" )
sphere0:SetMesheName( "sphere_meshe" )
sphere0:RegisterPassSlot( "normales_pass" )
sphere0:SetPassSlotFxName( "normales_pass", "normales_fx" )
sphere0:RegisterPassSlot( "texture_pass" )
sphere0:SetPassSlotFxName( "texture_pass", "texture_fx" )
sphere0:SetPassSlotTextureName( "texture_pass", "texture_shelby", 0 )


sphere0:LinkTo( "scene0", "spheretf" )
print( "sphere0 loaded..." )




tf_teapot = TransformationNode( "tf_teapot" )
tf_teapot:LinkTo( "scene0", "scene0" )

mat3 = Matrix()
mat3:Translation( 4, -1, -11 )
tf_teapot:AddMatrix( mat3 )

roty_axis = Vector( 0, 1, 0, 1 )

teapot_roty_mat = Matrix();
teapot_roty_mat:Rotation( roty_axis, 0 );

tf_teapot:AddMatrix( teapot_roty_mat )


teapot = ChunkNode( "teapot" )
teapot:SetMesheName( "teapot_meshe" )

teapot:RegisterPassSlot( "normales_pass" )
teapot:SetPassSlotFxName( "normales_pass", "normales_fx" )
teapot:RegisterPassSlot( "texture_pass" )
teapot:SetPassSlotFxName( "texture_pass", "texture_fx" )
teapot:SetPassSlotTextureName( "texture_pass", "texture_shelby", 0 )


teapot:LinkTo( "scene0", "tf_teapot" )


scene_ready = true

