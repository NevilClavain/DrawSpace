/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2015                              
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

#ifndef _D3D9RENDERER_H_
#define _D3D9RENDERER_H_

#include <memalloc.h>
#include <tracedefs.h>
#include <renderer.h>
#include <parser.h>
#include <fx.h>
#include <vector.h>
#include <matrix.h>
#include <transformation.h>


#include <d3dx9.h>
#include "d3d9vertex.h"
#include "d3d9triangle.h"



#define D3D9_RELEASE( p_Obj ) { if( p_Obj ) { p_Obj->Release(); p_Obj = NULL; } }


#define DECLARE_D3D9ASSERT_VARS HRESULT hRes; \
                                    dsstring d3dErrStr;

#define D3D9_CHECK( p_mName ) \
    if( hRes != D3D_OK ) \
    { \
        TranslateD3DD9Error( hRes, d3dErrStr ); \
        dsstring dstr = " "#p_mName" -> "; \
        dstr += d3dErrStr; \
        dstr += "\n"; \
        _DSERROR( logger, dstr.c_str() ); \
        return false; \
    }

class D3D9Renderer : public DrawSpace::Interface::Renderer
{
protected:

    class Config : public DrawSpace::Utils::Parser
    {
    protected:
        bool on_new_line( const dsstring& p_line, long p_line_num, std::vector<dsstring>& p_words );

    public:
        int         m_adapter_ordinal;
        int         m_fullscreen_width;
        int         m_fullscreen_height;
        int         m_refreshrate;
        D3DFORMAT   m_fullscreen_format;
        DWORD       m_vertex_processing;
    };


    typedef struct
    {
        LPDIRECT3DTEXTURE9  texture;
        D3DSURFACE_DESC     descr;
        bool                render_texture;
        void*               bits;

    } TextureInfos;

    typedef struct
    {
        LPDIRECT3DSURFACE9      surface;
        LPD3DXRENDERTOSURFACE   render_to_surface;

    } TargetTextureInfos;

    typedef struct
    {
        LPDIRECT3DVERTEXBUFFER9 vertex_buffer;
		LPDIRECT3DINDEXBUFFER9	index_buffer;

    } MesheData;

    typedef struct
    {
        LPDIRECT3DVERTEXSHADER9         vertex_shader;
        LPDIRECT3DPIXELSHADER9          pixel_shader;

        DrawSpace::Core::Fx*            fx;

    } FxData;

    typedef struct
    {
        LPDIRECT3DVERTEXSHADER9         vertex_shader;
        LPDIRECT3DPIXELSHADER9          pixel_shader;        
        
        long                            nb_stages;
        LPDIRECT3DTEXTURE9              textures[8];

    } NodeInfos;



    Config                                                      m_config;

    HWND                                                        m_hwnd;

    dsstring                                                    m_lasterror;
    LPDIRECT3D9                                                 m_lpd3d;
    LPDIRECT3DDEVICE9                                           m_lpd3ddevice;

    D3DVIEWPORT9                                                m_viewport;
    //DrawSpace::Utils::Matrix                                    m_projection;
    LPDIRECT3DVERTEXDECLARATION9                                m_vertexdeclaration;

    D3DFORMAT                                                   m_depthbuffer_format;

    LPD3DXFONT                                                  m_font;

    std::map<DrawSpace::Core::RenderingNode*, NodeInfos>        m_nodes;

    
    std::map<dsstring, TextureInfos*>                           m_textures_base;
    std::map<DrawSpace::Core::Texture*, TargetTextureInfos>     m_targettextures_base;
    
    std::map<dsstring, MesheData*>                              m_meshes_base;
    std::map<dsstring, FxData*>                                 m_fx_bases;


    Characteristics                                             m_characteristics;
        
    void set_vertexshader_constants( DWORD p_startreg, dsreal *p_ftab, DWORD p_v4fCount );
    void set_pixelshader_constants( DWORD p_startreg, dsreal *p_ftab, DWORD p_v4fCount );

    D3DFORMAT find_depthbuffer_format( int p_adapterordinal, D3DFORMAT p_displayformat );

public:
    D3D9Renderer( void );
    ~D3D9Renderer( void );

    virtual void GetDescr( dsstring& p_descr );
    virtual void DumpMemoryAllocs( void );

    virtual bool Init( HWND p_hwnd, bool p_fullscreen, long p_w_width, long p_w_height, DrawSpace::Logger::Configuration* p_logconf );
    virtual void Release( void );

    virtual void SetViewport( bool p_automatic, long p_vpx, long p_vpy, long p_vpwidth, long p_vpheight, float p_vpminz, float p_vpmaxz );
    
    virtual void BeginScreen( void );
    virtual void EndScreen( void );
    virtual void FlipScreen( void );

    virtual void ClearScreen( unsigned char p_r, unsigned char p_g, unsigned char p_b );
    virtual void ClearDepth( dsreal p_value = 1.0 );

    virtual void BeginTarget( DrawSpace::Core::Texture* p_texture );
    virtual void EndTarget( DrawSpace::Core::Texture* p_texture );

    virtual bool CreateMeshe( DrawSpace::Core::Meshe* p_meshe, void** p_data );
    virtual void RemoveMeshe( DrawSpace::Core::Meshe* p_meshe, void* p_data );
    virtual bool SetMeshe( void* p_data );

    virtual bool CreateTexture( DrawSpace::Core::Texture* p_texture, void** p_data );
    virtual bool SetTexture( void* p_data, int p_stage );
    virtual bool SetVertexTexture( void* p_data, int p_stage );
	virtual bool UnsetTexture( int p_stage );
    virtual bool UnsetVertexTexture( int p_stage );
    virtual bool AllocTextureContent( void* p_texturedata );
    virtual void RemoveTextureContent( void* p_texturedata );
    virtual void* GetTextureContent( void* p_texturedata );


    virtual bool CreateFx( DrawSpace::Core::Fx* p_fx, void** p_data );
    virtual bool SetFx( void* p_data );
	virtual bool UnsetFx( void* p_data );
    virtual bool SetFxShaderParams( int p_shader_index, long p_register, DrawSpace::Utils::Vector& p_vector );

	virtual bool DrawMeshe( long p_nbvertices, long p_nbtriangles, DrawSpace::Utils::Matrix p_world, DrawSpace::Utils::Matrix p_view, DrawSpace::Utils::Matrix p_proj );

    virtual void SetRenderState( DrawSpace::Core::RenderState* p_renderstate );

    virtual void GetRenderCharacteristics( Characteristics& p_characteristics );

    virtual void DrawText( long p_r, long p_g, long p_b, int p_posX, int p_posY, const char* p_format, ... );

    virtual void PointProjection( DrawSpace::Utils::Matrix p_view, DrawSpace::Utils::Matrix p_proj, DrawSpace::Utils::Vector& p_point, dsreal& p_outx, dsreal& p_outy, dsreal& p_outz );
};

#endif
