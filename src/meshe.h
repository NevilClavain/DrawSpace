/* -*-LIC_BEGIN-*- */
/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2020                     
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
#include "matrix.h"


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
public:

    using NormalesGenerationMode = enum
    {
        NORMALES_DISCARDED,
        NORMALES_AUTO,
		NORMALES_AUTO_SMOOTH,
        NORMALES_FROMLOADER,
		NORMALES_FROMLOADER_SMOOTH,
        NORMALES_COMPUTED
    };

    using TangentBinormalesGenerationMode = enum
    {
        TB_DISCARDED,
        TB_AUTO,
        TB_FROMLOADER,
        TB_COMPUTED
    };

protected:
    std::vector<Vertex>                         m_vertices;
    std::vector<Triangle>                       m_triangles;

    // list of triangles for each vertex
    std::map<long, std::vector<Triangle>>       m_triangles_for_vertex;

    DrawSpace::Interface::MesheImport*          m_importer;

    void*                                       m_render_data;

    dsstring                                    m_path;

    NormalesGenerationMode                      m_n_gen_mode;
    TangentBinormalesGenerationMode             m_tb_gen_mode;
    Utils::Matrix                               m_normales_transf;

    void compute_TBN( const Vertex& p_v1, const Vertex& p_v2, const Vertex& p_v3, int p_stage,
                        Utils::Vector& p_T, Utils::Vector& p_B, Utils::Vector& p_N );

public:

    Meshe( void );
    ~Meshe( void );

    void SetImporter( DrawSpace::Interface::MesheImport* p_importer );
    bool LoadFromFile( const dsstring& p_filepath, long p_index );

    void ComputeNormales( void );
    void ComputeTBs(void);

    long GetVertexListSize( void ) const;
    long GetTrianglesListSize( void ) const;

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

    void SetNGenerationMode(NormalesGenerationMode p_mode);
    NormalesGenerationMode GetNGenerationMode( void ) const;

    void SetTBGenerationMode(TangentBinormalesGenerationMode p_mode);
    TangentBinormalesGenerationMode GetTBGenerationMode(void) const;

    void SetNormalesTransf(const Utils::Matrix& p_transf);
    Utils::Matrix GetNormalesTransf( void ) const;
};
}
}
#endif
