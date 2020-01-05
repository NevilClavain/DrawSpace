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


static std::map<dsstring, PlugInManager<MesheImport>::Handle>   m_mesheimportplugins;



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
