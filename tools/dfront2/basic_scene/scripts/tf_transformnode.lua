
ds:AngleSpeedInc( sb_roty_angle, 10 )

sb_roty:Rotation( Vector( 0, 1, 0, 1 ), sb_roty_angle:GetValue() )
tf0node:UpdateMatrix( 0, sb_roty )

