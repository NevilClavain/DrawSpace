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

#include "bindingsinit.h"
#include "lua_assetsbase.h"
#include "lua_texture.h"
#include "lua_meshe.h"
#include "lua_shader.h"
#include "lua_renderstate.h"
#include "lua_fx.h"
#include "lua_renderingnode.h"
#include "lua_renderingqueue.h"
#include "lua_pass.h"
#include "lua_intermediatepass.h"
#include "lua_finalpass.h"
#include "lua_viewportquad.h"
#include "lua_scenegraph.h"
#include "lua_chunk.h"
#include "lua_vector.h"
#include "lua_matrix.h"
#include "lua_transformnode.h"

using namespace DrawSpace;

void DrawSpaceLuaBindingsInit( lua_State* p_L )
{
    Luna<LuaAssetsBase>::Register( p_L );
    Luna<LuaTexture>::Register( p_L );
    Luna<LuaMeshe>::Register( p_L );
    Luna<LuaShader>::Register( p_L );
    Luna<LuaRenderState>::Register( p_L );
    Luna<LuaFx>::Register( p_L );
    Luna<LuaRenderingNode>::Register( p_L );
    Luna<LuaRenderingQueue>::Register( p_L );
    Luna<LuaPass>::Register( p_L );
    Luna<LuaIntermediatePass>::Register( p_L );
    Luna<LuaFinalPass>::Register( p_L );
    Luna<LuaViewportQuad>::Register( p_L );
    Luna<LuaScenegraph>::Register( p_L );
    Luna<LuaChunk>::Register( p_L );
    Luna<LuaVector>::Register( p_L );
    Luna<LuaMatrix>::Register( p_L );
    Luna<LuaTransformNode>::Register( p_L );
}
