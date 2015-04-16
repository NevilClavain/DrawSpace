
if ds:IsCurrentCamera( "scene0", cam0 ) > 0 then
  print( "mouse event : dx = " .. mouse:GetLastDeltaXMouse() .. " dy = " .. mouse:GetLastDeltaYMouse() )

  fps0:RotateYaw( mouse:GetLastDeltaXMouse(), ds )
  fps0:RotatePitch( mouse:GetLastDeltaYMouse(), ds )

end