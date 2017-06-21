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

#ifndef _FX_H_
#define _FX_H_

#include "renderstatesset.h"
#include "shader.h"

namespace DrawSpace
{
namespace Core
{
class Fx
{
protected:
    std::vector<Shader*>                        m_shaders;
    RenderStatesSet                             m_renderstates;

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

    void Serialize( Utils::Archive& p_archive  );
    bool Unserialize( Utils::Archive& p_archive );


    void GetShadersMD5( dsstring& p_md5 );
    void GetRenderStatesSetMD5( dsstring& p_md5 );

    void DumpProperties( dsstring& p_text );
    bool ParseProperties( const dsstring& p_text );

    void ApplyProperties( void );

    void GetKeyword( dsstring& p_outkeyword );

    void SetRenderStateUniqueQueueID( const dsstring& p_id );

    void SetRenderStates( const RenderStatesSet& p_renderstates );

    RenderStatesSet* GetRenderStatesSetRef( void );

};
}
}
#endif
