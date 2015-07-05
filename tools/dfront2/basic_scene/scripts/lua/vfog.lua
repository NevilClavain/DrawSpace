
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
fps0:SetInitpos( 0, 0, 25 )
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

cube0:RegisterPassSlot( "vfogmask_pass" )
cube0:SetPassSlotFxName( "vfogmask_pass", "vfogmask_fx" )
cube0:AddPassSlotShaderParam( "vfogmask_pass", "color", 1, 0, Vector( 0.0, 0.0, 0.0, 1.0 ) )



cube0:LinkTo( "scene0", "body0" )
print( "cube0 loaded..." )



localtf = TransformationNode( "localtf" )
localtf:LinkTo( "scene0", "scene0" )

mat0 = Matrix()
mat0:Translation( 0, -4.5, 0 )

localtf:AddMatrix( mat0 )

mat1 = Matrix()
mat1:Scale( 10, 2.5, 10 )
localtf:AddMatrix( mat1 )




bloc = ChunkNode( "bloc" )
bloc:SetMesheName( "cube_meshe" )

bloc:RegisterPassSlot( "vfogmask_pass" )
bloc:SetPassSlotFxName( "vfogmask_pass", "vfogmask_fx" )
bloc:AddPassSlotShaderParam( "vfogmask_pass", "color", 1, 0, Vector( 1.0, 1.0, 1.0, 1.0 ) )

bloc:RegisterPassSlot( "vfogzback_pass" )
bloc:SetPassSlotFxName( "vfogzback_pass", "vfogzback_fx" )

bloc:RegisterPassSlot( "vfogzfront_pass" )
bloc:SetPassSlotFxName( "vfogzfront_pass", "vfogzfront_fx" )


bloc:LinkTo( "scene0", "localtf" )



scene_ready = true

