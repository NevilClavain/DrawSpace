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

/*
	The Universe Development Kit
	Copyright (C) 2000  Sean O'Neil
	soneil@home.com

	This library is free software; you can redistribute it and/or
	modify it under the terms of the GNU Lesser General Public
	License as published by the Free Software Foundation; either
	version 2.1 of the License, or (at your option) any later version.

	This library is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
	Lesser General Public License for more details.

	You should have received a copy of the GNU Lesser General Public
	License along with this library; if not, write to the Free Software
	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include <random>
#include "noise.h"
#include "maths.h"

#define SWAP(a, b, t)		t = a; a = b; b = t
#define DELTA				1e-6f						// Small number for comparing floating point numbers

using namespace DrawSpace;
using namespace DrawSpace::Utils;

void Noise::Init(int nDimensions, unsigned int nSeed)
{
    
    m_nDimensions = Maths::Min(nDimensions, MAX_DIMENSIONS);
    //CRandom r(nSeed);

    std::default_random_engine generator;
    std::uniform_real_distribution<double> rf( -0.5, 0.5 );
    std::uniform_int_distribution<int> ri( 0, 255 );
    generator.seed( nSeed );

	int i, j, k;
	for(i=0; i<256; i++)
	{
		m_nMap[i] = i;
		for(j=0; j<m_nDimensions; j++)
			m_nBuffer[i][j] = rf( generator );

		Normalize(m_nBuffer[i], m_nDimensions);        
	}

	while(--i)
	{
        j = ri( generator );
		SWAP(m_nMap[i], m_nMap[j], k);
	}
	//_fpreset();	// Bug in rand! Causes messed up floating point operations!
}

double Noise::GetNoise(double *f)
{
    
	int n[MAX_DIMENSIONS];			// Indexes to pass to lattice function
	double r[MAX_DIMENSIONS];		// Remainders to pass to lattice function
	double w[MAX_DIMENSIONS];		// Cubic values to pass to interpolation function

	for(int i=0; i<m_nDimensions; i++)
	{
        n[i] = Maths::Floor(f[i]);
		r[i] = f[i] - n[i];
        w[i] = Maths::Cubic(r[i]);
	}

	double fValue;
	switch(m_nDimensions)
	{
		case 1:
			fValue = Maths::Lerp(Lattice(n[0], r[0]),
						  Lattice(n[0]+1, r[0]-1),
						  w[0]);
			break;
		case 2:
			fValue = Maths::Lerp(Maths::Lerp(Lattice(n[0], r[0], n[1], r[1]),
							   Lattice(n[0]+1, r[0]-1, n[1], r[1]),
							   w[0]),
						  Maths::Lerp(Lattice(n[0], r[0], n[1]+1, r[1]-1),
							   Lattice(n[0]+1, r[0]-1, n[1]+1, r[1]-1),
							   w[0]),
						  w[1]);
			break;
		case 3:

			fValue = Maths::Lerp(Maths::Lerp(Maths::Lerp(Lattice(n[0], r[0], n[1], r[1], n[2], r[2]),
									Lattice(n[0]+1, r[0]-1, n[1], r[1], n[2], r[2]),
									w[0]),
							   Maths::Lerp(Lattice(n[0], r[0], n[1]+1, r[1]-1, n[2], r[2]),
									Lattice(n[0]+1, r[0]-1, n[1]+1, r[1]-1, n[2], r[2]),
									w[0]),
							   w[1]),
						  Maths::Lerp(Maths::Lerp(Lattice(n[0], r[0], n[1], r[1], n[2]+1, r[2]-1),
									Lattice(n[0]+1, r[0]-1, n[1], r[1], n[2]+1, r[2]-1),
									w[0]),
							   Maths::Lerp(Lattice(n[0], r[0], n[1]+1, r[1]-1, n[2]+1, r[2]-1),
									Lattice(n[0]+1, r[0]-1, n[1]+1, r[1]-1, n[2]+1, r[2]-1),
									w[0]),
							   w[1]),
						  w[2]);
			break;
		case 4:
			fValue = Maths::Lerp(Maths::Lerp(Maths::Lerp(Maths::Lerp(Lattice(n[0], r[0], n[1], r[1], n[2], r[2], n[3], r[3]),
										 Lattice(n[0]+1, r[0]-1, n[1], r[1], n[2], r[2], n[3], r[3]),
										 w[0]),
									Maths::Lerp(Lattice(n[0], r[0], n[1]+1, r[1]-1, n[2], r[2], n[3], r[3]),
										 Lattice(n[0]+1, r[0]-1, n[1]+1, r[1]-1, n[2], r[2], n[3], r[3]),
										 w[0]),
									w[1]),
									Maths::Lerp(Maths::Lerp(Lattice(n[0], r[0], n[1], r[1], n[2]+1, r[2]-1, n[3], r[3]),
										 Lattice(n[0]+1, r[0]-1, n[1], r[1], n[2]+1, r[2]-1, n[3], r[3]),
										 w[0]),
									Maths::Lerp(Lattice(n[0], r[0], n[1]+1, r[1]-1, n[2]+1, r[2]-1),
										 Lattice(n[0]+1, r[0]-1, n[1]+1, r[1]-1, n[2]+1, r[2]-1, n[3], r[3]),
										 w[0]),
									w[1]),
							   w[2]),
						  Maths::Lerp(Maths::Lerp(Maths::Lerp(Lattice(n[0], r[0], n[1], r[1], n[2], r[2], n[3]+1, r[3]-1),
										 Lattice(n[0]+1, r[0]-1, n[1], r[1], n[2], r[2], n[3]+1, r[3]-1),
										 w[0]),
									Maths::Lerp(Lattice(n[0], r[0], n[1]+1, r[1]-1, n[2], r[2], n[3]+1, r[3]-1),
										 Lattice(n[0]+1, r[0]-1, n[1]+1, r[1]-1, n[2], r[2], n[3]+1, r[3]-1),
										 w[0]),
									w[1]),
									Maths::Lerp(Maths::Lerp(Lattice(n[0], r[0], n[1], r[1], n[2]+1, r[2]-1, n[3]+1, r[3]-1),
										 Lattice(n[0]+1, r[0]-1, n[1], r[1], n[2]+1, r[2]-1, n[3]+1, r[3]-1),
										 w[0]),
									Maths::Lerp(Lattice(n[0], r[0], n[1]+1, r[1]-1, n[2]+1, r[2]-1),
										 Lattice(n[0]+1, r[0]-1, n[1]+1, r[1]-1, n[2]+1, r[2]-1, n[3]+1, r[3]-1),
										 w[0]),
									w[1]),
							   w[2]),
						  w[3]);
			break;
	}	
    return Maths::Clamp( -0.999999, 0.999999, fValue );
}

unsigned char Noise::GetNMap( int p_index )
{
    return m_nMap[p_index];
}

double Noise::GetNBuffer( int p_index, int p_index2 )
{
    return m_nBuffer[p_index][p_index2];
}

double Fractal::fBm(double *f, int fOctaves)
{
    
	int i;
	// Initialize locals
	double fValue = 0;
    double fexp = 1.0;
	double fTemp[MAX_DIMENSIONS];
	for(i=0; i<m_nDimensions; i++)
		fTemp[i] = f[i];

	// Inner loop of spectral construction, where the fractal is built
	for(i=0; i<fOctaves; i++)
	{
		fValue += GetNoise(fTemp) * pow( fexp, -m_fH );
		for(int j=0; j<m_nDimensions; j++)
			fTemp[j] *= m_fLacunarity;
        fexp *= m_fLacunarity;
	}
		
	return Maths::Clamp(-1.0, 1.0, fValue);    
}