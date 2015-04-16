
-- ds:AngleSpeedInc( sb_roty_angle, 10 )
sb_roty:Rotation( sb_roty_axis, sb_roty_angle:GetValue() )
tf0node:UpdateMatrix( 0, sb_roty )

