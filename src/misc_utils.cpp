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


#include "misc_utils.h"
#include "memalloc.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Interface;
using namespace DrawSpace::Utils;
using namespace DrawSpace::Gui;

static std::map<dsstring, PlugInManager<Drawable>::Handle>      m_drawableplugins;
static std::map<dsstring, PlugInManager<FontImport>::Handle>    m_fontimportplugins;
static std::map<dsstring, PlugInManager<MesheImport>::Handle>   m_mesheimportplugins;

TextWidget* DrawSpace::Utils::BuildText( DrawSpace::Core::Font* p_font, long p_width, long p_height, const Vector& p_color, const dsstring& p_name )
{
    TextWidget* text_widget;

    text_widget = _DRAWSPACE_NEW_( TextWidget, TextWidget( p_name, p_width, p_height, p_font, false, NULL ) );

    text_widget->GetImageFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( false ) ) );
    text_widget->GetImageFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( false ) ) );

    text_widget->GetImageFx()->GetShader( 0 )->SetText( 
        
        "float4x4 matWorldViewProjection: register(c0);"
        "struct VS_INPUT"
        "{"
           "float4 Position : POSITION0;"
           "float4 TexCoord0: TEXCOORD0;"
        "};"
        "struct VS_OUTPUT"
        "{"
           "float4 Position : POSITION0;"
           "float4 TexCoord0: TEXCOORD0;"
        "};"
        "VS_OUTPUT vs_main( VS_INPUT Input )"
        "{"
           "VS_OUTPUT Output;"
           "Output.Position = mul( Input.Position, matWorldViewProjection );"
           "Output.TexCoord0 = Input.TexCoord0;"  
           "return( Output );"
        "}"     
        );

    text_widget->GetImageFx()->GetShader( 1 )->SetText( 

        "float4 text_color: register(c0);"
        "sampler2D Texture0;"
        "struct PS_INTPUT"
        "{"
           "float4 Position : POSITION0;"
           "float4 TexCoord0: TEXCOORD0;"
        "};"
        "float4 ps_main( PS_INTPUT input ) : COLOR0"
        "{"
           "float4 color = tex2D( Texture0, input.TexCoord0 );"

           "float4 final_color;"

           "final_color.r = text_color.r;"
           "final_color.g = text_color.g;"
           "final_color.b = text_color.b;"
           "final_color.a = color.r;"

           "return final_color;"

        "}"        
      );

    text_widget->GetImageFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ALPHABLENDENABLE, "true" ) );
    text_widget->GetImageFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ALPHABLENDOP, "add"  ) );
    text_widget->GetImageFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ALPHABLENDFUNC, "always"  ) );
    text_widget->GetImageFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ALPHABLENDDEST, "invsrcalpha"  ) );
    text_widget->GetImageFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ALPHABLENDSRC, "srcalpha"  ) );
    text_widget->GetImageFx()->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ALPHABLENDENABLE, "false" ) );
    text_widget->GetImageFx()->AddShaderRealVectorParameter( 1, "color", 0 );
    text_widget->GetImageFx()->SetShaderRealVector( "color", p_color );

    text_widget->GetTextFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( false ) ) );
    text_widget->GetTextFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( false ) ) );

    text_widget->GetTextFx()->GetShader( 0 )->SetText( 

        "float4x4 matWorldViewProjection: register(c0);"

        "struct VS_INPUT"
        "{"
           "float4 Position : POSITION0;"
           "float4 TexCoord0: TEXCOORD0;"  
        "};"

        "struct VS_OUTPUT"
        "{"
           "float4 Position : POSITION0;"
           "float4 TexCoord0: TEXCOORD0;"
        "};"

        "VS_OUTPUT vs_main( VS_INPUT Input )"
        "{"
           "VS_OUTPUT Output;"

           "Output.Position = mul( Input.Position, matWorldViewProjection );"
           "Output.TexCoord0 = Input.TexCoord0;"
              
           "return( Output );"
        "}"
    );


    text_widget->GetTextFx()->GetShader( 1 )->SetText( 
        
        "float4 text_color: register(c0);"
        "sampler2D Texture0;"

        "struct PS_INTPUT"
        "{"
           "float4 Position : POSITION0;"
           "float4 TexCoord0: TEXCOORD0;"
        "};"

        "float4 ps_main( PS_INTPUT input ) : COLOR0"
        "{"
           "float4 color = tex2D( Texture0, input.TexCoord0 );"
           "if( color.r == 0.0 && color.g == 0.0 && color.b == 0.0 )"
           "{"
              "clip( -1.0 );"
           "}"
           "return color * text_color;"
        "}"
    );

    text_widget->GetTextFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETTEXTUREFILTERTYPE, "linear" ) );
    text_widget->GetTextFx()->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETTEXTUREFILTERTYPE, "none" ) );
    text_widget->GetTextFx()->AddShaderRealVectorParameter( 1, "color", 0 );
    text_widget->GetTextFx()->SetShaderRealVector( "color", Utils::Vector( 1.0, 1.0, 1.0, 0.0 ) );

    text_widget->SetPassTargetClearingColor( 0, 0, 0 );

    return text_widget;
}

bool DrawSpace::Utils::LoadDrawablePlugin( const dsstring& p_path, const dsstring& p_pluginalias )
{
    PlugInManager<Drawable>::Handle pihandle;
    PluginManagerStatus pistatus = PlugInManager<Drawable>::LoadPlugin( p_path.c_str(), pihandle );
    if( pistatus != PIM_OK )
    {
        return false;
    }
    m_drawableplugins[p_pluginalias] = pihandle;
    return true;
}

Interface::Drawable* DrawSpace::Utils::InstanciateDrawableFromPlugin( const dsstring& p_pluginalias )
{
    Drawable* drawable;

    if( m_drawableplugins.count( p_pluginalias ) > 0 )
    {
        if( PIM_OK == PlugInManager<Drawable>::Instanciate( m_drawableplugins[p_pluginalias], &drawable ) )
        {
            return drawable;
        }        
    }
    return NULL;
}

bool DrawSpace::Utils::LoadFontImportPlugin( const dsstring& p_path, const dsstring& p_pluginalias )
{
    PlugInManager<FontImport>::Handle pihandle;
    PluginManagerStatus pistatus = PlugInManager<FontImport>::LoadPlugin( p_path.c_str(), pihandle );
    if( pistatus != PIM_OK )
    {
        return false;
    }
    m_fontimportplugins[p_pluginalias] = pihandle;
    return true;
}

Interface::FontImport* DrawSpace::Utils::InstanciateFontImportFromPlugin( const dsstring& p_pluginalias )
{
    FontImport* fontimp;

    if( m_fontimportplugins.count( p_pluginalias ) > 0 )
    {
        if( PIM_OK == PlugInManager<FontImport>::Instanciate( m_fontimportplugins[p_pluginalias], &fontimp ) )
        {
            return fontimp;
        }        
    }
    return NULL;
}

bool DrawSpace::Utils::LoadMesheImportPlugin( const dsstring& p_path, const dsstring& p_pluginalias )
{
    PlugInManager<MesheImport>::Handle pihandle;
    PluginManagerStatus pistatus = PlugInManager<MesheImport>::LoadPlugin( p_path.c_str(), pihandle );
    if( pistatus != PIM_OK )
    {
        return false;
    }
    m_mesheimportplugins[p_pluginalias] = pihandle;
    return true;
}

Interface::MesheImport* DrawSpace::Utils::InstanciateMesheImportFromPlugin( const dsstring& p_pluginalias )
{
    MesheImport* mesheimp;

    if( m_mesheimportplugins.count( p_pluginalias ) > 0 )
    {
        if( PIM_OK == PlugInManager<MesheImport>::Instanciate( m_mesheimportplugins[p_pluginalias], &mesheimp ) )
        {
            return mesheimp;
        }        
    }
    return NULL;
}

void DrawSpace::Utils::BuildSpaceboxFx( Interface::Drawable* p_spacebox, const dsstring& p_passname, const dsstring& p_nodeid )
{
    p_spacebox->GetNodeFromPass( p_passname, p_nodeid )->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( false ) ) );
    p_spacebox->GetNodeFromPass( p_passname, p_nodeid )->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( false ) ) );

    p_spacebox->GetNodeFromPass( p_passname, p_nodeid )->GetFx()->GetShader( 0 )->SetText(

        "float4x4 matWorldViewProjection: register(c0);"

        "struct VS_INPUT"
        "{"
           "float4 Position : POSITION0;"
           "float4 TexCoord0: TEXCOORD0;"              
        "};"

        "struct VS_OUTPUT"
        "{"
           "float4 Position : POSITION0;"
           "float4 TexCoord0: TEXCOORD0;"
        "};"

        "VS_OUTPUT vs_main( VS_INPUT Input )"
        "{"
           "VS_OUTPUT Output;"

           "Output.Position = mul( Input.Position, matWorldViewProjection );"
           "Output.TexCoord0 = Input.TexCoord0;"
              
           "return( Output );"
        "}"

            );

    p_spacebox->GetNodeFromPass( p_passname, p_nodeid )->GetFx()->GetShader( 1 )->SetText(

        "sampler2D Texture0;"

        "struct PS_INTPUT"
        "{"
           "float4 Position : POSITION0;"
           "float4 TexCoord0: TEXCOORD0;"
        "};"

        "float4 ps_main( PS_INTPUT input ) : COLOR0"
        "{"
           "return tex2D( Texture0, input.TexCoord0 );"
        "}"

            );

    p_spacebox->GetNodeFromPass( p_passname, p_nodeid )->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "false" ) );
    p_spacebox->GetNodeFromPass( p_passname, p_nodeid )->GetFx()->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "true" ) );

    p_spacebox->GetNodeFromPass( p_passname, p_nodeid )->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETTEXTUREFILTERTYPE, "linear" ) );
    p_spacebox->GetNodeFromPass( p_passname, p_nodeid )->GetFx()->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETTEXTUREFILTERTYPE, "none" ) );

    
}

