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

#pragma once 

#include <vector>

#include "renderstatesset.h"
#include "shader.h"

namespace DrawSpace
{
namespace Core
{
struct Fx
{
public:
    
    Shader* GetShader( long p_index ) const;
    RenderState GetRenderStateIn( long p_index ) const;
    RenderState GetRenderStateOut( long p_index ) const;
    long GetShadersListSize( void ) const;
    void ClearShaders( void );
    long GetRenderStatesInListSize( void ) const;
    long GetRenderStatesOutListSize( void ) const;
    void AddShader( Shader* p_shader );

    void GetShadersMD5( dsstring& p_md5 ) const;
    void GetRenderStatesSetMD5( dsstring& p_md5 ) const;


    void SetRenderStateUniqueQueueID( const dsstring& p_id );

    void SetRenderStates( const RenderStatesSet& p_renderstates );

    RenderStatesSet* GetRenderStatesSetRef( void );

private:
    std::vector<Shader*>                        m_shaders;
    RenderStatesSet                             m_renderstates;

};
}
}

