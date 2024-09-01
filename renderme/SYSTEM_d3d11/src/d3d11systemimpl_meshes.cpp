
/* -*-LIC_BEGIN-*- */
/*
*
* renderMe grafx framework
* Emmanuel Chaumont Copyright (c) 2013-2023
*
* This file is part of renderMe.
*
*    renderMe is free software: you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
*
*    renderMe is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with renderMe.  If not, see <http://www.gnu.org/licenses/>.
*
*/
/* -*-LIC_END-*- */

#include "d3d11systemimpl.h"

bool D3D11SystemImpl::createLineMeshe(const renderMe::LineMeshe& p_lm)
{
    DECLARE_D3D11ASSERT_VARS

    const auto md5{ p_lm.getMd5() };
    if ("" == md5)
    {
        _EXCEPTION("no md5 hash for line meshe")
    }

    _RENDERME_DEBUG(m_localLogger, "Line meshe loading : " + md5);

    if (m_lines.count(md5))
    {
        _RENDERME_DEBUG(m_localLogger, "Line meshe already loaded : " + md5);
    }
    else
    {
        const auto nb_vertices{ p_lm.getVerticesListSize() };
        const auto nb_lines{ p_lm.getLinesListSize() };

        D3D11_SUBRESOURCE_DATA id = { 0 };

        ID3D11Buffer* vertex_buffer{ nullptr };
        ID3D11Buffer* index_buffer{ nullptr };

        {
            // vertex buffer creation
            const auto v{ new d3d11vertex[nb_vertices] };

            const auto vertices{ p_lm.getVertices() };

            for (size_t i = 0; i < nb_vertices; i++)
            {
                const auto vertex{ vertices[i] };

                v[i].pos.x = (float)vertex.x;
                v[i].pos.y = (float)vertex.y;
                v[i].pos.z = (float)vertex.z;

                for (size_t j = 0; j < nbTextureStages; j++)
                {
                    v[i].t[j].x = vertex.tu[j];
                    v[i].t[j].y = vertex.tv[j];
                    v[i].t[j].z = vertex.tw[j];
                    v[i].t[j].w = vertex.ta[j];
                }
            }
           
            id.pSysMem = v;
            id.SysMemPitch = 0;//sizeof( d3d11vertex );
            id.SysMemSlicePitch = 0;

            D3D11_BUFFER_DESC vertexBufferDesc = { 0 };

            vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
            vertexBufferDesc.ByteWidth = nb_vertices * sizeof(d3d11vertex);
            vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
            vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
            vertexBufferDesc.MiscFlags = 0;
          
            hRes = m_lpd3ddevice->CreateBuffer(&vertexBufferDesc, &id, &vertex_buffer);
            D3D11_CHECK(CreateBuffer)

            delete[] v;
        }

        {
            // index buffer creation

            const auto t{ new d3d11line[nb_lines] };

            const auto lines { p_lm.getLines() };

            for (size_t i = 0; i < nb_lines; i++)
            {
                const auto line{ lines[i] };

                t[i].vertex1 = line.at(0);
                t[i].vertex2 = line.at(1);
            }

            id.pSysMem = t;

            id.SysMemPitch = 0;
            id.SysMemSlicePitch = 0;

            D3D11_BUFFER_DESC indexBufferDesc = { 0 };

            indexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
            indexBufferDesc.ByteWidth = nb_lines * sizeof(d3d11line);
            indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
            indexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
            indexBufferDesc.MiscFlags = 0;

            hRes = m_lpd3ddevice->CreateBuffer(&indexBufferDesc, &id, &index_buffer);
            D3D11_CHECK(CreateBuffer)

            delete[] t;
        }

        m_lines[md5] = { vertex_buffer, index_buffer, nb_vertices, nb_lines };
    }

    _RENDERME_DEBUG(m_localLogger, "Line meshe loading SUCCESS : " + md5);
	return true;
}

void D3D11SystemImpl::setLineMeshe(const std::string& p_md5)
{
    if (!m_lines.count(p_md5))
    {
        _EXCEPTION("unknown line meshes :" + p_md5)
    }

    if (m_currentMeshe != p_md5)
    {
        const auto lmData{ m_lines.at(p_md5) };

        const UINT stride{ sizeof(d3d11vertex) };
        const UINT offset = 0;

        m_lpd3ddevcontext->IASetVertexBuffers(0, 1, &lmData.vertex_buffer, &stride, &offset);
        m_lpd3ddevcontext->IASetIndexBuffer(lmData.index_buffer, DXGI_FORMAT_R32_UINT, 0);

        m_next_nbvertices = lmData.nb_vertices;
        m_next_nblines = lmData.nb_primitives;

        m_currentMeshe = p_md5;
    }
}

void D3D11SystemImpl::destroyLineMeshe(const std::string& p_md5)
{
    if (!m_lines.count(p_md5))
    {
        _EXCEPTION("unknown line meshe :" + p_md5)
    }
    const auto lmData{ m_lines.at(p_md5) };

    lmData.vertex_buffer->Release();
    lmData.index_buffer->Release();

    m_lines.erase(p_md5);
    _RENDERME_DEBUG(m_localLogger, "Line meshe release SUCCESS : " + p_md5);
}

bool D3D11SystemImpl::createTriangleMeshe(const renderMe::TriangleMeshe& p_tm)
{
    DECLARE_D3D11ASSERT_VARS

    const auto md5{ p_tm.getMd5()};
    if ("" == md5)
    {
        _EXCEPTION("no md5 hash for triangle meshe")
    }

    _RENDERME_DEBUG(m_localLogger, "Triangle meshe loading : " + md5);

    if (m_triangles.count(md5))
    {
        _RENDERME_DEBUG(m_localLogger, "Triangle meshe already loaded : " + md5);
    }
    else
    {
        const auto nb_vertices{ p_tm.getVerticesListSize() };
        const auto nb_triangles{ p_tm.getTrianglesListSize() };

        D3D11_SUBRESOURCE_DATA id = { 0 };

        ID3D11Buffer* vertex_buffer{ nullptr };
        ID3D11Buffer* index_buffer{ nullptr };

        {
            // vertex buffer creation
            const auto v{ new d3d11vertex[nb_vertices] };

            const auto vertices{ p_tm.getVertices() };

            for (size_t i = 0; i < nb_vertices; i++)
            {
                const auto vertex{ vertices[i] };

                v[i].pos.x = (float)vertex.x;
                v[i].pos.y = (float)vertex.y;
                v[i].pos.z = (float)vertex.z;

                for (size_t j = 0; j < nbTextureStages; j++)
                {
                    v[i].t[j].x = vertex.tu[j];
                    v[i].t[j].y = vertex.tv[j];
                    v[i].t[j].z = vertex.tw[j];
                    v[i].t[j].w = vertex.ta[j];
                }
            }

            id.pSysMem = v;
            id.SysMemPitch = 0;//sizeof( d3d11vertex );
            id.SysMemSlicePitch = 0;

            D3D11_BUFFER_DESC vertexBufferDesc = { 0 };

            vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
            vertexBufferDesc.ByteWidth = nb_vertices * sizeof(d3d11vertex);
            vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
            vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
            vertexBufferDesc.MiscFlags = 0;

            hRes = m_lpd3ddevice->CreateBuffer(&vertexBufferDesc, &id, &vertex_buffer);
            D3D11_CHECK(CreateBuffer)

            delete[] v;
        }

        {
            // index buffer creation

            const auto t{ new d3d11triangle[nb_triangles] };

            const auto triangles{ p_tm.getTriangles() };

            for (size_t i = 0; i < nb_triangles; i++)
            {
                const auto triangle{ triangles[i] };

                t[i].vertex1 = triangle.at(0);
                t[i].vertex2 = triangle.at(1);
                t[i].vertex3 = triangle.at(2);
            }

            id.pSysMem = t;

            id.SysMemPitch = 0;
            id.SysMemSlicePitch = 0;

            D3D11_BUFFER_DESC indexBufferDesc = { 0 };

            indexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
            indexBufferDesc.ByteWidth = nb_triangles * sizeof(d3d11triangle);
            indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
            indexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
            indexBufferDesc.MiscFlags = 0;

            hRes = m_lpd3ddevice->CreateBuffer(&indexBufferDesc, &id, &index_buffer);
            D3D11_CHECK(CreateBuffer)

            delete[] t;
        }

        m_triangles[md5] = { vertex_buffer, index_buffer, nb_vertices, nb_triangles };
    }

    _RENDERME_DEBUG(m_localLogger, "Triangle meshe loading SUCCESS : " + md5);
    return true;

}

void D3D11SystemImpl::setTriangleMeshe(const std::string& p_md5)
{
    if (!m_triangles.count(p_md5))
    {
        _EXCEPTION("unknown triangle meshes :" + p_md5)
    }

    if (m_currentMeshe != p_md5)
    {
        const auto tmData{ m_triangles.at(p_md5) };

        const UINT stride{ sizeof(d3d11vertex) };
        const UINT offset = 0;

        m_lpd3ddevcontext->IASetVertexBuffers(0, 1, &tmData.vertex_buffer, &stride, &offset);
        m_lpd3ddevcontext->IASetIndexBuffer(tmData.index_buffer, DXGI_FORMAT_R32_UINT, 0);

        m_next_nbvertices = tmData.nb_vertices;
        m_next_nbtriangles = tmData.nb_primitives;

        m_currentMeshe = p_md5;
    }
}

void D3D11SystemImpl::destroyTriangleMeshe(const std::string& p_md5)
{
    if (!m_triangles.count(p_md5))
    {
        _EXCEPTION("unknown triangle meshe :" + p_md5)
    }
    const auto tmData{ m_triangles.at(p_md5) };

    tmData.vertex_buffer->Release();
    tmData.index_buffer->Release();

    m_triangles.erase(p_md5);
    _RENDERME_DEBUG(m_localLogger, "Triangle meshe release SUCCESS : " + p_md5);
}


void D3D11SystemImpl::forceCurrentMeshe()
{
    if (m_triangles.count(m_currentMeshe))
    {
        const auto tmData{ m_triangles.at(m_currentMeshe) };

        const UINT stride{ sizeof(d3d11vertex) };
        const UINT offset = 0;

        m_lpd3ddevcontext->IASetVertexBuffers(0, 1, &tmData.vertex_buffer, &stride, &offset);
        m_lpd3ddevcontext->IASetIndexBuffer(tmData.index_buffer, DXGI_FORMAT_R32_UINT, 0);

        m_next_nbvertices = tmData.nb_vertices;
        m_next_nbtriangles = tmData.nb_primitives;
    }
    else if (m_lines.count(m_currentMeshe))
    {
        const auto lmData{ m_lines.at(m_currentMeshe) };

        const UINT stride{ sizeof(d3d11vertex) };
        const UINT offset = 0;

        m_lpd3ddevcontext->IASetVertexBuffers(0, 1, &lmData.vertex_buffer, &stride, &offset);
        m_lpd3ddevcontext->IASetIndexBuffer(lmData.index_buffer, DXGI_FORMAT_R32_UINT, 0);

        m_next_nbvertices = lmData.nb_vertices;
        m_next_nblines = lmData.nb_primitives;
    }
}