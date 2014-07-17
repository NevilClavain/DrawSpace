/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2014                                
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

#ifndef _RENDERER_H_
#define _RENDERER_H_

#include "drawspace_commons.h"
#include "matrix.h"
#include "renderingnode.h"

namespace DrawSpace
{
namespace Interface
{
class Renderer
{
public:

    typedef struct
    {
        long    width_resol;
        long    height_resol;
        bool    fullscreen;
        dsreal  width_viewport;
        dsreal  height_viewport;

    } Characteristics;

    virtual void GetDescr( dsstring& p_descr ) = 0;
    virtual void DumpMemoryAllocs( void ) = 0;

    virtual bool Init( HWND p_hwnd, bool p_fullscreen, long p_w_width, long p_w_height ) = 0;
    virtual void Release( void ) = 0;

    virtual void SetViewport( bool p_windowed, long p_vpx, long p_vpy, long p_vpwidth, long p_vpheight, float p_vpminz, float p_vpmaxz ) = 0;
    //virtual void SetProjection( float p_vw, float p_vh, float p_zn, float p_zf ) = 0;

    virtual void BeginScreen( void ) = 0;
    virtual void EndScreen( void ) = 0;
    virtual void FlipScreen( void ) = 0;

    virtual void ClearScreen( unsigned char p_r, unsigned char p_g, unsigned char p_b ) = 0;
    virtual void ClearDepth( dsreal p_value = 1.0 ) = 0;

    virtual void BeginTarget( DrawSpace::Core::Texture* p_texture ) = 0;
    virtual void EndTarget( DrawSpace::Core::Texture* p_texture ) = 0;


    virtual bool CreateMeshe( DrawSpace::Core::Meshe* p_meshe, void** p_data ) = 0;
    virtual void RemoveMeshe( DrawSpace::Core::Meshe* p_meshe, void* p_data ) = 0;
    virtual bool SetMeshe( void* p_data ) = 0;

    virtual bool CreateTexture( DrawSpace::Core::Texture* p_texture, void** p_data ) = 0;
    virtual bool SetTexture( void* p_data, int p_stage ) = 0;
	virtual bool UnsetTexture( int p_stage ) = 0;

    virtual bool CreateFx( DrawSpace::Core::Fx* p_fx, void** p_data ) = 0;
    virtual bool SetFx( void* p_data ) = 0;
	virtual bool UnsetFx( void* p_data ) = 0;
    virtual bool SetFxShaderParams( int p_shader_index, long p_register, DrawSpace::Utils::Vector& p_vector ) = 0;

	virtual bool DrawMeshe( long p_nbvertices, long p_nbtriangles, DrawSpace::Utils::Matrix p_world, DrawSpace::Utils::Matrix p_view, DrawSpace::Utils::Matrix p_proj ) = 0;

    virtual void SetRenderState( DrawSpace::Core::RenderState* p_renderstate ) = 0;

    virtual void GetRenderCharacteristics( Characteristics& p_characteristics ) = 0;

    virtual void DrawText( long p_r, long p_g, long p_b, int p_posX, int p_posY, const char* p_format, ... ) = 0;

    virtual void PointProjection( DrawSpace::Utils::Matrix p_view, DrawSpace::Utils::Matrix p_proj, DrawSpace::Utils::Vector& p_point, dsreal& p_outx, dsreal& p_outy, dsreal& p_outz ) = 0;
    
};
}
}

#endif