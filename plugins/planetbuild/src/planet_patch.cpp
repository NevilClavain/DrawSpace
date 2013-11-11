/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2014                                
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

#include "planet_patch.h"
#include "quadtree.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Utils;

Patch::Patch( int p_resolution, dsreal p_ray, int p_orientation, const dsstring& p_name, Patch* p_parent, int p_parentnodeid ) : 
m_resolution( p_resolution ), 
m_orientation( p_orientation ),
m_name( p_name ),
m_ray( p_ray )
{
	for( long i = 0; i < 8; i++ )
	{
		m_neighbours[i] = NULL;
	}

	if( NULL == p_parent )
	{
		m_xpos = m_ypos = 0.0;
		m_sidelength = 2.0;    // on travaille sur une sphere de rayon = 1.0, donc diametre = 2.0
	}
	else
	{
		m_sidelength = p_parent->m_sidelength / 2.0;

		switch( p_parentnodeid )
		{
			case BaseQuadtreeNode::NorthWestNode:

				m_xpos = -p_parent->m_sidelength / 4.0;
				m_xpos += p_parent->m_xpos;
				m_ypos = p_parent->m_sidelength / 4.0;
				m_ypos += p_parent->m_ypos;
				break;

			case BaseQuadtreeNode::NorthEastNode:

				m_xpos = p_parent->m_sidelength / 4.0;
				m_xpos += p_parent->m_xpos;
				m_ypos = p_parent->m_sidelength / 4.0;
				m_ypos += p_parent->m_ypos;
				break;

			case BaseQuadtreeNode::SouthEastNode:

				m_xpos = p_parent->m_sidelength / 4.0;
				m_xpos += p_parent->m_xpos;
				m_ypos = -p_parent->m_sidelength / 4.0;
				m_ypos += p_parent->m_ypos;
				break;

			case BaseQuadtreeNode::SouthWestNode:

				m_xpos = -p_parent->m_sidelength / 4.0;
				m_xpos += p_parent->m_xpos;
				m_ypos = -p_parent->m_sidelength / 4.0;
				m_ypos += p_parent->m_ypos;
				break;

			default:
				m_xpos = 0.0; m_ypos = 0.0;
				break;
		}       
	}

	build();
}

Patch::~Patch( void )
{
}

void Patch::SetNeighbour( Patch* p_patch, int p_id )
{
	m_neighbours[p_id] = p_patch;
}

Patch* Patch::GetNeighbour( int p_id )
{
	return m_neighbours[p_id];
}

void Patch::cubetosphere( const Vector& p_in, Vector& p_out )
{
	dsreal x = p_in[0];
	dsreal y = p_in[1];
	dsreal z = p_in[2];

	p_out[0] = x * sqrt( 1.0 - y * y * 0.5 - z * z * 0.5 + y * y * z * z / 3.0 );
	p_out[1] = y * sqrt( 1.0 - z * z * 0.5 - x * x * 0.5 + x * x * z * z / 3.0 );
	p_out[2] = z * sqrt( 1.0 - x * x * 0.5 - y * y * 0.5 + x * x * y * y / 3.0 );
}

void Patch::build( void )
{
	dsreal xcurr, ycurr;

	dsreal interval = m_sidelength / ( m_resolution - 1 );
	for( long i = 0; i < m_resolution; i++ )
	{
		for( long j = 0; j < m_resolution; j++ )
		{
			xcurr = j * interval - m_sidelength / 2.0;
			xcurr += m_xpos;

			ycurr = i * interval - m_sidelength / 2.0;
			ycurr += m_ypos;
			
			Vector coords, coords2;
			Vertex vertex;

			switch( m_orientation )
			{
				case TopPlanetFace:

					coords[0] = xcurr;
					coords[1] = 1.0;
					coords[2] = -ycurr;
					break;

				case BottomPlanetFace:

					coords[0] = xcurr;
					coords[1] = -1.0;
					coords[2] = ycurr;
					break;

				case FrontPlanetFace:

					coords[0] = xcurr;
					coords[1] = ycurr;
					coords[2] = 1.0;
					break;

				case RearPlanetFace:

					coords[0] = -xcurr;
					coords[1] = ycurr;
					coords[2] = -1.0;
					break;
					
				case LeftPlanetFace:

					coords[0] = -1.0;
					coords[1] = ycurr;
					coords[2] = xcurr;
					break;

				case RightPlanetFace:

					coords[0] = 1.0;
					coords[1] = ycurr;
					coords[2] = -xcurr;
					break;
			}
			
			cubetosphere( coords, coords2 );

			coords2.Scale( m_ray );


			vertex.x = coords2[0];
			vertex.y = coords2[1];
			vertex.z = coords2[2];
			AddVertex( vertex );
		}
	}

	long current_index = 0;

	for( long i = 0; i < m_resolution - 1; i++  )
	{
		current_index = i * m_resolution;

		for( long j = 0; j < m_resolution - 1; j++ )
		{
			Triangle triangle;

			triangle.vertex1 = current_index;
			triangle.vertex2 = current_index + 1;
			triangle.vertex3 = current_index + m_resolution;
			AddTriangle( triangle );

			
			triangle.vertex1 = current_index + 1;
			triangle.vertex2 = current_index + 1 + m_resolution;
			triangle.vertex3 = current_index + m_resolution;
			AddTriangle( triangle );
			

			current_index++;
		}        
	}
}

void Patch::GetName( dsstring& p_name )
{
	p_name = m_name;
}

dsreal Patch::GetSideLength( void )
{
	return m_sidelength;
}

void Patch::GetPos( dsreal& p_xpos, dsreal& p_ypos )
{
	p_xpos = m_xpos;
	p_ypos = m_ypos;
}