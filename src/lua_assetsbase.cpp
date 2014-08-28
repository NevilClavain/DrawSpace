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

#include "lua_assetsbase.h"
#include "memalloc.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;

const char LuaAssetsBase::className[] = "AssetsBase";
const DrawSpace::Luna<LuaAssetsBase>::RegType LuaAssetsBase::Register[] =
{
  { 0 }
};


LuaAssetsBase::LuaAssetsBase( lua_State* p_L ) 
{
    m_assetsbase = _DRAWSPACE_NEW_( AssetsBase, AssetsBase );
}

LuaAssetsBase::~LuaAssetsBase( void ) 
{
    _DRAWSPACE_DELETE_( m_assetsbase );
}
