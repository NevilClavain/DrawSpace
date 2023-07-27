/* -*-LIC_BEGIN-*- */
/*
*
* DrawSpace Rendering engine
* Emmanuel Chaumont Copyright (c) 2013-2023
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

#pragma once

#include <vector>
#include <map>

#include "ds_types.h"
#include "primitives.h"
#include "vector.h"
#include "matrix.h"

namespace DrawSpace
{
    using namespace Commons;

    namespace Core
    {

        class Meshe
        {
        public:

            enum class NormalesGenerationMode
            {
                NORMALES_DISCARDED,
                NORMALES_AUTO,
                NORMALES_AUTO_SMOOTH,
                NORMALES_FROMLOADER,
                NORMALES_FROMLOADER_SMOOTH,
                NORMALES_COMPUTED
            };

            enum class TangentBinormalesGenerationMode
            {
                TB_DISCARDED,
                TB_AUTO,
                TB_FROMLOADER,
                TB_COMPUTED
            };

            Meshe( void );
            ~Meshe( void );

            void ComputeNormales( void );
            void ComputeTBs(void);

            void AddVertex( const Vertex& p_vertex );    
            void AddTriangle(const TrianglePrimitive<unsigned int>& p_triangle, bool p_fastmode = false);

            void ClearTriangles( void );
            void ClearVertices( void );

            bool UpdateIndexes( void );
            bool UpdateVertices( void );

            void SetVertex(long p_index, const Vertex& p_vertex);
            void SetRenderData( void* p_renderdata );
            void SetPath( const dsstring& p_path );
            void SetNGenerationMode(NormalesGenerationMode p_mode);    
            void SetTBGenerationMode(TangentBinormalesGenerationMode p_mode);
            void SetNormalesTransf(const Utils::Matrix& p_transf);
            void SetVertices(const std::vector<Vertex>& p_vertices);
            void SetTriangles(const std::vector<TrianglePrimitive<unsigned int>>& p_triangles);


            size_t GetVertexListSize(void) const;
            size_t GetTrianglesListSize(void) const;
            void GetVertex(long p_index, Vertex& p_vertex) const;
            void GetTriangles(long p_index, TrianglePrimitive<unsigned int>& p_triangle) const;
            std::vector<Vertex> GetVertices(void) const;
            std::vector<TrianglePrimitive<unsigned int>> GetTriangles(void) const;
            Utils::Matrix GetNormalesTransf( void ) const;
            void* GetRenderData(void) const;
            void GetMD5(dsstring& p_md5) const;
            void GetPath(dsstring& p_path) const;
            void GetCenter(DrawSpace::Utils::Vector& p_vector) const;
            void GetAABB(Utils::Vector& p_min, Utils::Vector& p_max) const;
            NormalesGenerationMode GetNGenerationMode(void) const;
            TangentBinormalesGenerationMode GetTBGenerationMode(void) const;

        private:
            std::vector<Vertex>                                                     m_vertices;
            std::vector<TrianglePrimitive<unsigned int>>                            m_triangles;
            // list of triangles for each vertex
            std::map<long, std::vector<TrianglePrimitive<unsigned int>>>            m_triangles_for_vertex;

            void*                                                                   m_render_data{ nullptr };

            dsstring                                                                m_path;

            NormalesGenerationMode                                                  m_n_gen_mode{ NormalesGenerationMode::NORMALES_COMPUTED };
            TangentBinormalesGenerationMode                                         m_tb_gen_mode{ TangentBinormalesGenerationMode::TB_DISCARDED };

            Utils::Matrix                                                           m_normales_transf;

            void compute_TBN(const Vertex& p_v1, const Vertex& p_v2, const Vertex& p_v3, int p_stage,
                Utils::Vector& p_T, Utils::Vector& p_B, Utils::Vector& p_N);
        };
    }
}

