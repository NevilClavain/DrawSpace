
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

tf0node = TransformationNodeBuilder( "mytf0" )
tf0node:LinkTo( "scene0", "scene0" )
mat = Matrix()
mat:Scale( 25, 25, 25 )
tf0node:AddMatrix( mat )

sb0 = SpaceBoxNodeBuilder( "sb0" )
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


keyboard = KeyboardWrapper()
mouse = MouseWrapper()

ds:LoadMouseScript( "mousehandler.lua" )
ds:LoadKeyUpScript( "keyuphandler.lua" )
ds:LoadKeyDownScript( "keyuphandler.lua" )
