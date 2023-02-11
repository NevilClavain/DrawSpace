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

#include "linemeshe.h"
#include "md5.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Utils;

LineMeshe::LineMeshe(void)
{
}

LineMeshe::~LineMeshe(void)
{
}

size_t LineMeshe::GetVertexListSize(void) const
{
    return m_vertices.size();
}

size_t LineMeshe::GetLinesListSize(void) const
{
    return m_lines.size();
}

dsstring LineMeshe::GetName(void) const
{
    return m_name;
}

void LineMeshe::GetVertex(long p_index, Vertex& p_vertex) const
{
    p_vertex = m_vertices.at(p_index);
}

void LineMeshe::GetLine(long p_index, LinePrimitive<unsigned int>& p_triangle) const
{
    p_triangle = m_lines.at(p_index);
}

std::vector<Vertex> LineMeshe::GetVertices(void) const
{
    return m_vertices;
}

std::vector<LinePrimitive<unsigned int>> LineMeshe::GetLines(void) const
{
    return m_lines;
}

void LineMeshe::ClearLines(void)
{
    m_lines.clear();
}

void LineMeshe::ClearVertices(void)
{
    m_vertices.clear();
}

void LineMeshe::AddVertex(const Vertex& p_vertex)
{
    m_vertices.push_back(p_vertex);
}

void LineMeshe::SetVertex(long p_index, const Vertex& p_vertex)
{
    m_vertices[p_index] = p_vertex;
}


void LineMeshe::AddLine(const LinePrimitive<unsigned int>& p_line)
{
    m_lines.push_back(p_line);
}

void LineMeshe::SetVertices(const std::vector<Vertex>& p_vertices)
{
    m_vertices = p_vertices;
}

void LineMeshe::SetLines(const std::vector<LinePrimitive<unsigned int>>& p_lines)
{
    m_lines = p_lines;
}

void LineMeshe::SetRenderData(void* p_renderdata)
{
    m_render_data = p_renderdata;
}

void LineMeshe::SetName(const dsstring& p_name)
{
    m_name = p_name;
}

void* LineMeshe::GetRenderData(void) const
{
    return m_render_data;
}

dsstring LineMeshe::GetMD5(void) const
{
    dsstring result;

    MD5 md5;

    const auto vbuff{ new Vertex[m_vertices.size()] };
    auto curr{ vbuff };

    for (size_t i = 0; i < m_vertices.size(); i++)
    {
        *curr = m_vertices[i];
        curr++;
    }

    const dsstring hash_v{ md5.digestMemory((BYTE*)vbuff, (int)(m_vertices.size() * sizeof(Vertex))) };

    auto tbuff{ new LinePrimitive<unsigned int>[m_lines.size()] };
    LinePrimitive<unsigned int>* curr2{ tbuff };

    for (size_t i = 0; i < m_lines.size(); i++)
    {
        LinePrimitive<unsigned int> triangle{ m_lines.at(i) };
        *curr2 = triangle;
        curr2++;
    }
    const dsstring hash_t{ md5.digestMemory((BYTE*)tbuff, (int)(m_lines.size() * sizeof(LinePrimitive<unsigned int>))) };

    result = hash_v + hash_t;

    delete[] vbuff;
    delete[] tbuff;

    return result;
}