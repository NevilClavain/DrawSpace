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
#include "WICTextureLoader.h"
#include "exception"

bool D3D11SystemImpl::createTexture(renderMe::Texture& p_texture)
{
	DECLARE_D3D11ASSERT_VARS

	const auto resource_uid{ p_texture.getResourceUID()};

	_RENDERME_DEBUG(m_localLogger, "Texture loading : " + resource_uid);

    if (m_textures.count(resource_uid))
    {
        _RENDERME_DEBUG(m_localLogger, "Texture already loaded : " + resource_uid);
    }
    else
    {
        if (renderMe::Texture::Source::CONTENT_FROM_RENDERINGQUEUE == p_texture.getSource())
        {
            // render target

            DXGI_FORMAT format;

            int bpp{ 0 };
            TextureData texture_data;

            switch (p_texture.getFormat())
            {
                case renderMe::Texture::Format::TEXTURE_RGB:

                    format = DXGI_FORMAT_B8G8R8A8_UNORM;
                    bpp = 4;
                    break;

                case renderMe::Texture::Format::TEXTURE_FLOAT:

                    format = DXGI_FORMAT_R16_FLOAT;
                    bpp = 2;
                    break;

                case renderMe::Texture::Format::TEXTURE_FLOAT32:

                    format = DXGI_FORMAT_R32_FLOAT;
                    bpp = 4;
                    break;

                case renderMe::Texture::Format::TEXTURE_FLOATVECTOR:

                    format = DXGI_FORMAT_R16G16B16A16_FLOAT;
                    bpp = 8;
                    break;

                case renderMe::Texture::Format::TEXTURE_FLOATVECTOR32:

                    format = DXGI_FORMAT_R32G32B32A32_FLOAT;
                    bpp = 16;
                    break;
            }

            unsigned int cpuAccessFlags{ 0 };
            D3D11_USAGE usage{ D3D11_USAGE_DEFAULT };

            D3D11_TEXTURE2D_DESC textureDesc;

            textureDesc.Width = p_texture.getWidth();
            textureDesc.Height = p_texture.getHeight();
            textureDesc.MipLevels = 1;
            textureDesc.ArraySize = 1;
            textureDesc.Format = format;
            textureDesc.SampleDesc.Count = 1;
            textureDesc.SampleDesc.Quality = 0;

            textureDesc.Usage = usage;
            textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
            textureDesc.CPUAccessFlags = cpuAccessFlags;
            textureDesc.MiscFlags = 0;

            ID3D11Texture2D* d3dt11{ nullptr };
            ID3D11Texture2D* d3dt11_clone{ nullptr };

            // Create the render target texture.
            hRes = m_lpd3ddevice->CreateTexture2D(&textureDesc, nullptr, &d3dt11);
            D3D11_CHECK(CreateTexture2D)

            // creation du render target view
            D3D11_RENDER_TARGET_VIEW_DESC       renderTargetViewDesc;
            D3D11_SHADER_RESOURCE_VIEW_DESC     shaderResourceViewDesc;

            ID3D11RenderTargetView*             rendertextureTargetView{ nullptr };
            ID3D11ShaderResourceView*           rendertextureResourceView{ nullptr };

            ZeroMemory(&renderTargetViewDesc, sizeof(renderTargetViewDesc));

            renderTargetViewDesc.Format = format;
            renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
            renderTargetViewDesc.Texture2D.MipSlice = 0;

            hRes = m_lpd3ddevice->CreateRenderTargetView(d3dt11, &renderTargetViewDesc, &rendertextureTargetView);
            D3D11_CHECK(CreateRenderTargetView)

            // creation du shader resource view associé
            ZeroMemory(&shaderResourceViewDesc, sizeof(shaderResourceViewDesc));
            shaderResourceViewDesc.Format = format;
            shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
            shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
            shaderResourceViewDesc.Texture2D.MipLevels = 1;

            hRes = m_lpd3ddevice->CreateShaderResourceView(d3dt11, &shaderResourceViewDesc, &rendertextureResourceView);
            D3D11_CHECK(CreateShaderResourceView)


            /////////////////////// creation texture "clone", pour lire le contenu d'une render target

            if (renderMe::Texture::ContentAccessMode::CONTENT_ACCESS == p_texture.getContentAccessMode())
            {
                
                textureDesc.Usage = D3D11_USAGE_STAGING;
                textureDesc.BindFlags = 0;
                textureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ /* | D3D11_CPU_ACCESS_WRITE */;

                hRes = m_lpd3ddevice->CreateTexture2D(&textureDesc, nullptr, &d3dt11_clone);
                D3D11_CHECK(CreateTexture2D) 

                const size_t blocksize{ bpp * textureDesc.Width * textureDesc.Height };

                void* bits { new unsigned char[blocksize] };

                texture_data.targetTextureClone = d3dt11_clone;
                texture_data.bits = bits;
                texture_data.blocksize = blocksize;
            }

            ////////////////////////////////////////////////////////////////////////


            ID3D11Texture2D*                    stencilDepthBuffer{ nullptr };;
            ID3D11DepthStencilView*             stencilDepthView{ nullptr };

            createDepthStencilBuffer(m_lpd3ddevice, p_texture.getWidth(), p_texture.getHeight(), DXGI_FORMAT_D24_UNORM_S8_UINT, &stencilDepthBuffer, &stencilDepthView);

            // store texture data

            
            texture_data.source = renderMe::Texture::Source::CONTENT_FROM_RENDERINGQUEUE;


            texture_data.shaderResourceView = rendertextureResourceView;
            texture_data.desc = textureDesc;

            texture_data.targetTexture = d3dt11;

            texture_data.rendertextureTargetView = rendertextureTargetView;
            texture_data.stencilDepthBuffer = stencilDepthBuffer;
            texture_data.stencilDepthView = stencilDepthView;

            texture_data.viewport.Width = p_texture.getWidth();
            texture_data.viewport.Height = p_texture.getHeight();

            texture_data.viewport.MinDepth = 0.0;
            texture_data.viewport.MaxDepth = 1.0;
            texture_data.viewport.TopLeftX = 0.0;
            texture_data.viewport.TopLeftY = 0.0;

            texture_data.targetTextureClone = d3dt11_clone;

            m_textures[resource_uid] = texture_data;
        }
        else
        {
            // from file content

            ID3D11Resource*             d3dt11{ nullptr };
            ID3D11ShaderResourceView*   textureResourceView{ nullptr };

            const auto& file_content{ p_texture.getFileContent()};

            D3D11_USAGE usage{ D3D11_USAGE_DEFAULT };

            unsigned int bindFlags{ D3D11_BIND_SHADER_RESOURCE };
            unsigned int cpuAccessFlags{ 0 };

            hRes = DirectX::CreateWICTextureFromMemoryEx(m_lpd3ddevice, file_content.getData(), file_content.getDataSize(), 0, 
                                                            usage, //D3D11_USAGE
                                                            bindFlags, //bindFlags
                                                            cpuAccessFlags, //cpuAccessFlags
                                                            0, //miscFlags
                                                            DirectX::DX11::WIC_LOADER_DEFAULT, &d3dt11, &textureResourceView);
            D3D11_CHECK(CreateWICTextureFromMemory);

            D3D11_RESOURCE_DIMENSION dims;
            d3dt11->GetType(&dims);

            D3D11_TEXTURE2D_DESC desc;

            if (D3D11_RESOURCE_DIMENSION_TEXTURE2D == dims)
            {
                // retrieve texture infos

                ID3D11Texture2D* pTextureInterface{ nullptr };
                hRes = d3dt11->QueryInterface<ID3D11Texture2D>(&pTextureInterface);

                
                pTextureInterface->GetDesc(&desc);

                _RENDERME_DEBUG(m_localLogger, "Texture infos : " + std::to_string(desc.Width) + "x" + std::to_string(desc.Height) + " format : " + std::to_string(desc.Format));

                //p_texture.setDims(desc.Width, desc.Height);
                p_texture.m_width = desc.Width;
                p_texture.m_height = desc.Height;

                renderMe::Texture::Format format;

                switch(desc.Format)
                {
                    case DXGI_FORMAT_R32G32B32A32_TYPELESS:
                    case DXGI_FORMAT_R32G32B32A32_FLOAT:
                    case DXGI_FORMAT_R32G32B32A32_UINT:
                    case DXGI_FORMAT_R32G32B32A32_SINT:
                    case DXGI_FORMAT_R32G32B32_TYPELESS:
                    case DXGI_FORMAT_R32G32B32_FLOAT:
                    case DXGI_FORMAT_R32G32B32_UINT:
                    case DXGI_FORMAT_R32G32B32_SINT:
                    case DXGI_FORMAT_R16G16B16A16_TYPELESS:
                    case DXGI_FORMAT_R16G16B16A16_FLOAT:
                    case DXGI_FORMAT_R16G16B16A16_UNORM:
                    case DXGI_FORMAT_R16G16B16A16_UINT:
                    case DXGI_FORMAT_R16G16B16A16_SNORM:
                    case DXGI_FORMAT_R16G16B16A16_SINT:
                    case DXGI_FORMAT_R10G10B10A2_TYPELESS:
                    case DXGI_FORMAT_R10G10B10A2_UNORM:
                    case DXGI_FORMAT_R10G10B10A2_UINT:
                    case DXGI_FORMAT_R8G8B8A8_TYPELESS:
                    case DXGI_FORMAT_R8G8B8A8_UNORM:
                    case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
                    case DXGI_FORMAT_R8G8B8A8_UINT:
                    case DXGI_FORMAT_R8G8B8A8_SNORM:
                    case DXGI_FORMAT_R8G8B8A8_SINT:

                        format = renderMe::Texture::Format::TEXTURE_RGB;
                        break;

                    default:
                        _EXCEPTION("unsupported texture format for :" + resource_uid + std::to_string(desc.Format))
                        break;
                }

                //p_texture.setFormat(format);
                p_texture.m_format = format;

                TextureData texture_data;
                texture_data.source = renderMe::Texture::Source::CONTENT_FROM_FILE;

                texture_data.textureResource = d3dt11;
                texture_data.shaderResourceView = textureResourceView;
                texture_data.desc = desc;
                m_textures[resource_uid] = texture_data;


            }
            else
            {
                _EXCEPTION("Supporting only 2D texture :" + resource_uid)
            }
           
        }
    }

	_RENDERME_DEBUG(m_localLogger, "Texture loading SUCCESS : " + resource_uid);
	return true;

}

void D3D11SystemImpl::bindTextureStage(const std::string& p_resource_uid, size_t p_stage)
{
    if (!m_textures.count(p_resource_uid))
    {
        _EXCEPTION("unknown texture :" + p_resource_uid)
    }

    const auto textureData{ m_textures.at(p_resource_uid) };
    m_lpd3ddevcontext->PSSetShaderResources(p_stage, 1, &textureData.shaderResourceView);

    m_currentTextures[p_stage] = p_resource_uid;
}

void D3D11SystemImpl::unbindTextureStage(size_t p_stage)
{
    
    ID3D11ShaderResourceView* nullSRV[1] = { nullptr };
    m_lpd3ddevcontext->PSSetShaderResources(p_stage, 1, nullSRV);

    m_currentTextures[p_stage] = "";
}

void D3D11SystemImpl::destroyTexture(const std::string& p_resource_uid)
{
    if (!m_textures.count(p_resource_uid))
    {
        _EXCEPTION("unknown texture :" + p_resource_uid)
    }
    const auto textureData{ m_textures.at(p_resource_uid) };

    if (textureData.shaderResourceView)
    {
        textureData.shaderResourceView->Release();
    }
    
    if (textureData.textureResource)
    {
        textureData.textureResource->Release();
    }

    if (textureData.targetTexture)
    {
        textureData.targetTexture->Release();
    }

    if(textureData.targetTextureClone)
    {
        textureData.targetTextureClone->Release();
    }

    if (textureData.bits)
    {
        delete[] textureData.bits;
    }

    if (textureData.rendertextureTargetView)
    {
        textureData.rendertextureTargetView->Release();
    }

    if (textureData.stencilDepthBuffer)
    {
        textureData.stencilDepthBuffer->Release();
    }

    if (textureData.stencilDepthView)
    {
        textureData.stencilDepthView->Release();
    }

    if (textureData.stencilDepthView)
    {
        textureData.stencilDepthView->Release();
    }

    m_textures.erase(p_resource_uid);

    _RENDERME_DEBUG(m_localLogger, "texture release SUCCESS : " + p_resource_uid);
}

void D3D11SystemImpl::forceTexturesBinding()
{
    for (int i = 0; i < nbTextureStages; i++)
    {
        if ("" != m_currentTextures[i])
        {
            const auto textureData{ m_textures.at(m_currentTextures[i])};
            m_lpd3ddevcontext->PSSetShaderResources(i, 1, &textureData.shaderResourceView);
        }
    }
}

bool D3D11SystemImpl::copyTextureContent(const std::string& p_resource_uid, void** p_data, size_t* p_dataSize)
{
    DECLARE_D3D11ASSERT_VARS

    if (!m_textures.count(p_resource_uid))
    {
        _EXCEPTION("unknown texture :" + p_resource_uid)
    }

    const auto textureData{ m_textures.at(p_resource_uid) };

    // copy GPU to GPU ...
    m_lpd3ddevcontext->CopyResource(textureData.targetTextureClone, textureData.targetTexture);

    D3D11_MAPPED_SUBRESOURCE mappedResource;
    hRes = m_lpd3ddevcontext->Map(textureData.targetTextureClone, 0, D3D11_MAP_READ, 0, &mappedResource);
    D3D11_CHECK(Map)

    memcpy(textureData.bits, mappedResource.pData, textureData.blocksize);
    m_lpd3ddevcontext->Unmap(textureData.targetTextureClone, 0);
   
    *p_dataSize = textureData.blocksize;
    *p_data = textureData.bits;
}
