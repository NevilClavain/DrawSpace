
function globals()
        for n,v in pairs(_G) do
                print(n)
                end
        end

renderframe = RenderFrame()
default_scene = Scene()
default_scene:SetName( "default" )
renderframe:SetCurrentScene( default_scene:GetObject() )

assetsbase = AssetsBase()
assetsbase:InstanciateObject( "assetsbase" )

print( "--*- Welcome to DrawFront -*--" )
print( "Lua version : " .. _VERSION )

