
float3 ProjectVectorToCube( int p_orientation, float3 p_vector )
{
    float3 res;

	if( 0 == p_orientation ) // front
	{
		res.x = p_vector.x;
		res.y = p_vector.y;
		res.z = p_vector.z;   
	}
	else if( 1 == p_orientation ) // rear
	{
		res.x = -p_vector.x;
		res.y = p_vector.y;
		res.z = -p_vector.z;   
	}
	else if( 2 == p_orientation ) // left
	{
		res.x = -p_vector.z;
		res.y = p_vector.y;
		res.z = p_vector.x;   
	}
	else if( 3 == p_orientation ) // right
	{
		res.x = p_vector.z;
		res.y = p_vector.y;
		res.z = -p_vector.x;   
	}
	else if( 4 == p_orientation ) // top
	{
		res.x = p_vector.x;
		res.y = p_vector.z;
		res.z = -p_vector.y;   
	}
	else //if( 5 == p_orientation ) // bottom
	{
		res.x = p_vector.x;
		res.y = -p_vector.z;
		res.z = p_vector.y;
	}

    return res;
}

float3 CubeToSphere( float3 p_vector )
{
    float3 res;
	float xtemp = p_vector.x;
	float ytemp = p_vector.y;
	float ztemp = p_vector.z;

	res.x = xtemp * sqrt( 1.0 - ytemp * ytemp * 0.5 - ztemp * ztemp * 0.5 + ytemp * ytemp * ztemp * ztemp / 3.0 );
	res.y = ytemp * sqrt( 1.0 - ztemp * ztemp * 0.5 - xtemp * xtemp * 0.5 + xtemp * xtemp * ztemp * ztemp / 3.0 );
	res.z = ztemp * sqrt( 1.0 - xtemp * xtemp * 0.5 - ytemp * ytemp * 0.5 + xtemp * xtemp * ytemp * ytemp / 3.0 );

    return res;
}


