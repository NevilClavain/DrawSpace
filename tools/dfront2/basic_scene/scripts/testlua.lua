
function globals()
        for n,v in pairs(_G) do
                print(n)
                end
        end


function print_vector( v )
	s = ""
	for i = 0, 3, 1 do
		s = s .. v:Get( i ) .. " "
	end
	print( s )
end

function print_matrix( m )

	for i = 0, 3, 1 do
		s = ""
		for j = 0, 3, 1 do
			s = s .. m:Get( i, j ) .. " "
		end
		print( s )
	end
end

print( "Lua version : " .. _VERSION )

ds = DrawSpace()
ds:DisplayFramerate( 1 )


ds:CreateSceneNodeGraph( "scene0" )

ds:DisplayCurrentCamera( "scene0", 1 )

tf0node = TransformationNode( "mytf0", 2 )
tf0node:LinkTo( "scene0", "scene0" )

-- mat = Matrix()
-- mat:Scale( 25, 25, 25 )

-- tf0node:AddMatrix( mat )
tf0node:LoadScript( "tf_transformnode.lua" )

sb0 = SpaceboxNode( "sb0" )
sb0:RegisterPassSlot( "texture_pass" )
sb0:SetPassSlotFxName( "texture_pass", "texture_fx" )
sb0:SetPassSlotTextureName( "texture_pass", "texture_sb0", 0, 0 )
sb0:SetPassSlotTextureName( "texture_pass", "texture_sb2", 1, 0 )
sb0:SetPassSlotTextureName( "texture_pass", "texture_sb3", 2, 0 )
sb0:SetPassSlotTextureName( "texture_pass", "texture_sb1", 3, 0 )
sb0:SetPassSlotTextureName( "texture_pass", "texture_shelby", 4, 0 )
sb0:SetPassSlotTextureName( "texture_pass", "texture_shelby", 5, 0 )

sb0:AddPassSlotShaderParam( "texture_pass", "intensity", 1, 12, Vector( 1.0, -2.0, 3.0, -4.0 ) )
sb0:LinkTo( "scene0", "mytf0" )



keyboard = Keyboard()
mouse = Mouse()

ds:LoadMouseScript( "mousehandler.lua" )
ds:LoadKeyUpScript( "keyuphandler.lua" )
ds:LoadKeyDownScript( "keydownhandler.lua" )

sb_roty_angle = Real()
sb_scale = Matrix()
sb_scale:Scale( 25, 25, 25 )

sb_roty_axis = Vector( 0, 1, 0, 1 )

sb_roty = Matrix()
sb_roty:Rotation( sb_roty_axis, 45 )

tf0node:UpdateMatrix( 0, sb_roty )
tf0node:UpdateMatrix( 1, sb_scale )


fps0 = FpsMovementNode( "fps0" )
fps0:SetInitialTheta( 34 )
fps0:LinkTo( "scene0", "scene0" )



cam0 = CameraPointNode( "cam0" )
cam0:LinkTo( "scene0", "fps0" )

