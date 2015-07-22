--[[ script combinant un effet de brouillard et un rendu sur texture ]]


scene_ready = false


ds:DisplayFramerate( 1 )

ds:CreateWorld( "world0", Vector( 0.0, -9.81, 0.0, 0.0 ) )
ds:CreateSceneNodeGraph( "scene0" )

ds:CreateSceneNodeGraph( "sub_scene" )


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


-- main scene construction

fps0 = FpsMovementNode( "fps0" )
fps0:SetYMvt( 1 )
fps0:LinkTo( "scene0", "scene0" )
print( "fps0 loaded..." )

cam0 = CameraPointNode( "cam0" )
cam0:LinkTo( "scene0", "fps0" )
print( "cam0 loaded..." )

ds:SetSceneNodeGraphCurrentCamera( "scene0", "cam0" )

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

ground_chunk:RegisterPassSlot( "fogint_pass" )
ground_chunk:SetPassSlotFxName( "fogint_pass", "fogint_fx" )
ground_chunk:AddPassSlotShaderParam( "fogint_pass", "fog intensity", 0, 12, Vector( 0.08, 0.0, 0.0, 1.0 ) )


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

-- cube0:SetPassSlotTextureName( "texture_pass", "texture_shelby", 0 )

cube0:RegisterPassSlot( "fogint_pass" )
cube0:SetPassSlotFxName( "fogint_pass", "fogint_fx" )
cube0:AddPassSlotShaderParam( "fogint_pass", "fog intensity", 0, 20, Vector( 0.08, 0.0, 0.0, 1.0 ) )


cube0:LinkTo( "scene0", "body0" )
print( "cube0 loaded..." )

cube0:UpdatePassSlotTexture( "texture_pass", Texture( "subscene_pass" ), 0 )


-- sub scene construction


circ0 = CircularMovementNode( "circ0" )
circ0:SetCenterpos( 0, 0, -12 )
circ0:SetDeltaCenterpos( -5.5, 0, 0 )
circ0:SetAxis( 0, 1, 0 )
circ0:LinkTo( "sub_scene", "sub_scene" )
print( "circ0 loaded..." )
circ0:SetAngularSpeed( 45.0 )



teapot = ChunkNode( "teapot" )
teapot:SetMesheName( "teapot_meshe" )
teapot:RegisterPassSlot( "subscene_pass" )
teapot:SetPassSlotFxName( "subscene_pass", "wireframe_fx" )
teapot:AddPassSlotShaderParam( "subscene_pass", "color", 1, 0, Vector( 1.0, 1.0, 1.0, 1.0 ) )

teapot:LinkTo( "sub_scene", "circ0" )
print( "teapot loaded..." )


cam1 = CameraPointNode( "cam1" )
cam1:LinkTo( "sub_scene", "sub_scene" )
print( "cam1 loaded..." )

cam1:LockOn( "teapot" )


ds:SetSceneNodeGraphCurrentCamera( "sub_scene", "cam1" )

scene_ready = true

