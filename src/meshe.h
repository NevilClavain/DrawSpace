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

#ifndef _MESHE_H_
#define _MESHE_H_

#include "drawspace_commons.h"
#include "vertex.h"
#include "triangle.h"
#include "asset.h"
#include "vector.h"

namespace DrawSpace
{
namespace Interface
{
class MesheImport;
}
namespace Core
{

class Meshe : public Asset
{
protected:
    std::vector<Vertex>                 m_vertices;
    std::vector<Triangle>               m_triangles;

    DrawSpace::Interface::MesheImport*  m_importer;

public:

    Meshe( void );
    ~Meshe( void );

    void SetImporter( DrawSpace::Interface::MesheImport* p_importer );
    bool LoadFromFile( const dsstring& p_filepath, long p_index );

    long GetVertexListSize( void );
    long GetTrianglesListSize( void );

    void GetVertex( long p_index, Vertex& p_vertex );
    void GetTriangles( long p_index, Triangle& p_triangle );

    void AddVertex( const Vertex& p_vertex );
    void SetVertex( long p_index, const Vertex& p_vertex );
    void AddTriangle( const Triangle& p_triangle );

    void GetCenter( DrawSpace::Utils::Vector& p_vector );
    void GetAABB( Utils::Vector& p_min, Utils::Vector& p_max );
    
    void Serialize( Core::Factory& p_factory, Utils::Archive& p_archive  );
    void Unserialize( Core::Factory& p_factory, Utils::Archive& p_archive );

    void GetMD5( dsstring& p_md5 );
};
}
}
#endif