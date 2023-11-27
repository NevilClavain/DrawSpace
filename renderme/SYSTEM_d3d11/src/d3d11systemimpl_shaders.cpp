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
#include <d3dcompiler.h>

D3D10Include::D3D10Include(const std::string& p_basepath, renderMe::core::logger::Sink& p_logger) :
m_basepath(p_basepath),
m_logger(p_logger)
{
}

HRESULT __stdcall D3D10Include::Open(D3D_INCLUDE_TYPE IncludeType, LPCSTR pFileName, LPCVOID pParentData, LPCVOID* ppData, UINT* pBytes)
{
    const std::string final_path{ m_basepath + "/" + pFileName};
    if (!m_fc)
    {
        m_fc = new renderMe::core::FileContent<const char>(final_path);
        m_fc->load();
        *pBytes = m_fc->getDataSize();
        *ppData = m_fc->getData();
    }
    else
    {
        _EXCEPTION("Previous file not properly closed");
    }
    return S_OK;
}

HRESULT __stdcall D3D10Include::Close(LPCVOID pData)
{
    if (m_fc)
    {
        delete m_fc;
        m_fc = nullptr;
    }
    return S_OK;
}



bool D3D11SystemImpl::createShaderBytesOnFile(int p_shadertype,
    const std::string& p_includes_path,
    const renderMe::core::FileContent<const char>& srcFile,
    std::unique_ptr<char[]>& p_shaderBytes,
    size_t& p_shaderBytesLength)
{
    DECLARE_D3D11ASSERT_VARS
    bool status{ false };

    ID3DBlob*       pBlob         { nullptr };
    ID3DBlob*       pErrBlob      { nullptr };

    D3D10Include    include_mgmt(p_includes_path, m_localLogger);

    _RENDERME_DEBUG(m_localLogger, std::string("Src Shader path : ") + srcFile.getPath());
    _RENDERME_DEBUG(m_localLogger, std::string("With shader type : ") + std::to_string(p_shadertype));
    _RENDERME_DEBUG(m_localLogger, std::string("include path : ") + p_includes_path);
    
    hRes = compileShaderFromMem((void*)srcFile.getData(), srcFile.getDataSize(), 
                                    srcFile.getPath().c_str(), 
                                    (p_shadertype == 0 ? "vs_main" : "ps_main"), 
                                    (p_shadertype == 0 ? "vs_4_0" : "ps_4_0"), 
                                    &include_mgmt,
                                    &pBlob, &pErrBlob);

    if (S_OK != hRes)
    {
        if (pErrBlob != nullptr)
        {
            const auto bytesLen{ pErrBlob->GetBufferSize() };

            p_shaderBytes = std::make_unique<char[]>(bytesLen);
            memcpy(p_shaderBytes.get(), pErrBlob->GetBufferPointer(), bytesLen);
            p_shaderBytesLength = bytesLen;

            pErrBlob->Release();

            _RENDERME_DEBUG(m_localLogger, "Shader compilation FAILED !");
        }
    }
    else
    {
        const auto bytesLen{ pBlob->GetBufferSize() };

        p_shaderBytes = std::make_unique<char[]>(bytesLen);
        memcpy(p_shaderBytes.get(), pBlob->GetBufferPointer(), bytesLen);
        p_shaderBytesLength = bytesLen;

        pBlob->Release();

        status = true;
        _RENDERME_DEBUG(m_localLogger, "Shader compilation SUCCESS !");
    }
    return status;
}


HRESULT D3D11SystemImpl::compileShaderFromMem(void* p_data, int p_size, LPCTSTR szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3D10Include* p_include, ID3DBlob** ppBlobOut, ID3DBlob** ppBlobErrOut)
{    
    DWORD dwShaderFlags{ D3DCOMPILE_ENABLE_STRICTNESS };

    // Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
    // Setting this flag improves the shader debugging experience, but still allows 
    // the shaders to be optimized and to run exactly the way they will run in 
    // the release configuration of this program.

#ifdef _DEBUG
    dwShaderFlags |= D3DCOMPILE_DEBUG;
    //dwShaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;  // decommmenter ce flag si besoin de debugger les shaders avec Graphics diagnostics tool
#endif

    ID3DBlob* pErrorBlob;
    const auto hr{ D3DX11CompileFromMemory((LPCTSTR)p_data, p_size, szFileName, NULL, p_include, szEntryPoint, szShaderModel, dwShaderFlags, 0, nullptr, ppBlobOut, &pErrorBlob, nullptr) };
    if (FAILED(hr))
    {
        if (pErrorBlob != nullptr)
        {
            *ppBlobErrOut = pErrorBlob;
        }
    }
    return hr;
}