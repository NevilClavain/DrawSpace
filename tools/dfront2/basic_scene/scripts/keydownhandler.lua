-- print( "key down : " .. keyboard:GetLastKeydownCode() )
if ds:IsCurrentCamera( "scene0", cam0 ) > 0 then
 
  if keyboard:GetLastKeydownCode() == 81 then
    fps0:SetSpeed( 6.0 )
  elseif keyboard:GetLastKeydownCode() == 87 then
    fps0:SetSpeed( -6.0 )
  end
  
elseif ds:IsCurrentCamera( "scene0", cam1 ) > 0 then

  if keyboard:GetLastKeydownCode() == 81 then
    free0:SetSpeed( 6.0 )
  elseif keyboard:GetLastKeydownCode() == 87 then
    free0:SetSpeed( -6.0 )
  end
  
end