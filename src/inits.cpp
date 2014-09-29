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

#include "inits.h"
#include "factory.h"
#include "texture.h"
#include "shader.h"
#include "meshe.h"
#include "font.h"
#include "fx.h"
#include "pass.h"
#include "spacebox.h"
#include "circularmovement.h"
#include "fpsmovement.h"
#include "renderstate.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;

// generic and statics initialisations done here
void DrawSpace::Initialize( void )
{
    Factory::GetInstance()->RegisterAssetInstanciationFuncByText( TEXTURE_TEXT_KEYWORD, Texture::Instanciate );
    Factory::GetInstance()->RegisterAssetInstanciationFuncByArc( TEXTURE_ARC_MAGICNUMBER, Texture::Instanciate );

    Factory::GetInstance()->RegisterAssetInstanciationFuncByText( SHADER_TEXT_KEYWORD, Shader::Instanciate );
    Factory::GetInstance()->RegisterAssetInstanciationFuncByArc( SHADER_ARC_MAGICNUMBER, Shader::Instanciate );

    Factory::GetInstance()->RegisterAssetInstanciationFuncByText( MESHE_TEXT_KEYWORD, Meshe::Instanciate );
    Factory::GetInstance()->RegisterAssetInstanciationFuncByArc( MESHE_ARC_MAGICNUMBER, Meshe::Instanciate );

    Factory::GetInstance()->RegisterAssetInstanciationFuncByText( FONT_TEXT_KEYWORD, Font::Instanciate );
    Factory::GetInstance()->RegisterAssetInstanciationFuncByArc( FONT_ARC_MAGICNUMBER, Font::Instanciate );

    Factory::GetInstance()->RegisterConfigInstanciationFuncByText( FX_TEXT_KEYWORD, Fx::Instanciate );
    Factory::GetInstance()->RegisterConfigInstanciationFuncByArc( FX_ARC_MAGICNUMBER, Fx::Instanciate );

    Factory::GetInstance()->RegisterConfigInstanciationFuncByText( INTERMEDIATEPASS_TEXT_KEYWORD, Fx::Instanciate );
    Factory::GetInstance()->RegisterConfigInstanciationFuncByArc( INTERMEDIATEPASS_ARC_MAGICNUMBER, Fx::Instanciate );

    Factory::GetInstance()->RegisterConfigInstanciationFuncByText( FINALPASS_TEXT_KEYWORD, Fx::Instanciate );
    Factory::GetInstance()->RegisterConfigInstanciationFuncByArc( FINALPASS_ARC_MAGICNUMBER, Fx::Instanciate );

    Factory::GetInstance()->RegisterConfigInstanciationFuncByText( SPACEBOX_TEXT_KEYWORD, Spacebox::Instanciate );
    Factory::GetInstance()->RegisterConfigInstanciationFuncByArc( SPACEBOX_ARC_MAGICNUMBER, Spacebox::Instanciate );

    Factory::GetInstance()->RegisterConfigInstanciationFuncByText( CIRCULARMVT_TEXT_KEYWORD, CircularMovement::Instanciate );
    Factory::GetInstance()->RegisterConfigInstanciationFuncByArc( CIRCULARMVT_ARC_MAGICNUMBER, CircularMovement::Instanciate );

    Factory::GetInstance()->RegisterConfigInstanciationFuncByText( FPSMVT_TEXT_KEYWORD, FPSMovement::Instanciate );
    Factory::GetInstance()->RegisterConfigInstanciationFuncByArc( FPSMVT_ARC_MAGICNUMBER, FPSMovement::Instanciate );

    RenderState::InitStringMaps();
}