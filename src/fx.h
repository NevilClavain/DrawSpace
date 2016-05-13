/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2016                              
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
#include "configurable.h"

#define FX_TEXT_KEYWORD    "Fx"
#define FX_ARC_MAGICNUMBER 0x4040


namespace DrawSpace
{
namespace Core
{
class Fx : public Configurable
{
protected:
    std::vector<Shader*>                        m_shaders;
    std::vector<RenderState>                    m_renderstates_in;
    std::vector<RenderState>                    m_renderstates_out;

    dsstring                                    m_renderstate_unique_queue_id; //si non vide, permet de forcer la rendering queue à garder
                                                                                // une entree setrenderstates spécialement dédiée au node associée
                                                                                // au lieu d'utiliser celle issue du regroupement
                                                                                // Utile pour le cas ou on doit updater un ou plusieurs renderstate
                                                                                // pour ce node pendant le rendu
                                                                                

    bool on_new_line( const dsstring& p_line, long p_line_num, std::vector<dsstring>& p_words );

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

    void UpdateRenderStateIn( int p_index,const RenderState& p_renderstate );
    void UpdateRenderStateOut( int p_index, const RenderState& p_renderstate );


    void Serialize( Utils::Archive& p_archive  );
    bool Unserialize( Utils::Archive& p_archive );
    //void GetMD5( dsstring& p_md5 );

    void GetShadersMD5( dsstring& p_md5 );
    void GetRenderStatesSetMD5( dsstring& p_md5 );

    void DumpProperties( dsstring& p_text );
    bool ParseProperties( const dsstring& p_text );

    void ApplyProperties( void );

    void GetKeyword( dsstring& p_outkeyword );

    void SetRenderStateUniqueQueueID( const dsstring& p_id );

    static Configurable* Instanciate( void );

};
}
}
#endif
