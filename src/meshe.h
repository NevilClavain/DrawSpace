/* -*-LIC_BEGIN-*- */
/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2018                        
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
/* -*-LIC_END-*- */


#ifndef _MESHE_H_
#define _MESHE_H_

#include "drawspace_commons.h"
#include "vertex.h"
#include "triangle.h"
#include "vector.h"


namespace DrawSpace
{
namespace Interface
{
class MesheImport;
}
namespace Core
{

class Meshe
{
protected:
    std::vector<Vertex>                         m_vertices;
    std::vector<Triangle>                       m_triangles;

    // list of triangles for each vertex
    std::map<long, std::vector<Triangle>>       m_triangles_for_vertex;

    DrawSpace::Interface::MesheImport*          m_importer;

    void*                                       m_render_data;

    dsstring                                    m_path;

public:

    Meshe( void );
    ~Meshe( void );

    void SetImporter( DrawSpace::Interface::MesheImport* p_importer );
    bool LoadFromFile( const dsstring& p_filepath, long p_index );

    void ComputeNormales( void );

    long GetVertexListSize( void );
    long GetTrianglesListSize( void );

    void GetVertex( long p_index, Vertex& p_vertex );
    void GetTriangles( long p_index, Triangle& p_triangle );

    void AddVertex( const Vertex& p_vertex );
    void SetVertex( long p_index, const Vertex& p_vertex );
    void AddTriangle( const Triangle& p_triangle, bool p_fastmode = false );

    void ClearTriangles( void );
    void ClearVertices( void );

    bool UpdateIndexes( void );
    bool UpdateVertices( void );

    void GetCenter( DrawSpace::Utils::Vector& p_vector );
    void GetAABB( Utils::Vector& p_min, Utils::Vector& p_max );

  
    void GetMD5( dsstring& p_md5 );


    void SetRenderData( void* p_renderdata );

    void* GetRenderData( void );

    void GetPath( dsstring& p_path );
    void SetPath( const dsstring& p_path );

};
}
}
#endif
