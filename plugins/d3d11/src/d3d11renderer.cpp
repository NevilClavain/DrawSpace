/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2016                              
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

#include "d3d11renderer.h"
#include <memalloc.h>

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Utils;

D3D11Renderer::D3D11Renderer( void )
{

}

D3D11Renderer::~D3D11Renderer( void )
{

}


void D3D11Renderer::GetDescr( dsstring& p_descr )
{

}

void D3D11Renderer::GetDeviceDescr( DeviceDescr& p_ddescr )
{

}

void D3D11Renderer::DumpMemoryAllocs( void )
{

}

bool D3D11Renderer::Init( HWND p_hwnd, bool p_fullscreen, long p_w_width, long p_w_height, DrawSpace::Logger::Configuration* p_logconf )
{
    return false;
}

void D3D11Renderer::Release( void )
{

}

void D3D11Renderer::SetViewport( bool p_automatic, long p_vpx, long p_vpy, long p_vpwidth, long p_vpheight, float p_vpminz, float p_vpmaxz )
{

}

void D3D11Renderer::BeginScreen( void )
{

}

void D3D11Renderer::EndScreen( void )
{

}

void D3D11Renderer::FlipScreen( void )
{

}


void D3D11Renderer::ClearScreen( unsigned char p_r, unsigned char p_g, unsigned char p_b, unsigned char p_a )
{

}

void D3D11Renderer::ClearDepth( dsreal p_value )
{

}

void D3D11Renderer::BeginTarget( DrawSpace::Core::Texture* p_texture )
{

}

void D3D11Renderer::EndTarget( DrawSpace::Core::Texture* p_texture )
{

}

bool D3D11Renderer::CreateMeshe( DrawSpace::Core::Meshe* p_meshe, void** p_data )
{
    return false;
}

void D3D11Renderer::RemoveMeshe( DrawSpace::Core::Meshe* p_meshe, void* p_data )
{

}

bool D3D11Renderer::SetMeshe( void* p_data )
{
    return false;
}

bool D3D11Renderer::UpdateMesheIndexes( DrawSpace::Core::Meshe* p_meshe, void* p_data )
{
    return false;
}

bool D3D11Renderer::UpdateMesheVertices( DrawSpace::Core::Meshe* p_meshe, void* p_data )
{
    return false;
}

bool D3D11Renderer::UpdateMesheVerticesFromImpostors( const DrawSpace::ImpostorsDisplayList& p_list, void* p_data )
{
    return false;
}

bool D3D11Renderer::CreateTexture( DrawSpace::Core::Texture* p_texture, void** p_data )
{
    return false;
}

void D3D11Renderer::DestroyTexture( void* p_data )
{

}

bool D3D11Renderer::SetTexture( void* p_data, int p_stage )
{
    return false;
}

bool D3D11Renderer::SetVertexTexture( void* p_data, int p_stage )
{
    return false;
}

bool D3D11Renderer::UnsetTexture( int p_stage )
{
    return false;
}

bool D3D11Renderer::UnsetVertexTexture( int p_stage )
{
    return false;
}

bool D3D11Renderer::AllocTextureContent( void* p_texturedata )
{
    return false;
}

void D3D11Renderer::ReleaseTextureContent( void* p_texturedata )
{

}

void* D3D11Renderer::GetTextureContentPtr( void* p_texturedata )
{
    return NULL;
}

bool D3D11Renderer::CopyTextureContent( void* p_texturedata )
{
    return false;
}

bool D3D11Renderer::UpdateTextureContent( void* p_texturedata )
{
    return false;
}

bool D3D11Renderer::CreateShaders( DrawSpace::Core::Fx* p_fx, void** p_data )
{
    return false;
}

bool D3D11Renderer::SetShaders( void* p_data )
{
    return false;
}

bool D3D11Renderer::CreateRenderStatesSet( DrawSpace::Core::Fx* p_fx, void** p_data )
{
    return false;
}

bool D3D11Renderer::ApplyRenderStatesIn( void* p_data )
{
    return false;
}

bool D3D11Renderer::ApplyRenderStatesOut( void* p_data )
{
    return false;
}

bool D3D11Renderer::SetFxShaderParams( int p_shader_index, long p_register, DrawSpace::Utils::Vector& p_vector )
{
    return false;
}

bool D3D11Renderer::SetFxShaderMatrix( int p_shader_index, long p_register, DrawSpace::Utils::Matrix& p_mat )
{
    return false;
}

bool D3D11Renderer::DrawMeshe( DrawSpace::Utils::Matrix p_world, DrawSpace::Utils::Matrix p_view, DrawSpace::Utils::Matrix p_proj )
{
    return false;
}

void D3D11Renderer::SetRenderState( DrawSpace::Core::RenderState* p_renderstate )
{

}

void D3D11Renderer::GetRenderCharacteristics( Characteristics& p_characteristics )
{

}

void D3D11Renderer::DrawText( long p_r, long p_g, long p_b, int p_posX, int p_posY, const char* p_format, ... )
{

}

void D3D11Renderer::PointProjection( DrawSpace::Utils::Matrix p_view, DrawSpace::Utils::Matrix p_proj, DrawSpace::Utils::Vector& p_point, dsreal& p_outx, dsreal& p_outy, dsreal& p_outz )
{

}

