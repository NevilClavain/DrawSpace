/***************************************************************************
*                                                                          *
* DrawSpace Rendering engine                                               *
* Emmanuel Chaumont Copyright (c) 2013-2014                                *
*                                                                          *
* This file is part of DrawSpace.                                          *
*                                                                          *
*    DrawSpace is free software: you can redistribute it and/or modify     *
*    it under the terms of the GNU General Public License as published by  *
*    the Free Software Foundation, either version 3 of the License, or     *
*    (at your option) any later version.                                   *
*                                                                          *
*    DrawSpace is distributed in the hope that it will be useful,          *
*    but WITHOUT ANY WARRANTY; without even the implied warranty of        *
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
*    GNU General Public License for more details.                          *
*                                                                          *
*    You should have received a copy of the GNU General Public License     *
*    along with DrawSpace.  If not, see <http://www.gnu.org/licenses/>.    *
*                                                                          *
***************************************************************************/

#include "meshe.h"
#include "mesheimport.h"

using namespace DrawSpace::Core;
using namespace DrawSpace::Utils;

Meshe::Meshe( void ) : m_importer( NULL )
{

}

Meshe::~Meshe( void )
{

}

void Meshe::SetImporter( DrawSpace::Interface::MesheImport* p_importer )
{
	m_importer = p_importer;
}

bool Meshe::LoadFromFile( const dsstring& p_filepath, long p_index )
{
	if( NULL == m_importer )
	{
		return false;
	}
	return m_importer->LoadFromFile( p_filepath, p_index, this );
}

long Meshe::GetVertexListSize( void )
{
	return (long)m_vertices.size();
}

long Meshe::GetTrianglesListSize( void )
{
	return (long)m_triangles.size();
}

void Meshe::GetVertex( long p_index, Vertex& p_vertex )
{
	p_vertex = m_vertices[p_index];
}

void Meshe::GetTriangles( long p_index, Triangle& p_triangle )
{
	p_triangle = m_triangles[p_index];
}

void Meshe::AddVertex( const Vertex& p_vertex )
{
	m_vertices.push_back( p_vertex );
}

void Meshe::SetVertex( long p_index, const Vertex& p_vertex )
{
	m_vertices[p_index] = p_vertex;
}

void Meshe::AddTriangle( const Triangle& p_triangle )
{
	m_triangles.push_back( p_triangle );
}

void Meshe::VertexAverage( DrawSpace::Utils::Vector& p_vector )
{
	dsreal xsum = 0.0;
	dsreal ysum = 0.0;
	dsreal zsum = 0.0;

	for( size_t i = 0; i < m_vertices.size(); i++ )
	{
		xsum += m_vertices[i].x;
		ysum += m_vertices[i].y;
		zsum += m_vertices[i].z;
	}

	p_vector[0] = xsum / m_vertices.size();
}

void Meshe::Serialize( Factory& p_factory, Archive& p_archive  )
{

}

void Meshe::Unserialize( Factory& p_factory, Archive& p_archive )
{


}

