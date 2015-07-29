
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

ambient_level_r = 0.05
ambient_level_g = 0.05
ambient_level_b = 0.05


roty_angle = Real()

light_angle = Real()

light_rot_axis = Vector( 1, 0, 0, 1 )
light_dir = Vector( 0, -1, 0, 1 )
light_tdir = Vector()

light_mat = Matrix()
light_mat:Rotation( light_rot_axis, 0 )

light_rot_speed = 0


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

ground_chunk:RegisterPassSlot( "normales_pass" )
ground_chunk:SetPassSlotFxName( "normales_pass", "normales_fx" )

ground_chunk:RegisterPassSlot( "color_pass" )
ground_chunk:SetPassSlotFxName( "color_pass", "color_fx" )
ground_chunk:AddPassSlotShaderParam( "color_pass", "color", 1, 0, Vector( ambient_level_r, ambient_level_g, ambient_level_b, 1.0 ) )


ground_chunk:RegisterPassSlot( "fogint_pass" )
ground_chunk:SetPassSlotFxName( "fogint_pass", "fogint_fx" )
ground_chunk:AddPassSlotShaderParam( "fogint_pass", "fog intensity", 0, 20, Vector( 0.08, 0.0, 0.0, 1.0 ) )


ground_chunk:RegisterPassSlot( "hl_pass" )
ground_chunk:SetPassSlotFxName( "hl_pass", "hl_fx" )
ground_chunk:AddPassSlotShaderParam( "hl_pass", "ldir", 0, 20, Vector( -1.0, -1.0, 0.0, 0.0 ) )




ground_chunk:LinkTo( "scene0", "ground" )
print( "ground_chunk loaded..." )




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

sphere0:RegisterPassSlot( "color_pass" )
sphere0:SetPassSlotFxName( "color_pass", "color_fx" )
sphere0:AddPassSlotShaderParam( "color_pass", "color", 1, 0, Vector( ambient_level_r, ambient_level_g, ambient_level_b, 1.0 ) )


sphere0:RegisterPassSlot( "fogint_pass" )
sphere0:SetPassSlotFxName( "fogint_pass", "fogint_fx" )
sphere0:AddPassSlotShaderParam( "fogint_pass", "fog intensity", 0, 20, Vector( 0.08, 0.0, 0.0, 1.0 ) )


sphere0:RegisterPassSlot( "hl_pass" )
sphere0:SetPassSlotFxName( "hl_pass", "hl_fx" )
sphere0:AddPassSlotShaderParam( "hl_pass", "ldir", 0, 20, Vector( -1.0, -1.0, 0.0, 0.0 ) )




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

teapot:RegisterPassSlot( "hl_pass" )
teapot:SetPassSlotFxName( "hl_pass", "hl_fx" )
teapot:AddPassSlotShaderParam( "hl_pass", "ldir", 0, 20, Vector( -1.0, -1.0, 0.0, 0.0 ) )


teapot:RegisterPassSlot( "texture_pass" )
teapot:SetPassSlotFxName( "texture_pass", "texture_fx" )


teapot:RegisterPassSlot( "color_pass" )
teapot:SetPassSlotFxName( "color_pass", "color_fx" )
teapot:AddPassSlotShaderParam( "color_pass", "color", 1, 0, Vector( ambient_level_r, ambient_level_g, ambient_level_b, 1.0 ) )


teapot:RegisterPassSlot( "fogint_pass" )
teapot:SetPassSlotFxName( "fogint_pass", "fogint_fx" )
teapot:AddPassSlotShaderParam( "fogint_pass", "fog intensity", 0, 20, Vector( 0.08, 0.0, 0.0, 1.0 ) )



teapot:SetPassSlotTextureName( "texture_pass", "texture_shelby", 0 )


teapot:LinkTo( "scene0", "tf_teapot" )


scene_ready = true

