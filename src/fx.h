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

#ifndef _FX_H_
#define _FX_H_

#include "drawspace_commons.h"
#include "renderstate.h"
#include "shader.h"
#include "asset.h"

namespace DrawSpace
{
namespace Core
{
class Fx : public Asset
{
protected:
    std::vector<Shader*>                        m_shaders;
    std::vector<RenderState>                    m_renderstates_in;
    std::vector<RenderState>                    m_renderstates_out;


public:
    Fx( void );
    ~Fx( void );
    
    Shader* GetShader( long p_index );
    RenderState GetRenderStateIn( long p_index );
    RenderState GetRenderStateOut( long p_index );
    long GetShadersListSize( void );
    long GetRenderStatesInListSize( void );
    long GetRenderStatesOutListSize( void );
    void AddShader( Shader* p_shader );
    void AddRenderStateIn( const RenderState& p_renderstate );
    void AddRenderStateOut( const RenderState& p_renderstate );
    void Serialize( Core::Factory& p_factory, Utils::Archive& p_archive  );
    void Unserialize( Core::Factory& p_factory, Utils::Archive& p_archive );
    void GetMD5( dsstring& p_md5 );
};
}
}
#endif