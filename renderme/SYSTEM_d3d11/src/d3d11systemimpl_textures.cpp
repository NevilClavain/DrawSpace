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

	const auto name{ p_texture.getName() };

	_RENDERME_DEBUG(m_localLogger, "Texture loading : " + name);

    if (m_textures.count(name))
    {
        _RENDERME_DEBUG(m_localLogger, "Texture already loaded : " + name);
    }
    else
    {
        if (renderMe::Texture::Source::CONTENT_FROM_RENDERINGQUEUE == p_texture.getSource())
        {
            // render target
        }
        else
        {
            // from file content

            ID3D11Resource*             d3dt11{ nullptr };
            ID3D11ShaderResourceView*   textureResourceView{ nullptr };

            const auto& file_content{ p_texture.getData() };

            D3D11_USAGE usage;

            unsigned int bindFlags{ D3D11_BIND_SHADER_RESOURCE };
            unsigned int cpuAccessFlags{ 0 };

            switch (p_texture.getContentAccessMode())
            {
                case renderMe::Texture::ContentAccessMode::NO_CONTENT_ACCESS:

                    usage = D3D11_USAGE_DEFAULT;
                    cpuAccessFlags = 0;

                    break;

                case renderMe::Texture::ContentAccessMode::CONTENT_ACCESS_READ:

                    usage = D3D11_USAGE_STAGING; // allow GPU/CPU transfert
                    cpuAccessFlags = D3D11_CPU_ACCESS_READ; // cpu can read content

                    break;
                case renderMe::Texture::ContentAccessMode::CONTENT_ACCESS_WRITE:

                    usage = D3D11_USAGE_DYNAMIC; // allow GPU/CPU transfert                    
                    cpuAccessFlags = D3D11_CPU_ACCESS_WRITE; // cpu can write content

                    break;

                case renderMe::Texture::ContentAccessMode::CONTENT_ACCESS_READWRITE:

                    usage = D3D11_USAGE_STAGING; // allow GPU/CPU transfert
                    cpuAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;

                    break;
            }


            hRes = DirectX::CreateWICTextureFromMemoryEx(m_lpd3ddevice, file_content.getData(), file_content.getDataSize(), 0, 
                                                            usage, //D3D11_USAGE
                                                            bindFlags, //bindFlags
                                                            cpuAccessFlags, //cpuAccessFlags
                                                            0, //miscFlags
                                                            DirectX::DX11::WIC_LOADER_DEFAULT, &d3dt11, &textureResourceView);
            D3D11_CHECK(CreateWICTextureFromMemory);

            D3D11_RESOURCE_DIMENSION dims;
            d3dt11->GetType(&dims);

            if (D3D11_RESOURCE_DIMENSION_TEXTURE2D == dims)
            {
                // retrieve texture infos

                ID3D11Texture2D* pTextureInterface{ nullptr };
                hRes = d3dt11->QueryInterface<ID3D11Texture2D>(&pTextureInterface);

                D3D11_TEXTURE2D_DESC desc;
                pTextureInterface->GetDesc(&desc);

                _RENDERME_DEBUG(m_localLogger, "Texture infos : " + std::to_string(desc.Width) + "x" + std::to_string(desc.Height) + " format : " + std::to_string(desc.Format));

                p_texture.setDims(desc.Width, desc.Height);

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
                        _EXCEPTION("unsupported texture format for :" + name + std::to_string(desc.Format))
                        break;
                }

                p_texture.setFormat(format);
            }
            else
            {
                _EXCEPTION("Supporting only 2D texture :" + name)
            }
           
        }
    }

	_RENDERME_DEBUG(m_localLogger, "Texture loading SUCCESS : " + name);
	return true;

}

void D3D11SystemImpl::setTexture(const std::string& p_name)
{

}

void D3D11SystemImpl::destroyTexture(const std::string& p_name)
{

}
