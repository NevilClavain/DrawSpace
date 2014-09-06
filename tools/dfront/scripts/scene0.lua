texture_vsh = Shader()

texture_vsh:InstanciateObject( "texture_vsh", "texture.vsh", 0 )
status = texture_vsh:LoadFromFile()
print( "texture_vsh loading = " .. status )

texture_psh = Shader()

texture_psh:InstanciateObject( "texture_psh", "texture.psh", 0 )
status = texture_psh:LoadFromFile()
print( "texture_psh loading = " .. status )



 -- passes creations

base_pass = Pass()
base_renderingnode = RenderingNode()

texturepass = IntermediatePass()
texturepass:InstanciateObject( "texture_pass", "texture_pass" )

base_pass:SetObject( texturepass:GetObject() )

texturepass_renderingqueue = RenderingQueue()

texturepass_renderingqueue:SetObject( base_pass:GetRenderingQueueObject() )

texturepass_renderingqueue:EnableDepthClearing( 1 )
texturepass_renderingqueue:EnableTargetClearing( 1 )
texturepass_renderingqueue:SetTargetClearingColor( 0, 0, 0 )

finalpass = FinalPass()
finalpass:InstanciateObject( "final_pass", "final_pass" )

base_pass:SetObject( finalpass:GetObject() )
base_pass:CreateViewportQuad()
final_viewportquad = ViewportQuad()
final_viewportquad:SetObject( base_pass:GetViewportQuadObject() )

final_fx = Fx()
final_fx:InstanciateObject( "final_fx" )
final_fx:AddShader( texture_vsh:GetObject() )
final_fx:AddShader( texture_psh:GetObject() )


base_renderingnode:SetObject( final_viewportquad:GetObject() )
base_renderingnode:SetFxObject( final_fx:GetObject() )

base_renderingnode:SetTextureObject( , 0 )



 -- load resources

texture0 = Texture() texture0:InstanciateObject( "bellerophon_texture", "bellerophon.jpg" )
status = texture0:LoadFromFile()
print( "texture0 loading = " .. status )

