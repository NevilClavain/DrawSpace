/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2015                              
*                                                                          
* This file is part of DrawSpace.                                          
*                                                                          
*    DrawSpace is free software: you can redistribute it and/or modify     
*    it under the terms of the GNU General Public License as published by  
*    the Free Software Foundation, either version 3 of the License, or     
*    (at your option) any later version.                                   
*                                                                          
*    DrawSpace is distributed in the hope that it will be useful,          
*    but WITHOUT ANY WARRANTY; without even the implied warranty of        
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         
*    GNU General Public License for more details.                          
*                                                                          
*    You should have received a copy of the GNU General Public License     
*    along with DrawSpace.  If not, see <http://www.gnu.org/licenses/>.    
*
*/

#ifndef _MATHS_H_
#define _MATHS_H_

#include <math.h>
#include "vector.h"

#ifndef PI
#define PI 3.1415927
#endif

namespace DrawSpace
{
namespace Utils
{
class Maths
{
public:

	static inline double	Square( dsreal a )						    { return a * a; };
	static inline int		Floor( dsreal a )							{ return ((int)a - (a < 0 && a != (int)a)); };
	static inline int		Ceiling( dsreal a )						    { return ((int)a + (a > 0 && a != (int)a)); };

	static inline double	Min( dsreal a, dsreal b )					{ return (a < b ? a : b); };
	static inline double	Max( dsreal a, dsreal b )					{ return (a > b ? a : b); };
	static inline double	Abs( dsreal a )							    { return (a < 0 ? -a : a); };
	static inline double	Clamp( dsreal a, dsreal b, dsreal x )		{ return (x < a ? a : (x > b ? b : x)); };
	static inline double	Lerp( dsreal a, dsreal b, dsreal x )		{ return a + x * (b - a); };
	static inline double	Cubic( dsreal a )							{ return a * a * (3 - 2*a); };
	static inline double	Pulse( dsreal a, dsreal b, dsreal x )		    { return (double)((x >= a) - (x >= b)); };
	static inline double	Gamma( dsreal a, dsreal g )				    { return pow(a, 1/g); };
	static inline double	Expose( dsreal l, dsreal k )				    { return (1 - exp(-l * k)); };
	static inline double	DegToRad( dsreal ang )					    { return ( ( ang * PI ) / 180.0 ); };
    static inline double	RadToDeg( dsreal ang )					    { return ( ( ang * 180.0 ) / PI ); };


	static inline void		SphericaltoCartesian( const Vector& p_in, Vector& p_out )
	{
		p_out[2] = ( p_in[0] * cos( p_in[2] ) * cos( p_in[1] ) );
		p_out[0] = ( p_in[0] * cos( p_in[2] ) * sin( p_in[1] ) );
		p_out[1] = ( p_in[0] * sin( p_in[2] ) );
	}
	
	static inline void		CartesiantoSpherical( Vector& p_in, Vector& p_out )
	{
		// cas particulier
		if( p_in[1] > 0.0 && 0.0 == p_in[0] && 0.0 == p_in[2] )
		{
			p_out[0] = p_in[1];
			p_out[2] = PI / 2.0;
			p_out[1] = 0.0;
			return;
		}
		else if( p_in[1] < 0.0 && 0.0 == p_in[0] && 0.0 == p_in[2] )
		{
			p_out[0] = -p_in[1];
			p_out[2] = -PI / 2.0;
			p_out[1] = 0.0;
			return;
		}
		
        p_out[0] = p_in.Length();
		p_out[2] = asin( p_in[1] / p_out[0] );		
		p_out[1] = atan2( p_in[0], p_in[2] );
	}
};
}
}

#endif
