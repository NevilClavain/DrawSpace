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
#include "drawspace_commons.h"
#include "primitives.h"

namespace DrawSpace
{
namespace Core
{

class LineMeshe
{
public:

    LineMeshe(void);
    ~LineMeshe(void);

    size_t GetVertexListSize(void) const;
    size_t GetLinesListSize(void) const;

    void GetVertex(long p_index, Vertex& p_vertex) const;
    void GetLine(long p_index, LinePrimitive<unsigned int>& p_triangle) const;
    dsstring GetName(void) const;
    std::vector<Vertex> GetVertices(void) const;
    std::vector<LinePrimitive<unsigned int>> GetLines(void) const;
    void* LineMeshe::GetRenderData(void) const;
    dsstring GetMD5(void) const;

    void ClearLines(void);
    void ClearVertices(void);

    void AddVertex(const Vertex& p_vertex);
    void AddLine(const LinePrimitive<unsigned int>& p_line);
    void SetVertex(long p_index, const Vertex& p_vertex);

    void SetName(const dsstring& p_name);

    void SetRenderData(void* p_renderdata);

    void SetVertices(const std::vector<Vertex>& p_vertices);

    void SetLines(const std::vector<LinePrimitive<unsigned int>>& p_lines);

private:

    std::vector<Vertex>                                                     m_vertices;
    std::vector<LinePrimitive<unsigned int>>                                m_lines;
    void*                                                                   m_render_data{ nullptr };
    dsstring                                                                m_name;

};

}
}