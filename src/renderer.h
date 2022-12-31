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

#ifndef _RENDERER_H_
#define _RENDERER_H_

#include "drawspace_commons.h"
#include "memalloc.h"
#include "matrix.h"
#include "renderingnode.h"

#include "logconf.h"

namespace DrawSpace
{
namespace Interface
{
class Renderer
{
public:

	struct Characteristics
    {
        long    width_resol;
        long    height_resol;
        bool    fullscreen;
        dsreal  width_viewport;
        dsreal  height_viewport;
    };

	struct DeviceDescr
    {
        dsstring driver;
        dsstring deviceName;
        dsstring description;
    };

	struct Blob
	{
		void*	data{ nullptr };
		size_t	data_size{ 0 };

		inline void Release(void)
		{
			if (data)
			{
				_DRAWSPACE_DELETE_N_(data);
				data = nullptr;
				data_size = 0;
			}
		}
	};

    virtual void GetDescr( dsstring& p_descr ) = 0;
    virtual void GetShadersDescr( dsstring& p_descr ) = 0;

    virtual void DumpMemoryAllocs( void ) = 0;

    virtual bool Init( HWND p_hwnd, bool p_fullscreen, long p_w_width, long p_w_height, Logger::Configuration* p_logcong ) = 0;
    virtual void Release( void ) = 0;
    
    virtual void BeginScreen( void ) = 0;
    virtual void EndScreen( void ) = 0;
    virtual void FlipScreen( void ) = 0;

    virtual void ClearScreen( unsigned char p_r, unsigned char p_g, unsigned char p_b, unsigned char p_a ) = 0;
    virtual void ClearDepth( dsreal p_value = 1.0 ) = 0;

    virtual void BeginTarget( void* p_data, int p_slice_index ) = 0;
    virtual void EndTarget( void* p_data ) = 0;

    virtual bool CreateMeshe( DrawSpace::Core::Meshe* p_meshe, void** p_data ) = 0;
    virtual void RemoveMeshe( DrawSpace::Core::Meshe* p_meshe, void* p_data ) = 0;    
    virtual bool SetMeshe( void* p_data ) = 0;
    virtual bool UpdateMesheIndexes( DrawSpace::Core::Meshe* p_meshe, void* p_data ) = 0;
    virtual bool UpdateMesheVertices( DrawSpace::Core::Meshe* p_meshe, void* p_data ) = 0;

    virtual bool CreateTexture( DrawSpace::Core::Texture* p_texture, void** p_data ) = 0;
    virtual void DestroyTexture( void* p_data ) = 0;
    virtual bool SetTexture( void* p_data, int p_stage ) = 0;
    virtual bool SetVertexTexture( void* p_data, int p_stage ) = 0;
	virtual bool UnsetTexture( int p_stage ) = 0;
    virtual bool UnsetVertexTexture( int p_stage ) = 0;
    virtual bool AllocTextureContent( void* p_texturedata ) = 0;
    virtual void ReleaseTextureContent( void* p_texturedata ) = 0;
    virtual void* GetTextureContentPtr( void* p_texturedata ) = 0;
    virtual bool CopyTextureContent( void* p_texturedata ) = 0;
    virtual bool UpdateTextureContent( void* p_texturedata ) = 0;

    virtual bool CreateShaders( DrawSpace::Core::Fx* p_fx, void** p_data ) = 0;
    virtual bool SetShaders( void* p_data ) = 0;

	
	////////////////// shaders compilation from source
	virtual bool CreateShaderBytes(char* p_source, int p_source_length, int p_shadertype, const dsstring& p_path, const dsstring& p_includes_path, void** p_data) = 0;
	
	virtual bool GetShaderCompilationStatus(void* p_data) = 0;
	
	virtual void* GetShaderBytes(void* p_data) = 0;
	virtual size_t GetShaderBytesLength(void* p_data) = 0;
	
	virtual dsstring GetShaderCompilationError(void* p_data) = 0;

	virtual void ReleaseShaderBytes(void* p_data) = 0;
	/////////////////////////////////////////////////////


    virtual bool ApplyRenderStatesIn( void* p_data ) = 0;
    virtual bool ApplyRenderStatesOut( void* p_data ) = 0;



    virtual bool SetFxShaderParams( int p_shader_index, long p_register, DrawSpace::Utils::Vector& p_vector ) = 0;
    virtual bool SetFxShaderMatrix( int p_shader_index, long p_register, DrawSpace::Utils::Matrix& p_mat ) = 0;

	virtual bool SetShaderVectorBuffer(int p_shader_index, long p_register, const std::vector<DrawSpace::Utils::Vector>& p_vectors) = 0;

	virtual bool DrawMeshe( DrawSpace::Utils::Matrix p_world, DrawSpace::Utils::Matrix p_view, DrawSpace::Utils::Matrix p_proj ) = 0;

    virtual void SetRenderState( DrawSpace::Core::RenderState* p_renderstate ) = 0;

    virtual void GetRenderCharacteristics( Characteristics& p_characteristics ) = 0;

    virtual void DrawText( long p_r, long p_g, long p_b, int p_posX, int p_posY, const char* p_format, ... ) = 0;

    virtual void PointProjection( DrawSpace::Utils::Matrix p_view, DrawSpace::Utils::Matrix p_proj, DrawSpace::Utils::Vector& p_point, dsreal& p_outx, dsreal& p_outy, dsreal& p_outz ) = 0;

    virtual bool GUI_InitSubSystem( void ) = 0;
    virtual void GUI_ReleaseSubSystem( void ) = 0;
    virtual void GUI_Render( void ) = 0;
    virtual void GUI_SetResourcesRootDirectory( const dsstring& p_path ) = 0;

    virtual void GUI_OnMouseMove( float p_xm, float p_ym, float p_dx, float p_dy ) = 0;
    virtual void GUI_OnMouseLeftButtonDown( void ) = 0;
    virtual void GUI_OnMouseLeftButtonUp( void ) = 0;
    virtual void GUI_OnMouseRightButtonDown( void ) = 0;
    virtual void GUI_OnMouseRightButtonUp( void ) = 0;
    virtual void GUI_OnKeyDown( long p_key ) = 0;
    virtual void GUI_OnKeyUp( long p_key ) = 0;
    virtual void GUI_OnChar( long p_key ) = 0;
    virtual void GUI_LoadLayout( const dsstring& p_layout_path, const dsstring& p_widgets_path ) = 0;
    virtual void GUI_UnloadAllLayouts( void ) = 0;
    virtual void GUI_LoadScheme( const dsstring& p_scheme_path ) = 0;
    virtual void GUI_UnloadAllSchemes( void ) = 0;
    virtual void GUI_SetLayout( const dsstring& p_layoutpath ) = 0;
    virtual void GUI_StoreWidget( const dsstring& p_layoutName, const dsstring& p_parentName, const dsstring& p_childName ) = 0;
    virtual void GUI_SetVisibleState( const dsstring& p_layoutName, const dsstring& p_widgetName, bool p_state ) = 0;
    virtual bool GUI_IsVisible( const dsstring& p_layoutName, const dsstring& p_widgetName ) = 0;
    virtual void GUI_SetWidgetText( const dsstring& p_layoutName, const dsstring& p_widgetName, const dsstring& p_text ) = 0;
    virtual void GUI_GetWidgetText( const dsstring& p_layoutName, const dsstring& p_widgetName, dsstring& p_outtext ) = 0;
    virtual void GUI_AddListboxTextItem( const dsstring& p_layoutName, const dsstring& p_widgetName, const dsstring& p_text, unsigned int p_colors, const dsstring& p_brushImage ) = 0;
    virtual void GUI_ClearListbox( const dsstring& p_layoutName, const dsstring& p_widgetName ) = 0;
    virtual bool GUI_GetListboxFirstSelectedItemIndex( const dsstring& p_layoutName, const dsstring& p_widgetName, int& p_index, dsstring& p_text ) = 0;
    virtual void GUI_AddComboboxTextItem( const dsstring& p_layoutName, const dsstring& p_widgetName, const dsstring& p_text, unsigned int p_colors, const dsstring& p_brushImage ) = 0;
    virtual void GUI_ClearCombobox( const dsstring& p_layoutName, const dsstring& p_widgetName ) = 0;
    virtual void GUI_SetComboBoxItemSelectionState( const dsstring& p_layoutName, const dsstring& p_widgetName, int p_index, bool p_state ) = 0;
    virtual int  GUI_GetComboBoxSelectionIndex( const dsstring& p_layoutName, const dsstring& p_widgetName ) = 0;
    virtual bool GUI_IsCheckBoxChecked( const dsstring& p_layoutName, const dsstring& p_widgetName ) = 0;
    virtual void GUI_SetCheckboxState( const dsstring& p_layoutName, const dsstring& p_widgetName, bool p_state ) = 0;
    virtual void GUI_SetMultiLineEditboxCaretIndex(const dsstring& p_layoutName, const dsstring& p_widgetName, int p_index) = 0;


    virtual void GUI_RegisterPushButtonEventClickedHandler( DrawSpace::Core::BaseCallback2<void, const dsstring&, const dsstring&>* p_handler ) = 0;
    virtual void GUI_RegisterCheckboxEventStateChangedHandler( DrawSpace::Core::BaseCallback3<void, const dsstring&, const dsstring&, bool>* p_handler ) = 0;
    virtual void GUI_SetMouseCursorImage( const dsstring& p_image ) = 0;
    virtual void GUI_ShowMouseCursor( bool p_show ) = 0;

    virtual void GUI_CreateSprite( const dsstring& p_scheme_object, const dsstring& p_spriteName ) = 0;
    virtual void GUI_SetSpritePosition( const dsstring& p_spriteName, dsreal p_xpos, dsreal p_ypos ) = 0;
    virtual void GUI_SetSpriteImage( const dsstring& p_spriteName, const dsstring& p_image ) = 0;
    virtual void GUI_SetSpriteScale( const dsstring& p_spriteName, dsreal p_scale ) = 0;
    virtual void GUI_SetSpriteRotation( const dsstring& p_spriteName, const DrawSpace::Utils::Vector& p_axis, dsreal p_deg_angle ) = 0;

    virtual void GUI_InitTest( void ) = 0; // temporaire

};
}
}

#endif
