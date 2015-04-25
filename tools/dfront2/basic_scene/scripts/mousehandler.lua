
if ds:IsCurrentCamera( "scene0", cam0 ) > 0 then
  --print( "mouse event : dx = " .. mouse:GetLastDeltaXMouse() .. " dy = " .. mouse:GetLastDeltaYMouse() )

  fps0:RotateYaw( -mouse:GetLastDeltaXMouse() / 10, ds )
  fps0:RotatePitch( -mouse:GetLastDeltaYMouse() / 10, ds )

end