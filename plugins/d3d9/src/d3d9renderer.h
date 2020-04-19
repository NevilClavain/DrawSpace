/* -*-LIC_BEGIN-*- */
/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2020                     
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

#include "CEGUIWrapper.h"
#include <CEGUI\RendererModules\Direct3D9\Renderer.h>

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

    typedef struct
    {
        LPDIRECT3DTEXTURE9          texture;

        // pour le cas des texture RENDER_TARGET : 2eme texture permettant d'acceder au contenu de la 1ere, via IDirect3DDevice9::GetRenderTargetData()
        LPDIRECT3DTEXTURE9          texture2;
        D3DSURFACE_DESC             descr;
        bool                        render_texture;
        void*                       bits;
        dsstring                    path;
        DrawSpace::Core::Texture*   texture_instance;
        dsstring                    hash; // hash des caracteristique class Texture associee

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
        int                     nb_vertices;
        int                     nb_triangles;

    } MesheData;

    typedef struct
    {
        LPDIRECT3DVERTEXSHADER9         vertex_shader;
        LPDIRECT3DPIXELSHADER9          pixel_shader;

    } ShadersData;
    
    

    typedef struct
    {
        LPDIRECT3DVERTEXSHADER9         vertex_shader;
        LPDIRECT3DPIXELSHADER9          pixel_shader;        
        
        long                            nb_stages;
        LPDIRECT3DTEXTURE9              textures[8];

    } NodeInfos;

	static const int NbMaxVectorForShadersBuffers = 512;


    HWND                                                        m_hwnd;

    dsstring                                                    m_lasterror;
    LPDIRECT3D9                                                 m_lpd3d;
    LPDIRECT3DDEVICE9                                           m_lpd3ddevice;

    D3DVIEWPORT9                                                m_viewport;
    
    LPDIRECT3DVERTEXDECLARATION9                                m_vertexdeclaration;

    D3DFORMAT                                                   m_depthbuffer_format;

    LPD3DXFONT                                                  m_font;

    std::map<DrawSpace::Core::RenderingNode*, NodeInfos>        m_nodes;

    
    std::map<dsstring, TextureInfos*>                           m_textures_base;
    //std::map<DrawSpace::Core::Texture*, TargetTextureInfos>     m_targettextures_base;
    std::map<dsstring, TargetTextureInfos>                      m_targettextures_base;
    
    std::map<dsstring, MesheData*>                              m_meshes_base;
    
    std::map<dsstring, ShadersData*>                            m_shaders_bases;
    std::map<dsstring, DrawSpace::Core::Fx*>                    m_fx_bases;


    Characteristics                                             m_characteristics;
    
    int                                                         m_next_nbvertices;
    int                                                         m_next_nbtriangles;

    CEGUIWrapper                                                m_guisubsystem;

    dsstring                                                    m_driver_type;
    D3DDEVTYPE                                                  m_dev_type;
    int                                                         m_vproc;

	float														m_shaders_array[NbMaxVectorForShadersBuffers * 4];


    void set_vertexshader_constants( DWORD p_startreg, dsreal *p_ftab, DWORD p_v4fCount );
    void set_pixelshader_constants( DWORD p_startreg, dsreal *p_ftab, DWORD p_v4fCount );

    D3DFORMAT find_depthbuffer_format( int p_adapterordinal, D3DFORMAT p_displayformat );

    void fullscreen_autoset_desktop_resolution( int& p_fullscreen_width, int& p_fullscreen_height, D3DFORMAT& p_fullscreen_format, UINT& p_fullscreen_refreshRate );

public:
    D3D9Renderer( void );
    ~D3D9Renderer( void );

    virtual void GetDescr( dsstring& p_descr );    
    virtual void GetShadersDescr( dsstring& p_descr );

    virtual void DumpMemoryAllocs( void );

    virtual bool Init( HWND p_hwnd, bool p_fullscreen, long p_w_width, long p_w_height, DrawSpace::Logger::Configuration* p_logconf );
    virtual void Release( void );
    
    virtual void BeginScreen( void );
    virtual void EndScreen( void );
    virtual void FlipScreen( void );

    virtual void ClearScreen( unsigned char p_r, unsigned char p_g, unsigned char p_b, unsigned char p_a );
    virtual void ClearDepth( dsreal p_value = 1.0 );

    virtual void BeginTarget( void* p_data, int p_slice_index );
    virtual void EndTarget( void* p_data );

    virtual bool CreateMeshe( DrawSpace::Core::Meshe* p_meshe, void** p_data );
    virtual void RemoveMeshe( DrawSpace::Core::Meshe* p_meshe, void* p_data );    
    virtual bool SetMeshe( void* p_data );
    virtual bool UpdateMesheIndexes( DrawSpace::Core::Meshe* p_meshe, void* p_data );
    virtual bool UpdateMesheVertices( DrawSpace::Core::Meshe* p_meshe, void* p_data );
    virtual bool UpdateMesheVerticesFromImpostors( const DrawSpace::ImpostorsDisplayList& p_list, void* p_data );

    virtual bool CreateTexture( DrawSpace::Core::Texture* p_texture, void** p_data );
    virtual void DestroyTexture( void* p_data );
    virtual bool SetTexture( void* p_data, int p_stage );
    virtual bool SetVertexTexture( void* p_data, int p_stage );
	virtual bool UnsetTexture( int p_stage );
    virtual bool UnsetVertexTexture( int p_stage );
    virtual bool AllocTextureContent( void* p_texturedata );
    virtual void ReleaseTextureContent( void* p_texturedata );
    virtual void* GetTextureContentPtr( void* p_texturedata );
    virtual bool CopyTextureContent( void* p_texturedata );
    virtual bool UpdateTextureContent( void* p_texturedata );

    virtual bool CreateShaders( DrawSpace::Core::Fx* p_fx, void** p_data );
    virtual bool SetShaders( void* p_data );

    virtual bool ApplyRenderStatesIn( void* p_data );
    virtual bool ApplyRenderStatesOut( void* p_data );


    virtual bool SetFxShaderParams( int p_shader_index, long p_register, DrawSpace::Utils::Vector& p_vector );
    virtual bool SetFxShaderMatrix( int p_shader_index, long p_register, DrawSpace::Utils::Matrix& p_mat );

	virtual bool SetShaderVectorBuffer(int p_shader_index, long p_register, const std::vector<DrawSpace::Utils::Vector>& p_vectors);

	virtual bool DrawMeshe( DrawSpace::Utils::Matrix p_world, DrawSpace::Utils::Matrix p_view, DrawSpace::Utils::Matrix p_proj );

    virtual void SetRenderState( DrawSpace::Core::RenderState* p_renderstate );

    virtual void GetRenderCharacteristics( Characteristics& p_characteristics );

    virtual void DrawText( long p_r, long p_g, long p_b, int p_posX, int p_posY, const char* p_format, ... );

    virtual void PointProjection( DrawSpace::Utils::Matrix p_view, DrawSpace::Utils::Matrix p_proj, DrawSpace::Utils::Vector& p_point, dsreal& p_outx, dsreal& p_outy, dsreal& p_outz );

    virtual bool GUI_InitSubSystem( void );
    virtual void GUI_ReleaseSubSystem( void );
    virtual void GUI_Render( void );
    virtual void GUI_SetResourcesRootDirectory( const dsstring& p_path );

    virtual void GUI_OnMouseMove( float p_xm, float p_ym, float p_dx, float p_dy );
    virtual void GUI_OnMouseLeftButtonDown( void );
    virtual void GUI_OnMouseLeftButtonUp( void );
    virtual void GUI_OnMouseRightButtonDown( void );
    virtual void GUI_OnMouseRightButtonUp( void );
    virtual void GUI_OnKeyDown( long p_key );
    virtual void GUI_OnKeyUp( long p_key );
    virtual void GUI_OnChar( long p_key );
    virtual void GUI_LoadLayout( const dsstring& p_layout_path, const dsstring& p_widgets_path );
    virtual void GUI_UnloadAllLayouts( void );
    virtual void GUI_LoadScheme( const dsstring& p_scheme_path );
    virtual void GUI_UnloadAllSchemes( void );
    virtual void GUI_SetLayout( const dsstring& p_layoutpath );
    virtual void GUI_StoreWidget( const dsstring& p_layoutName, const dsstring& p_parentName, const dsstring& p_childName );
    virtual void GUI_SetVisibleState( const dsstring& p_layoutName, const dsstring& p_widgetName, bool p_state );
    virtual bool GUI_IsVisible( const dsstring& p_layoutName, const dsstring& p_widgetName );
    virtual void GUI_SetWidgetText( const dsstring& p_layoutName, const dsstring& p_widgetName, const dsstring& p_text );
    virtual void GUI_GetWidgetText( const dsstring& p_layoutName, const dsstring& p_widgetName, dsstring& p_outtext );
    virtual void GUI_AddListboxTextItem( const dsstring& p_layoutName, const dsstring& p_widgetName, const dsstring& p_text, unsigned int p_colors, const dsstring& p_brushImage );
    virtual void GUI_ClearListbox( const dsstring& p_layoutName, const dsstring& p_widgetName );
    virtual bool GUI_GetListboxFirstSelectedItemIndex( const dsstring& p_layoutName, const dsstring& p_widgetName, int& p_index, dsstring& p_text );
    virtual void GUI_AddComboboxTextItem( const dsstring& p_layoutName, const dsstring& p_widgetName, const dsstring& p_text, unsigned int p_colors, const dsstring& p_brushImage );
    virtual void GUI_ClearCombobox( const dsstring& p_layoutName, const dsstring& p_widgetName );
    virtual void GUI_SetComboBoxItemSelectionState( const dsstring& p_layoutName, const dsstring& p_widgetName, int p_index, bool p_state );
    virtual int  GUI_GetComboBoxSelectionIndex( const dsstring& p_layoutName, const dsstring& p_widgetName );
    virtual bool GUI_IsCheckBoxChecked( const dsstring& p_layoutName, const dsstring& p_widgetName );
    virtual void GUI_SetCheckboxState( const dsstring& p_layoutName, const dsstring& p_widgetName, bool p_state );

    virtual void GUI_RegisterPushButtonEventClickedHandler( DrawSpace::Core::BaseCallback2<void, const dsstring&, const dsstring&>* p_handler );
    virtual void GUI_RegisterCheckboxEventStateChangedHandler( DrawSpace::Core::BaseCallback3<void, const dsstring&, const dsstring&, bool>* p_handler );
    virtual void GUI_SetMouseCursorImage( const dsstring& p_image );
    virtual void GUI_ShowMouseCursor( bool p_show );

    virtual void GUI_CreateSprite( const dsstring& p_scheme_object, const dsstring& p_spriteName );
    virtual void GUI_SetSpritePosition( const dsstring& p_spriteName, dsreal p_xpos, dsreal p_ypos );
    virtual void GUI_SetSpriteImage( const dsstring& p_spriteName, const dsstring& p_image );
    virtual void GUI_SetSpriteScale( const dsstring& p_spriteName, dsreal p_scale );
    virtual void GUI_SetSpriteRotation( const dsstring& p_spriteName, const DrawSpace::Utils::Vector& p_axis, dsreal p_deg_angle );

    virtual void GUI_InitTest( void ); // temporaire

};

#endif
