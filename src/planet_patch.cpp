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

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Planet;

Patch::Patch( int p_resolution, dsreal p_sidelength, Orientation p_orientation, const dsstring& p_name ) : 
m_resolution( p_resolution ), 
m_orientation( p_orientation ),
m_sidelength( p_sidelength ),
m_name( p_name )
{
    for( long i = 0; i < 8; i++ )
    {
        m_neighbours[i] = NULL;
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

void Patch::build( void )
{
    dsreal xcurr, ycurr;

    dsreal interval = m_sidelength / ( m_resolution - 1 );
    for( long i = 0; i < m_resolution; i++ )
    {
        for( long j = 0; j < m_resolution; j++ )
        {
            xcurr = j * interval;
			ycurr = i * interval;
            
            Vertex vertex;

            switch( m_orientation )
            {
                case Up:

                    vertex.x = xcurr;
                    vertex.y = m_sidelength / 2.0;
                    vertex.z = -ycurr;
                    break;

                case Down:

                    vertex.x = xcurr;
                    vertex.y = -m_sidelength / 2.0;
                    vertex.z = ycurr;
                    break;

                case Front:

                    vertex.x = xcurr;
                    vertex.y = ycurr;
                    vertex.z = m_sidelength / 2.0;
                    break;

                case Rear:

                    vertex.x = -xcurr;
                    vertex.y = ycurr;
                    vertex.z = -m_sidelength / 2.0;
                    break;
                    

                case Left:

                    vertex.x = -m_sidelength / 2.0;
                    vertex.y = ycurr;
                    vertex.z = -xcurr;
                    break;

                case Right:

                    vertex.x = -m_sidelength / 2.0;
                    vertex.y = ycurr;
                    vertex.z = -xcurr;
                    break;

            }

			AddVertex( vertex );
        }
    }

	long current_index = 0;
	for( long i = 0; i < m_resolution - 1; i++  )
	{
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
