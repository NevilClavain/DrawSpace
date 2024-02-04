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
#include "matrixchain.h"


using namespace renderMe::core::maths;
using namespace renderMe::transform;

void D3D11SystemImpl::drawLineMeshe(const renderMe::core::maths::Matrix& p_world, const renderMe::core::maths::Matrix& p_view, const renderMe::core::maths::Matrix& p_proj)
{   
    // setting transformation    
    Matrix inv;    
    inv.identity();
    inv(2, 2) = -1.0;
    const auto final_view{ p_view * inv };
    
    MatrixChain chain;
    chain.pushMatrix(p_proj);
    chain.pushMatrix(final_view);
    chain.pushMatrix(p_world);
    chain.buildResult();
    auto result{ chain.getResultTransform() };
    result.transpose();

    setVertexshaderConstantsMat(0, result);
    setPixelshaderConstantsMat(100, result);

    //////////////////////////////////////////////////////////////////////
    
    Matrix worldview{ p_world * p_view };
    worldview.transpose();

    setVertexshaderConstantsMat(4, worldview);
    setPixelshaderConstantsMat(104, worldview);

    //////////////////////////////////////////////////////////////////////

    auto world{ p_world };
    world.transpose();

    auto view{ p_view };
    view.transpose();

    setVertexshaderConstantsMat(8, world);
    setVertexshaderConstantsMat(12, view);

    setPixelshaderConstantsMat(108, world);
    setPixelshaderConstantsMat(112, view);

    //////////////////////////////////////////////////////////////////////

    auto cam{ view };
    cam.inverse();
    cam.transpose();

    setVertexshaderConstantsMat(16, cam);
    setPixelshaderConstantsMat(116, cam);

    auto proj { p_proj };

    proj.transpose();
    setVertexshaderConstantsMat(20, proj);
    setPixelshaderConstantsMat(120, proj);

    // update des shaders legacy constants buffers...

    m_lpd3ddevcontext->UpdateSubresource(m_vertexShaderArgsBuffer, 0, nullptr, &m_vertexshader_args, 0, 0);
    m_lpd3ddevcontext->UpdateSubresource(m_pixelShaderArgsBuffer, 0, nullptr, &m_pixelshader_args, 0, 0);

    ///////////////

    m_lpd3ddevcontext->VSSetConstantBuffers(0, 1, &m_vertexShaderArgsBuffer);
    m_lpd3ddevcontext->PSSetConstantBuffers(0, 1, &m_pixelShaderArgsBuffer);
    m_lpd3ddevcontext->DrawIndexed(m_next_nblines * 2, 0, 0);
}