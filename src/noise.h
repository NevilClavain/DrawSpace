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

#ifndef _NOISE_H_
#define _NOISE_H_

#define MAX_DIMENSIONS		4							// Maximum number of dimensions in a noise object
//#define MAX_OCTAVES			128							// Maximum # of octaves in an fBm object

#include <stdlib.h>
#include <math.h>

//..//

// ECH : namespaces added by myself...
namespace DrawSpace
{
namespace Utils
{

/*******************************************************************************
* Class: CNoise
********************************************************************************
* This class implements the Perlin noise function. Initialize it with the number
* of dimensions (1 to 4) and a random seed. I got the source for the first 3
* dimensions from "Texturing & Modeling: A Procedural Approach". I added the
* extra dimension because it may be desirable to use 3 spatial dimensions and
* one time dimension. The noise buffers are set up as member variables so that
* there may be several instances of this class in use at the same time, each
* initialized with different parameters.
*******************************************************************************/
class Noise
{
protected:
	int                 m_nDimensions;						// Number of dimensions used by this object
	unsigned char       m_nMap[256];				// Randomized map of indexes into buffer
	double              m_nBuffer[256][MAX_DIMENSIONS];	// Random n-dimensional buffer

	double Lattice(int ix, double fx, int iy=0, double fy=0, int iz=0, double fz=0, int iw=0, double fw=0)
	{
		int i;
		int n[4] = {ix, iy, iz, iw};
		double f[4] = {fx, fy, fz, fw};
		int nIndex = 0;
		for(i=0; i<m_nDimensions; i++)
			nIndex = m_nMap[(nIndex + n[i]) & 0xFF];
		double fValue = 0;
		for(i=0; i<m_nDimensions; i++)
			fValue += m_nBuffer[nIndex][i] * f[i];
		return fValue;
	}

    inline void Normalize(double *f, int n)
    {
	    int i;
	    double fMagnitude = 0;
	    for(i=0; i<n; i++)
		    fMagnitude += f[i]*f[i];
	    fMagnitude = 1 / sqrt(fMagnitude);
	    for(i=0; i<n; i++)
		    f[i] *= fMagnitude;
    }

public:
	Noise( void ) {}
	
	void Init(int nDimensions, unsigned int nSeed);
	double GetNoise(double *f);
    
    unsigned char GetNMap( int p_index );
    double        GetNBuffer( int p_index, int p_index2 );
};

/*******************************************************************************
* Class: CFractal
********************************************************************************
* This class implements fBm, or fractal Brownian motion. Since fBm uses Perlin
* noise, this class is derived from CNoise. Initialize it with the number of
* dimensions (1 to 4), a random seed, H (roughness ranging from 0 to 1), and
* the lacunarity (2.0 is often used). Many of the fractal routines came from
* "Texturing & Modeling: A Procedural Approach". fBmTest() is my own creation,
* and I created it to generate my first planet.
*******************************************************************************/
class Fractal : public Noise
{
public:
    static const int MaxOctaves = 128;

protected:
	double m_fH;
	double m_fLacunarity;
	double m_fExponent[MaxOctaves];

public:
	Fractal()	{}
	Fractal(int nDimensions, unsigned int nSeed, double fH, double fLacunarity)
	{
		Init(nDimensions, nSeed, fH, fLacunarity);
	}
	void Init(int nDimensions, unsigned int nSeed, double fH, double fLacunarity)
	{
		Noise::Init(nDimensions, nSeed);
		m_fH = fH;
		m_fLacunarity = fLacunarity;
		double f = 1;
		for(int i=0; i<MaxOctaves; i++) 
		{
			m_fExponent[i] = pow(f, -m_fH);
			f *= m_fLacunarity;
		}
	}
	double fBm(double *f, double fOctaves);
    double GetExponent( int p_index ); 

    double GetLacunarity( void )
    {
        return m_fLacunarity;
    }
};

}
}

#endif