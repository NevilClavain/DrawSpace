
ds:AngleSpeedInc( roty_angle, 25 )
roty:Rotation( roty_axis, roty_angle:GetValue() )
tf2:UpdateMatrix( 1, roty )

