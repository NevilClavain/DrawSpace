
if ds:IsCurrentCamera( "scene0", cam0 ) > 0 then
 
  fps0:RotateYaw( -mouse:GetLastDeltaXMouse() / 10, ds )
  fps0:RotatePitch( -mouse:GetLastDeltaYMouse() / 10, ds )

elseif ds:IsCurrentCamera( "scene0", cam1 ) > 0 then

  if mouse:IsLeftButtonDown() > 0 then
     free0:RotateRoll( -mouse:GetLastDeltaXMouse() / 10, ds )
  else
     free0:RotateYaw( -mouse:GetLastDeltaXMouse() / 10, ds )
     free0:RotatePitch( -mouse:GetLastDeltaYMouse() / 10, ds )
  end

end