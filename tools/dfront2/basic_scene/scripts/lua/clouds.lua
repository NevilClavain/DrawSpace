
scene_ready = false

--create_collision_meshes = true


ds:DisplayFramerate( 1 )

ds:CreateWorld( "world0", Vector( 0.0, -9.81, 0.0, 0.0 ) )
ds:CreateSceneNodeGraph( "scene0" )


ds:DisplayCurrentCamera( "scene0", 1 )

-- collisions_pass[1] = "texture_pass"


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

cube0:LinkTo( "scene0", "body0" )
print( "cube0 loaded..." )



imp_tf = TransformationNode( "imp_tf" )
imp_tf:LinkTo( "scene0", "scene0" )

mat0 = Matrix()
mat0:Translation( 0, 2, -6 )
imp_tf:AddMatrix( mat0 )




free0 = FreeMovementNode( "free0" )
free0:SetInitpos( 0, 0, 25 )
free0:LinkTo( "scene0", "scene0" )
print( "free0 loaded..." )

cam1 = CameraPointNode( "cam1" )
cam1:LinkTo( "scene0", "free0" )
print( "cam1 loaded..." )


screenimpostors = { 
					{ 
            pos = { x = -5.0, y = 0.0, z = 0.0 } , scale = { width = 1.0, height = 1.0 }, 
					  uv = { u1 = 0.0, v1 = 0.0, u2 = 1.0, v2 = 0.0, u3 = 1.0, v3 = 1.0, u4 = 0.0, v4 = 1.0 } 
					}     
}

screenimp = ChunkNode( "screenimp" )
screenimp:SetImpostorsDisplayList( screenimpostors )
screenimp:RegisterPassSlot( "texture_pass" )
screenimp:SetPassSlotFxName( "texture_pass", "spaceimpostor_fx" )
screenimp:SetPassSlotTextureName( "texture_pass", "texture_shelby", 0 )
screenimp:AddPassSlotShaderParam( "texture_pass", "scale", 0, 24, Vector( 1.0, 1.0, 0.0, 0.0 ) )
screenimp:AddPassSlotShaderParam( "texture_pass", "flags", 1, 0, Vector( 0.0, 0.0, 0.0, 0.0 ) )
screenimp:AddPassSlotShaderParam( "texture_pass", "color", 1, 1, Vector( 0.0, 0.0, 0.0, 0.0 ) )
screenimp:LinkTo( "scene0", "imp_tf" )




clouds_tf = TransformationNode( "clouds_tf" )
clouds_tf:LinkTo( "scene0", "scene0" )

mat1 = Matrix()
mat1:Translation( 0, 800.0, 0 )
clouds_tf:AddMatrix( mat1 )



clouds = CloudsNode( "clouds_0" )
clouds:RegisterPassSlot( "texture_pass" )
clouds:SetPassSlotFxName( "texture_pass", "spaceimpostor_fx" )
clouds:SetPassSlotTextureName( "texture_pass", "clouds", 0 )
clouds:AddPassSlotShaderParam( "texture_pass", "flagv", 0, 24, Vector( 0.5, 0.0, 0.0, 0.0 ) )
clouds:AddPassSlotShaderParam( "texture_pass", "clouds_dims", 0, 25, Vector( 400, -200.0, 1.0, 0.76 ) )
clouds:AddPassSlotShaderParam( "texture_pass", "flagp", 1, 0, Vector( 0.5, 0.0, 0.0, 0.0 ) )
clouds:AddPassSlotShaderParam( "texture_pass", "clouds_color", 1, 1, Vector( 0.99, 0.99, 0.99, 0.99 ) )

clouds:EnableDetails( 1 )
clouds:SetSeed( 56645 )
clouds:SetProceduralFilePath( "clouds.rules" )

clouds:LinkTo( "scene0", "clouds_tf" )

scene_ready = true

