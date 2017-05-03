/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2017                        
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

static std::map<dsstring, PlugInManager<FontImport>::Handle>    m_fontimportplugins;
static std::map<dsstring, PlugInManager<MesheImport>::Handle>   m_mesheimportplugins;

TextWidget* DrawSpace::Utils::BuildText( DrawSpace::Core::Font* p_font, long p_width, long p_height, const Vector& p_color, const dsstring& p_name )
{
    TextWidget* text_widget;

    text_widget = _DRAWSPACE_NEW_( TextWidget, TextWidget( p_name, p_width, p_height, p_font, false, NULL ) );

    text_widget->GetImage()->SetFx( _DRAWSPACE_NEW_( Fx, Fx ) );
    
    text_widget->GetImage()->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( false ) ) );
    text_widget->GetImage()->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( false ) ) );

    text_widget->GetImage()->GetFx()->GetShader( 0 )->SetText( 
        
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

    text_widget->GetImage()->GetFx()->GetShader( 1 )->SetText( 

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

    RenderStatesSet rss;

    rss.AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ALPHABLENDENABLE, "true" ) );
    rss.AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ALPHABLENDOP, "add"  ) );
    rss.AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ALPHABLENDFUNC, "always"  ) );
    rss.AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ALPHABLENDDEST, "invsrcalpha"  ) );
    rss.AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ALPHABLENDSRC, "srcalpha"  ) );
    rss.AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ALPHABLENDENABLE, "false" ) );

    /*
    text_widget->GetImage()->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ALPHABLENDENABLE, "true" ) );
    text_widget->GetImage()->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ALPHABLENDOP, "add"  ) );
    text_widget->GetImage()->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ALPHABLENDFUNC, "always"  ) );
    text_widget->GetImage()->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ALPHABLENDDEST, "invsrcalpha"  ) );
    text_widget->GetImage()->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ALPHABLENDSRC, "srcalpha"  ) );
    text_widget->GetImage()->GetFx()->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ALPHABLENDENABLE, "false" ) );
    */
    text_widget->GetImage()->GetFx()->SetRenderStates( rss );

    text_widget->GetImage()->AddShaderParameter( 1, "color", 0 );
    text_widget->GetImage()->SetShaderRealVector( "color", p_color );


    text_widget->GetText()->SetFx( _DRAWSPACE_NEW_( Fx, Fx ) );

    text_widget->GetText()->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( false ) ) );
    text_widget->GetText()->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( false ) ) );

    text_widget->GetText()->GetFx()->GetShader( 0 )->SetText( 

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


    text_widget->GetText()->GetFx()->GetShader( 1 )->SetText( 
        
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

    RenderStatesSet rss2;
    rss2.AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETTEXTUREFILTERTYPE, "linear" ) );
    rss2.AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETTEXTUREFILTERTYPE, "none" ) );

/*
    text_widget->GetText()->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETTEXTUREFILTERTYPE, "linear" ) );
    text_widget->GetText()->GetFx()->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETTEXTUREFILTERTYPE, "none" ) );
*/
    text_widget->GetText()->GetFx()->SetRenderStates( rss2 );

    text_widget->GetText()->AddShaderParameter( 1, "color", 0 );
    text_widget->GetText()->SetShaderRealVector( "color", Utils::Vector( 1.0, 1.0, 1.0, 0.0 ) );

    text_widget->SetPassTargetClearingColor( 0, 0, 0, 0 );
    
    text_widget->GetInternalPass()->GetRenderingQueue()->UpdateOutputQueue();

    return text_widget;
}

bool DrawSpace::Utils::LoadFontImportPlugin( const dsstring& p_path, const dsstring& p_pluginalias )
{
	dsstring complete_path = p_path;
#ifdef _DEBUG
	complete_path += ".dll";
#else
	complete_path += "_r.dll";
#endif
    PlugInManager<FontImport>::Handle pihandle;
    PluginManagerStatus pistatus = PlugInManager<FontImport>::LoadPlugin( complete_path.c_str(), pihandle );
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

void DrawSpace::Utils::BuildSpaceboxFx( Spacebox* p_spacebox, Pass* p_pass )
{
    for( long i = 0; i < 6; i++ )
    {

        p_spacebox->GetNodeFromPass( p_pass, i )->SetFx( _DRAWSPACE_NEW_( Fx, Fx ) );
        p_spacebox->GetNodeFromPass( p_pass, i )->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( false ) ) );
        p_spacebox->GetNodeFromPass( p_pass, i )->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( false ) ) );

        p_spacebox->GetNodeFromPass( p_pass, i )->GetFx()->GetShader( 0 )->SetText(

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

        p_spacebox->GetNodeFromPass( p_pass, i )->GetFx()->GetShader( 1 )->SetText(

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

        RenderStatesSet rss;

        rss.AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "false" ) );
        rss.AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "true" ) );

        rss.AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETTEXTUREFILTERTYPE, "linear" ) );
        rss.AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETTEXTUREFILTERTYPE, "none" ) );

        p_spacebox->GetNodeFromPass( p_pass, i )->GetFx()->SetRenderStates( rss );

        /*
        p_spacebox->GetNodeFromPass( p_pass, i )->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "false" ) );
        p_spacebox->GetNodeFromPass( p_pass, i )->GetFx()->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "true" ) );

        p_spacebox->GetNodeFromPass( p_pass, i )->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETTEXTUREFILTERTYPE, "linear" ) );
        p_spacebox->GetNodeFromPass( p_pass, i )->GetFx()->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETTEXTUREFILTERTYPE, "none" ) );
        */
    }    
}

long DrawSpace::Utils::StringToInt( const dsstring& p_value )
{
    return atoi( p_value.c_str() );
}

dsreal DrawSpace::Utils::StringToReal( const dsstring& p_value )
{
    return atof( p_value.c_str() );
}

void DrawSpace::Utils::IntToString( long p_value, dsstring& p_str )
{
    char str[128];
    sprintf( str, "%d", p_value );
    p_str = dsstring( str );
}

void DrawSpace::Utils::RealToString( dsreal p_value, dsstring& p_str )
{
    char str[128];
    sprintf( str, "%f", p_value );
    p_str = dsstring( str );
}

dswstring DrawSpace::Utils::String2WString( const dsstring& p_s )
{
    int len;
    int slength = (int)p_s.length() + 1;
    len = MultiByteToWideChar( CP_ACP, 0, p_s.c_str(), slength, 0, 0 ); 
    dswstring r( len, L'\0' );
    MultiByteToWideChar( CP_ACP, 0, p_s.c_str(), slength, &r[0], len );
    return r;
}

dsstring DrawSpace::Utils::WString2String( const dswstring& p_s )
{
    int len;
    int slength = (int)p_s.length() + 1;
    len = WideCharToMultiByte( CP_ACP, 0, p_s.c_str(), slength, 0, 0, 0, 0 ); 
    dsstring r( len, '\0' );
    WideCharToMultiByte( CP_ACP, 0, p_s.c_str(), slength, &r[0], len, 0, 0 ); 
    return r;
}
